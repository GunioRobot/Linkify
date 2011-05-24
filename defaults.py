#!/usr/bin/env python
# -*- coding: utf-8 -*-


# TODO: Set default source code file encoding to UTF-8.
# TODO: Import default __future__ features.
# TODO: Use final fix for issue 1441530.


# Standard library:
from __future__ import division, print_function, unicode_literals
import ConfigParser, email.feedparser, email.parser, imaplib
from abc import *


__author__ = 'Marcio Faustino'
__doc__ = 'Sets some defaults and implements important fixes.'
__version__ = '2011-05-17'


def externals(*modules):
    import __main__
    missing = set()
    
    for module in modules:
        package = module.split('.', 1).pop(0)
        
        if package != module:
            try:
                setattr(__main__, package, __import__(package))
            except ImportError:
                missing.add(package)
                continue
        
        try:
            setattr(__main__, module, __import__(module))
        except ImportError:
            missing.add(module)
    
    if len(missing) > 0:
        import sys
        sys.exit('Modules not found: ' + ', '.join(sorted(missing)))


def fix(object = None, version = None, name = None, call = False):
    if not hasattr(fix, 'symbols'):
        setattr(fix, 'symbols', set())
    
    
    # Namespace cleanup.
    if object is None and version is None:
        symbols = globals()
        
        for symbol in fix.symbols:
            del symbols[symbol]
        
        fix.symbols.clear()
        return
    
    
    def apply_fix(value):
        import sys
        
        if sys.hexversion <= version:
            setattr(object,
                value.__name__ if name is None else name,
                value(object) if call else value)
        
        fix.symbols.add(value.__name__)
        return value
    
    
    import inspect
    symbol = inspect.getmodule(object).__name__.split('.', 1).pop(0)
    fix.symbols.add(symbol)
    
    return apply_fix


@fix(email.parser.Parser, 0x20604F0)
def parsestr(self, text, headersonly = False):
    '''
    Optimized version - decreases memory usage and speeds up parsing.
    <http://bugs.python.org/issue8009>
    '''
    
    feed_parser = email.feedparser.FeedParser(self._class)
    
    if headersonly:
        feed_parser._set_headersonly()
    
    feed_parser.feed(text)
    return feed_parser.close()


@fix(ConfigParser.SafeConfigParser, 0x20602F0, '_badpercent_re', call = True)
class CheckBadPercent (object):
    '''
    Fixes the regular expression that checks for invalid percent interpolations.
    <http://bugs.python.org/issue5741>
    '''
    
    
    class Result:
        def __init__(self, index):
            self._index = index
        
        
        def start(self, *args, **kargs):
            return self._index
    
    
    def __init__(self, safe_config_parser_type):
        self._interpvar_re = safe_config_parser_type._interpvar_re
    
    
    def search(self, value, *args, **kargs):
        index = self._interpvar_re.sub('', value).find('%')
        return False if index < 0 else CheckBadPercent.Result(index)


@fix(imaplib, 0x20604F0)
class IMAP4_SSL (imaplib.IMAP4_SSL):
    '''
    Fixes memory errors that sometimes occur when downloading a large e-mail
    message and adds a check for the SSL socket read function return value.
    <http://bugs.python.org/issue1441530>
    '''
    
    
    def read(self, size):
        import cStringIO
        data_buffer = cStringIO.StringIO()
        size_read = 0
        
        while size_read < size:
            data = self.ssl().read(min(size - size_read, 2 ** 14))
            size_read += len(data)
            
            if data == '':
                break
            else:
                data_buffer.write(data)
        
        try:
            return data_buffer.getvalue()
        finally:
            data_buffer.close()
    
    
    def readline(self):
        import cStringIO
        data_buffer = cStringIO.StringIO()
        char = '\0'
        
        while (char != '\n') and (char != ''):
            char = self.ssl().read(1)
            data_buffer.write(char)
        
        try:
            return data_buffer.getvalue()
        finally:
            data_buffer.close()


@fix(ConfigParser.SafeConfigParser, 0x20602F0, '_interpvar_re', call = True)
class RemoveDoublePercents (object):
    '''
    Fixes the regular expression that removes double percent signs.
    <http://bugs.python.org/issue5741>
    '''
    
    
    def __init__(self, safe_config_parser_type):
        self._interpvar_re = safe_config_parser_type._interpvar_re
    
    
    def match(self, *args, **kargs):
        return self._interpvar_re.match(*args, **kargs)
    
    
    def sub(self, replacement, value, *args, **kargs):
        return value.replace('%%', '')


fix()

NaN = float('NaN')
Infinity = float('Infinity')
