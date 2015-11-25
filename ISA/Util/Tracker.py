import socket
import logging
import struct as s
import bencodepy as b
import threading

from urllib.parse import urlparse, urlencode
from random import choice
from string import ascii_uppercase, digits
from binascii import a2b_hex
from queue import Queue

ID = ''.join(choice(ascii_uppercase + digits) for _ in range(20))
TIMEOUT = 5.
TRACKER_PARAMS = {'info_hash': None,
                  'peer_id': ID,
                  'downloaded': 0,
                  'uploaded': 0,
                  'left': 1,
                  'port': 6881,
                  'event': 'started',
                  'compact': 1,
                  'numwant': -1}
HTTP_REQUEST = ('GET {0}?{1} HTTP/1.1\r\n'
                'User-Agent: AntiPirate/1.0\r\n'
                'Connection: close\r\n'
                'Host: {2}:{3}\r\n'
                '\r\n')
CHUNK_SIZE = 4096
MAX_RETRY = 3

logger = logging.getLogger('antipirat')
socket.setdefaulttimeout = TIMEOUT

def peers_b2l(response):
    """
    Parse the response fron tracker, dig out the peers addresses
    :param response: binary data, response from the tracker
    """
    if response == b'':
        raise ValueError('No data returned')

    data = b.decode(response)
    lst = list()

    if not isinstance(data[b'peers'], bytes):
        raise TypeError('Peer list is {0}'.format(data[b'peers']))

    # parse the response
    for offset in range(0,len(data[b'peers']), 6):
        # all data are stored as big endian
        # dig out the IP address (integer)
        raw_ip = s.unpack_from(">i", data[b'peers'], offset)[0]
        ip = socket.inet_ntoa(s.pack(">i", raw_ip))

        # do the same for the port number (insigned short)
        port = s.unpack_from(">H", data[b'peers'], offset+4)[0]

        lst.append('{0}:{1}'.format(ip, port))

    return lst

class HttpTracker(threading.Thread):
    """
    Hit the HTTP tracker via socket, send GET request and recieve data
    """

    def __init__(self, url, query, result_lst):
        """
        Initalize the thread
        :param url: tracker url as ParseResult object
        :param query: the GET request query as str
        :return list: list of peers found
        """
        # Create a thread for this instance
        threading.Thread.__init__(self)

        # set proper path
        self.path = url.path if url.path != "" else "/"

        # parse the url to get host and port
        self.host, _, self.port = url.netloc.rpartition(':')
        if self.host == '':
            self.host, self.port = url.netloc, 80

        # rememer important data
        self.query = query
        self.lst = result_lst
        self.s = None

    def run(self):
        req = HTTP_REQUEST.format(self.path, self.query,
                                  self.host, self.port).encode('utf-8')
        self.send_request(req)

        if self.s:
            self.recieve_response()

    def send_request(self, request):
        """
        Create socket, connect and send data
        """
        # get tracker ip address
        try:
            address = socket.getaddrinfo(self.host, self.port)
        except IOError as e:
            logger.error('Unable to locate {0}:{1} remote host: {2}'
                        ''.format(self.host, self.port, e))
            return

        # create connection to remote hsot
        for retry in range(1, MAX_RETRY+1):
            try:
                # create socket
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.settimeout(TIMEOUT)
                s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                # bind it to the address
                s.connect((self.host, int(self.port)))
                break
            except IOError as e:
                s.close()
                logger.error('Retry [{3}/{4}]: Unable to connect to {0}:{1}'
                             ' tracker: {2}'.format(self.host, self.port,
                                                    e, retry, MAX_RETRY))
                if retry == MAX_RETRY:
                    logger.error('Skipping {0}:{1} tracker'.format(self.host,
                                                                   self.port))
                    return
                continue

        self.s = s
        # send request
        s.send(request)

    def recieve_response(self):
        """
        Recieve data, parse the response and update the list with peers
        """
        # recieve response
        data = bytes()
        while True:
            # let's wait for response
            for retry in range(1, MAX_RETRY+1):
                try:
                    # recieve data and exit the retry if everything goes fine
                    chunk = self.s.recv(CHUNK_SIZE)
                    break
                except IOError as e:
                    # error when recieving data -> timeout
                    logger.error('Retry [{3}/{4}]: Failed to recieve data from '
                                '{0}:{1} tracker: {2}'.format(
                                    self.host, self.port, e, retry, MAX_RETRY))
                    if retry == MAX_RETRY:
                        # we encounter a really huge delay from the server, quit
                        logger.error('Skipping {0}:{1} tracker'
                                     ''.format(self.host, self.port))
                        return
                    # let's try it again
                    continue
            if not chunk:
                break
            data += chunk

        # we don't care about the header, only content matters
        data = data.split(b'\r\n\r\n')[-1]

        # get the peers from the response
        try:
            peers_lst = peers_b2l(data)
        except (b.DecodingError, ValueError, KeyError, TypeError) as e:
            logger.error('Failed to parse response from {0}:{1} tracker: {2}({3})'
                        ''.format(self.host, self.port, type(e).__name__, e))
            return
        logger.info('Found {2} peers on tracker {0}:{1}'
                    ''.format(self.host, self.port, len(peers_lst)))

        [self.lst.append(peer) for peer in peers_lst]


class UdpTracker(threading.Thread):
    """
    Hit the UDP tracker via socket. Not supported so far
    """
    def __init__(self, url, infohash, result_lst):
        # Create a thread for this instance
        threading.Thread.__init__(self)
        # Not implemented
        logger.debug('UDP tracker, skipping')

    def run(self):
        pass


def get_peerlist(trackers, infohash):
    """
    Wrapper above trackers that differentiate between UDP and HTTP trackers and
    launches proper query functions
    :param trackers: list of trackers urls as string
    :param infohash: infohash as hexstring
    :return: list of peers found for this torrent
    """
    peers = list()
    TRACKER_PARAMS['info_hash'] = a2b_hex(infohash.encode("utf-8"))
    http_param = urlencode(TRACKER_PARAMS)

    threads = list()
    for t in trackers:
        url = urlparse(t)
        if url.scheme == 'http':
            thread = HttpTracker(url, http_param, peers)
        elif url.scheme == 'udp':
            thread = UdpTracker(url, infohash, peers)
        else:
            logger.error('Invalid tracker: {0}'.format(t))
            continue
        threads.append(thread)

    # start all threads
    for thread in threads:
        thread.start()

    # wait until all threads finish
    for thread in threads:
        thread.join()

    return peers

def save_peerlist(infohash, data):
    """
    Dump peers list to a file named as infohash.peerlist
    :param infohash: infohash as hexstring
    :param data: list of peers as strings
    """
    with open('{0}.peerlist'.format(infohash), 'w') as f:
        for peer in data:
            f.write('{0}\n'.format(peer))
        f.flush()
    logger.info('File {0}.peerlist was successfully saved'.format(infohash))

def main():
    print('Not directly callable')

if __name__ == '__main__':
    main()
