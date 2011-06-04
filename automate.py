#!/usr/bin/env python
# -*- coding: utf-8 -*-


# TODO: Create source TV shows with automatic backup.
# TODO: Use command line options to choose the download manager, execute
#       download finished events (both automatically and manually via command
#       line), etc.
# TODO: Query download sources in parallel.
# TODO: Refactor into separate modules.
# TODO: Cut the first few seconds of the IGN Daily Fix videos.
# TODO: Create MS Win32 system service?
# TODO: Add documentation.
# TODO: Profile time execution.


# Standard library:
from __future__ import division, print_function, unicode_literals
import codecs, datetime, locale, logging, os.path, Queue, re, sys, threading, \
    time, Tkinter, urllib, urllib2, urlparse

# Internal modules:
from defaults import *


externals('feedparser', 'lxml.html', 'PIL.Image', 'unipath')


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


class Logger (object):
    def __init__(self):
        stream = codecs.getwriter(locale.getpreferredencoding())(sys.stderr)
        
        handler = logging.StreamHandler(stream)
        handler.setFormatter(logging.Formatter(
            '[%(asctime)s] [%(levelname)s] %(message)s'))
        
        self._logger = logging.getLogger(self.__class__.__name__)
        self._logger.addHandler(handler)
        self._logger.setLevel(logging.INFO)
    
    
    @property
    def logger(self):
        return self._logger


class DownloadManager (object):
    __metaclass__ = ABCMeta
    
    
    @abstractmethod
    def download_url(self, url):
        pass
    
    
    @abstractmethod
    def has_url(self, url):
        pass


class ThreadSafeDownloadManager (DownloadManager, threading.Thread):
    def __init__(self, create_instance):
        DownloadManager.__init__(self)
        threading.Thread.__init__(self)
        
        self.daemon = True
        self._call_error = None
        self._call_input = Queue.Queue(maxsize = 1)
        self._call_output = Queue.Queue(maxsize = 1)
        self._in_call = threading.Lock()
        self._create_instance = create_instance
    
    
    def download_url(self, url):
        self._thread_call(url)
    
    
    def has_url(self, url):
        return self._thread_call(url)
    
    
    def run(self):
        download_manager = self._create_instance()
        
        while True:
            (method_name, args) = self._call_input.get()
            method = getattr(download_manager, method_name)
            
            try:
                result = method(*args)
                self._call_error = False
            except Exception as error:
                self._call_error = True
                result = sys.exc_info()
            
            self._call_output.put(result)
    
    
    def _thread_call(self, *args):
        if not self.is_alive():
            raise RuntimeError('Thread safe download manager not running.')
        
        self._in_call.acquire()
        self._call_input.put([sys._getframe(1).f_code.co_name, args])
        
        result = self._call_output.get()
        error = self._call_error
        self._in_call.release()
        
        if error:
            # Keep proper traceback across threads.
            raise result[0], result[1], result[2]
        
        return result


class FreeDownloadManager (DownloadManager, MsWindowsTypeLibrary, Logger):
    _CACHE_REFRESH_FREQUENCY = datetime.timedelta(hours = 1)
    _FILE_NAME_DOWNLOAD_TEXT = 0
    
    
    def __init__(self):
        DownloadManager.__init__(self)
        MsWindowsTypeLibrary.__init__(self, 'fdm.tlb')
        Logger.__init__(self)
        
        self._cached_downloads_stat = False
        self._last_cache_completed = datetime.datetime.min
        self._urls = set()
        self._urls_by_file_name = {}
    
    
    def download_url(self, url):
        wg_url_receiver = self.get_data_type('WGUrlReceiver')
        
        wg_url_receiver.Url = unicode(url)
        wg_url_receiver.DisableURLExistsCheck = False
        wg_url_receiver.ForceDownloadAutoStart = True
        wg_url_receiver.ForceSilent = True
        wg_url_receiver.ForceSilentEx = True
        
        if url.save_as is not None:
            wg_url_receiver.FileName = url.save_as
        
        if url.comment is not None:
            wg_url_receiver.Comment = url.comment
        
        wg_url_receiver.AddDownload()
        self._urls.add(url)
        self._urls.add(url.resolve())
        
        self.logger.debug('Download: %s', url)
    
    
    def has_url(self, url):
        resolved_url = url.resolve()
        
        if resolved_url in self._list_urls():
            return True
        
        if resolved_url != url:
            self.logger.debug('Redirect: %s', resolved_url)
        
        self.logger.debug('Download not found: %s', url)
        return False
    
    
    def get_urls_by_file_name(self, name):
        # Cache URL's.
        list(self._list_urls())
        
        return self._urls_by_file_name.get(name, [])
    
    
    def _list_urls(self):
        if self._cached_downloads_stat:
            elapsed = datetime.datetime.now() - self._last_cache_completed
            urls = self._urls
            
            if elapsed >= self._CACHE_REFRESH_FREQUENCY:
                self._cached_downloads_stat = False
                self._urls = set()
                self._urls_by_file_name = {}
            
            for url in urls:
                yield url
        else:
            downloads_stat = self.get_data_type('FDMDownloadsStat')
            downloads_stat.BuildListOfDownloads(True, True)
            
            # Don't start at the oldest URL to find newer downloads faster.
            for i in reversed(xrange(0, downloads_stat.DownloadCount)):
                download = downloads_stat.Download(i)
                file_name = download.DownloadText(self._FILE_NAME_DOWNLOAD_TEXT)
                url = Url(download.Url)
                
                self._urls.add(url)
                self._urls_by_file_name.setdefault(file_name, set())
                self._urls_by_file_name[file_name].add(url)
                
                yield url
            
            self._cached_downloads_stat = True
            self._last_cache_completed = datetime.datetime.now()


class DownloadSource (object):
    __metaclass__ = ABCMeta
    
    
    def download_finished(self, url, file_path):
        pass
    
    
    @abstractmethod
    def list_urls(self):
        pass
    
    
    @abstractproperty
    def name(self):
        pass


class Feed (object):
    def __init__(self, url):
        self._url = url
    
    
    def get_feed(self):
        return feedparser.parse(self._url)


class IgnDailyFix (DownloadSource, Feed):
    _TITLE = 'IGN Daily Fix'
    
    
    def __init__(self):
        DownloadSource.__init__(self)
        Feed.__init__(self, 'http://feeds.ign.com/ignfeeds/podcasts/games/')
    
    
    def list_urls(self):
        for entry in self.get_feed().entries:
            if entry.title.startswith(self._TITLE + ':'):
                url = Url(entry.enclosures[0].href)
                url.comment = entry.link
                
                yield url
    
    
    @property
    def name(self):
        return self._TITLE


class HdTrailers (DownloadSource, Feed, Logger):
    @classmethod
    def _find_highest_resolution(cls, strings):
        strings.sort(
            lambda x, y: cmp(cls._get_resolution(x), cls._get_resolution(y)),
            reverse = True)
        
        return strings[0]
    
    
    @classmethod
    def _get_resolution(cls, text):
        resolution = re.findall(r'(\d{3,4})p', text)
        
        if len(resolution) == 0:
            resolution = re.findall(r'(480|720|1080)', text)
            
            if len(resolution) == 0:
                return 0
        
        return int(resolution[0])
    
    
    def __init__(self, skip_documentaries = True, skip_foreign = True):
        DownloadSource.__init__(self)
        Feed.__init__(self, 'http://feeds.hd-trailers.net/hd-trailers/blog')
        Logger.__init__(self)
        
        self._skip_documentaries = skip_documentaries
        self._skip_foreign = skip_foreign
    
    
    def list_urls(self):
        keywords_re = r'\b(%s)\b' % '|'.join(['teaser', 'trailer'])
        
        for entry in self.get_feed().entries:
            if not re.search(keywords_re, entry.title, re.IGNORECASE):
                continue
            
            url = self._find_best_url(entry)
            
            if url is None:
                continue
            
            url.comment = entry.link
            
            if url.host_name != 'playlist.yahoo.com':
                yield url
                continue
            
            try:
                file = url.resolve().path.name
            except urllib2.URLError as (error,):
                self.logger.error('%s: %s', str(error), url)
                continue
            
            if re.search(r'^\d+$', file.stem):
                title = Url(entry.feedburner_origlink).path.components[-1]
                file = '%s (%s)%s' % (title, file.stem, file.ext)
                self.logger.debug('File name rewrite: %s', file)
            else:
                file = None
            
            url = PathUrl(url)
            url.save_as = file
            
            yield url
    
    
    @property
    def name(self):
        return 'HD Trailers'
    
    
    def _find_best_url(self, entry):
        if self._skip_documentaries:
            genre = entry.tags[0].term
            
            if genre == 'Documentary':
                self.logger.warning('Skip documentary: %s', entry.title)
                return
        
        if self._skip_foreign:
            genre = entry.tags[1].term
            
            if genre == 'Foreign':
                self.logger.warning('Skip foreign movie: %s', entry.title)
                return
        
        if hasattr(entry, 'enclosures'):
            return Url(self._find_highest_resolution(
                [enclosure.href for enclosure in entry.enclosures]))
        else:
            # Parse HTML to find movie links.
            html = lxml.html.fromstring(entry.content[0].value)
            (url, highest_resolution) = (None, 0)
            
            for link in html.xpath('//a[text() != ""]'):
                resolution = self._get_resolution(link.text)
                
                if resolution > highest_resolution:
                    url = link.attrib['href']
                    highest_resolution = resolution
            
            return Url(url)


class InterfaceLift (DownloadSource, Feed, Logger):
    _HOST_NAME = 'interfacelift.com'
    
    
    def __init__(self):
        DownloadSource.__init__(self)
        Feed.__init__(self,
            'http://' + self._HOST_NAME + '/wallpaper/rss/index.xml')
        Logger.__init__(self)
        
        tk = Tkinter.Tk()
        self._screen_ratio = tk.winfo_screenwidth() / tk.winfo_screenheight()
    
    
    def download_finished(self, url, file_path):
        if url.host_name == self._HOST_NAME:
            image = PIL.Image.open(file_path)
            image.save(file_path, quality = 85)
    
    
    def list_urls(self):
        try:
            session_code = self._session_code
        except urllib2.URLError as (error,):
            self.logger.error('%s: %s\'s session code', str(error), self.name)
            return
        
        for entry in self.get_feed().entries:
            html = lxml.html.fromstring(entry.summary)
            url = FileUrl(html.xpath('//img/@src')[0])
            (path, ext) = url.path.split_ext()
            
            url.comment = entry.link
            url.path = re.sub(
                '(?<=/)previews(?=/)',
                session_code,
                path + '_' + self._find_best_resolution(html) + ext)
            
            yield url
    
    
    @property
    def name(self):
        return 'InterfaceLIFT'
    
    
    def _find_best_resolution(self, entry_html):
        resolutions = re.findall(r'\d+x\d+',
            entry_html.xpath('//p[b/text() = "Resolutions:"]/text()')[0])
        
        # Should be already sorted in descending order.
        for resolution in resolutions:
            (width, height) = map(int, resolution.split('x'))
            
            if self._screen_ratio == (width / height):
                return resolution
    
    
    @property
    def _session_code(self):
        script = Url('http://' + self._HOST_NAME + '/inc_NEW/jscript.js')
        return re.findall('"/wallpaper/([^/]+)/"', script.open().read())[0]


class GameTrailersVideos (Logger):
    BASE_URL = 'http://www.gametrailers.com'
    
    
    def __init__(self, skip_indies = False):
        Logger.__init__(self)
        self._skip_indies = skip_indies
    
    
    def get_video_url(self, page_url):
        page_html = page_url.open().read()
        video_id = re.findall(r'mov_game_id\s*=\s*(\d+)', page_html)
        
        if len(video_id) == 0:
            # Not all videos are available for download.
            self.logger.error('Movie ID not found: %s', page_url)
            return
        
        page = lxml.html.fromstring(page_html)
        
        if self._skip_indies:
            [publisher] = page.xpath('//*[@class = "publisher"]/text()')
            
            if publisher.strip() == 'N/A':
                self.logger.warning('Skip indie game: %s', page_url)
                return
        
        video_url = Url(page.xpath('//*[@class = "Downloads"]' \
            + '/a[starts-with(text(), "Quicktime")]/@href')[0])
        
        url = Url('http://trailers-ak.gametrailers.com/gt_vault/%s/%s' \
            % (video_id[0], video_url.path.components[-1]))
        url.comment = page_url
        
        return url


class ScrewAttack (DownloadSource, GameTrailersVideos):
    def list_urls(self):
        main_html = lxml.html.fromstring(
            Url(self.BASE_URL + '/screwattack').open().read())
        videos = main_html.xpath(
            '//*[@id = "nerd"]//a[@class = "gamepage_content_row_title"]/@href')
        
        for page_url in [Url(self.BASE_URL + path) for path in videos]:
            yield self.get_video_url(page_url)
    
    
    @property
    def name(self):
        return 'ScrewAttack'


class GameTrailers (DownloadSource, GameTrailersVideos, Feed):
    def __init__(self):
        options = {
            'limit': 50,
            'orderby': 'newest',
            'quality[hd]': 'on',
        }
        
        for system in ['pc', 'ps3', 'xb360']:
            options['favplats[%s]' % system] = system
        
        url = Url(self.BASE_URL + '/rssgenerate.php')
        url.query = options
        
        DownloadSource.__init__(self)
        GameTrailersVideos.__init__(self, skip_indies = True)
        Feed.__init__(self, unicode(url))
    
    
    def list_urls(self):
        keywords_re = r'\b(%s)\b' % '|'.join(
            ['gameplay', 'preview', 'review', 'teaser', 'trailer'])
        
        for entry in self.get_feed().entries:
            if re.search(keywords_re, entry.title, re.IGNORECASE):
                try:
                    url = self.get_video_url(Url(entry.link))
                except urllib2.URLError as (error,):
                    self.logger.error('%s: %s', str(error), entry.link)
                    continue
                
                if url is not None:
                    url.comment = entry.link
                    yield url
    
    
    @property
    def name(self):
        return 'GameTrailers'


class GameTrailersVideosNewest (DownloadSource, GameTrailersVideos):
    def __init__(self, game):
        DownloadSource.__init__(self)
        GameTrailersVideos.__init__(self)
        
        self._game = game
    
    
    def list_urls(self):
        main_html = lxml.html.fromstring(
            Url(self.BASE_URL + '/game/' + self._game).open().read())
        
        videos = main_html.xpath(
            '//*[@id = "GamepageMedialistFeatures"]' \
            + '//*[@class = "newestlist_movie_format_SDHD"]/a[1]/@href')
        
        for page_url in [Url(self.BASE_URL + path) for path in videos]:
            yield self.get_video_url(page_url)    


class PopFiction (GameTrailersVideosNewest):
    def __init__(self):
        GameTrailersVideosNewest.__init__(self, 'pop-fiction/13123')
    
    
    @property
    def name(self):
        return 'Pop-Fiction'


class GtCountdown (GameTrailersVideosNewest):
    def __init__(self):
        GameTrailersVideosNewest.__init__(self, 'gt-countdown/2111')
    
    
    @property
    def name(self):
        return 'GT Countdown'


class PeriodicTask (threading.Thread, Logger):
    def __init__(self):
        threading.Thread.__init__(self)
        Logger.__init__(self)
        
        self.daemon = True
    
    
    @abstractproperty
    def name(self):
        pass
    
    
    @abstractmethod
    def process(self):
        pass
    
    
    def run(self):
        while True:
            self.logger.debug('Task start: %s', self.name)
            self.process()
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
        
        for (root, dirs, files) in os.walk(Path.documents()):
            for file in filter(lambda f: re.search(log_file, f), files):
                path = Path(root, file)
                self.logger.warning('Remove backup data log: %s', path)
                
                try:
                    path.remove()
                except OSError as (code, message):
                    self.logger.debug('%s: %s', message, path)
    
    
    def _clean_webkit_folder(self):
        webkit = Path.documents().child('webkit')
        
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
        
        for (root, dirs, files) in os.walk(Path.documents()):
            for file in filter(lambda f: re.search(bookmarks, f), files):
                path = Path(root, file)
                
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
        cache = Path.documents().child('.dropbox.cache')
        
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
        config_file = Path.documents().child('desktop.ini')
        
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


dl_manager = ThreadSafeDownloadManager(FreeDownloadManager)
dl_manager.start()

sources = [
    GameTrailers(),
    GtCountdown(),
    HdTrailers(),
    IgnDailyFix(),
    InterfaceLift(),
    PopFiction(),
    ScrewAttack(),
]

for task in [Dropbox(), GnuCash(), Opera(), Windows()]:
    task.start()

while True:
    for source in sources:
        print('Source check: %s' % source.name)
        
        for url in source.list_urls():
            try:
                if not dl_manager.has_url(url):
                    dl_manager.download_url(url)
            except urllib2.URLError as (error,):
                dl_manager.logger.error('%s: %s', str(error), url)
    
    print('Paused')
    time.sleep(10 * 60)
