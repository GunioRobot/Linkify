# -*- coding: utf-8 -*-


# Standard library:
from __future__ import division, print_function, unicode_literals
import datetime, re, sys, Tkinter, urllib2

# External modules:
from defaults import *

# Internal modules:
import automate.util


externals('feedparser', 'lxml.etree', 'lxml.html', 'PIL.Image')


class DownloadManager (object):
    __metaclass__ = ABCMeta
    
    
    @abstractmethod
    def download_url(self, url):
        pass
    
    
    @abstractmethod
    def has_url(self, url):
        pass


class FreeDownloadManager \
        (DownloadManager, automate.util.MsWindowsTypeLibrary, automate.util.Logger):
    
    _CACHE_REFRESH_FREQUENCY = datetime.timedelta(hours = 1)
    _FILE_NAME_DOWNLOAD_TEXT = 0
    
    
    def __init__(self):
        DownloadManager.__init__(self)
        automate.util.MsWindowsTypeLibrary.__init__(self, 'fdm.tlb')
        automate.util.Logger.__init__(self)
        
        self._cached_downloads_stat = False
        self._last_cache_reset = datetime.datetime.now()
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
        self.logger.debug('Check for download: %s', url)
        
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
        elapsed = datetime.datetime.now() - self._last_cache_reset
        urls = self._urls
        
        if elapsed >= self._CACHE_REFRESH_FREQUENCY:
            self.logger.info('Reset downloads list cache')
            
            self._cached_downloads_stat = False
            self._last_cache_reset = datetime.datetime.now()
            self._urls = set()
            self._urls_by_file_name = {}
        
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
                url = automate.util.Url(download.Url)
                
                self._urls.add(url)
                self._urls_by_file_name.setdefault(file_name, set())
                self._urls_by_file_name[file_name].add(url)
                
                yield url
            
            self._cached_downloads_stat = True
            self._last_cache_reset = datetime.datetime.now()


class DownloadSource (automate.util.Logger):
    __metaclass__ = ABCMeta
    
    
    def download_finished(self, url, file_path):
        pass
    
    
    @abstractmethod
    def list_urls(self):
        pass
    
    
    @abstractproperty
    def name(self):
        pass


class IgnDailyFix (DownloadSource):
    _TITLE = 'IGN Daily Fix'
    
    
    def list_urls(self):
        feed = feedparser.parse('http://feeds.ign.com/ignfeeds/podcasts/games/')
        
        for entry in feed.entries:
            if entry.title.startswith(self._TITLE + ':'):
                url = automate.util.Url(entry.enclosures[0].href)
                url.comment = entry.link
                
                yield url
    
    
    @property
    def name(self):
        return self._TITLE


class HdTrailers (DownloadSource):
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
        
        self._skip_documentaries = skip_documentaries
        self._skip_foreign = skip_foreign
        self._skipped_items = set()
    
    
    def list_urls(self):
        keywords_re = r'\b(%s)\b' % '|'.join(['teaser', 'trailer'])
        feed = feedparser.parse('http://feeds.hd-trailers.net/hd-trailers/blog')
        
        for entry in feed.entries:
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
            except urllib2.URLError as error:
                self.logger.error('%s: %s', str(error), url)
                continue
            
            if re.search(r'^\d+$', file.stem):
                title = automate.util.Url(entry.feedburner_origlink) \
                    .path.components[-1]
                
                file = '%s (%s)%s' % (title, file.stem, file.ext)
                self.logger.debug('File name rewrite: %s', file)
            else:
                file = None
            
            url = automate.util.PathUrl(url)
            url.save_as = file
            
            yield url
    
    
    @property
    def name(self):
        return 'HD Trailers'
    
    
    def _find_best_url(self, entry):
        if entry.title in self._skipped_items:
            return
        
        if self._skip_documentaries:
            genre = entry.tags[0].term
            
            if genre == 'Documentary':
                self.logger.warning('Skip documentary: %s', entry.title)
                self._skipped_items.add(entry.title)
                return
        
        if self._skip_foreign:
            genre = entry.tags[1].term
            
            if genre == 'Foreign':
                self.logger.warning('Skip foreign movie: %s', entry.title)
                self._skipped_items.add(entry.title)
                return
        
        if hasattr(entry, 'enclosures'):
            return automate.util.Url(self._find_highest_resolution(
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
            
            return automate.util.Url(url)


class InterfaceLift (DownloadSource):
    _HOST_NAME = 'interfacelift.com'
    
    
    def __init__(self):
        DownloadSource.__init__(self)
        
        tk = Tkinter.Tk()
        self._screen_ratio = tk.winfo_screenwidth() / tk.winfo_screenheight()
    
    
    def download_finished(self, url, file_path):
        if url.host_name == self._HOST_NAME:
            image = PIL.Image.open(file_path)
            image.save(file_path, quality = 85)
    
    
    def list_urls(self):
        try:
            session_code = self._session_code
        except urllib2.URLError as error:
            self.logger.error('%s: %s\'s session code', str(error), self.name)
            return
        
        feed = feedparser.parse('http://%s/wallpaper/rss/index.xml' \
            % self._HOST_NAME)
        
        for entry in feed.entries:
            html = lxml.html.fromstring(entry.summary)
            url = automate.util.FileUrl(html.xpath('//img/@src')[0])
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
        script = automate.util.Url('http://%s/inc_NEW/jscript.js' \
            % self._HOST_NAME)
        
        return re.findall('"/wallpaper/([^/]+)/"', script.open().read())[0]


class GameTrailersVideos (DownloadSource):
    BASE_URL = 'http://www.gametrailers.com'
    
    
    def __init__(self, skip_indies = False):
        DownloadSource.__init__(self)
        
        self._skip_indies = skip_indies
        self._skipped_urls = set()
    
    
    def get_video_url(self, page_url):
        if page_url in self._skipped_urls:
            return
        
        page_html = page_url.open().read()
        video_game_id = re.findall(r'mov_game_id\s*=\s*(\d+)', page_html)
        
        if len(video_game_id) == 0:
            # Not all videos are available for download.
            self.logger.error('Movie ID not found: %s', page_url)
            return
        else:
            [video_game_id] = video_game_id
        
        page = lxml.html.fromstring(page_html)
        
        if self._skip_indies:
            [publisher] = page.xpath('//*[@class = "publisher"]/text()')
            
            if publisher.strip() == 'N/A':
                self.logger.warning('Skip indie game: %s', page_url)
                self._skipped_urls.add(page_url)
                return
        
        video_url = page.xpath('//*[@class = "Downloads"]' \
            + '/a[starts-with(text(), "Quicktime")]/@href')
        
        if len(video_url) == 0:
            self.logger.debug('QuickTime video URL not found: %s', page_url)
            
            # From <http://userscripts.org/scripts/show/46320>.
            info_url = automate.util.Url('http://www.gametrailers.com/neo/')
            info_url.query = {
                'movieId': page_url.path.components[-1],
                'page': 'xml.mediaplayer.Mediagen',
            }
            
            url = automate.util.Url(lxml.etree.parse(unicode(info_url)).xpath(
                '//rendition/src/text()')[0])
        else:
            video_url = automate.util.Url(video_url[0])
            
            url = automate.util.Url(
                'http://trailers-ak.gametrailers.com/gt_vault/%s/%s' \
                    % (video_game_id, video_url.path.components[-1]))
        
        url.comment = page_url
        return url


class ScrewAttack (GameTrailersVideos):
    def list_urls(self):
        main_html = lxml.html.fromstring(
            automate.util.Url(self.BASE_URL + '/screwattack').open().read())
        videos = main_html.xpath(
            '//*[@id = "nerd"]//a[@class = "gamepage_content_row_title"]/@href')
        
        for page_url in [automate.util.Url(self.BASE_URL + p) for p in videos]:
            yield self.get_video_url(page_url)
    
    
    @property
    def name(self):
        return 'ScrewAttack'


class GameTrailers (GameTrailersVideos):
    def __init__(self):
        GameTrailersVideos.__init__(self, skip_indies = True)
        
        options = {
            'limit': 50,
            'orderby': 'newest',
            'quality[hd]': 'on',
        }
        
        for system in ['pc', 'ps3', 'xb360']:
            options['favplats[%s]' % system] = system
        
        self._feed_url = automate.util.Url(self.BASE_URL + '/rssgenerate.php')
        self._feed_url.query = options
    
    
    def list_urls(self):
        keywords_re = r'\b(%s)\b' % '|'.join(
            ['gameplay', 'preview', 'review', 'teaser', 'trailer'])
        
        for entry in feedparser.parse(unicode(self._feed_url)).entries:
            if re.search(keywords_re, entry.title, re.IGNORECASE):
                try:
                    url = self.get_video_url(automate.util.Url(entry.link))
                except urllib2.URLError as error:
                    self.logger.error('%s: %s', str(error), entry.link)
                    continue
                
                if url is not None:
                    url.comment = entry.link
                    yield url
    
    
    @property
    def name(self):
        return 'GameTrailers'


class GameTrailersVideosNewest (GameTrailersVideos):
    def __init__(self, game):
        GameTrailersVideos.__init__(self)
        self._game = game
    
    
    def list_urls(self):
        main_url = automate.util.Url(self.BASE_URL + '/game/' + self._game)
        main_html = lxml.html.fromstring(main_url.open().read())
        
        videos = main_html.xpath(
            '//*[@id = "GamepageMedialistFeatures"]' \
            + '//*[@class = "newestlist_movie_format_SDHD"]/a[1]/@href')
        
        for page_url in [automate.util.Url(self.BASE_URL + p) for p in videos]:
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
