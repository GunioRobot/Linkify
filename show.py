#!/usr/bin/env python
# -*- coding: utf8 -*-


# Standard library:
import argparse, codecs, locale, re, subprocess, sys

# External modules:
import pygments, pygments.formatters, pygments.lexers


def create_arguments_parser():
    def natural(value):
        number = int(value, 10)
        
        if number < 0:
            raise argparse.ArgumentTypeError('%d is not a natural number'
                % value)
        
        return number
    
    parser = argparse.ArgumentParser(description = '''
        Smart pager with automatic syntax highlighting and diff support.
    ''')
    
    parser.add_argument('-l',
        dest = 'lines',
        default = 15,
        type = natural,
        help = 'Number of lines to display inline before paging.')
    
    parser.add_argument('-L',
        action = 'append',
        help = '(diff)')
    
    parser.add_argument('-p',
        dest = 'pager',
        action = 'append',
        help = 'Custom pager program to use and arguments.')
    
    parser.add_argument('-u',
        action = 'store_true',
        help = '(diff)')
    
    parser.add_argument('file',
        nargs = '*',
        default = [sys.stdin],
        type = file,
        help = 'File to be shown, otherwise read from standard input.')
    
    return parser


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
    
    lexer.add_filter('codetagify')
    lexer.add_filter('whitespace', tabs = True, spaces = True)
    return lexer


def locale_writer(stream):
    return codecs.getwriter(locale.getpreferredencoding())(stream)


args = create_arguments_parser().parse_args()
source = None

if (args.u is True) and (len(args.L) == 2) and (len(args.file) == 2):
    # Kompare chokes on tab characters in labels.
    labels = [l.replace('\t', ' ') for l in args.L]
    
    diff_args = ['diff', '-u', '-L', labels[0], '-L', labels[1]]
    diff_args.extend([f.name for f in args.file])
    
    source = subprocess.Popen(diff_args, stdout = subprocess.PIPE).stdout
else:
    (source,) = args.file

formatter = pygments.formatters.Terminal256Formatter()
lexer = None
pager = None
lines = []

for line in source:
    if pager is not None:
        display(pager.stdin, line, lexer, formatter)
        continue
    
    lines.append(line)
    
    if len(lines) >= args.lines:
        text = ''.join(lines)
        lexer = guess_lexer(source.name, text)
        
        if args.pager is None:
            if isinstance(lexer, pygments.lexers.DiffLexer):
                args.pager = ['kompare', '-o', '-']
                lexer = None
            else:
                args.pager = ['less', '-cRx4']
        
        pager = subprocess.Popen(args.pager, stdin = subprocess.PIPE)
        pager.stdin = locale_writer(pager.stdin)
        display(pager.stdin, text, lexer, formatter)

if pager is not None:
    pager.communicate()
    pager.stdin.close()
    pager.wait()
elif len(lines) > 0:
    text = ''.join(lines)
    lexer = guess_lexer(source.name, text)
    display(locale_writer(sys.stdout), text, lexer, formatter)

source.close()
