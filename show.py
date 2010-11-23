#!/usr/bin/env python
# -*- coding: utf-8 -*-


# TODO: Handle the KeyboardInterrupt exception gracefully.
# TODO: Use a percentage of the current terminal height instead of a fixed
#       number of lines.
# TODO: Detect missing programs and provide automatic installation or fallbacks
#       (e.g. opendiff kdiff3 tkdiff xxdiff meld kompare gvimdiff diffuse
#       ecmerge p4merge araxis emerge vimdiff).


# Standard library:
import abc, codecs, locale, re, subprocess, sys

try:
    import argparse
except ImportError as error:
    sys.exit('Python 2.7 or newer is required: %s' % error)

# External modules:
try:
    import pygments, pygments.formatters, pygments.lexers
except ImportError as error:
    sys.exit('Pygments is required, see <http://pygments.org/>: %s' % error)


class Arguments (argparse.ArgumentParser):
    def __init__(self):
        super(Arguments, self).__init__(description = '''
            Smart pager with automatic syntax highlighting and diff support.''')
        
        def natural(value):
            number = int(value, 10)
            
            if number < 0:
                raise argparse.ArgumentTypeError('%d is not a natural number'
                    % value)
            
            return number
        
        arguments = [
            ('-L', {
                'dest': 'label',
                'action': 'append',
                'help': '(diff)',
            }),
            ('-u', {
                'action': 'store_true',
                'default': True,
                'help': '(diff)',
            }),
            ('file', {
                'nargs': '?',
                'default': sys.stdin,
                'type': argparse.FileType(),
                'help': 'File to be shown, otherwise use standard input.',
            }),
            ('file2', {
                'nargs': '?',
                'type': argparse.FileType(),
                'help': 'File to be compared against, and switch to diff mode.',
            }),
            ('git', {
                'nargs': '*',
                'help': 'Assume git diff arguments, and switch to diff mode.',
            }),
        ]
        
        for name, options in arguments:
            self.add_argument(name, **options)
    
    
    def parse_args(self):
        args = super(Arguments, self).parse_args()
        
        if len(args.git) == 5:
            self._parse_git_diff_arguments(args)
        
        if args.file2 is None:
            args.diff_mode = False
        else:
            args.diff_mode = True
            self._parse_diff_arguments(args)
        
        return args
    
    
    def _parse_diff_arguments(self, args):
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
        
        args.file = subprocess.Popen(diff, stdout = subprocess.PIPE).stdout
    
    
    def _parse_git_diff_arguments(self, args):
        (path, old_file) = (args.file, args.file2)
        (old_hex, old_mode, new_file, new_hex, new_mode) = args.git
        
        (args.file, args.file2) = (old_file, path)
        args.label = [path.name + ' (%s)' % h for h in [old_hex, new_hex]]


class Pager (object):
    __metaclass__ = abc.ABCMeta
    
    
    def accepts_color(self):
        return True
    
    
    @abc.abstractmethod
    def close(self):
        pass
    
    
    @abc.abstractmethod
    def write(self, text):
        pass


class StreamPager (Pager):
    def __init__(self, stream):
        self._stream = codecs.getwriter(locale.getpreferredencoding())(stream)
    
    
    def close(self):
        self._stream.close()
    
    
    def write(self, text):
        self._stream.write(text)


class ProgramPager (StreamPager):
    def __init__(self, command):
        self._process = subprocess.Popen(command, stdin = subprocess.PIPE)
        super(ProgramPager, self).__init__(self._process.stdin)
    
    
    def close(self):
        super(ProgramPager, self).close()
        self._process.communicate()
        self._process.wait()


class DiffPager (ProgramPager):
    def __init__(self):
        super(DiffPager, self).__init__(['kompare', '-o', '-'])
    
    
    def accepts_color(self):
        return False


class TextPager (ProgramPager):
    def __init__(self, tab_size = 4):
        super(TextPager, self).__init__([
            'less',
            '--clear-screen',
            '--RAW-CONTROL-CHARS',
            '--tabs=%d' % tab_size,
        ])


class AutomaticPager (Pager):
    def __init__(self, source_name, max_inline_lines, diff_mode):
        self._source_name = source_name
        self._max_inline_lines = max_inline_lines
        self._diff_mode = diff_mode
        
        self._output = None
        self._inline_buffer = ''
        self._inline_lines = 0
        
        self._line_separator = '\n'
        self._ansi_color_escape = r'\x1B\[\d+(;\d+)*m'
    
    
    def close(self):
        if self._output is None:
            self._setup_output(self._inline_buffer)
            self._display(self._inline_buffer)
        
        self._output.close()
    
    
    def write(self, text):
        text = re.sub(self._ansi_color_escape, '', text)
        
        if self._output is None:
            self._inline_buffer += text
            self._inline_lines += text.count(self._line_separator)
            
            if self._inline_lines <= self._max_inline_lines:
                return
            
            text = self._inline_buffer
            self._setup_output(text)
        
        self._display(text)
    
    
    def _display(self, text):
        self._output.write(pygments.highlight(text,
            self._lexer, self._formatter))
    
    
    def _guess_lexer(self, text):
        if self._diff_mode:
            lexer = pygments.lexers.DiffLexer()
        else:
            try:
                lexer = pygments.lexers.guess_lexer_for_filename(
                    self._source_name, text)
            except pygments.util.ClassNotFound:
                lexer = pygments.lexers.guess_lexer(text)
        
        lexer.add_filter('codetagify')
        return lexer
    
    
    def _setup_output(self, text):
        lexer = self._guess_lexer(text)
        
        if self._inline_lines <= self._max_inline_lines:
            self._output = StreamPager(sys.stdout)
        elif self._diff_mode or isinstance(lexer, pygments.lexers.DiffLexer):
            self._output = DiffPager()
        else:
            self._output = TextPager()
        
        if self._output.accepts_color():
            self._lexer = lexer
            self._formatter = pygments.formatters.Terminal256Formatter()
        else:
            self._lexer = pygments.lexers.TextLexer()
            self._formatter = pygments.formatters.NullFormatter()


args = Arguments().parse_args()
pager = AutomaticPager(args.file.name, 15, args.diff_mode)

for line in args.file:
    pager.write(line)

args.file.close()
pager.close()
