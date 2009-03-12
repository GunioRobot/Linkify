# -*- coding: utf-8 -*-

# To do:
# - Enumerations: <http://code.activestate.com/recipes/413486/>
# - Binary operators (e.g. instanceof): <http://code.activestate.com/recipes/384122/>
# - Named tuples: <http://code.activestate.com/recipes/500261/>


# Standard library:
import abc, re, types

# External libraries:
import multidispatch    # PyMultimethods.


Method = multidispatch.multimethod

AbstractMetaClass = abc.ABCMeta
AbstractMethod = abc.abstractmethod

Boolean = types.BooleanType
CharSequence = basestring
Float = types.FloatType
Function = types.FunctionType
Integer = types.IntType
List = types.ListType
Long = types.LongType
Map = types.DictionaryType
Object = types.ObjectType
Set = set
String = types.StringType
Tuple = types.TupleType
Type = types.TypeType

try:
    Complex = types.ComplexType
except NameError:
    pass

try:
    UnicodeString = types.UnicodeType
except NameError:
    pass


@Method(List)
def flatten(sequence):
    """
    Removes all nested sequences.
    
    @type sequence: List
    @param sequence: sequence for which to remove sub-sequences
    @rtype: List
    @return: one-dimensional version of the given sequence
    """
    
    if is_empty(sequence):
        return []
    elif isinstance(sequence[0], List):
        return flatten(sequence[0]) + flatten(sequence[1:])
    else:
        return [sequence[0]] + flatten(sequence[1:])


def identity(value):
    """
    Returns its sole argument.
    """
    
    return value


@Method(CharSequence)
@Method(List)
@Method(Map)
@Method(Tuple)
def is_empty(container):
    """
    Checks if a container is empty.
    
    @type container: CharSequence, List, Map, Tuple
    @param container: container for which to check if it contains no elements
    @rtype: Boolean
    @return: True if the container has no elements or False otherwise
    """
    
    return len(container) == 0


@Method(List, CharSequence)
def join(sequence, separator = ''):
    """
    Concatenates a sequence.
    
    @type sequence: List
    @param sequence: sequence with the values to concatenate
    @type separator: CharSequence
    @param separator: string to use as the separator
    @rtype: CharSequence
    @return: string formed by joining all sequence elements
    """
    
    return separator.join(map(String, sequence))


@Method(CharSequence, CharSequence)
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


@Method(CharSequence, CharSequence, CharSequence)
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


@Method(CharSequence, CharSequence)
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
    
    if is_empty(regex):
        return map(identity, string)
    else:
        return re.split(re.compile(regex), string)
