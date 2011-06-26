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
import automate.backup, automate.cleanup, automate.download, automate.util


externals('argparse')


class TaskClassType (object):
    @classmethod
    def _list_concrete_classes(cls, base_class, module = None):
        def is_concrete_class(object):
            return inspect.isclass(object) \
                and not inspect.isabstract(object) \
                and issubclass(object, base_class)
        
        return inspect.getmembers(
            object = module or inspect.getmodule(base_class),
            predicate= is_concrete_class)
    
    
    def __init__(self):
        self._tasks = dict(
            self._list_concrete_classes(automate.backup.BackupTask) \
            + self._list_concrete_classes(automate.download.DownloadSource) \
            + self._list_concrete_classes(automate.util.PeriodicTask,
                automate.cleanup))
    
    
    def __call__(self, task_name):
        task_class = self._tasks.get(task_name)
        
        if task_class is None:
            raise argparse.ArgumentTypeError('Unknown task: ' + task_name)
        
        return task_class
    
    
    @property
    def task_classes(self):
        return self._tasks.values()
    
    
    @property
    def task_names(self):
        names = self._tasks.keys()
        names.sort()
        
        return names


class ArgumentsParser (argparse.ArgumentParser):
    def __init__(self):
        self._task_class_type = TaskClassType()
        
        argparse.ArgumentParser.__init__(self,
            description = 'Task automation.',
            epilog = 'Available tasks: '
                + ', '.join(self._task_class_type.task_names))
        
        arguments = [
            ('--start', {
                b'action': 'append',
                b'dest': 'task',
                b'help': 'start task automation process',
                b'nargs': '?',
                b'type': self._task_class_type,
            }),
            ('--download', {
                b'action': 'store',
                b'dest': 'url',
                b'help': 'download an URL using the default manager',
                b'type': automate.util.Url,
            }),
            ('--log', {
                b'action': 'store',
                b'default': 'INFO',
                b'dest': 'level',
                b'help': 'set the default logging level',
                b'type': lambda level: logging._levelNames[level.upper()],
            }),
        ]
        
        for name, options in arguments:
            self.add_argument(name, **options)


class Automate (ArgumentsParser):
    def execute(self):
        nothing_done = True
        arguments = self.parse_args()
        automate.util.Logger.DEFAULT_LEVEL = arguments.level
        download_manager = automate.download.FreeDownloadManager()
        
        if arguments.url:
            nothing_done = False
            download_manager.download_url(arguments.url)
        
        if arguments.task:
            task_classes = set(arguments.task)
            
            if None in task_classes:
                task_classes = self._task_class_type.task_classes
            
            if len(task_classes) > 0:
                nothing_done = False
                self._start_tasks(download_manager, task_classes)
        
        if nothing_done:
            self.print_help()
    
    
    def _start_tasks(self, download_manager, task_classes):
        tasks = []
        
        for task_class in task_classes:
            task = task_class()
            
            if isinstance(task, automate.download.DownloadSource):
                task = automate.download.Downloader(download_manager, task)
            
            task.start()
            tasks.append(task)
        
        try:
            while any([task.is_alive() for task in tasks]):
                time.sleep(1)
        except KeyboardInterrupt:
            for task in tasks:
                task.stop()
