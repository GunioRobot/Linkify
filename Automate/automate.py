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


def query_source(dl_manager, dl_source):
    dl_source.logger.info('Start')
    
    while True:
        dl_source.logger.debug('Resume')
        
        for url in dl_source.list_urls():
            try:
                if not dl_manager.has_url(url):
                    dl_manager.download_url(url)
            except urllib2.URLError as error:
                dl_source.logger.error('%s: %s', str(error), url)
        
        dl_source.logger.debug('Pause')
        time.sleep(10 * 60)


map(lambda task: task.start(), [
    automate.task.Dropbox(),
    automate.task.GnuCash(),
    automate.task.Opera(),
    automate.task.Windows(),
])

dl_manager = automate.download.ThreadSafeDownloadManager(
    automate.download.FreeDownloadManager)
dl_manager.start()

dl_sources = [
    automate.download.GameTrailers(),
    automate.download.GtCountdown(),
    automate.download.HdTrailers(),
    automate.download.IgnDailyFix(),
    automate.download.InterfaceLift(),
    automate.download.PopFiction(),
    automate.download.ScrewAttack(),
]

for dl_source in dl_sources:
    thread = threading.Thread(
        args = (dl_manager, dl_source),
        name = dl_source.name,
        target = query_source)
    
    thread.daemon = True
    thread.start()

while True:
    time.sleep(1 * 60 * 60)
