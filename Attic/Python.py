# -*- coding: utf-8 -*-


# Standard library:
from types import *

# External libraries:
import multidispatch as _PyMultimethods


def Method(*signature):
    """
    Decorates a function as a multi-method, to support multiple dispatch.
    
    @param signature: function parameter types
    """
    
    return _PyMultimethods.multimethod(*signature)


def identity(value):
    """
    Returns its sole argument.
    """
    
    return value


@Method(DictionaryType)
@Method(ListType)
@Method(StringType)
@Method(UnicodeType)
@Method(TupleType)
def is_empty(container):
    """
    Checks if a container is empty.
    
    @type container: DictionaryType, ListType, StringType, UnicodeType, TupleType
    @param container: container for which to check if it contains no elements
    @rtype: BooleanType
    @return: True if the container has no elements or False otherwise
    """
    
    return len(container) == 0


@Method(ListType, StringType)
@Method(ListType, UnicodeType)
def join(sequence, separator = ''):
    """
    Concatenates a sequence.
    
    @type sequence: ListType
    @param sequence: sequence with the values to concatenate
    @type separator: StringType, UnicodeType
    @param separator: string to use as the separator
    @rtype: StringType
    @return: string formed by joining all sequence elements
    """
    
    return separator.join(map(StringType, sequence))


@Method(ListType)
def flatten(sequence):
    """
    Removes all nested sequences.
    
    @type sequence: ListType
    @param sequence: sequence for which to remove sub-sequences
    @rtype: ListType
    @return: one-dimensional version of the given sequence
    """
    
    if is_empty(sequence):
        return []
    elif isinstance(sequence[0], ListType):
        return flatten(sequence[0]) + flatten(sequence[1:])
    else:
        return [sequence[0]] + flatten(sequence[1:])


@Method(StringType, StringType)
@Method(StringType, UnicodeType)
@Method(UnicodeType, StringType)
@Method(UnicodeType, UnicodeType)
def split(string, separator = ''):
    """
    Splits a string into tokens.
    
    A token is a non-empty string not occurring in the separator.
    
    @type string: StringType, UnicodeType
    @param string: string to split
    @type separator: StringType, UnicodeType
    @param separator: string to use as the separator
    @rtype: ListType
    @return: list of all tokens
    """
    
    if is_empty(separator):
        return map(identity, string)
    else:
        return string.split(separator)
