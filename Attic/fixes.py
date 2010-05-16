# -*- coding: utf-8 -*-


import ConfigParser, cStringIO, email.feedparser, email.parser, imaplib, sys


# Fixes the regular expression that checks for invalid percent interpolations.
# <http://bugs.python.org/issue5741>
class _CheckBadPercent:
    class Result:
        def __init__(self, index):
            self.__index = index
        
        
        def start(self, *args, **kargs):
            return self.__index
    
    
    def __init__(self, interpolate_var):
        self.__interpolate_var = interpolate_var
    
    
    def search(self, value, *args, **kargs):
        index = self.__interpolate_var.sub('', value).find('%')
        return False if index < 0 else _CheckBadPercent.Result(index)


# Fixes the regular expression that removes double percent signs.
# <http://bugs.python.org/issue5741>
class _RemoveDoublePercents (object):
    def __init__(self, interpolate_var):
        self.__interpolate_var = interpolate_var
    
    
    def match(self, *args, **kargs):
        return self.__interpolate_var.match(*args, **kargs)
    
    
    def sub(self, replacement, value, *args, **kargs):
        return value.replace('%%', '')


# Optimized version - decreases memory usage by half and speeds up parsing.
def _parsestr(self, text, headersonly = False):
    feed_parser = email.feedparser.FeedParser(self._class)
    
    if headersonly:
        feed_parser._set_headersonly()
    
    feed_parser.feed(text)
    return feed_parser.close()


# Fixes the memory error that sometimes occurs when downloading a large e-mail
# message and adds a check for the SSL socket read function return value.
class _ImapSsl (imaplib.IMAP4_SSL):
    def read(self, size):
        buffer = cStringIO.StringIO()
        read = 0
        
        while read < size:
            data = self.ssl().read(min(size - read, 16384))
            read += len(data)
            
            if data == '':
                break
            else:
                buffer.write(data)
        
        try:
            return buffer.getvalue()
        finally:
            buffer.close()
    
    
    def readline(self):
        buffer = cStringIO.StringIO()
        char = '\0'
        
        while (char != '\n') and (char != ''):
            char = self.ssl().read(1)
            buffer.write(char)
        
        try:
            return buffer.getvalue()
        finally:
            buffer.close()


_fixes = [
    # (module, name, target Python version, object)
    (ConfigParser.SafeConfigParser, '_badpercent_re', 0x20602F0, _CheckBadPercent(ConfigParser.SafeConfigParser._interpvar_re)),
    (ConfigParser.SafeConfigParser, '_interpvar_re', 0x20602F0, _RemoveDoublePercents(ConfigParser.SafeConfigParser._interpvar_re)),
    (email.parser.Parser, 'parsestr', 0x20604F0, _parsestr),
    (imaplib, 'IMAP4_SSL', 0x20604F0, _ImapSsl),
]


for _module, _name, _version, _object in _fixes:
    if sys.hexversion <= _version:
        _module.__dict__[_name] = _object
