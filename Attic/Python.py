# -*- coding: utf-8 -*-


# Standard library:
from types import *
import re

# External libraries:
import multidispatch as _PyMultimethods


BaseStringType = basestring


def Method(*signature):
    """
    Decorates a function as a multi-method, to support multiple dispatch.
    
    @param signature: function parameter types
    """
    
    return _PyMultimethods.multimethod(*signature)


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


def identity(value):
    """
    Returns its sole argument.
    """
    
    return value


@Method(BaseStringType)
@Method(DictionaryType)
@Method(ListType)
@Method(TupleType)
def is_empty(container):
    """
    Checks if a container is empty.
    
    @type container: BaseStringType, DictionaryType, ListType, TupleType
    @param container: container for which to check if it contains no elements
    @rtype: BooleanType
    @return: True if the container has no elements or False otherwise
    """
    
    return len(container) == 0


@Method(ListType, BaseStringType)
def join(sequence, separator = ''):
    """
    Concatenates a sequence.
    
    @type sequence: ListType
    @param sequence: sequence with the values to concatenate
    @type separator: BaseStringType
    @param separator: string to use as the separator
    @rtype: BaseStringType
    @return: string formed by joining all sequence elements
    """
    
    return separator.join(map(StringType, sequence))


@Method(BaseStringType, BaseStringType)
def matches(regex, string):
    """
    Checks if a string matches a regular expression.
    
    @type regex: BaseStringType
    @param regex: regular expression to use
    @type string: BaseStringType
    @param string: string to match against
    @rtype: BooleanType
    @return: True if the string matched successfully or False otherwise
    """
    
    return re.search(re.compile(regex), string) is not None


@Method(BaseStringType, BaseStringType, BaseStringType)
def replace(regex, replacement, string):
    """
    Replaces all occurrences of a regular expression with a string.
    
    @type regex: BaseStringType
    @param regex: regular expression to use
    @type replacement: BaseStringType
    @param replacement: string to use as the replacement
    @type string: BaseStringType
    @param string: string in which to replace occurrences
    @rtype: BaseStringType
    @return: new string with all occurrences replaced
    """
    
    return re.sub(re.compile(regex), replacement, string)


@Method(BaseStringType, BaseStringType)
def split(string, separator = ''):
    """
    Splits a string into tokens.
    
    A token is a non-empty string not occurring in the separator.
    
    @type string: BaseStringType
    @param string: string to split
    @type separator: BaseStringType
    @param separator: string to use as the separator
    @rtype: ListType
    @return: list of all tokens
    """
    
    if is_empty(separator):
        return map(identity, string)
    else:
        return string.split(separator)
