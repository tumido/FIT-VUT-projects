import requests
import logging
import lxml.etree as etree

logger = logging.getLogger('antipirat')

OUTPUT_NAME = 'movies_announce'
CHANNEL_DETAILS = ('title',
                   'link',
                   'description')
ITEM_DETAILS = ('title',
                'category',
                'author',
                'link',
                'pubDate',
                'torrent:infoHash',
                'torrent:fileName')

def get_announce(url):
    """
    Make the HTTP request for RSS feed on the given url. Store the data in
    an XML file
    :param url: remote location of the RSS feed
    """
    logger.info('Downloading {0}.xml file from {1}'.format(OUTPUT_NAME, url))
    try:
        r = requests.get(url)
        r.raise_for_status()
    except (requests.HTTPError, requests.ConnectionError) as e:
        response = 'No response'
        if 'response' in dir(e):
            response = e.response.status_code
        logger.critical('Unable to download {0}.xml from {1}: Error {2}'.format(
            OUTPUT_NAME, url, response))
        raise

    with open('{0}.xml'.format(OUTPUT_NAME), 'w') as f:
        f.write(r.text)
        f.flush()
    logger.info('File {0}.xml was successfully saved'.format(OUTPUT_NAME))

    return '{0}.xml'.format(OUTPUT_NAME)

def announce_to_txt(filename):
    """
    Convert and save the announce description from XML to plain text
    :param filename: name of the RSS xml file
    """
    first_torrent = None
    tree = etree.parse(filename)
    root = tree.getroot()
    ns = root.nsmap

    logger.info('Parsing the {0} file to {1}_list.txt'.format(filename,
                                                              OUTPUT_NAME))

    with open('{0}_list.txt'.format(OUTPUT_NAME), 'w') as f:
        for idx, channel in enumerate(root):
            # if we have multiple channels in the RSS, add separator
            if idx != 0:
                f.write('\n\n')

            # display the channel info
            for info in CHANNEL_DETAILS:
                try:
                    f.write('{0}: {1}\n'.format(info, channel.find(info).text))
                except AttributeError:
                    logger.warning('Channel is missing the "{0}" information'.format(
                        info))
            f.write('\n')

            # for each item in the channel display its data
            for idx, item in enumerate(channel.findall('item')):
                f.write('\n')
                for info in ITEM_DETAILS:
                    try:
                        infotext = item.find(info, ns).text
                    except AttributeError:
                        infotext = ''
                        label = 'no. {0}'.format(idx) if info == 'title' else item.find('title').text
                        logger.warning('Item "{0}" doesn\'t contain "{1}" data'
                                       ''.format(label, info))
                    f.write('{0}: {1}\n'.format(info, infotext))
                # if we're processing the first item, save its torrent file
                if not first_torrent:
                    name = item.find('torrent:fileName', ns).text
                    url = item.find('enclosure').attrib['url']
                    first_torrent = {'filename': name, 'url': url}

            logger.info('Channel "{0}" parsed with {1} item(s) listed'.format(
                channel.find('title').text, len(channel.findall('item'))))
        f.flush()

    logger.info('File {0}_list.txt was successfully saved'.format(OUTPUT_NAME))

    return first_torrent

def main():
    print('Not directly callable')

if __name__ == '__main__':
    main()
