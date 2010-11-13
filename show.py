#!/usr/bin/env python
# -*- coding: utf8 -*-


# Standard library:
import argparse, codecs, locale, re, subprocess, sys

# External modules:
import pygments, pygments.formatters, pygments.lexers


# TODO: Add support for diff-like options and launch a graphical tool instead.


def display(stream, text, lexer, formatter):
    if lexer is not None:
        text = pygments.highlight(text, lexer, formatter)
    
    stream.write(text)


def guess_lexer(file_name, text):
    # Detect ANSI "color" escape sequences.
    if re.search(r'\x1B\[\d+(;\d+)*m', text):
        return None
    
    try:
        lexer = pygments.lexers.guess_lexer_for_filename(file_name, text)
    except pygments.util.ClassNotFound:
        lexer = pygments.lexers.guess_lexer(text)
    
    lexer.add_filter('whitespace', tabs = True, spaces = True)
    return lexer


def locale_writer(stream):
    return codecs.getwriter(locale.getpreferredencoding())(stream)


def parse_arguments():
    def natural(value):
        number = int(value, 10)
        
        if number < 0:
            raise argparse.ArgumentTypeError('%d is not a natural number'
                % value)
        
        return number
    
    parser = argparse.ArgumentParser()
    
    parser.add_argument('-l',
        dest = 'lines',
        default = 15,
        type = natural,
        help = 'Number of lines to display inline before paging.')
    
    parser.add_argument('-p',
        dest = 'pager',
        action = 'append',
        help = 'Custom pager program to use and arguments.')
    
    parser.add_argument('file',
        nargs = '?',
        default = sys.stdin,
        type = file,
        help = 'File to be show, otherwise read from standard input.')
    
    args = parser.parse_args()
    
    if args.pager is None:
        args.pager = ['less', '-cRx4']
    
    return args


args = parse_arguments()
formatter = pygments.formatters.Terminal256Formatter()
lexer = None
pager = None
lines = []

for line in args.file:
    if pager is None:
        lines.append(line)
        
        if len(lines) >= args.lines:
            pager = subprocess.Popen(args.pager, stdin = subprocess.PIPE)
            text = ''.join(lines)
            lexer = guess_lexer(args.file.name, text)
            pager.stdin = locale_writer(pager.stdin)
            
            display(pager.stdin, text, lexer, formatter)
    else:
        display(pager.stdin, line, lexer, formatter)

if pager is not None:
    pager.communicate()
    pager.stdin.close()
    pager.wait()
elif len(lines) > 0:
    text = ''.join(lines)
    lexer = guess_lexer(args.file.name, text)
    display(locale_writer(sys.stdout), text, lexer, formatter)
