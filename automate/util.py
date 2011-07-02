# -*- coding: utf-8 -*-


# Standard library:
from __future__ import division, print_function, unicode_literals
import codecs, httplib, json, locale, logging, os.path, re, sys, threading, \
    urllib, urllib2, urlparse

# Internal modules:
from defaults import *


externals('colorconsole.terminal', 'unipath')


class CachedSet (set):
    def __init__(self, generator):
        set.__init__(self)
        
        self._generator = generator
        self._iteration_lock = threading.Lock()
        self._last_position = None
    
    
    def __contains__(self, search_item):
        with self._iteration_lock:
            if set.__contains__(self, search_item):
                return True
            
            for (position, item) in self._generator(self._last_position):
                self.add(item)
                self._last_position = position
                
                if search_item == item:
                    return True
            
            return False
    
    
    def clear(self):
        set.clear(self)
        self._last_position = None


class ColorStreamHandler (logging.StreamHandler):
    (_BLACK, _BLUE, _GREEN, _CYAN, _RED, _PURPLE, _BROWN, _LGREY,
     _DGRAY, _LBLUE, _LGREEN, _LCYAN, _LRED, _LPURPLE, _YELLOW, _WHITE) \
        = range(16)
    
    
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
                self._terminal.set_color(self._BLUE)
            elif record.levelno == logging.WARNING:
                self._terminal.set_color(self._BROWN)
            elif record.levelno == logging.ERROR:
                self._terminal.set_color(self._RED)
        
        logging.StreamHandler.emit(self, record)
        
        if self._terminal is not None: 
            self._terminal.reset()


class Logger (object):
    DEFAULT_LEVEL = logging.INFO
    _HANDLER = ColorStreamHandler()
    
    
    def __init__(self, name = None):
        self._logger = logging.getLogger(
            name if name is not None else self.__class__.__name__)
        
        self._logger.addHandler(self._HANDLER)
        self._logger.setLevel(self.DEFAULT_LEVEL)
    
    
    @property
    def logger(self):
        return self._logger


class MsWindowsTypeLibrary (object):
    def __init__(self, path):
        import pythoncom, win32com.client
        global pythoncom, win32com
        
        self._iid_by_type_name = {}
        self._lib = pythoncom.LoadTypeLib(path)
        self._path = path
    
    
    def get_data_type(self, type_name):
        pythoncom.CoInitialize()
        
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


class PeriodicTask (threading.Thread, Logger):
    __metaclass__ = ABCMeta
    
    
    def __init__(self):
        threading.Thread.__init__(self, name = self.name)
        Logger.__init__(self, self.name)
        
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


class Path (unipath.Path):
    @staticmethod
    def documents():
        from win32com.shell import shellcon
        return Path._windows_path(shellcon.CSIDL_PERSONAL)
    
    
    @staticmethod
    def settings():
        from win32com.shell import shellcon
        return Path._windows_path(shellcon.CSIDL_APPDATA)
    
    
    @staticmethod
    def _windows_path(folder_id):
        from win32com.shell import shell
        return Path(shell.SHGetFolderPath(0, folder_id, 0, 0))
    
    
    @property
    def components(self):
        return super(Path, self).components()
    
    
    def split_ext(self):
        return os.path.splitext(self)


class Url (object):
    _class_by_host_name = {}
    _class_by_host_name_re = {}
    
    
    @classmethod
    def from_host_name(cls, url):
        url_class = cls._class_by_host_name.get(url.host_name)
        
        if url_class is None:
            for (host_name, url_class) in cls._class_by_host_name_re.items():
                if host_name.search(url.host_name):
                    return url_class(url)
            
            return url
        else:
            return url_class(url)
    
    
    @classmethod
    def register_host_name(cls, host_name):
        if isinstance(host_name, basestring):
            cls._class_by_host_name[host_name] = cls
        else:
            cls._class_by_host_name_re[host_name] = cls
    
    
    def __init__(self, url, comment = None, query = None, save_as = None):
        if isinstance(url, Url):
            self._components = url._components
            self.comment = comment or url.comment
            self.save_as = save_as or url.save_as
        else:
            self._components = urlparse.urlparse(url)
            self.comment = comment
            self.save_as = save_as
        
        if query is not None:
            self.query = query
    
    
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
        if isinstance(path, list):
            path = re.sub(r'^/+', '/', '/'.join(path))
        
        components = self._components._asdict()
        components['path'] = path
        
        self._components = urlparse.ParseResult(**components)
    
    
    @property
    def query(self):
        query = urlparse.parse_qs(self._components.query)
        
        for key, values in query.items():
            query[key] = [value.decode('UTF-8') for value in values]
        
        return query
    
    
    @query.setter
    def query(self, query):
        encoded_query = {}
        
        for key, value in query.items():
            if not isinstance(value, basestring):
                value = unicode(value)
            
            encoded_query[key] = value.encode('UTF-8')
        
        components = self._components._asdict()
        components['query'] = urllib.urlencode(encoded_query)
        
        self._components = urlparse.ParseResult(**components)
    
    
    def resolve(self):
        connection = self.open()
        
        url = type(self)(connection.geturl())
        url.comment = self.comment
        
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


class NoQueryUrl (Url):
    def __init__(self, *args, **kargs):
        Url.__init__(self, *args, **kargs)
        Url.query.fset(self, {})
    
    
    @property
    def query(self):
        return {}
    
    
    @query.setter
    def query(self, query):
        raise NotImplementedError()


class ImdbApi (Logger):
    def __init__(self):
        Logger.__init__(self)
        self._info_by_query = {}
    
    
    def query(self, term):
        info = self._info_by_query.get(term)
        
        if info is not None:
            return info
        
        url = Url('http://www.deanclatworthy.com/imdb/', query = {'q': term})
        self.logger.debug('Query: %s', url)
        
        try:
            info = json.loads(url.open().read())
        except (httplib.HTTPException, urllib2.URLError) as error:
            info = {'error': error}
        
        if 'error' in info:
            self.logger.debug('%s: %s', info['error'], url)
            return None
        else:
            self._info_by_query[term] = info
            return info
