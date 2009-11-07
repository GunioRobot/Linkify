# -*- coding: utf-8 -*-


import ConfigParser, fnmatch, functools, glob, math, os, shutil, types


_NaN = float('NaN')
_Infinity = float('inf')


class _CheckBadPercent (object):
    class Result (object):
        def __init__(self, index):
            self.__index = index
        
        
        def start(self, *args, **kargs):
            return self.__index
    
    
    def __init__(self, interpolate_var):
        self.__interpolate_var = interpolate_var
    
    
    def search(self, value, *args, **kargs):
        index = self.__interpolate_var.sub('', value).find('%')
        return False if index < 0 else _CheckBadPercent.Result(index)


class _RemoveDoublePercents (object):
    def __init__(self, interpolate_var):
        self.__interpolate_var = interpolate_var
    
    
    def match(self, *args, **kargs):
        return self.__interpolate_var.match(*args, **kargs)
    
    
    def sub(self, replacement, value, *args, **kargs):
        return value.replace('%%', '')


def _flatten(sequence):
    """
    Removes all nested sequences.
    
    @type sequence: list, set, tuple
    @param sequence: sequence for which to remove nested sequences
    @rtype: list, set, tuple
    @return: one-dimensional version of the given sequence
    """
    
    values = []
    
    for element in sequence:
        if isinstance(element, (list, set, tuple)):
            values.extend(_flatten(element))
        else:
            values.append(element)
    
    return type(sequence)(values)


def _glob(filter, root = os.path.curdir, levels = _Infinity):
    """
    Recursive version of the standard
    U{glob.glob<http://docs.python.org/library/glob.html#glob.glob>} function.
    
    @type filter: basestring, function
    @param filter: a function that checks whether a path should be included
           or a string describing the glob pattern
    @type root: basestring
    @param root: directory in which to start searching
    @type levels: int
    @param levels: maximum number of nested directories to search
    @rtype: list
    @return: list with all collected paths
    """
    
    names = []
    
    if isinstance(filter, basestring):
        pattern = filter
        filter = lambda name: fnmatch.fnmatch(name, pattern)
    
    if levels > 0:
        for name in os.listdir(root):
            path = os.path.join(root, name)
            
            if os.path.isdir(path):
                names.extend(_glob(filter, path, levels - 1))
            if filter(path):
                names.append(path)
    
    return names


def _make_path(path):
    """
    Recursively creates a path, with intermediate directories as needed.
    
    @type path: basestring
    @param path: path to create
    """
    
    try:
        os.makedirs(path)
    except OSError:
        if not os.path.exists(path):
            raise


def _remove_path(path):
    """
    Removes a file or an entire directory tree.
    
    @type path: basestring
    @param path: file or directory to remove
    """
    
    try:
        os.remove(path)
    except OSError:
        shutil.rmtree(path)


ConfigParser.RawConfigParser.optionxform = str
ConfigParser.ConfigParser.optionxform = str
ConfigParser.SafeConfigParser.optionxform = str

try:
    for value in ['%%', '%%(var)s']:
        ConfigParser.SafeConfigParser().set('DEFAULT', 'option', value)
except ValueError:
    SafeParser = ConfigParser.SafeConfigParser
    _interpvar = SafeParser._interpvar_re
    
    SafeParser._badpercent_re = _CheckBadPercent(_interpvar)
    SafeParser._interpvar_re = _RemoveDoublePercents(_interpvar)

functools.flatten = _flatten
glob.rglob = _glob

math.NaN = _NaN
math.Infinity = _Infinity

os.path.HOME = os.path.expanduser('~')
os.path.make = _make_path
os.path.remove = _remove_path

types.BaseStringType = basestring
types.SetType = set
