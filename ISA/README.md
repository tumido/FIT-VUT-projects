# ISA project - Analyze Bittorrent service

Antipirat is a command-line tool written in Python that allows you to analyze
Announce RSS feeds, convert them to a human readable format for easier
orientation. Also it's capable of downloading the lastly added torrent file (the
first one in the feed) and hit it's trackers and fetch the peers IP addresses.

## Specification

Program implements [this](https://wis.fit.vutbr.cz/FIT/st/course-sl.php?id=589542&item=54113&cpa=1)
project specification.

### Supported extensions

**Argument -t extension** is the only extension supported by this time.

## Usage

Program supports couple of execution modes and some extra parameters, here's its
signature:
```
antipirat [-h] (-r  | -i  | -t ) [-a]
```
### Required argument

One and only of these parameters is required for each run. By this argument you
specify the mode of execution -  what type on input file is proceed.
- **r** selects an url where RSS feed can be found
- **i** instead of RSS feed loads the XML directly
- **t** doesn't bother with announce RSS or XML, only torrent file matters

### Optional arguments
- **h** is for help
- **a** sets an url that will be used instead of any tracker

### Output

Program saves the XML fetched from RSS feed, generated text file, the torrent
file (last entry) and its peerlist.

Here's an examlpe of execution:
```
$ ./antipirat -r https://kat.cr/movies/?rss=1
INFO: Downloading movies_announce.xml file from https://kat.cr/movies/?rss=1
INFO: File movies_announce.xml was successfully saved
INFO: Parsing the movies_announce.xml file to movies_announce_list.txt
INFO: Channel "movies torrents RSS feed - KickassTorrents" parsed with 25 item(s) listed
INFO: File movies_announce_list.txt was successfully saved
INFO: Downloading frequency.2000.1080p.bdremux.avc.dts.hd.ma.7.1.leon.345.torrent file from https://torcache.net/torrent/B97CA062DF35C687982F0B1E437CC60F197F1367.torrent?title=[kat.cr]frequency.2000.1080p.bdremux.avc.dts.hd.ma.7.1.leon.345
INFO: File frequency.2000.1080p.bdremux.avc.dts.hd.ma.7.1.leon.345.torrent was successfully saved
INFO: Parsing torrent file frequency.2000.1080p.bdremux.avc.dts.hd.ma.7.1.leon.345.torrent
DEBUG: UDP tracker, skipping
DEBUG: UDP tracker, skipping
DEBUG: UDP tracker, skipping
DEBUG: UDP tracker, skipping
DEBUG: UDP tracker, skipping
DEBUG: UDP tracker, skipping
INFO: Found 6 peers on tracker tracker.torrenty.org:6969
INFO: Found 7 peers on tracker tracker.filetracker.pl:8089
INFO: File b97ca062df35c687982f0b1e437cc60f197f1367.peerlist was successfully saved

```

## Installation

Unpack this archive and run `make`. On regular systems this will install all the
required Python packages using `pip` for you and you're ready to go.

### Target system

For the purpose of testing I simply can't relly on this, because `pip` is not
available on target system. To bypass this nonsense the only missing package
there - the [BencodePy](https://github.com/eweast/BencodePy.git) package has been
attached directly _as is_ to the archive and the `sys.path` in `antipirat` file
has also been updated accordingly. I'm not the Author of this package
all credits belong to [Eric Weast](https://github.com/eweast).

### Archive content
```
xcoufa09.tgz
    ├──  BencodePy - contains BencodePy package
    |               (git clone https://github.com/eweast/BencodePy.git
    |                && rm -r tests build misc .git)
    ├──  Util
    │    ├── __init__.py
    │    ├── Announce.py - RSS feed heavy lifting
    │    ├── Torrent.py - Torrent file parsing
    │    └── Tracker.py - Communication with trackers
    ├── Makefile
    ├── manual.pdf
    ├── README -> ./README.md
    ├── README.md
    ├── requirements.txt - For pip installation
    └── antipirat - Execute this
```