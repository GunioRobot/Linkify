#!/usr/bin/env python
# -*- coding: utf-8 -*-


__author__ = u'Marcio Faustino'
__doc__ = u'Sets some defaults and important fixes.'
__version__ = u'2011-05-15'


# TODO: Implement constants?
# TODO: Export Infinity constant?
# TODO: Enable UTF-8 source text automatically?
# TODO: Automate namespace cleanup to allow? from defaults import *


# Standard library:
import ConfigParser, email.feedparser, email.parser, imaplib


def fix(object, name, version, call = False):
    def apply_fix(value):
        import sys
        
        if sys.hexversion <= version:
            setattr(object, name, value(object) if call else value)
        
        return value
    
    return apply_fix


@fix(email.parser.Parser, u'parsestr', 0x20604F0)
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


@fix(ConfigParser.SafeConfigParser, u'_badpercent_re', 0x20602F0, True)
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


@fix(imaplib, u'IMAP4_SSL', 0x20604F0)
class Imap4Ssl (imaplib.IMAP4_SSL):
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


@fix(ConfigParser.SafeConfigParser, u'_interpvar_re', 0x20602F0, True)
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


del ConfigParser, email, imaplib
del fix
del parsestr, CheckBadPercent, Imap4Ssl, RemoveDoublePercents
