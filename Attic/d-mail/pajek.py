def node2graph(node):
    graph = {}
    addNode(graph, node)
    current = node.successor()
    
    while current != node:    
        graph[current.id] = []
        addNode(graph, current)
        current = current.successor()
    
    return graph


def addNode(graph, node):
    graph[node.id] = []
    
    for n in node.finger.values():
        graph[node.id].append(n.id)


def toPajek(node, filename):
    graph = node2graph(node)
    f = open(filename, 'w')
    size = len(graph)
    f.write('*Vertices ' + str(size) + '\n')
    
    for i in range(size):
        f.write(' ' + str(i + 1) + ' "' + str(graph.keys()[i]) + '"\n')
    
    f.write('*Arcs\n')
    
    for i in graph:
        for conn in graph[i]:
            f.write(' ' + str(graph.keys().index(i) + 1) + ' ' + str(graph.keys().index(conn) + 1) + ' 1\n')
    
    f.close()


def dec2bin2(dec, top):
    result = ''
    remainder = dec
    
    if dec == 0:
        for i in range(top):
            result += '0'
        return result
    
    for i in range(top - 1, -1, -1):
        result += str(remainder / 2 ** i)
        remainder = remainder % 2 ** i
    
    return result


def toPajek2(node, filename):
    graph = node2graph(node)
    f = open(filename, 'w')
    size = len(graph)
    f.write('*Vertices ' + str(size) + '\n')
    
    for i in range(size):
        id1 = dec2bin2(graph.keys()[i], k)
        f.write(' ' + str(i + 1) + ' "' + str(id1) + '"\n')
    
    f.write('*Arcs\n')
    
    for i in graph:
        for conn in graph[i]:
            f.write(' ' + str(graph.keys().index(i) + 1) + ' ' + str(graph.keys().index(conn) + 1) + ' 1\n')
    
    f.close()
