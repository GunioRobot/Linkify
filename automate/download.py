# -*- coding: utf-8 -*-


# Standard library:
from __future__ import division, print_function, unicode_literals
import datetime, httplib, re, sys, Tkinter, urllib2

# External modules:
from defaults import *

# Internal modules:
import automate.task, automate.util


externals('feedparser', 'lxml.etree', 'lxml.html', 'PIL.Image')


class DownloadManager (object):
    __metaclass__ = ABCMeta
    
    
    @abstractmethod
    def download_url(self, url):
        pass
    
    
    @abstractmethod
    def has_url(self, url):
        pass


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


class Downloader (automate.task.PeriodicTask):
    def __init__(self, manager, source):
        self._manager = manager
        self._source = source
        
        automate.task.PeriodicTask.__init__(self)
    
    
    @property
    def name(self):
        return self._source.name
    
    
    def process(self):
        for url in self._source.list_urls():
            if self.is_stopping:
                break
            
            try:
                if not self._manager.has_url(url):
                    self._manager.download_url(url)
            except (httplib.HTTPException, urllib2.URLError) as error:
                self._source.logger.error('%s: %s', str(error), url)


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
            wg_url_receiver.FileName = unicode(url.save_as)
        
        if url.comment is not None:
            wg_url_receiver.Comment = unicode(url.comment)
        
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


class GameTrailersVideos (DownloadSource):
    class VideoUrlUnavailable (Exception):
        pass
    
    
    BASE_URL = 'http://www.gametrailers.com'
    
    
    def __init__(self, skip_cam_videos = True, skip_indie_games = False):
        DownloadSource.__init__(self)
        
        self._skip_cam_videos = skip_cam_videos
        self._skip_indie_games = skip_indie_games
        self._skipped_urls = set()
    
    
    def get_video_url(self, page_url):
        if page_url in self._skipped_urls:
            raise self.VideoUrlUnavailable()
        
        try:
            page_html = page_url.open().read()
        except (httplib.HTTPException, urllib2.URLError) as error:
            self.logger.error('%s: %s', str(error), page_url)
            raise self.VideoUrlUnavailable()
        
        video_id = self._get_video_id(page_html, page_url)
        page = lxml.html.fromstring(page_html)
        
        if self._skip_indie_game(page, page_url):
            raise self.VideoUrlUnavailable()
        
        url = self._get_video_url_from_html(page, video_id) \
            or self._get_flash_video_url(page_url)
        
        if (url is not None) and (not self._skip_cam_video(url, page_url)):
            url.comment = page_url
            url.save_as = re.sub(r'^t_', '', url.path.name)
            return url
        
        raise self.VideoUrlUnavailable()
    
    
    def _get_flash_video_url(self, page_url):
        self.logger.debug('QuickTime video URL not found: %s', page_url)
        
        # From <http://userscripts.org/scripts/show/46320>.
        info_url = automate.util.Url('http://www.gametrailers.com/neo/',
            query = {
                'movieId': page_url.path.components[-1],
                'page': 'xml.mediaplayer.Mediagen',
            })
        
        try:
            info_xml = lxml.etree.parse(unicode(info_url))
        except (IOError, lxml.etree.XMLSyntaxError) as error:
            self.logger.error(error)
        else:
            return automate.util.Url(
                info_xml.xpath('//rendition/src/text()')[0])
    
    
    def _get_video_id(self, page_html, page_url):
        video_id = re.findall(r'mov_game_id\s*=\s*(\d+)', page_html)
        
        if len(video_id) > 0:
            return video_id[0]
        
        # Not all videos are available for download, e.g. Bonus Round.
        self.logger.error('Movie ID not found: %s', page_url)
        self._skipped_urls.add(page_url)
        raise self.VideoUrlUnavailable()
    
    
    def _get_video_url_from_html(self, page, video_id):
        for video_type in ['WMV', 'Quicktime']:
            video_url = page.xpath('//*[@class = "Downloads"]' \
                + '/a[starts-with(text(), "%s")]/@href' % video_type)
            
            if len(video_url) > 0:
                video_url = automate.util.Url(video_url[0])
                
                return automate.util.Url(
                    'http://trailers-ak.gametrailers.com/gt_vault/%s/%s' \
                        % (video_id, video_url.path.components[-1]))
    
    
    def _has_publisher(self, page):
        [publisher] = page.xpath('//*[@class = "publisher"]/text()')
        return publisher.strip() != 'N/A'
    
    
    def _skip_cam_video(self, video_url, page_url):
        if self._skip_cam_videos and (video_url.path.stem.find('_cam_') > 0):
            self.logger.debug('Skip cam video: %s', page_url)
            self._skipped_urls.add(page_url)
            return True
        else:
            return False
    
    
    def _skip_indie_game(self, page, page_url):
        if self._skip_indie_games and not self._has_publisher(page):
            self.logger.debug('Skip indie game: %s', page_url)
            self._skipped_urls.add(page_url)
            return True
        else:
            return False


class GameTrailersNewestVideos (GameTrailersVideos):
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
            try:
                yield self.get_video_url(page_url)
            except self.VideoUrlUnavailable:
                pass


class GameTrailers (GameTrailersVideos):
    def __init__(self):
        GameTrailersVideos.__init__(self, skip_indie_games = True)
        
        options = {
            'limit': 50,
            'orderby': 'newest',
            'quality[hd]': 'on',
        }
        
        for system in ['pc', 'ps3', 'xb360']:
            options['favplats[%s]' % system] = system
        
        self._feed_url = automate.util.Url(self.BASE_URL + '/rssgenerate.php',
            query = options)
    
    
    def list_urls(self):
        keywords_re = r'\b(%s)\b' % '|'.join(
            ['gameplay', 'preview', 'review', 'teaser', 'trailer'])
        
        for entry in feedparser.parse(unicode(self._feed_url)).entries:
            if re.search(keywords_re, entry.title, re.IGNORECASE):
                try:
                    yield self.get_video_url(automate.util.Url(entry.link))
                except self.VideoUrlUnavailable:
                    pass
    
    
    @property
    def name(self):
        return 'GameTrailers'


class GtCountdown (GameTrailersNewestVideos):
    def __init__(self):
        GameTrailersNewestVideos.__init__(self, 'gt-countdown/2111')
    
    
    @property
    def name(self):
        return 'GT Countdown'


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
            
            yield automate.util.PathUrl(url, save_as = file)
    
    
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


class IgnDailyFix (DownloadSource):
    _TITLE = 'IGN Daily Fix'
    
    
    def list_urls(self):
        feed = feedparser.parse('http://feeds.ign.com/ignfeeds/podcasts/games/')
        
        for entry in feed.entries:
            if entry.title.startswith(self._TITLE + ':'):
                yield automate.util.Url(entry.enclosures[0].href,
                    comment = entry.link)
    
    
    @property
    def name(self):
        return self._TITLE


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


class PopFiction (GameTrailersNewestVideos):
    def __init__(self):
        GameTrailersNewestVideos.__init__(self, 'pop-fiction/13123')
    
    
    @property
    def name(self):
        return 'Pop-Fiction'


class ScrewAttack (GameTrailersVideos):
    def list_urls(self):
        main_html = lxml.html.fromstring(
            automate.util.Url(self.BASE_URL + '/screwattack').open().read())
        videos = main_html.xpath(
            '//*[@id = "nerd"]//a[@class = "gamepage_content_row_title"]/@href')
        
        for page_url in [automate.util.Url(self.BASE_URL + p) for p in videos]:
            try:
                yield self.get_video_url(page_url)
            except self.VideoUrlUnavailable:
                pass
    
    
    @property
    def name(self):
        return 'ScrewAttack'
