# -*- coding: utf-8 -*-


"""
@author: Márcio Faustino
@author: Pedro Garcia Lopez
"""


class Node (object):
    BITS = 6
    
    
    @classmethod
    def between(cls, value, begin, end):
        if begin == end:
            return True
        elif begin > end:
            shift = cls.max_nodes() - begin
            begin = 0
            end = (end + shift) % cls.max_nodes()
            value = (value + shift) % cls.max_nodes()
        
        return (begin < value) and (value < end)
    
    
    @classmethod
    def between_begin(cls, value, begin, end):
        if value == begin:
            return True
        else:
            return cls.between(value, begin, end)
    
    
    @classmethod
    def between_end(cls, value, begin, end):
        if value == end:
            return True
        else:
            return cls.between(value, begin, end)
    
    
    @classmethod
    def decr(cls, value, size):
        if size <= value:
            return value - size
        else:
            return cls.max_nodes() - (size - value)
    
    
    @classmethod
    def max_nodes(cls):
        return 2 ** cls.BITS
    
    
    def __init__(self, ident):
        self.ident = ident
        self.finger = {}
        self.start = {}
        
        for i in xrange(self.BITS):
            self.start[i] = (self.ident + (2 ** i)) % (2 ** self.BITS)
    
    
    def closest_preceding_finger(self, ident):
        for i in xrange(self.BITS - 1, -1, -1):
            if self.between(self.finger[i].ident, self.ident, ident):
                return self.finger[i]
        
        return self
    
    
    def find_predecessor(self, ident):
        if ident == self.ident:
            return self.predecessor
        
        n1 = self
        
        while not self.between_end(ident, n1.ident, n1.successor().ident):
            n1 = n1.closest_preceding_finger(ident)
        
        return n1
    
    
    def find_successor(self, ident):
        if self.between_end(ident, self.predecessor.ident, self.ident):
            return self
        
        return self.find_predecessor(ident).successor()
    
    
    def init_finger_table(self, n1):
        self.finger[0] = n1.find_successor(self.start[0])
        self.predecessor = self.successor().predecessor
        self.successor().predecessor = self
        self.predecessor.finger[0] = self
        
        for i in xrange(self.BITS - 1):
            if self.between_begin(self.start[i + 1], self.ident, self.finger[i].ident):
                self.finger[i + 1] = self.finger[i]
            else :
                self.finger[i + 1] = n1.find_successor(self.start[i + 1])
    
    
    def join(self, n1):
        if self == n1:
            for i in xrange(self.BITS):
                self.finger[i] = self
            self.predecessor = self
        else:
            self.init_finger_table(n1)
            self.update_others()  
            # Move keys.
    
    
    # Unchecked.
    def leave(self):
        self.successor().predecessor = self.predecessor
        self.predecessor.set_successor(self.successor())
        self.update_others_leave()
    
    
    def print_nodes(self):
        print ' Ring nodes:'
        end = self
        print self.ident
        
        while end != self.successor():
            self = self.successor()
            print self.ident
        
        print '-----------'
    
    
    def set_successor(self, succ):
        self.finger[0] = succ
    
    
    def show_finger(self):
        print 'Finger table of node', self.ident
        print 'start:node'
        
        for i in xrange(self.BITS):
            print self.start[i], ':', self.finger[i].ident
        
        print '-----------'
    
    
    def successor(self):
        return self.finger[0]
    
    
    def update_finger_table(self, s, i):
        if self.between_begin(s.ident, self.ident, self.finger[i].ident) and (self.ident != s.ident):
            self.finger[i] = s
            p = self.predecessor
            p.update_finger_table(s, i)
    
    
    def update_others(self):
        for i in xrange(self.BITS):
            prev = self.decr(self.ident, 2 ** i)
            p = self.find_predecessor(prev)
            
            if prev == p.successor().ident:
                p = p.successor()
            
            p.update_finger_table(self, i)
    
    
    def update_others_leave(self):
        for i in xrange(self.BITS):
            prev = self.decr(self.ident, 2 ** i)
            p = self.find_predecessor(prev)
            p.update_finger_table(self.successor(), i)
