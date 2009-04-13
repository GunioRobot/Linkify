# -*- coding: utf-8 -*-


import fnmatch, functools, glob, math, os, shutil, types


_NaN = float('NaN')
_Infinity = float('inf')


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


functools.flatten = _flatten
glob.rglob = _glob

math.NaN = _NaN
math.Infinity = _Infinity

os.path.HOME = os.path.expanduser('~')
os.path.make = _make_path
os.path.remove = _remove_path

types.BaseStringType = basestring
types.SetType = set
