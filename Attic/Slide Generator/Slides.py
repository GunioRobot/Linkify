#!/usr/bin/python
# -*- coding: utf8 -*-


# Standard library:
from __future__ import division
import os, re, sys, urllib

# External libraries:
import Image, ImageChops, ImageDraw, ImageFont    # Python Imaging Library
import flickr                                     # flickrpy
import yaml                                       # PyYAML


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
    search_text_pattern = r'[*]([^*]+)[*]'
    (titles, search_terms) = ([], [])
    
    for slide in slides:
        (search_text,) = re.findall(search_text_pattern, slide)
        title = re.sub(search_text_pattern, r'\1', slide)
        
        titles.append(title)
        search_terms.append(search_text)
    
    return (titles, search_terms)


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
    
    new_height = height * WIDTH // width
    new_width = width * HEIGHT // height
    
    if new_height <= HEIGHT:
        new_width = WIDTH
    elif new_width <= WIDTH:
        new_height = HEIGHT
    
    x = (WIDTH - new_width) // 2
    y = (HEIGHT - new_height) // 2
    
    slide = image.resize((new_width, new_height), Image.ANTIALIAS)
    slide = slide.crop((0, 0, WIDTH, HEIGHT))
    slide = ImageChops.offset(slide, x, y)
    
    draw = ImageDraw.Draw(slide)
    color = settings['Text color']['Background']
    
    if new_height < HEIGHT:
        draw.rectangle([(0, 0), (WIDTH, y)], fill = color)
        draw.rectangle([(0, y + new_height), (WIDTH, HEIGHT)], fill = color)
    elif new_width < WIDTH:
        draw.rectangle([(0, 0), (x, HEIGHT)], fill = color)
        draw.rectangle([(x + new_width, 0), (WIDTH, HEIGHT)], fill = color)
    
    return slide


def make_slides(config):
    flickr.API_KEY = config['Flickr']['API key']
    flickr.API_SECRET = config['Flickr']['API secret']
    
    (titles, search_terms) = extract_slides(config['Slides'])
    (searched_text, slide_nr) = ({}, 1)
    
    main_title = config['Title']
    settings = config['Settings']
    color = settings['Text color']
    
    for name in color:
        color[name] = tuple(color[name])
    
    if not os.path.exists(main_title):
        os.mkdir(main_title)
    
    for title, search_text in zip(titles, search_terms):
        print('Creating slide %i...' % slide_nr)
        
        if search_text not in searched_text:
            urls = get_urls(search_text)
            
            if len(urls) == 0:
                print('No images found for "%s"' % search_text)
                continue
            
            searched_text[search_text] = urls
        
        if len(searched_text[search_text]) == 0:
            print('No more images for "%s"' % search_text)
            continue
        
        url = searched_text[search_text].pop()
        print('Downloading image from <%s>...' % url)
        
        (image_file_name, headers) = urllib.urlretrieve(url)
        simple_slide = make_slide(settings, Image.open(image_file_name))
        slide = draw_title(settings, simple_slide, title)
        
        slide_file_name = '%s/%i.%s' % (main_title, slide_nr, settings['Format'])
        slide.save(slide_file_name)
        print('Saved as "%s".\n' % slide_file_name)
        
        slide_nr += 1


def main(args):
    if len(args) != 1:
        print('Usage: <configuration file>')
    else:
        make_slides(yaml.load(file(args[0])))
    
    return 0


if __name__ == '__main__':
    sys.argv.pop(0)
    sys.exit(main(sys.argv))
