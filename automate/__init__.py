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


class Automate (ArgumentsParser):
    @staticmethod
    def query_download_source(manager, source):
        source.logger.info('Start')
        
        while True:
            source.logger.debug('Resume')
            
            for url in source.list_urls():
                try:
                    if not manager.has_url(url):
                        manager.download_url(url)
                except (httplib.HTTPException, urllib2.URLError) as error:
                    source.logger.error('%s: %s', str(error), url)
            
            source.logger.debug('Pause')
            time.sleep(15 * 60)
    
    
    def execute(self):
        nothing_done = True
        arguments = self.parse_args()
        download_manager = automate.download.FreeDownloadManager()
        
        if arguments.download:
            nothing_done = False
            download_manager.download_url(arguments.download)
        
        if arguments.start:
            nothing_done = False
            self._start_tasks()
            threads = self._start_download_sources(download_manager)
            
            while any([thread.is_alive() for thread in threads]):
                time.sleep(15 * 60)
        
        if nothing_done:
            self.print_help()
    
    
    def _start_download_sources(self, manager):
        sources = [
            automate.download.GameTrailers(),
            automate.download.GtCountdown(),
            automate.download.HdTrailers(),
            automate.download.IgnDailyFix(),
            automate.download.InterfaceLift(),
            automate.download.PopFiction(),
            automate.download.ScrewAttack(),
        ]
        
        threads = []
        
        for source in sources:
            thread = threading.Thread(
                args = (manager, source),
                name = source.name,
                target = self.query_download_source)
            
            thread.daemon = True
            thread.start()
            threads.append(thread)
        
        return threads
    
    
    def _start_tasks(self):
        map(lambda task: task.start(), [
            automate.task.Dropbox(),
            automate.task.GnuCash(),
            automate.task.Opera(),
            automate.task.Windows(),
        ])
