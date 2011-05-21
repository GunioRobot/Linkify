#!/usr/bin/env python
# -*- coding: utf-8 -*-


# TODO: Check HTTP timeouts.
# TODO: Query download sources in parallel.
# TODO: Hide terminal while executing download finished events.
# TODO: Use command line options to choose the download manager, execute
#       download finished events (both automatically and manually via command
#       line), etc.
# TODO: Handle HTTP connection errors (off-line, not found, etc).
# TODO: Create MS Win32 system service?
# TODO: Cut the first few seconds of the IGN Daily Fix videos.
# TODO: Fix ScrewAttack 404 errors. Use default browser's cookies (e.g. Opera)?
# TODO: Create sources for GameTrailers videos (and Pop-Fiction, GT Countdown).
# TODO: Create source for TV shows and automatic backup of watched episodes.
# TODO: Refresh FDM's cached list of URL's every X seconds?


# Standard library:
from __future__ import division, print_function, unicode_literals
import logging, os.path, re, time, Tkinter, urllib2, urlparse

# Internal modules:
from defaults import *


externals('feedparser', 'lxml.html', 'PIL.Image', 'unipath')


class Path (unipath.Path):
    @property
    def components(self):
        return super(Path, self).components()
    
    
    def split_ext(self):
        return os.path.splitext(self)


class Url (object):
    def __init__(self, url):
        if isinstance(url, Url):
            self._components = url._components
        else:
            self._components = urlparse.urlparse(url)
    
    
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
        
        self._path = path
        self._lib = pythoncom.LoadTypeLib(self._path)
    
    
    def get_data_type(self, type_name):
        for i in xrange(0, self._lib.GetTypeInfoCount()):
            (name, doc, help_ctxt, help_file) = self._lib.GetDocumentation(i)
            
            if name == type_name:
                iid = self._lib.GetTypeInfo(i).GetTypeAttr().iid
                return win32com.client.Dispatch(iid)
        
        raise Exception('Type "%s" not found in type library "%s".'
            % (name, self._path))


class Logger (object):
    def __init__(self):
        handler = logging.StreamHandler()
        handler.setFormatter(logging.Formatter(
            '[%(levelname)s] [%(asctime)s] [%(name)s] %(message)s'))
        
        self._logger = logging.getLogger(self.__class__.__name__)
        self._logger.addHandler(handler)
        self._logger.setLevel(logging.DEBUG)
    
    
    @property
    def logger(self):
        return self._logger


class DownloadManager (Logger):
    __metaclass__ = ABCMeta
    
    
    @abstractmethod
    def download_url(self, url, to = None):
        pass
    
    
    @abstractmethod
    def has_url(self, url):
        pass


class FreeDownloadManager (DownloadManager, MsWindowsTypeLibrary):
    _FILE_NAME_DOWNLOAD_TEXT = 0
    
    
    def __init__(self):
        DownloadManager.__init__(self)
        MsWindowsTypeLibrary.__init__(self, 'fdm.tlb')
        
        self._cached_downloads_stat = False
        self._urls = set()
        self._urls_by_file_name = {}
    
    
    def download_url(self, url, to = None):
        wg_url_receiver = self.get_data_type('WGUrlReceiver')
        
        wg_url_receiver.Url = unicode(url)
        wg_url_receiver.DisableURLExistsCheck = False
        wg_url_receiver.ForceDownloadAutoStart = True
        wg_url_receiver.ForceSilent = True
        wg_url_receiver.ForceSilentEx = True
        
        if to is not None:
            wg_url_receiver.FileName = to
        
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
            for url in self._urls:
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


class Feed (DownloadSource):
    def __init__(self, url):
        self._url = url
    
    
    def get_feed(self):
        return feedparser.parse(self._url)


class IgnDailyFix (Feed):
    _TITLE = 'IGN Daily Fix'
    
    
    def __init__(self):
        super(IgnDailyFix, self).__init__(
            'http://feeds.ign.com/ignfeeds/podcasts/games/')
    
    
    def list_urls(self):
        for entry in self.get_feed().entries:
            if entry.title.startswith(self._TITLE + ':'):
                yield (Url(entry.enclosures[0].href), None)
    
    
    @property
    def name(self):
        return self._TITLE


class HdTrailers (Feed):
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
    
    
    def __init__(self):
        super(HdTrailers, self).__init__(
            'http://feeds.hd-trailers.net/hd-trailers/blog')
    
    
    def list_urls(self):
        for entry in self.get_feed().entries:
            if not re.search(r'\b(teaser|trailer)\b', entry.title, re.I):
                continue
            
            if hasattr(entry, 'enclosures'):
                url = Url(self._find_highest_resolution(
                    [enclosure.href for enclosure in entry.enclosures]))
            else:
                # Parse HTML to find movie links.
                html = lxml.html.fromstring(entry.content[0].value)
                (url, highest_resolution) = (None, 0)
                
                for link in html.xpath('//a[text() != ""]'):
                    resolution = self._get_resolution(link.text)
                    
                    if resolution > highest_resolution:
                        url = Url(link.attrib['href'])
                        highest_resolution = resolution
            
            if url.host_name == 'playlist.yahoo.com':
                file_name = '%s (%s).mov' \
                    % (Url(entry.feedburner_origlink).path.components[-1],
                        url.query['sid'][0])
                
                yield (PathUrl(url), file_name)
            else:
                yield (url, None)
    
    
    @property
    def name(self):
        return 'HD Trailers'


class InterfaceLift (Feed):
    _HOST_NAME = 'interfacelift.com'
    
    
    def __init__(self):
        super(InterfaceLift, self).__init__(
            'http://' + self._HOST_NAME + '/wallpaper/rss/index.xml')
        
        tk = Tkinter.Tk()
        self._screen_ratio = tk.winfo_screenwidth() / tk.winfo_screenheight()
    
    
    def download_finished(self, url, file_path):
        if url.host_name == self._HOST_NAME:
            image = PIL.Image.open(file_path)
            image.save(file_path, quality = 85)
    
    
    def list_urls(self):
        session_code = self._session_code
        
        for entry in self.get_feed().entries:
            html = lxml.html.fromstring(entry.summary)
            url = FileUrl(html.xpath('//img/@src')[0])
            (path, ext) = url.path.split_ext()
            
            url.path = re.sub(
                '(?<=/)previews(?=/)',
                session_code,
                path + '_' + self._find_best_resolution(html) + ext)
            
            yield (url, None)
    
    
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


class ScrewAttack (DownloadSource):
    _BASE_URL = 'http://www.gametrailers.com'
    _QUICKTIME_VIDEO_HREF = '//span[@class="Downloads"]' \
        + '/a[starts-with(text(), "Quicktime")]/@href'
    
    
    def list_urls(self):
        main_url = Url(self._BASE_URL + '/screwattack')
        main_html = lxml.html.fromstring(main_url.open().read())
        
        videos = main_html.xpath(
            '//div[@id="nerd"]//a[@class="gamepage_content_row_title"]/@href')
        
        for page_url in [Url(self._BASE_URL + path) for path in videos]:
            video_html = lxml.html.fromstring(page_url.open().read())
            video_url = Url(video_html.xpath(self._QUICKTIME_VIDEO_HREF)[0])
            
            url = Url('http://trailers-ak.gametrailers.com/gt_vault/3000/' \
                + video_url.path.components[-1])
            
            yield (url, None)
    
    
    @property
    def name(self):
        return 'ScrewAttack'


dl_manager = FreeDownloadManager()

sources = [source() for source in [
    IgnDailyFix,
    InterfaceLift,
    HdTrailers,
    ScrewAttack,
]]

while True:
    dl_manager.logger.info('Starting...')
    
    for source in sources:
        dl_manager.logger.info('Checking source: %s', source.name)
        
        for (url, file_name) in source.list_urls():
            try:
                if not dl_manager.has_url(url):
                    dl_manager.download_url(url, to = file_name)
            except urllib2.HTTPError as error:
                dl_manager.logger.error('%s: %s', str(error), url)
    
    dl_manager.logger.info('Pausing...')
    time.sleep(10 * 60)
