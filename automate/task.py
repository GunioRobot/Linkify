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
        threading.Thread.__init__(self)
        automate.util.Logger.__init__(self)
        
        self.daemon = True
    
    
    @abstractproperty
    def name(self):
        pass
    
    
    @abstractmethod
    def process(self):
        pass
    
    
    def run(self):
        self.logger.info('Start')
        
        while True:
            self.logger.debug('Resume')
            self.process()
            
            self.logger.debug('Pause')
            time.sleep(10 * 60)


class GnuCash (PeriodicTask):
    @property
    def name(self):
        return 'GnuCash'
    
    
    def process(self):
        self._clean_webkit_folder()
        self._clean_logs()
    
    
    def _clean_logs(self):
        # http://wiki.gnucash.org/wiki/FAQ
        log_file = r'\.gnucash\.\d{14}\.log$'
        
        for (root, dirs, files) in os.walk(automate.util.Path.documents()):
            for file in filter(lambda f: re.search(log_file, f), files):
                path = automate.util.Path(root, file)
                self.logger.warning('Remove backup data log: %s', path)
                
                try:
                    path.remove()
                except OSError as (code, message):
                    self.logger.debug('%s: %s', message, path)
    
    
    def _clean_webkit_folder(self):
        webkit = automate.util.Path.documents().child('webkit')
        
        if webkit.exists():
            self.logger.warning('Remove folder: %s', webkit)
            
            try:
                webkit.rmtree()
            except OSError as (code, message):
                self.logger.debug('%s: %s', message, webkit)


class Opera (PeriodicTask):
    @property
    def name(self):
        return 'Opera'
    
    
    def process(self):
        bookmark_header = 'Opera Hotlist version 2.0\n'
        bookmarks = r'^opr[\dA-F]{3,4}\.tmp$'
        
        for (root, dirs, files) in os.walk(automate.util.Path.documents()):
            for file in filter(lambda f: re.search(bookmarks, f), files):
                path = automate.util.Path(root, file)
                
                with open(path) as bookmark:
                    if bookmark.readline() != bookmark_header:
                        continue
                
                self.logger.warning('Remove backup bookmark: %s', path)
                
                try:
                    path.remove()
                except OSError as (code, message):
                    self.logger.debug('%s: %s', message, path)


class Dropbox (PeriodicTask):
    @property
    def name(self):
        return 'Dropbox'
    
    
    def process(self):
        cache = automate.util.Path.documents().child('.dropbox.cache')
        
        if cache.exists():
            self.logger.warning('Remove cache folder: %s', cache)
            
            try:
                cache.rmtree()
            except OSError as (code, message):
                self.logger.debug('%s: %s', message, cache)


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
        
        self.logger.warning('Remove configuration file: %s', config_file)
        
        try:
            config_file.remove()
        except OSError as (code, message):
            self.logger.debug('%s: %s', message, config_file)
