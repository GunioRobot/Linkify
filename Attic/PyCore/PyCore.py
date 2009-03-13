# -*- coding: utf-8 -*-


import abc, fnmatch, os, re, types


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


class Regex:
    @staticmethod
    def matches(regex, string):
        """
        Checks if a string matches a regular expression.
        
        @type regex: CharSequence
        @param regex: regular expression to use
        @type string: CharSequence
        @param string: string to match against
        @rtype: Boolean
        @return: True if the string matched successfully or False otherwise
        """
        
        return re.search(re.compile(regex), string) is not None
    
    
    @staticmethod
    def replace(regex, replacement, string):
        """
        Replaces all occurrences of a regular expression with a string.
        
        @type regex: CharSequence
        @param regex: regular expression to use
        @type replacement: CharSequence
        @param replacement: string to use as the replacement
        @type string: CharSequence
        @param string: string in which to replace occurrences
        @rtype: CharSequence
        @return: new string with all occurrences replaced
        """
        
        return re.sub(re.compile(regex), replacement, string)
    
    
    @staticmethod
    def split(regex, string):
        """
        Splits a string by the occurrences of a regular expression.
        
        @type regex: CharSequence
        @param regex: regular expression to use as the separator
        @type string: CharSequence
        @param string: string to split
        @rtype: List
        @return: list of all sub strings that didn't match the regular expression
        """
        
        if regex == '':
            return map(lambda x: x, string)
        else:
            return re.split(re.compile(regex), string)


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
