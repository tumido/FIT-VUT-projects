import requests
import logging
import bencodepy as b

from hashlib import sha1

logger = logging.getLogger('antipirat')

def get_torrent_file(t_dict):
    """
    Download a Torrent file from given url
    :param t_dict: dict in format {'filename': <filename>, 'url': <source_url>}
    """
    logger.info('Downloading {0} file from {1}'.format(t_dict['filename'],
                                                       t_dict['url']))
    try:
        headers = {'User-Agent': 'Mozzila/5.0'}
        r = requests.get(t_dict['url'], headers=headers, stream=True)
        r.raise_for_status()

        with open(t_dict['filename'], 'wb') as f:
            for chunk in r.iter_content(chunk_size=1024):
                if chunk:
                    f.write(chunk)
                    f.flush()
        logger.info('File {0} was successfully saved'.format(t_dict['filename']))

    except (requests.HTTPError, requests.ConnectionError) as e:
        response = 'No response'
        if 'response' in dir(e):
            response = e.response.status_code
        logger.critical('Unable to download {0} from {1}: Error {2}'.format(
            t_dict['filename'], t_dict['url'], response))
        raise

def flatten(mixed):
    lst = list()
    for item in mixed:
        if isinstance(item, list):
            lst += flatten(item)
        else:
            lst.append(item.decode())
    return lst

def parse_torrent_file(filename):
    """
    Decode Torrent file and sig out data that matters: trackers, and infohash
    """
    try:
        logger.info('Parsing torrent file {0}'.format(filename))
        data = b.decode_from_file(filename)
    except (b.DecodingError, IOError) as e:
        logger.critical('Unable to parse torrent file {0}: {1}'.format(filename, e))
        raise

    trackers = [data[b'announce'].decode()] + flatten(data[b'announce-list'])
    trackers = set(trackers)

    infohash = sha1(b.encode(data[b'info'])).hexdigest()

    return trackers, infohash

def main():
    print('Not directly callable')

if __name__ == '__main__':
    main()
