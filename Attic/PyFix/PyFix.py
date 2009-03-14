# -*- coding: utf-8 -*-


import abc, fnmatch, os, types, zipfile


abstract = abc.abstractmethod
AbstractType = abc.ABCMeta

NaN = float('NaN')
Infinity = float('inf')

Boolean = types.BooleanType
CharSequence = basestring
Float = types.FloatType
Function = types.FunctionType
Integer = types.IntType
List = types.ListType
Map = types.DictionaryType
Object = types.ObjectType
Set = set
String = types.StringType
Tuple = types.TupleType
Type = types.TypeType


class Path:
    HOME = os.path.expanduser('~')
    
    
    @staticmethod
    def create(path):
        if not os.path.exists(path):
            os.makedirs(path)
    
    
    @staticmethod
    def glob(filter, root = os.path.curdir, levels = Infinity):
        """
        Flexible recursive version of the standard
        U{glob<http://docs.python.org/library/glob.html#glob.glob>} function.
        
        @type filter: Function, CharSequence
        @param filter: a function that checks whether a path should be included
               or a string describing the Unix-style glob pattern
        @type root: CharSequence
        @param root: directory in which to start searching
        @type levels: Integer
        @param levels: maximum number of nested directories to search
        @rtype: List
        @return: list with all collected paths
        """
        
        names = []
        
        if isinstance(filter, CharSequence):
            pattern = filter
            filter = lambda name: fnmatch.fnmatch(name, pattern)
        
        if levels > 0:
            for name in os.listdir(root):
                path = os.path.join(root, name)
                
                if os.path.isdir(path):
                    names.extend(Path.glob(filter, path, levels - 1))
                if filter(path):
                    names.append(path)
        
        return names


class Sequence:
    @staticmethod
    def flatten(sequence):
        """
        Removes all nested sequences.
        
        @type sequence: List, Set, Tuple
        @param sequence: sequence for which to remove sub-sequences
        @rtype: List, Set, Tuple
        @return: one-dimensional version of the given sequence
        """
        
        values = []
        
        for element in sequence:
            if isinstance(element, (List, Set, Tuple)):
                values.extend(Sequence.flatten(element))
            else:
                values.append(element)
        
        return type(sequence)(values)
    
    
    @staticmethod
    def join(sequence, separator = ''):
        """
        Concatenates a sequence.
        
        @type sequence: List, Set, Tuple
        @param sequence: sequence with the values to concatenate
        @type separator: CharSequence
        @param separator: string to use as the separator
        @rtype: CharSequence
        @return: string formed by joining all sequence elements
        """
        
        return separator.join(map(String, sequence))


class Zip:
    @staticmethod
    def extract_all(file, path = os.path.curdir, members = None, password = None):
        zip = file
        
        if isinstance(file, CharSequence):
            zip = zipfile.ZipFile(file, 'r')
        
        if members is None:
            members = zip.namelist()
        
        # Place directories first to create them before extracting files.
        members.sort()
        
        for name in members:
            if name.endswith('/'):
                Path.create(os.path.join(path, name))
            else:
                zip.extract(name, path, password)
        
        if isinstance(file, CharSequence):
            zip.close()
