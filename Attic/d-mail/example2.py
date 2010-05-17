# -*- coding: utf-8 -*-


"""
@author: Márcio Faustino
@author: Pedro Garcia Lopez
"""


# Standard library:
import hashlib, random, time

# Internal modules:
import chord


def generate_id():
    return long(random.uniform(0, 2 ** chord.Node.BITS))


def generate_key(data):
    return long(hashlib.sha1(data).hexdigest(), 16)


chord.Node.BITS = 160
t1 = time.time()
nodes = {}

for i in xrange(100):
    nodes[i] = chord.Node(generate_id())
    #print nodes[i].ident

for i in xrange(100):
    nodes[i].join(nodes[0])

t2 = time.time()
print 'Time to create 100 nodes:', t2 - t1

key = generate_key('pedro')
print key
found = nodes[0].find_predecessor(key)
print found.ident
