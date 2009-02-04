#!/usr/bin/python
# -*- coding: utf8 -*-

# Dependencies:
# - PyYAML, <http://pyyaml.org/>
# - flickrpy, <http://code.google.com/p/flickrpy/>
# - Python Imaging Library, <http://www.pythonware.com/products/pil/>


from __future__ import division
import Image, ImageChops, ImageDraw, ImageFont
import flickr, yaml
import os, re, sys, urllib


def draw_title(settings, slide, title, padding = 15):
    (draw, size) = (ImageDraw.Draw(slide), 40)
    color = settings['Text color']
    
    while True:
        font = ImageFont.truetype(settings['Font'], size)
        (width, height) = draw.textsize(title, font = font)
        
        if abs(width - settings['Width']) < padding:
            draw.text((3, 3), title, fill = color['Background'], font = font)
            draw.text((0, 0), title, fill = color['Foreground'], font = font)
            return slide
        
        size += 1 if width < settings['Width'] else -1


def extract_slides(slides):
    searchTextPattern = ur'[*]([^*]+)[*]'
    (titles, searchTerms) = ([], [])
    
    for slide in slides:
        (searchText,) = re.findall(searchTextPattern, slide)
        title = re.sub(searchTextPattern, ur'\1', slide)
        
        titles.append(title)
        searchTerms.append(searchText)
    
    return (titles, searchTerms)


def get_url(photo):
    try:
        return photo.getURL(urlType = 'source')
    except flickr.FlickrError:
        return 'http://static.flickr.com/%s/%s_%s.jpg' \
            % (photo.server, photo.id, photo.secret)


def get_urls(text, limit = 20):
    print('Searching Flickr for "%s"...' % text)
    return map(get_url, flickr.photos_search(text = text, per_page = limit))


def make_slide(settings, image):
    (WIDTH, HEIGHT) = (settings['Width'], settings['Height'])
    (width, height) = image.size
    
    newHeight = height * WIDTH // width
    newWidth = width * HEIGHT // height
    
    if newHeight <= HEIGHT:
        newWidth = WIDTH
    elif newWidth <= WIDTH:
        newHeight = HEIGHT
    
    x = (WIDTH - newWidth) // 2
    y = (HEIGHT - newHeight) // 2
    
    slide = image.resize((newWidth, newHeight), Image.ANTIALIAS)
    slide = slide.crop((0, 0, WIDTH, HEIGHT))
    slide = ImageChops.offset(slide, x, y)
    
    draw = ImageDraw.Draw(slide)
    color = settings['Text color']['Background']
    
    if newHeight < HEIGHT:
        draw.rectangle([(0, 0), (WIDTH, y)], fill = color)
        draw.rectangle([(0, y + newHeight), (WIDTH, HEIGHT)], fill = color)
    elif newWidth < WIDTH:
        draw.rectangle([(0, 0), (x, HEIGHT)], fill = color)
        draw.rectangle([(x + newWidth, 0), (WIDTH, HEIGHT)], fill = color)
    
    return slide


def make_slides(config):
    flickr.API_KEY = config['Flickr']['API key']
    flickr.API_SECRET = config['Flickr']['API secret']
    
    (titles, searchTerms) = extract_slides(config['Slides'])
    (searchedText, slideNumber) = ({}, 1)
    
    mainTitle = config['Title']
    settings = config['Settings']
    color = settings['Text color']
    
    for name in color:
        color[name] = tuple(color[name])
    
    if not os.path.exists(mainTitle):
        os.mkdir(mainTitle)
    
    for title, searchText in zip(titles, searchTerms):
        print('Creating slide %i...' % slideNumber)
        
        if searchText not in searchedText:
            urls = get_urls(searchText)
            
            if len(urls) == 0:
                print('No images found for "%s"' % searchText)
                continue
            
            searchedText[searchText] = urls
        
        if len(searchedText[searchText]) == 0:
            print('No more images for "%s"' % searchText)
            continue
        
        url = searchedText[searchText].pop()
        print('Downloading image from <%s>...' % url)
        
        (imageFileName, headers) = urllib.urlretrieve(url)
        simpleSlide = make_slide(settings, Image.open(imageFileName))
        slide = draw_title(settings, simpleSlide, title)
        
        slideFileName = u'%s/%i.%s' % (mainTitle, slideNumber, settings['Format'])
        slide.save(slideFileName)
        print('Saved as "%s".\n' % slideFileName)
        
        slideNumber += 1


def main(args):
    if len(args) != 2:
        print('Usage: <configuration file>')
    else:
        make_slides(yaml.load(file(args.pop())))
    
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
