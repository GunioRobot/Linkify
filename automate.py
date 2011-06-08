#!/usr/bin/env python
# -*- coding: utf-8 -*-


# TODO: Create source TV shows with automatic backup.
# TODO: Use command line options to choose the download manager, execute
#       download finished events (both automatically and manually via command
#       line), etc.
# TODO: Cut the first few seconds of the IGN Daily Fix videos.
# TODO: Create MS Win32 system service? Desktop gadget?
# TODO: Web server with RSS feed for errors?
# TODO: Implement a graphical interface? System tray icon?
# TODO: Add documentation.
# TODO: Profile time execution.


# Standard library:
from __future__ import division, print_function, unicode_literals
import threading, time, urllib2

# External modules:
from defaults import *

# Internal modules:
import automate.download, automate.task, automate.util


externals('argparse')


class ArgumentsParser (argparse.ArgumentParser):
    def __init__(self):
        argparse.ArgumentParser.__init__(self, description = 'Task automation.')
        
        arguments = [
            ('--start', {
                b'action': 'store_true',
                b'default': False,
                b'help': 'start task automation process',
            }),
            ('--download', {
                b'action': 'store',
                b'type': automate.util.Url,
                b'help': 'download an URL using the default manager',
            }),
        ]
        
        for name, options in arguments:
            self.add_argument(name, **options)


def query_source(dl_manager, dl_source):
    dl_source.logger.info('Start')
    
    while True:
        dl_source.logger.debug('Resume')
        
        for url in dl_source.list_urls():
            try:
                if not dl_manager.has_url(url):
                    dl_manager.download_url(url)
            except (httplib.HTTPException, urllib2.URLError) as error:
                dl_source.logger.error('%s: %s', str(error), url)
        
        dl_source.logger.debug('Pause')
        time.sleep(15 * 60)


args_parser = ArgumentsParser()
args = args_parser.parse_args()
dl_manager = automate.download.FreeDownloadManager()
nothing_done = True

if args.download:
    nothing_done = False
    dl_manager.download_url(args.download)

if args.start:
    nothing_done = False
    
    map(lambda task: task.start(), [
        automate.task.Dropbox(),
        automate.task.GnuCash(),
        automate.task.Opera(),
        automate.task.Windows(),
    ])
    
    dl_sources = [
        automate.download.GameTrailers(),
        automate.download.GtCountdown(),
        automate.download.HdTrailers(),
        automate.download.IgnDailyFix(),
        automate.download.InterfaceLift(),
        automate.download.PopFiction(),
        automate.download.ScrewAttack(),
    ]
    
    dl_sources_threads = []
    
    for dl_source in dl_sources:
        thread = threading.Thread(
            args = (dl_manager, dl_source),
            name = dl_source.name,
            target = query_source)
        
        thread.daemon = True
        thread.start()
        dl_sources_threads.append(thread)
    
    while any([thread.is_alive() for thread in dl_sources_threads]):
        time.sleep(15 * 60)

if nothing_done:
    args_parser.print_help()
