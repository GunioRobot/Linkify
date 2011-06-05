# -*- coding: utf-8 -*-


# Standard library:
from __future__ import division, print_function, unicode_literals
import codecs, locale, logging, os.path, sys, urllib, urllib2, urlparse

# Internal modules:
from defaults import *


externals('colorconsole.terminal', 'feedparser', 'unipath')


class Path (unipath.Path):
    @staticmethod
    def documents():
        from win32com.shell import shell, shellcon
        return Path(shell.SHGetFolderPath(0, shellcon.CSIDL_PERSONAL, 0, 0))
    
    
    @property
    def components(self):
        return super(Path, self).components()
    
    
    def split_ext(self):
        return os.path.splitext(self)


class ColorStreamHandler (logging.StreamHandler):
    (BLACK, BLUE, GREEN, CYAN, RED, PURPLE, BROWN, LGREY) = range(8)
    (DGRAY, LBLUE, LGREEN, LCYAN, LRED, LPURPLE, YELLOW, WHITE) = range(8, 16)
    
    
    def __init__(self):
        logging.StreamHandler.__init__(self,
            codecs.getwriter(locale.getpreferredencoding())(sys.stderr))
        
        self.setFormatter(logging.Formatter(
            fmt = '[%(asctime)s] [%(levelname)s] [%(name)s] %(message)s',
            datefmt = '%Y-%m-%d %H:%M:%S'))
        
        if sys.stderr.isatty():
            self._terminal = colorconsole.terminal.get_terminal()
        else:
            self._terminal = None
    
    
    def emit(self, record):
        if self._terminal is not None:
            if record.levelno == logging.INFO:
                self._terminal.set_color(self.BLUE)
            elif record.levelno == logging.WARNING:
                self._terminal.set_color(self.BROWN)
            elif record.levelno == logging.ERROR:
                self._terminal.set_color(self.RED)
        
        logging.StreamHandler.emit(self, record)
        
        if self._terminal is not None: 
            self._terminal.reset()


class Logger (object):
    _HANDLER = ColorStreamHandler()
    
    
    def __init__(self):
        self._logger = logging.getLogger(self.__class__.__name__)
        self._logger.addHandler(self._HANDLER)
        self._logger.setLevel(logging.INFO)
    
    
    @property
    def logger(self):
        return self._logger


class Url (object):
    def __init__(self, url):
        if isinstance(url, Url):
            self._components = url._components
            
            self.comment = url.comment
            self.save_as = url.save_as
        else:
            self._components = urlparse.urlparse(url)
            
            self.comment = None
            self.save_as = None
    
    
    @property
    def host_name(self):
        return self._components.hostname
    
    
    def open(self):
        request = urllib2.Request(unicode(self))
        
        if self.host_name == 'trailers.apple.com':
            request.add_header('User-Agent', 'QuickTime')
        
        return urllib2.build_opener().open(request)
    
    
    @property
    def path(self):
        return Path(self._components.path)
    
    
    @path.setter
    def path(self, path):
        components = self._components._asdict()
        components['path'] = path
        
        self._components = urlparse.ParseResult(**components)
    
    
    @property
    def query(self):
        return urlparse.parse_qs(self._components.query)
    
    
    @query.setter
    def query(self, query):
        components = self._components._asdict()
        components['query'] = urllib.unquote(urllib.urlencode(query))
        
        self._components = urlparse.ParseResult(**components)
    
    
    def resolve(self):
        connection = self.open()
        url = type(self)(connection.geturl())
        
        connection.close()
        return url
    
    
    def __cmp__(self, url):
        return cmp(unicode(self), unicode(url))
    
    
    def __hash__(self):
        return hash(unicode(self))
    
    
    def __str__(self):
        return unicode(self).encode('UTF-8')
    
    
    def __unicode__(self):
        return urlparse.urlunparse(self._components)


class PathUrl (Url):
    def __cmp__(self, url):
        return cmp(self.path, url.path)
    
    
    def __hash__(self):
        return hash(self.path)


class FileUrl (PathUrl):
    def __cmp__(self, url):
        return cmp(self.path.name, url.path.name)
    
    
    def __hash__(self):
        return hash(self.path.name)


class MsWindowsTypeLibrary (object):
    def __init__(self, path):
        import pythoncom, win32com.client
        global pythoncom, win32com
        
        self._iid_by_type_name = {}
        self._lib = pythoncom.LoadTypeLib(path)
        self._path = path
    
    
    def get_data_type(self, type_name):
        if type_name in self._iid_by_type_name:
            return win32com.client.Dispatch(self._iid_by_type_name[type_name])
        
        for i in xrange(0, self._lib.GetTypeInfoCount()):
            (name, doc, help_ctxt, help_file) = self._lib.GetDocumentation(i)
            
            if type_name == name:
                iid = self._lib.GetTypeInfo(i).GetTypeAttr().iid
                self._iid_by_type_name[type_name] = iid
                return win32com.client.Dispatch(iid)
        
        raise Exception('Type "%s" not found in type library "%s".'
            % (name, self._path))


class Feed (object):
    def __init__(self, url):
        self._url = url
    
    
    def get_feed(self):
        return feedparser.parse(self._url)
