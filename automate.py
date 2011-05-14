#!/usr/bin/env python
# -*- coding: utf-8 -*-


# TODO: Use logging.
# TODO: Create MS Win32 system service?
# TODO: Cut the first few seconds of the IGN Daily Fix videos.
# TODO: Compress InterfaceLIFT wallpapers after download.
# TODO: Choose the highest available InterfaceLIFT wallpaper resolution that 
#       most closely matches the running computer's.


# Standard library:
import abc, datetime, os.path, re, time, urllib2, urlparse

# External modules:
import feedparser, lxml.html


class Downloader (object):
    def open_url(self, url):
        request = urllib2.Request(url)
        
        if urlparse.urlparse(url).hostname == u'trailers.apple.com':
            request.add_header(u'User-Agent', u'QuickTime')
        
        return urllib2.build_opener().open(request)


class DownloadManager (object):
    __metaclass__ = abc.ABCMeta
    
    
    @abc.abstractmethod
    def download(self, url):
        pass
    
    
    @abc.abstractmethod
    def has_downloaded(self, source, url):
        pass


class MsWindowsTypeLibrary (object):
    def __init__(self, path):
        import pythoncom, win32com.client
        global pythoncom, win32com
        
        self._path = path
        self._lib = pythoncom.LoadTypeLib(self._path)
    
    
    def get_type(self, type_name):
        for i in xrange(0, self._lib.GetTypeInfoCount()):
            (name, doc, help_ctxt, help_file) = self._lib.GetDocumentation(i)
            
            if name == type_name:
                iid = self._lib.GetTypeInfo(i).GetTypeAttr().iid
                return win32com.client.Dispatch(iid)
        
        raise Exception(u'Type "%s" not found in type library "%s".'
            % (name, self._path))


class FreeDownloadManager (DownloadManager, MsWindowsTypeLibrary, Downloader):
    def __init__(self):
        DownloadManager.__init__(self)
        MsWindowsTypeLibrary.__init__(self, u'fdm.tlb')
        Downloader.__init__(self)
        
        self._cached_downloads_list = None
    
    
    def download(self, url):
        wg_url_receiver = self.get_type(u'WGUrlReceiver')
        
        wg_url_receiver.Url = url
        wg_url_receiver.DisableURLExistsCheck = False
        wg_url_receiver.ForceDownloadAutoStart = True
        wg_url_receiver.ForceSilent = True
        wg_url_receiver.ForceSilentEx = True
        
        wg_url_receiver.AddDownload()
    
    
    def has_downloaded(self, source, url):
        redirected_url = self.open_url(url).geturl()
        
        for downloaded_url in self._list_downloads():
            if source.equal_urls(url, downloaded_url, redirected_url):
                return True
        
        return False
    
    
    def _list_downloads(self):
        if self._cached_downloads_list is None:
            downloads_stat = self.get_type(u'FDMDownloadsStat')
            downloads_stat.BuildListOfDownloads(True, True)
            urls = set()
            
            # Don't start at the oldest URL to find newer downloads faster.
            for i in reversed(xrange(0, downloads_stat.DownloadCount)):
                url = downloads_stat.Download(i).Url
                
                urls.add(url)
                yield url
            
            self._cached_downloads_list = urls
        else:
            for url in self._cached_downloads_list:
                yield url


class DownloadSource (object):
    __metaclass__ = abc.ABCMeta
    
    
    def equal_urls(self, original, redirected, downloaded):
        return redirected == downloaded
    
    
    @abc.abstractmethod
    def list_downloads(self):
        pass


class Feed (DownloadSource):
    def __init__(self, url):
        self._url = url
    
    
    def get_feed(self):
        return feedparser.parse(self._url)


class IgnDailyFixFeed (Feed):
    def __init__(self):
        super(IgnDailyFixFeed, self).__init__(
            u'http://feeds.ign.com/ignfeeds/podcasts/games/')
    
    
    def list_downloads(self):
        videos = set()
        
        for entry in self.get_feed().entries:
            if entry.title.startswith(u'IGN Daily Fix:'):
                videos.add(entry.enclosures[0].href)
        
        return videos


class HdTrailersFeed (Feed):
    def __init__(self):
        super(HdTrailersFeed, self).__init__(
            u'http://feeds.hd-trailers.net/hd-trailers/blog')
    
    
    def equal_urls(self, original, redirected, downloaded):
        if urlparse.urlparse(original).hostname == u'playlist.yahoo.com':
            return urlparse.urlparse(redirected).path \
                == urlparse.urlparse(downloaded).path
        
        return super(HdTrailersFeed, self).equal_urls(
            original, redirected, downloaded)
    
    
    def list_downloads(self):
        videos = set()
        
        for entry in self.get_feed().entries:
            if re.search(ur'\b(teaser|trailer)\b', entry.title, re.IGNORECASE):
                if hasattr(entry, u'enclosures'):
                    urls = [enclosure.href for enclosure in entry.enclosures]
                    videos.add(self._find_highest_resolution(urls))
                    continue
                
                # Parse HTML to find movie links.
                entry_doc = lxml.html.fromstring(entry.content[0].value)
                (last_url, last_resolution) = (None, 0)
                
                for link in entry_doc.xpath(u'//a[text() != ""]'):
                    resolution = self._get_resolution(link.text)
                    
                    if resolution > last_resolution:
                        last_url = link.attrib[u'href']
                        last_resolution = resolution
                
                videos.add(last_url)
        
        return videos
    
    
    def _find_highest_resolution(self, strings):
        strings.sort(
            lambda x, y: cmp(self._get_resolution(x), self._get_resolution(y)),
            reverse = True)
        
        return strings[0]
    
    
    def _get_resolution(self, text):
        resolution = re.findall(ur'(\d{3,4})p', text)
        
        if len(resolution) != 1:
            resolution = re.findall(ur'(480|720|1080)', text)
            
            if len(resolution) != 1:
                return 0
        
        return int(resolution.pop())


class InterfaceLiftFeed (Feed, Downloader):
    BASE_URL = u'http://interfacelift.com'
    
    
    def __init__(self):
        super(InterfaceLiftFeed, self).__init__(
            self.BASE_URL + u'/wallpaper/rss/index.xml')
    
    
    def equal_urls(self, original, redirected, downloaded):
        return os.path.basename(redirected) == os.path.basename(downloaded)
    
    
    def list_downloads(self, resolution = u'1600x900'):
        download_code = self._get_download_code()
        wallpapers = set()
        
        for entry in self.get_feed().entries:
            entry_doc = lxml.html.fromstring(entry.summary)
            
            # Get preview image URL to build the actual wallpaper URL.
            preview_url = entry_doc.xpath(u'//img/@src')[0]
            image_url = preview_url.replace(u'previews', download_code)
            wallpaper_url = urlparse.urlparse(image_url)
            
            # Build the final wallpaper URL for the intended resolution.
            (path, ext) = os.path.splitext(wallpaper_url.path)
            wallpaper_url = list(wallpaper_url)
            wallpaper_url[2] = path + u'_' + resolution + ext
            
            wallpapers.add(urlparse.urlunparse(wallpaper_url))
        
        return wallpapers
    
    
    def _get_download_code(self):
        script = self.open_url(self.BASE_URL + u'/inc_NEW/jscript.js').read()
        return re.findall(u'"/wallpaper/([^/]+)/"', script).pop()


while True:
    print datetime.datetime.now().isoformat(), u'Starting...'
    fdm = FreeDownloadManager()
    
    for source in [IgnDailyFixFeed(), InterfaceLiftFeed(), HdTrailersFeed()]:
        for url in source.list_downloads():
            if not fdm.has_downloaded(source, url):
                print u'>', url
                fdm.download(url)
    
    print datetime.datetime.now().isoformat(), u'Pausing...'
    time.sleep(5 * 60)
