# -*- coding: utf-8 -*-


# TODO: Use command line options to execute download finished events.
# TODO: Create task for TV shows (backup, etc).
#       TV Rage API: http://services.tvrage.com/info.php?page=main
# TODO: Settings backup with built-in support for Winamp, MPC, Opera, etc.
# TODO: Clean up JavaScript package.
# TODO: Create UserJS to show content sensitive buttons on text selection:
#       search, translate, IMDb movie link/ratings, etc.
# TODO: Web server with RSS feed for errors?
# TODO: Implement a GUI? System tray icon? Desktop gadget? System service?
# TODO: Cut the first few seconds of the IGN Daily Fix videos.
# TODO: Add documentation.


# Standard library:
from __future__ import division, print_function, unicode_literals
import inspect, logging, operator, sys, time

# External modules:
from defaults import *

# Internal modules:
import automate.backup, automate.download, automate.task, automate.util


externals('argparse')


class ArgumentsParser (argparse.ArgumentParser):
    def __init__(self):
        argparse.ArgumentParser.__init__(self,
            description = 'Task automation.',
            epilog = 'Available tasks: '
                + ', '.join(self._list_available_task_names()))
        
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
    
    
    def _list_available_tasks(self):
        return self._list_concrete_classes(automate.backup.BackupTask) \
            + self._list_concrete_classes(automate.download.DownloadSource) \
            + self._list_concrete_classes(automate.task.PeriodicTask)
    
    
    def _list_available_task_classes(self):
        return map(operator.itemgetter(1), self._list_available_tasks())
    
    
    def _list_available_task_names(self):
        names = map(operator.itemgetter(0), self._list_available_tasks())
        names.sort()
        
        return names
    
    
    def _list_concrete_classes(self, base_class):
        def is_concrete_class(object):
            return inspect.isclass(object) \
                and not inspect.isabstract(object) \
                and issubclass(object, base_class)
        
        return inspect.getmembers(
            inspect.getmodule(base_class), is_concrete_class)


class Automate (ArgumentsParser):
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
            task_names = set(arguments.start)
            
            if not ((None in task_names) and (len(task_names) > 1)):
                nothing_done = False
                
                tasks = self._start_tasks(
                    download_manager,
                    None if None in task_names else task_names)
                
                try:
                    while any([task.is_alive() for task in tasks]):
                        time.sleep(1)
                except KeyboardInterrupt:
                    for task in tasks:
                        task.stop()
        
        if nothing_done:
            self.print_help()
    
    
    def _start_task(self, download_manager, task_class):
        task = task_class()
        
        if isinstance(task, automate.download.DownloadSource):
            task = automate.download.Downloader(download_manager, task)
        
        task.start()
        return task
    
    
    def _start_tasks(self, download_manager, task_names = None):
        available_tasks = self._list_available_task_classes()
        
        if task_names is None:
            return [self._start_task(download_manager, t) \
                for t in available_tasks]
        
        tasks = []
        
        for task in task_names:
            try:
                task_class = next(t for t in available_tasks \
                    if task == t.__name__)
            except StopIteration:
                sys.exit('Unknown task: ' + task)
            else:
                tasks.append(self._start_task(download_manager, task_class))
        
        return tasks
