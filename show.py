#!/usr/bin/env python
# -*- coding: utf-8 -*-


# TODO: Support Git diff file add/removal.
# TODO: Follow file automatically if it changes size?
# TODO: Clean up exception handling.
#       $ ./show.py -f file ^C ^C
#       $ ./show.py long-file ^C
# TODO: Do a text search if given a directory as the second file (e.g. ack-grep,
#       git-grep).
#       $ ./show.py text .
#       $ ./show.py '\d+' ~
# TODO: Do a file search if given a directory as the fist file (e.g. find).
#       $ ./show.py . '*.txt'
# TODO: Show the Perl module names when doing a diff.
# TODO: Take line width and wrapping into account when paging.
# TODO: Profile speed execution.


# Internal modules:
import defaults

# Standard library:
import abc, codecs, difflib, errno, locale, os, re, StringIO, struct, \
    subprocess, sys, time, urllib2, urlparse

defaults.externals(
    u'argparse', u'chardet', u'pygments.formatters', u'pygments.lexers')


class InputType (argparse.FileType):
    def __call__(self, path, *args):
        try:
            return super(InputType, self).__call__(path, *args)
        except IOError as error:
            if error.errno == errno.ENOENT:
                try:
                    return self._open_url(path)
                except urllib2.URLError:
                    pass
                
                try:
                    return self._open_perldoc(path)
                except IOError:
                    pass
                
                if path == u'self':
                    return file(__main__.__file__)
            
            raise error
    
    
    def _open_perldoc(self, module):
        identifier = ur'^[A-Z_a-z][0-9A-Z_a-z]*(?:::[0-9A-Z_a-z]+)*$'
        error = u'Not a Perl module: '
        
        if not re.match(identifier, module):
            raise IOError(error + module)
        
        try:
            process = subprocess.Popen([u'perldoc', module],
                stderr = file(os.devnull),
                stdout = subprocess.PIPE)
        except OSError as error:
            if error.errno == errno.ENOENT:
                raise IOError(str(error))
            else:
                raise
        
        if process.wait() == 0:
            return process.stdout
        else:
            raise IOError(error + module)
    
    
    def _open_url(self, url):
        url_like = (urlparse.urlparse(url).scheme == u'') \
            and re.match(ur'^www\.', url, re.IGNORECASE)
        
        if url_like:
            url = u'http://' + url
        
        try:
            stream = urllib2.urlopen(url)
        except ValueError as error:
            if re.match(ur'^unknown url type:', str(error), re.IGNORECASE):
                raise urllib2.URLError(str(error))
            else:
                raise
        
        setattr(stream, u'name', url)
        return stream


class Arguments (argparse.ArgumentParser):
    def __init__(self):
        super(Arguments, self).__init__(
            description = u'Automatic pager with syntax highlighting and diff\
                support.',
            epilog = u'''An input can be '-' for standard input (default), a\
                file path, an URL, a Perl module name, or 'self' for the\
                source code.''')
        
        arguments = [
            (u'-f', {
                u'dest': u'follow',
                u'action': u'store_true',
                u'default': False,
                u'help': u'follow file like tail',
            }),
            (u'-L', {
                u'dest': u'label',
                u'action': u'append',
                u'help': u'diff labels',
            }),
            (u'-u', {
                u'action': u'store_const',
                u'const': None,
                u'help': u'ignored for diff compatibility',
            }),
            (u'file', {
                u'nargs': u'?',
                u'default': sys.stdin,
                u'type': InputType(),
                u'help': u'input to display',
            }),
            (u'file2', {
                u'nargs': u'?',
                u'type': InputType(),
                u'help': u'input to compare with, and switch to diff mode',
            }),
            (u'git', {
                u'nargs': u'*',
                u'help': u'assume git diff arguments, and switch to diff mode',
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
        if args.label is None:
            args.label = [self._resolve_path(f) for f in args.file, args.file2]
        
        args.file = StringIO.StringIO(
            u'diff -u %s\n' % u' '.join(args.label)
            + u''.join(difflib.unified_diff(
                args.file.readlines(), args.file2.readlines(), *args.label)))
    
    
    def _parse_git_diff_arguments(self, args):
        (stream, old_file) = (args.file, args.file2)
        (args.file, args.file2) = (old_file, stream)
        
        path = self._resolve_path(stream)
        args.label = [path, path]
    
    
    def _resolve_path(self, stream):
        if stream is sys.stdin:
            return stream.name
        else:
            path = os.path.realpath(stream.name)
            return path if os.path.exists(path) else stream.name


class Reader (object):
    __metaclass__ = abc.ABCMeta
    ansi_color_escape = ur'\x1B\[(\d+(;\d+)*)?m'
    
    
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
    def __init__(self, command, stderr = None):
        try:
            self._process = subprocess.Popen(command,
                stderr = stderr,
                stdin = subprocess.PIPE)
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
        super(DiffReader, self).__init__([u'kompare', u'-o', u'-'],
            stderr = file(os.path.devnull))
    
    
    @property
    def accepts_color(self):
        return False


class TextReader (ProgramReader):
    def __init__(self):
        super(TextReader, self).__init__([u'less'])


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
                yield self._clean_input(line.decode(encoding))
            except UnicodeDecodeError:
                if detected:
                    raise
                
                text = self._buffer.encode() + line
                (detected, encoding) = (True, chardet.detect(text)[u'encoding'])
                yield self._clean_input(line.decode(encoding))
        
        if self._follow:
            while True:
                line = self._input.readline()
                
                if len(line) == 0:
                    previous_size = os.path.getsize(self._input.name)
                    time.sleep(1)
                    
                    if os.path.getsize(self._input.name) < previous_size:
                        self._input.seek(0)
                else:
                    yield self._clean_input(line.decode(encoding))
        
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
    
    
    def _clean_input(self, text):
        # Clean up the backspace control character.
        return re.sub(ur'.\x08', u'', text)
    
    
    def _display(self, text):
        self._output.write(pygments.highlight(text,
            self._lexer, self._formatter))
    
    
    def _guess_lexer(self, text):
        if self._diff_mode:
            return pygments.lexers.DiffLexer()
        else:
            clean_text = re.sub(self.ansi_color_escape, u'', text)
            
            try:
                return pygments.lexers.guess_lexer(clean_text)
            except TypeError:
                # See <http://bitbucket.org/birkenfeld/pygments-main/issue/618/>
                # $ echo .text | pygmentize -g
                pass
            
            try:
                return pygments.lexers.guess_lexer_for_filename(
                    self._input.name, clean_text)
            except pygments.util.ClassNotFound:
                return pygments.lexers.TextLexer()
    
    
    def _guess_terminal_height(self):
        def ioctl_GWINSZ(fd):
            import fcntl, termios
            size_data = fcntl.ioctl(fd, termios.TIOCGWINSZ, u'1234')
            (rows, columns) = struct.unpack(u'hh', size_data)
            return rows
        
        for stream in sys.stdin, sys.stdout, sys.stderr:
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
            stty = subprocess.Popen([u'stty', u'size'], stdout = subprocess.PIPE)
            (rows, columns) = stty.stdout.read().split()
            return rows
        except:
            pass
        
        return 0
    
    
    @property
    def _max_inline_lines(self):
        if not sys.stdout.isatty():
            return defaults.Infinity
        
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
            self._lexer.add_filter(u'codetagify')
        
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
