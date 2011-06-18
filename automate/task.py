# -*- coding: utf-8 -*-


# Standard library:
from __future__ import division, print_function, unicode_literals
import os, re, threading, time

# External modules:
from defaults import *

# Internal modules:
import automate.util


class PeriodicTask (threading.Thread, automate.util.Logger):
    def __init__(self):
        threading.Thread.__init__(self, name = self.name)
        automate.util.Logger.__init__(self, self.name)
        
        self._is_stopping = threading.Event()
    
    
    @property
    def is_stopping(self):
        return self._is_stopping.is_set()
    
    
    @abstractproperty
    def name(self):
        pass
    
    
    @abstractmethod
    def process(self):
        pass
    
    
    def run(self):
        self.logger.info('Start')
        
        while not self.is_stopping:
            self.logger.debug('Resume')
            self.process()
            
            self.logger.debug('Pause')
            self._is_stopping.wait(15 * 60)
        
        self.logger.info('Stop')
    
    
    def stop(self):
        self._is_stopping.set()


class Dropbox (PeriodicTask):
    @property
    def name(self):
        return 'Dropbox'
    
    
    def process(self):
        cache = automate.util.Path.documents().child('.dropbox.cache')
        
        if cache.exists():
            self.logger.debug('Remove cache folder: %s', cache)
            
            try:
                cache.rmtree()
            except OSError as error:
                self.logger.debug('%s: %s', error, cache)


class GnuCash (PeriodicTask):
    @property
    def name(self):
        return 'GnuCash'
    
    
    def process(self):
        try:
            self._remove_log_files()
        except OSError as error:
            self.logger.error(error)
        
        try:
            self._remove_webkit_folder()
        except OSError as error:
            self.logger.error(error)
    
    
    # See <http://wiki.gnucash.org/wiki/FAQ> for details.
    def _remove_log_files(self):
        log_files = automate.util.Path.documents().walk(filter =
            lambda path: re.search(r'\.gnucash\.\d{14}\.log$', path.name))
        
        for log_file in log_files:
            self.logger.debug('Remove backup data log: %s', log_file)
            
            try:
                log_file.remove()
            except OSError as error:
                self.logger.debug('%s: %s', error, log_file)
    
    
    def _remove_webkit_folder(self):
        webkit = automate.util.Path.documents().child('webkit')
        
        if webkit.exists():
            self.logger.debug('Remove folder: %s', webkit)
            
            try:
                webkit.rmtree()
            except OSError as error:
                self.logger.debug('%s: %s', error, webkit)


class Opera (PeriodicTask):
    @property
    def name(self):
        return 'Opera'
    
    
    def process(self):
        try:
            self._remove_bookmark_files()
        except OSError as error:
            self.logger.error(error)
    
    
    def _remove_bookmark_files(self):
        bookmark_header = 'Opera Hotlist version 2.0\n'
        bookmark_files = automate.util.Path.documents().walk(filter =
            lambda path: re.search(r'^opr[\dA-F]{3,4}\.tmp$', path.name))
        
        for bookmark_file in bookmark_files:
            with open(bookmark_file) as bookmark:
                if bookmark.readline() != bookmark_header:
                    continue
            
            self.logger.debug('Remove backup bookmark: %s', bookmark_file)
            
            try:
                bookmark_file.remove()
            except OSError as error:
                self.logger.debug('%s: %s', error, bookmark_file)


class Windows (PeriodicTask):
    @property
    def name(self):
        return 'Windows'
    
    
    def process(self):
        config_file = automate.util.Path.documents().child('desktop.ini')
        
        if not config_file.exists():
            return
        
        with open(config_file) as config:
            if config.readline() != '[.ShellClassInfo]\n':
                return
        
        self.logger.debug('Remove configuration file: %s', config_file)
        
        try:
            config_file.remove()
        except OSError as error:
            self.logger.debug('%s: %s', error, config_file)
