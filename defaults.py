#!/usr/bin/env python
# -*- coding: utf-8 -*-


__author__ = u'Marcio Faustino'
__doc__ = u'Sets some defaults and important fixes.'
__version__ = u'2011-0515'


# TODO: Implement a @fix decorator?
# TODO: Implement constants?
# TODO: Export Infinity constant?
# TODO: Enable UTF-8 source text automatically?
# TODO: Automate namespace cleanup to allow: from defaults import *


# Standard library:
import ConfigParser, cStringIO, email.feedparser, email.parser, imaplib, sys


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
    
    
    def __init__(self, interpolate_var_re):
        self._interpolate_var_re = interpolate_var_re
    
    
    def search(self, value, *args, **kargs):
        index = self._interpolate_var_re.sub(u'', value).find(u'%')
        return False if index < 0 else CheckBadPercent.Result(index)


class Imap4Ssl (imaplib.IMAP4_SSL):
    '''
    Fixes memory errors that sometimes occur when downloading a large e-mail
    message and adds a check for the SSL socket read function return value.
    <http://bugs.python.org/issue1441530>
    '''
    
    
    def read(self, size):
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
        data_buffer = cStringIO.StringIO()
        char = u'\0'
        
        while (char != u'\n') and (char != u''):
            char = self.ssl().read(1)
            data_buffer.write(char)
        
        try:
            return data_buffer.getvalue()
        finally:
            data_buffer.close()


class RemoveDoublePercents (object):
    '''
    Fixes the regular expression that removes double percent signs.
    <http://bugs.python.org/issue5741>
    '''
    
    
    def __init__(self, interpolate_var_re):
        self._interpolate_var_re = interpolate_var_re
    
    
    def match(self, *args, **kargs):
        return self._interpolate_var_re.match(*args, **kargs)
    
    
    def sub(self, replacement, value, *args, **kargs):
        return value.replace(u'%%', u'')


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


fixes = [
    (ConfigParser.SafeConfigParser, u'_badpercent_re', 0x20602F0, CheckBadPercent(ConfigParser.SafeConfigParser._interpvar_re)),
    (ConfigParser.SafeConfigParser, u'_interpvar_re', 0x20602F0, RemoveDoublePercents(ConfigParser.SafeConfigParser._interpvar_re)),
    (email.parser.Parser, u'parsestr', 0x20604F0, parsestr),
    (imaplib, u'IMAP4_SSL', 0x20604F0, Imap4Ssl),
]

for module, name, version, fix in fixes:
    if sys.hexversion <= version:
        setattr(module, name, fix)

del ConfigParser, cStringIO, email, imaplib, sys
del CheckBadPercent, Imap4Ssl, RemoveDoublePercents, parsestr
del fixes, module, name, version, fix
