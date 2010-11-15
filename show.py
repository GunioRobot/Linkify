#!/usr/bin/env python
# -*- coding: utf-8 -*-


# TODO: Use a percentage of the current terminal height instead of a fixed
#       number of lines.
# TODO: Clean up (abstract logic behind classes e.g. Pager, LessPager, etc).
# TODO: Handle the KeyboardInterrupt exception gracefully.
# TODO: Detect missing programs and provide automatic installation or fallbacks.
# TODO: Guess input syntax even if already colored to use an appropriate pager.
# TODO: Pass real files to Kompare instead of diff output?
# TODO: Force diff lexer in diff mode.
# TODO: Allow override of the default diff pager program (e.g. opendiff kdiff3
#       tkdiff xxdiff meld kompare gvimdiff diffuse ecmerge p4merge araxis
#       emerge vimdiff).


# Standard library:
import codecs, locale, re, subprocess, sys

try:
    import argparse
except ImportError as error:
    sys.exit('Python 2.7 or newer is required: %s' % error)

# External modules:
try:
    import pygments, pygments.formatters, pygments.lexers
except ImportError as error:
    sys.exit('Pygments is required, see <http://pygments.org/>: %s' % error)


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
        help = 'If given, file to be compared against, and switch to diff mode.')
    
    parser.add_argument('git',
        nargs = '*',
        help = 'If given, assume git diff arguments, and switch to diff mode.')
    
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
    return lexer


def locale_writer(stream):
    return codecs.getwriter(locale.getpreferredencoding())(stream)


args = create_arguments_parser().parse_args()
formatter = pygments.formatters.Terminal256Formatter()
source = args.file
lexer = None
pager = None
lines = []

if len(args.git) == 5:
    # Parse git diff arguments.
    (path, old_file) = (args.file, args.file2)
    (old_hex, old_mode, new_file, new_hex, new_mode) = args.git
    
    (args.file, args.file2) = (old_file, path)
    args.label = [path.name + ' (%s)' % h for h in [old_hex, new_hex]]

if args.file2 is not None:
    # Switch to diff mode.
    files = [args.file, args.file2]
    diff = ['diff']
    
    if args.u:
        diff.append('-u')
    
    if args.label is None:
        args.label = [file.name for file in files]
    
    for label in args.label:
        # Kompare chokes on tab characters in labels.
        diff.extend(['-L', label.replace('\t', ' ')])
    
    if args.file2 is sys.stdin:
        # Compare standard input with given file, not the other way around.
        files.reverse()
    
    for file in files:
        diff.append('-' if file is sys.stdin else file.name)
    
    source = subprocess.Popen(diff, stdout = subprocess.PIPE).stdout

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
