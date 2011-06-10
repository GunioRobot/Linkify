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
import httplib, logging, threading, time, urllib2

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
            ('--log', {
                b'action': 'store',
                b'default': 'INFO',
                b'type': lambda level: logging._levelNames[level],
                b'help': 'set the default logging level',
            }),
        ]
        
        for name, options in arguments:
            self.add_argument(name, **options)


class Automate (ArgumentsParser):
    def __init__(self):
        ArgumentsParser.__init__(self)
        self._exit = threading.Event()
    
    
    def execute(self):
        nothing_done = True
        arguments = self.parse_args()
        automate.util.Logger.DEFAULT_LEVEL = arguments.log
        download_manager = automate.download.FreeDownloadManager()
        
        if arguments.download:
            nothing_done = False
            download_manager.download_url(arguments.download)
        
        if arguments.start:
            nothing_done = False
            self._start_tasks()
            threads = self._start_download_sources(download_manager)
            
            while any([thread.is_alive() for thread in threads]):
                try:
                    time.sleep(1)
                except KeyboardInterrupt:
                    self._exit.set()
                    break
        
        if nothing_done:
            self.print_help()
    
    
    def _query_download_source(self, manager, source):
        source.logger.info('Start')
        
        while not self._exit.is_set():
            source.logger.debug('Resume')
            
            for url in source.list_urls():
                if self._exit.is_set():
                    break
                
                try:
                    if not manager.has_url(url):
                        manager.download_url(url)
                except (httplib.HTTPException, urllib2.URLError) as error:
                    source.logger.error('%s: %s', str(error), url)
            
            source.logger.debug('Pause')
            self._exit.wait(15 * 60)
        
        source.logger.info('Stop')
    
    
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
                target = self._query_download_source)
            
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
