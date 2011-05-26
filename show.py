#!/usr/bin/env python
# -*- coding: utf-8 -*-


# TODO: Don't display diff for removed files.
# TODO: Take line width and wrapping into account when paging.
# TODO: Follow file automatically if it changes size?
# TODO: Only ask for HTTP basic auth credentials in interactive mode.
# TODO: Clean up exception handling.
#       $ ./show.py -f file ^C ^C
#       $ ./show.py long-file ^C
# TODO: Do a text search if given a directory as the second file (e.g. ack-grep,
#       git-grep).
#       $ ./show.py text .
#       $ ./show.py '\d+' ~
# TODO: Do a file search if given a directory as the fist file (e.g. find).
#       $ ./show.py . '*.txt'
# TODO: Profile time execution.
# TODO: Implement color support on Windows.
# TODO: Simplify logic (e.g. Pager.run, encoding, etc), add documentation.
# TODO: Detect missing programs and provide fallbacks (e.g. KDiff3, Meld).


# Standard library:
from __future__ import division, print_function, unicode_literals
import codecs, difflib, errno, getpass, httplib, inspect, locale, os, re, \
    StringIO, struct, subprocess, sys, time, urllib2

# Internal modules:
from defaults import *


externals('argparse', 'chardet', 'filelike',
    'pygments', 'pygments.formatters', 'pygments.lexers')


class InputType (argparse.FileType):
    def __init__(self):
        super(InputType, self).__init__()
        self._password_manager = urllib2.HTTPPasswordMgrWithDefaultRealm()
    
    
    def __call__(self, path, *args):
        try:
            return super(InputType, self).__call__(path, *args)
        except IOError as error:
            if error.errno == errno.ENOENT:
                for url in [path, 'http://' + path]:
                    try:
                        return self._open_url(url)
                    except (IOError, httplib.InvalidURL):
                        pass
                
                try:
                    return self._open_perldoc(path)
                except IOError:
                    pass
                
                if path == 'self':
                    return file(inspect.getfile(InputType))
            
            raise error
    
    
    def _open_perldoc(self, module):
        identifier = r'^[A-Z_a-z][0-9A-Z_a-z]*(?:::[0-9A-Z_a-z]+)*$'
        error_message = 'Not a Perl module: '
        process = None
        
        if not re.match(identifier, module):
            raise IOError(error_message + module)
        
        for implementation in ['perldoc', 'perldoc.bat']:
            try:
                process = subprocess.Popen([implementation, module],
                    stderr = file(os.devnull),
                    stdout = subprocess.PIPE)
                
                break
            except OSError as error:
                if error.errno != errno.ENOENT:
                    raise
        
        if process is None:
            raise IOError(str(OSError(errno.ENOENT)))
        
        if process.wait() == 0:
            output = filelike.wrappers.FileWrapper(process.stdout)
            output.name = module
            return output
        else:
            raise IOError(error_message + module)
    
    
    def _open_auth_url(self, url, error):
        while True:
            password_manager = self._password_manager
            (user, password) = password_manager.find_user_password(
                None, url)
            
            if (user is None) and (password is None):
                print(str(error) + ': ' + url, file = sys.stderr)
                user = raw_input('User: ')
                password = getpass.getpass()
                password_manager = urllib2.HTTPPasswordMgrWithDefaultRealm()
                password_manager.add_password(None, url, user, password)
            
            handler = urllib2.HTTPBasicAuthHandler(password_manager)
            request = urllib2.Request(url)
            
            try:
                stream = urllib2.build_opener(handler).open(request)
                stream.name = url
                
                self._password_manager.add_password(None, url, user, password)
                return stream
            except urllib2.HTTPError as error:
                if error.code != httplib.UNAUTHORIZED:
                    raise
    
    
    def _open_url(self, url):
        try:
            return filelike.open(url)
        except IOError:
            pass
        
        try:
            return urllib2.urlopen(url)
        except urllib2.HTTPError as error:
            if error.code == httplib.UNAUTHORIZED:
                return self._open_auth_url(url, error)
            else:
                raise
        except ValueError as (error,):
            if error.startswith('unknown url type: '):
                raise httplib.InvalidURL(url)
            else:
                raise


class Arguments (argparse.ArgumentParser):
    def __init__(self):
        super(Arguments, self).__init__(
            description = 'Automatic pager with syntax highlighting and diff\
                support.',
            epilog = '''An input can be '-' for standard input (default), a\
                file path, an URL, a Perl module name, or 'self' for the\
                source code.''')
        
        self._input_type = InputType()
        
        arguments = [
            ('-f', {
                b'dest': 'follow',
                b'action': 'store_true',
                b'default': False,
                b'help': 'follow file like tail, and disable paging',
            }),
            ('-L', {
                b'dest': 'label',
                b'action': 'append',
                b'help': 'diff labels',
            }),
            ('-u', {
                b'action': 'store_const',
                b'const': None,
                b'help': 'ignored for diff compatibility',
            }),
            ('input', {
                b'nargs': '?',
                b'default': sys.stdin,
                b'help': 'input to display, or Git diff file path',
            }),
            ('input2', {
                b'nargs': '?',
                b'type': self._input_type,
                b'help': 'input to compare with, or current Git file version',
            }),
        ]
        
        git_arguments = [
            ('old_hex', {
                b'nargs': '?',
                b'help': 'current Git file commit',
            }),
            ('old_mode', {
                b'nargs': '?',
                b'help': 'current Git file mode',
            }),
            ('new_file', {
                b'nargs': '?',
                b'type': self._input_type,
                b'help': 'working copy Git file version',
            }),
            ('new_hex', {
                b'nargs': '?',
                b'help': 'working copy Git file commit',
            }),
            ('new_mode', {
                b'nargs': '?',
                b'help': 'working copy Git file mode',
            }),
        ]
        
        git_group = self.add_argument_group(
            title = 'Git external diff arguments')
        
        for (group, args) in [(self, arguments), (git_group, git_arguments)]:
            for name, options in args:
                group.add_argument(name, **options)
    
    
    def parse_args(self):
        args = super(Arguments, self).parse_args()
        
        if args.new_file is not None:
            self._parse_git_diff_arguments(args)
        elif isinstance(args.input, basestring):
            args.input = self._input_type(args.input)
        
        if args.input2 is None:
            args.diff_mode = False
        else:
            args.diff_mode = True
            self._parse_diff_arguments(args)
        
        return args
    
    
    def _parse_diff_arguments(self, args):
        labels = args.label if args.label is not None else \
            [self._resolve_path(input) for input in args.input, args.input2]
        
        diff = ''.join(difflib.unified_diff(
            [self._to_unicode(line) for line in args.input.readlines()],
            [self._to_unicode(line) for line in args.input2.readlines()],
            *labels))
        
        args.input = StringIO.StringIO(
            'diff -u %s %s\n' % tuple(labels) + diff)
    
    
    def _parse_git_diff_arguments(self, args):
        path = self._resolve_path(args.input)
        args.label = [path, path]
        (args.input, args.input2) = (args.input2, args.new_file)
    
    
    def _resolve_path(self, stream):
        if isinstance(stream, basestring):
            return os.path.realpath(stream)
        elif stream is sys.stdin:
            return stream.name
        else:
            path = os.path.realpath(stream.name)
            return path if os.path.exists(path) else stream.name
    
    
    def _to_unicode(self, string):
        if isinstance(string, unicode):
            return string
        
        try:
            return unicode(string, locale.getpreferredencoding())
        except UnicodeDecodeError:
            encoding = chardet.detect(string)['encoding']
            return unicode(string, encoding, 'replace')


class Reader (object):
    __metaclass__ = ABCMeta
    ansi_color_escape = r'\x1B\[(\d+(;\d+)*)?m'
    
    
    @property
    def accepts_color(self):
        return True
    
    
    @abstractmethod
    def close(self):
        pass
    
    
    @abstractmethod
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
            text = re.sub(self.ansi_color_escape, '', text)
        
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
        
        if not self.detached:
            self._process.communicate()
    
    
    @property
    def detached(self):
        return False


class DiffReader (ProgramReader):
    def __init__(self):
        super(DiffReader, self).__init__(['kompare', '-o', '-'],
            stderr = file(os.path.devnull))
    
    
    @property
    def accepts_color(self):
        return False
    
    
    @property
    def detached(self):
        return True


class TextReader (ProgramReader):
    def __init__(self):
        try:
            super(TextReader, self).__init__(['less'])
            self._accepts_color = True
        except NotImplementedError:
            super(TextReader, self).__init__(['cmd', '/C', 'more'])
            self._accepts_color = False
    
    
    @property
    def accepts_color(self):
        return self._accepts_color


class Pager (Reader):
    def __init__(self, input, diff_mode, follow):
        self._input = input
        self._diff_mode = diff_mode
        self._follow = follow
        
        self._buffer = ''
        self._buffered_lines = 0
        self._output = None
        
        self._line_separator = '\n'
        self._inline_lines_threshold = 0.375
    
    
    def __iter__(self):
        (detected, encoding) = (False, locale.getpreferredencoding())
        
        for line in self._input:
            try:
                if not isinstance(line, unicode):
                    line = line.decode(encoding)
                
                yield self._clean_input(line)
            except UnicodeDecodeError:
                if detected:
                    raise
                
                text = self._buffer.encode() + line
                (detected, encoding) = (True, chardet.detect(text)['encoding'])
                yield self._clean_input(line.decode(encoding))
        
        if self._follow:
            (text, self._buffer) = (self._buffer, '')
            self._setup_output(text)
            self._display(text)
            
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
        
        if self._buffer != '':
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
            
            (text, self._buffer) = (self._buffer, '')
            self._setup_output(text)
        
        self._display(text)
    
    
    def _clean_input(self, text):
        # Clean up the backspace control character.
        return re.sub(r'.\x08', '', text)
    
    
    def _display(self, text):
        self._output.write(pygments.highlight(text,
            self._lexer, self._formatter))
    
    
    def _guess_lexer(self, text):
        if self._diff_mode:
            return pygments.lexers.DiffLexer(stripnl = False)
        else:
            clean_text = re.sub(self.ansi_color_escape, '', text)
            
            try:
                return pygments.lexers.guess_lexer(clean_text, stripnl = False)
            except TypeError:
                # See <http://bitbucket.org/birkenfeld/pygments-main/issue/618/>
                # $ echo .text | pygmentize -g
                pass
            
            try:
                return pygments.lexers.guess_lexer_for_filename(
                    self._input.name, clean_text, stripnl = False)
            except pygments.util.ClassNotFound:
                return pygments.lexers.TextLexer(stripnl = False)
    
    
    def _guess_terminal_size(self):
        def ioctl_GWINSZ(fd):
            import fcntl, termios
            size_data = fcntl.ioctl(fd, termios.TIOCGWINSZ, b'1234')
            (rows, columns) = struct.unpack(b'hh', size_data)
            return (rows, columns)
        
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
            stty = subprocess.Popen(['stty', 'size'],
                stdout = subprocess.PIPE)
            
            (rows, columns) = stty.stdout.read().split()
            return (rows, columns)
        except:
            pass
        
        return (0, 0)
    
    
    @property
    def _max_inline_lines(self):
        if not sys.stdout.isatty() or self._follow:
            return Infinity
        
        (rows, columns) = self._guess_terminal_size()
        return int(round(rows * self._inline_lines_threshold))
    
    
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
            self._lexer = pygments.lexers.TextLexer(stripnl = False)
        else:
            self._lexer = lexer
            self._lexer.add_filter('codetagify')
        
        self._formatter = pygments.formatters.Terminal256Formatter()


try:
    args = Arguments().parse_args()
except KeyboardInterrupt:
    print()
    sys.exit()
except IOError as error:
    if error.errno in (errno.ENOENT, errno.EISDIR):
        sys.exit(str(error))
    else:
        raise

pager = Pager(args.input, args.diff_mode, args.follow)

try:
    for line in pager:
        pager.write(line)
except KeyboardInterrupt:
    print()
except EOFError:
    pass

pager.close()
