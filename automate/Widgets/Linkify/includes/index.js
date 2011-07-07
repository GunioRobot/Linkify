var httpHandler = {
    pattern: /http:\/\/\w+\.\w+\.\w+/,
    replacement: function (url) {
        var anchor = document.createElement('a');
        
        anchor.href = url;
        anchor.textContent = url;
        
        return anchor;
    }
};


var mailtoHandler = {
    pattern: /mailto:\s*\w+/,
    replacement: function (url) {
        var anchor = document.createElement('a');
        
        anchor.href = url;
        anchor.textContent = url;
        
        return anchor;
    }
};


var options = {
    excludedTags: /^(a|applet|area|button|embed|frame|frameset|iframe|img|map|object|option|param|script|select|style|textarea)$/i,
    handlers: [httpHandler, mailtoHandler]
};


Array.from = function(object) {
    return Array.prototype.map.call(object, function (element) {
        return element;
    });
};


function addLinksToElement(element, options) {
    if (options.excludedTags.test(element.tagName)) {
        return;
    }
    
    var nodes = element.childNodes;
    
    for (var i = 0; i < nodes.length; ++i) {
        var node = nodes.item(i);
        
        if (node instanceof window.Element) {
            addLinksToElement(node, options);
        }
        else if (node instanceof window.Text) {
            var subNodes = splitTextNode(node, options);
            
            if (subNodes.length > 0) {
                for (var j = 0; j < subNodes.length; ++j) {
                    element.insertBefore(subNodes[j], node);
                }
                
                element.removeChild(node);
                i += subNodes.length;
            }
        }
    }
}


function splitTextNode(textNode, options) {
    var nodes = [textNode];
    
    for (var i = 0; i < options.handlers.length; ++i) {
        var handler = options.handlers[i];
        var pattern = handler.pattern.source;
        
        for (var j = 0; j < nodes.length; ++j) {
            var node = nodes[j];
            
            if (node instanceof window.Text) {
                var newNodes = [];
                var textParts = node.nodeValue.split(
                    RegExp('(' + pattern + ')'));
                
                if ((textParts.length == 1)
                    && (textParts[0] == node.nodeValue))
                {
                    continue
                }
                
                for (var l = 0; l < textParts.length; ++l) {
                    var text = textParts[l];
                    
                    if (text.length > 0) {
                        if ((l % 2) == 0) {
                            newNodes.push(document.createTextNode(
                                text));
                        }
                        else {
                            newNodes.push(handler.replacement.call(
                                handler, text));
                        }
                    }
                }
                
                if (newNodes.length > 0) {
                    nodes.splice.apply(nodes,
                        [j, 1].concat(newNodes));
                    
                    j += newNodes.length;
                }
            }
        }
    }
    
    if ((nodes.length == 1) && (nodes[0] === textNode)) {
        return [];
    }
    else {
        return nodes;
    }
}


function log(/* ... */) {
    opera.postError(Array.from(arguments).join(' '));
}


document.addEventListener('readystatechange', function() {
    log('Update:', document.body);
    addLinksToElement(document.body, options);
    
//    document.addEventListener('DOMNodeInserted', function (event) {
//        log('Update:', event.target);
//        addLinksToElement(event.target, options);
//    }, false);
}, false);
