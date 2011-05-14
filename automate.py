#!/usr/bin/env python
# -*- coding: utf-8 -*-


# TODO: Use logging.


# Standard library:
import abc, difflib, HTMLParser, os.path, re, urllib2, urlparse

# External modules:
import feedparser, lxml.html


class DownloadManager (object):
    __metaclass__ = abc.ABCMeta
    
    
    @abc.abstractmethod
    def download(self, url):
        pass
    
    
    @abc.abstractmethod
    def has_download(self, url):
        pass


class MicrosoftWindowsTypeLibrary (object):
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


class FreeDownloadManager (DownloadManager, MicrosoftWindowsTypeLibrary):
    def __init__(self):
        DownloadManager.__init__(self)
        MicrosoftWindowsTypeLibrary.__init__(self, u'fdm.tlb')
        
        self._cached_downloads_list = None
    
    
    def download(self, url):
        wg_url_receiver = self.get_type(u'WGUrlReceiver')
        
        wg_url_receiver.Url = url
        wg_url_receiver.DisableURLExistsCheck = False
        wg_url_receiver.ForceDownloadAutoStart = True
        wg_url_receiver.ForceSilent = True
        
        wg_url_receiver.AddDownload()
    
    
    def has_download(self, url, fuzzy = False, redirect = True):
        if redirect:
            url = urllib2.urlopen(url).geturl()
        
        found_url = url in self._list_downloads()
        
        if found_url or not fuzzy:
            return found_url
        
        # TODO: Warn about fuzzy matches.
        matches = difflib.get_close_matches(url, self._list_downloads(),
            cutoff = 0.9,
            n = 1)
        
        return len(matches) > 0
    
    
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


class Feed (object):
    __metaclass__ = abc.ABCMeta
    
    
    def __init__(self, url):
        self._url = url
    
    
    def get_feed(self):
        return feedparser.parse(self._url)
    
    
    @abc.abstractmethod
    def list_downloads(self):
        pass


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


# TODO: Use lxml.html.
class TrailerLinksHtmlParser (HTMLParser.HTMLParser, object):
    @classmethod
    def get_resolution(cls, url):
        resolution = re.findall(ur'(\d{3,4})p', url)
        
        if len(resolution) != 1:
            resolution = re.findall(ur'(480|720|1080)', url)
            
            if len(resolution) != 1:
                return 0
        
        return int(resolution.pop())
    
    
    @classmethod
    def sort_by_resolution(cls, urls):
        urls.sort(
            lambda x, y: cmp(cls.get_resolution(x), cls.get_resolution(y)),
            reverse = True)
    
    
    def handle_data(self, data):
        if self._current_link is not None:
            resolution = self.get_resolution(data)
            
            if resolution > self._resolution:
                self._link = self._current_link
                self._resolution = resolution
    
    
    def handle_endtag(self, tag):
        self._current_link = None
    
    
    def handle_starttag(self, tag, attrs):
        if tag == u'a':
            for name, value in attrs:
                if name == u'href':
                    self._current_link = value
                    break
    
    
    @property
    def link(self):
        return self._link
    
    
    def reset(self):
        super(TrailerLinksHtmlParser, self).reset()
        
        self._current_link = None
        self._link = None
        self._resolution = 0


class HdTrailersFeed (Feed, TrailerLinksHtmlParser):
    def __init__(self):
        super(HdTrailersFeed, self).__init__(
            u'http://feeds.hd-trailers.net/hd-trailers/blog')
    
    
    def list_downloads(self):
        videos = set()
        
        for entry in self.get_feed().entries:
            if re.search(ur'\b(teaser|trailer)\b', entry.title, re.IGNORECASE):
                if hasattr(entry, u'enclosures'):
                    urls = [enclosure.href for enclosure in entry.enclosures]
                    self.sort_by_resolution(urls)
                    videos.add(urls[0])
                else:
                    self.reset()
                    self.feed(entry.content[0].value)
                    videos.add(self.link)
        
        return videos


class InterfaceLiftFeed (Feed):
    BASE_URL = u'http://interfacelift.com'
    
    
    def __init__(self):
        super(InterfaceLiftFeed, self).__init__(
            self.BASE_URL + u'/wallpaper/rss/index.xml')
    
    
    # TODO: Choose the highest available resolution.
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
        script_url = self.BASE_URL + u'/inc_NEW/jscript.js'
        script = urllib2.urlopen(script_url).read()
        
        return re.findall(u'"/wallpaper/([^/]+)/"', script).pop()


fdm = FreeDownloadManager()

for wallpaper in InterfaceLiftFeed().list_downloads():
    print wallpaper, fdm.has_download(wallpaper, fuzzy = True)

#for video in HdTrailersFeed().list_downloads():
    #print video, fdm.has_download(video)

#for video in IgnDailyFixFeed().list_downloads():
    #print video, fdm.has_download(video)
