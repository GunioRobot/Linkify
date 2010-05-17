# -*- coding: utf-8 -*-


"""
@author: Márcio Faustino
@author: Pedro Garcia Lopez
"""


# Internal modules:
import chord


n1 = chord.Node(1)
n2 = chord.Node(8)
n3 = chord.Node(14)
n4 = chord.Node(21)
n5 = chord.Node(32)
n6 = chord.Node(38)
n7 = chord.Node(42)
n8 = chord.Node(48)
n9 = chord.Node(51)
n10 = chord.Node(56)

n1.join(n1)
n2.join(n1)
n3.join(n1)
n4.join(n1)
n5.join(n1)
n6.join(n1)
n7.join(n1)
n8.join(n1)
n9.join(n1)
n10.join(n1)

n1.show_finger()
n2.show_finger()
n3.show_finger()
n1.print_nodes()

found = n1.find_successor(40)
print 'found:', found.ident

#toPajek(n1,'michord.net')
#toPajek2(n1,'michord2.net')
#print 'finish !!!'
