#!/usr/bin/env python
# -*- coding: utf-8 -*-


# TODO: Support Git diff file add/removal.
# TODO: Follow file automatically if it changes size?
# TODO: Clean up exception handling.
#       $ ./show.py -f file ^C ^C
#       $ ./show.py long-file ^C
# TODO: Show documentation and/or code of a Perl module when given a file name
#       that looks like a namespace and doesn't point to a valid file? E.g.
#       $ ./show.py IO::Handle


# Standard library:
import abc, codecs, errno, locale, os, re, struct, subprocess, sys, time

try:
    import argparse
except ImportError as error:
    sys.exit('argparse is required, see <http://code.google.com/p/argparse/>: %s'
        % error)

# External modules:
try:
    import chardet
except ImportError as error:
    sys.exit('chardet is required, see <http://chardet.feedparser.org/>: %s'
        % error)

try:
    import pygments, pygments.formatters, pygments.lexers
except ImportError as error:
    sys.exit('Pygments is required, see <http://pygments.org/>: %s' % error)


class Arguments (argparse.ArgumentParser):
    def __init__(self):
        super(Arguments, self).__init__(description =
            '''Automatic pager with syntax highlighting and diff support.''')
        
        arguments = [
            ('-f', {
                'dest': 'follow',
                'action': 'store_true',
                'default': False,
                'help': 'follow file like tail',
            }),
            ('-L', {
                'dest': 'label',
                'action': 'append',
                'help': 'diff labels',
            }),
            ('-u', {
                'action': 'store_const',
                'const': None,
                'help': 'ignored for diff compatibility',
            }),
            ('file', {
                'nargs': '?',
                'default': sys.stdin,
                'type': argparse.FileType(),
                'help': 'file to display, otherwise use standard input',
            }),
            ('file2', {
                'nargs': '?',
                'type': argparse.FileType(),
                'help': 'file to compare with, and switch to diff mode',
            }),
            ('git', {
                'nargs': '*',
                'help': 'assume git diff arguments, and switch to diff mode',
            }),
        ]
        
        for name, options in arguments:
            self.add_argument(name, **options)
    
    
    def parse_args(self):
        try:
            args = super(Arguments, self).parse_args()
        except IOError as error:
            if error.errno in (errno.ENOENT, errno.EISDIR):
                sys.exit(str(error))
            else:
                raise
        
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
        diff = ['diff', '-u']
        
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
        args.label = []
        
        for commit in (old_hex, new_hex):
            if re.match(r'^0+$', commit):
                args.label.append('%s (working copy)' % path.name)
            else:
                args.label.append('%s (commit %s)' % (path.name, commit))


class Reader (object):
    __metaclass__ = abc.ABCMeta
    ansi_color_escape = ur'\x1B\[\d+(;\d+)*m'
    
    
    @property
    def accepts_color(self):
        return True
    
    
    @abc.abstractmethod
    def close(self):
        pass
    
    
    @abc.abstractmethod
    def write(self, text):
        pass


class StreamReader (Reader):
    def __init__(self, stream):
        self._stream = codecs.getwriter(locale.getpreferredencoding())(stream)
    
    
    def close(self):
        if self._stream.stream is not sys.stdout:
            self._stream.close()
    
    
    def write(self, text):
        if not self.accepts_color:
            text = re.sub(self.ansi_color_escape, u'', text)
        
        try:
            self._stream.write(text)
        except IOError as error:
            if error.errno == errno.EPIPE:
                raise EOFError()
            else:
                raise


class ProgramReader (StreamReader):
    def __init__(self, command):
        try:
            self._process = subprocess.Popen(command, stdin = subprocess.PIPE)
        except OSError as error:
            if error.errno == errno.ENOENT:
                raise NotImplementedError
            else:
                raise
        
        super(ProgramReader, self).__init__(self._process.stdin)
    
    
    def close(self):
        super(ProgramReader, self).close()
        self._process.communicate()


class DiffReader (ProgramReader):
    def __init__(self):
        super(DiffReader, self).__init__(['kompare', '-o', '-'])
    
    
    @property
    def accepts_color(self):
        return False


class TextReader (ProgramReader):
    def __init__(self, tab_size = 4):
        super(TextReader, self).__init__([
            'less',
            '--clear-screen',
            '--RAW-CONTROL-CHARS',
            '--tabs=%d' % tab_size,
        ])


class Pager (Reader):
    def __init__(self, input, diff_mode, follow):
        self._input = input
        self._diff_mode = diff_mode
        self._follow = follow
        
        self._buffer = u''
        self._buffered_lines = 0
        self._output = None
        
        self._line_separator = u'\n'
        self._inline_lines_threshold = 0.375
    
    
    def __iter__(self):
        (detected, encoding) = (False, locale.getpreferredencoding())
        
        for line in self._input:
            try:
                yield line.decode(encoding)
            except UnicodeDecodeError as error:
                if detected:
                    raise
                
                text = self._buffer.encode() + line
                (detected, encoding) = (True, chardet.detect(text)['encoding'])
                yield line.decode(encoding)
        
        if self._follow:
            while True:
                line = self._input.readline()
                
                if len(line) == 0:
                    previous_size = os.path.getsize(self._input.name)
                    time.sleep(1)
                    
                    if os.path.getsize(self._input.name) < previous_size:
                        self._input.seek(0)
                else:
                    yield line.decode(encoding)
        
        raise StopIteration
    
    
    def close(self):
        self._input.close()
        
        if self._buffer != u'':
            self._setup_output(self._buffer)
            self._display(self._buffer)
        
        if self._output is not None:
            self._output.close()
    
    
    def write(self, text):
        if self._output is None:
            self._buffer += text
            self._buffered_lines += text.count(self._line_separator)
            
            if self._buffered_lines <= self._max_inline_lines:
                return
            
            (text, self._buffer) = (self._buffer, u'')
            self._setup_output(text)
        
        self._display(text)
    
    
    def _display(self, text):
        self._output.write(pygments.highlight(text,
            self._lexer, self._formatter))
    
    
    def _guess_lexer(self, text):
        if self._diff_mode:
            return pygments.lexers.DiffLexer()
        else:
            clean_text = re.sub(self.ansi_color_escape, u'', text)
            
            try:
                return pygments.lexers.guess_lexer_for_filename(
                    self._input.name, clean_text)
            except pygments.util.ClassNotFound:
                pass
            
            try:
                return pygments.lexers.guess_lexer(clean_text)
            except TypeError:
                # See <http://bitbucket.org/birkenfeld/pygments-main/issue/618/>
                # $ echo .text | pygmentize -g
                return pygments.lexers.TextLexer()
    
    
    def _guess_terminal_height(self):
        def ioctl_GWINSZ(fd):
            import fcntl, termios
            size_data = fcntl.ioctl(fd, termios.TIOCGWINSZ, '1234')
            (rows, columns) = struct.unpack('hh', size_data)
            return rows
        
        for stream in (sys.stdin, sys.stdout, sys.stderr):
            try:
                return ioctl_GWINSZ(stream.fileno())
            except:
                continue
        
        try:
            fd = os.open(os.ctermid(), os.O_RDONLY)
            try:
                return ioctl_GWINSZ(fd)
            finally:
                os.close(fd)
        except:
            pass
        
        try:
            stty = subprocess.Popen(['stty', 'size'], stdout = subprocess.PIPE)
            (rows, columns) = stty.stdout.read().split()
            return rows
        except:
            pass
        
        return 25
    
    
    @property
    def _max_inline_lines(self):
        height = self._guess_terminal_height()
        return int(round(height * self._inline_lines_threshold))
    
    
    def _setup_output(self, text):
        lexer = self._guess_lexer(text)
        
        if self._buffered_lines <= self._max_inline_lines:
            self._output = StreamReader(sys.stdout)
        elif self._diff_mode or isinstance(lexer, pygments.lexers.DiffLexer):
            try:
                self._output = DiffReader()
            except NotImplementedError:
                self._output = TextReader()
        else:
            self._output = TextReader()
        
        if re.search(self.ansi_color_escape, text):
            self._lexer = pygments.lexers.TextLexer()
        else:
            self._lexer = lexer
            self._lexer.add_filter('codetagify')
        
        self._formatter = pygments.formatters.Terminal256Formatter()


args = Arguments().parse_args()
pager = Pager(args.file, args.diff_mode, args.follow)

try:
    for line in pager:
        pager.write(line)
except KeyboardInterrupt:
    print
except EOFError:
    pass

pager.close()
