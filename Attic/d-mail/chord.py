# -*- coding: utf-8 -*-


"""
@author: Pedro Garcia Lopez
"""


# Standard library:
import random


BITS = 6
MAX_NODES = 2 ** BITS


def decr(value, size):
    if size <= value:
        return value - size
    else:
        return MAX_NODES - (size - value)


def between(value, init, end):
    if init == end:
        return True
    elif init > end:
        shift = MAX_NODES - init
        init = 0
        end = (end + shift) % MAX_NODES
        value = (value + shift) % MAX_NODES
    
    return (init < value) and (value < end)


def Ebetween(value, init, end):
    if value == init:
        return True
    else:
        return between(value, init, end)


def betweenE(value, init, end):
    if value == end:
        return True
    else:
        return between(value, init, end)


class Node:
    def __init__(self, ident):
        self.ident = ident
        self.finger = {}
        self.start = {}
        
        for i in xrange(BITS):
            self.start[i] = (self.ident + (2 ** i)) % (2 ** BITS)
    
    
    def successor(self):
        return self.finger[0]
    
    
    def find_successor(self, ident):
        if betweenE(ident, self.predecessor.ident, self.ident):
            return self
        
        return self.find_predecessor(ident).successor()
    
    
    def find_predecessor(self, ident):
        if ident == self.ident:
            return self.predecessor
        
        n1 = self
        
        while not betweenE(ident, n1.ident, n1.successor().ident):
            n1 = n1.closest_preceding_finger(ident)
        
        return n1
    
    
    def closest_preceding_finger(self, ident):
        for i in xrange(BITS - 1, -1, -1):
            if between(self.finger[i].ident, self.ident, ident):
                return self.finger[i]
        
        return self
    
    
    def join(self, n1):
        if self == n1:
            for i in xrange(BITS):
                self.finger[i] = self
            self.predecessor = self
        else:
            self.init_finger_table(n1)
            self.update_others()  
            # Move keys.
    
    
    def init_finger_table(self, n1):
        self.finger[0] = n1.find_successor(self.start[0])
        self.predecessor = self.successor().predecessor
        self.successor().predecessor = self
        self.predecessor.finger[0] = self
        
        for i in xrange(BITS - 1):
            if Ebetween(self.start[i + 1], self.ident, self.finger[i].ident):
                self.finger[i + 1] = self.finger[i]
            else :
                self.finger[i + 1] = n1.find_successor(self.start[i + 1])
    
    
    def update_others(self):
        for i in xrange(BITS):
            prev = decr(self.ident, 2 ** i)
            p = self.find_predecessor(prev)
            
            if prev == p.successor().ident:
                p = p.successor()
            
            p.update_finger_table(self, i)
    
    
    def update_finger_table(self, s, i):
        if Ebetween(s.ident, self.ident, self.finger[i].ident) and self.ident != s.ident:
            self.finger[i] = s
            p = self.predecessor
            p.update_finger_table(s, i)
    
    
    def update_others_leave(self):
        for i in xrange(BITS):
            prev = decr(self.ident, 2 ** i)
            p = self.find_predecessor(prev)
            p.update_finger_table(self.successor(), i)
    
    
    # Unchecked.
    def leave(self):
        self.successor().predecessor = self.predecessor
        self.predecessor.setSuccessor(self.successor())
        self.update_others_leave()
    
    
    def setSuccessor(self, succ):
        self.finger[0] = succ


def hash_key(line):
    import sha
    key = long(sha.new(line).hexdigest(), 16)
    return key


def ident():
    return long(random.uniform(0, 2 ** BITS))


def printNodes(node):
    print ' Ring nodes:'
    end = node
    print node.ident
    
    while end != node.successor():
        node = node.successor()
        print node.ident
    
    print '-----------'


def showFinger(node):
    print 'Finger table of node', node.ident
    print 'start:node'
    
    for i in xrange(BITS):
        print node.start[i], ':', node.finger[i].ident
    
    print '-----------'
