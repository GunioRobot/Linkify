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
import logging, sys, time

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
                b'action': 'append',
                b'nargs': '?',
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
                b'type': lambda level: logging._levelNames[level.upper()],
                b'help': 'set the default logging level',
            }),
        ]
        
        for name, options in arguments:
            self.add_argument(name, **options)


class Automate (ArgumentsParser):
    _AVAILABLE_TASKS = [
        automate.download.GameTrailers,
        automate.download.GtCountdown,
        automate.download.HdTrailers,
        automate.download.IgnDailyFix,
        automate.download.InterfaceLift,
        automate.download.PopFiction,
        automate.download.ScrewAttack,
        automate.task.Dropbox,
        automate.task.GnuCash,
        automate.task.Opera,
        automate.task.Windows,
    ]
    
    
    def __init__(self):
        ArgumentsParser.__init__(self)
    
    
    def execute(self):
        nothing_done = True
        arguments = self.parse_args()
        automate.util.Logger.DEFAULT_LEVEL = arguments.log
        download_manager = automate.download.FreeDownloadManager()
        
        if arguments.download:
            nothing_done = False
            download_manager.download_url(arguments.download)
        
        if arguments.start:
            start_tasks = set(arguments.start)
            
            if not ((None in start_tasks) and (len(start_tasks) > 1)):
                nothing_done = False
                
                tasks = self._start_tasks(
                    download_manager,
                    None if None in start_tasks else start_tasks)
                
                while any([task.is_alive() for task in tasks]):
                    try:
                        time.sleep(1)
                    except KeyboardInterrupt:
                        for task in tasks:
                            task.stop()
                        break
        
        if nothing_done:
            self.print_help()
    
    
    def _start_tasks(self, download_manager, start_tasks = None):
        tasks = []
        
        if start_tasks is None:
            tasks = [task_class() for task_class in self._AVAILABLE_TASKS]
        else:
            for task in start_tasks:
                task_class = next(
                    (t for t in self._AVAILABLE_TASKS if task == t.__name__),
                    None)
                
                if task_class is None:
                    sys.exit('Unknown task: ' + task)
                
                tasks.append(task_class())
        
        started_tasks = []
        
        for task in tasks:
            if isinstance(task, automate.download.DownloadSource):
                task = automate.download.Downloader(download_manager, task)
            
            task.start()
            started_tasks.append(task)
        
        return started_tasks
