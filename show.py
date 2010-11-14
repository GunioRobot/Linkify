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
        dest = 'label',
        action = 'append',
        help = '(diff)')
    
    parser.add_argument('-p',
        dest = 'pager',
        action = 'append',
        help = 'Custom pager program to use and arguments.')
    
    parser.add_argument('-u',
        action = 'store_true',
        default = True,
        help = '(diff)')
    
    parser.add_argument('file',
        nargs = '?',
        default = sys.stdin,
        type = argparse.FileType(),
        help = 'File to be shown, otherwise use standard input.')
    
    parser.add_argument('file2',
        nargs = '?',
        type = argparse.FileType(),
        help = 'If given, file to be compared against, switching to diff mode.')
    
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
source = args.file

if args.file2 is not None:
    files = [args.file, args.file2]
    diff_args = ['diff']
    
    if args.u:
        diff_args.append('-u')
    
    if args.label is None:
        args.label = [file.name for file in files]
    
    for label in args.label:
        # Kompare chokes on tab characters in labels.
        diff_args.extend(['-L', label.replace('\t', ' ')])
    
    if args.file2 is sys.stdin:
        # Compare standard input with given file, not the other way around.
        files.reverse()
    
    for file in files:
        diff_args.append('-' if file is sys.stdin else file.name)
    
    source = subprocess.Popen(diff_args, stdout = subprocess.PIPE).stdout

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
