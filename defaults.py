#!/usr/bin/env python
# -*- coding: utf-8 -*-


__author__ = u'Marcio Faustino'
__doc__ = u'Sets some defaults and implements important fixes.'
__version__ = u'2011-05-15'


# TODO: Implement constants?
# TODO: Enable UTF-8 source text automatically?


# Standard library:
import ConfigParser, email.feedparser, email.parser, imaplib


def fix(object = None, version = None, name = None, call = False):
    if not hasattr(fix, u'symbols'):
        setattr(fix, u'symbols', set())
    
    
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
    symbol = inspect.getmodule(object).__name__.split(u'.', 1).pop(0)
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


@fix(ConfigParser.SafeConfigParser, 0x20602F0, u'_badpercent_re', call = True)
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
        index = self._interpvar_re.sub(u'', value).find(u'%')
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
            
            if data == u'':
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
        char = u'\0'
        
        while (char != u'\n') and (char != u''):
            char = self.ssl().read(1)
            data_buffer.write(char)
        
        try:
            return data_buffer.getvalue()
        finally:
            data_buffer.close()


@fix(ConfigParser.SafeConfigParser, 0x20602F0, u'_interpvar_re', call = True)
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
        return value.replace(u'%%', u'')


fix()

NaN = float('NaN')
Infinity = float('Infinity')
