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


Array.from = function(object) {
    return Array.prototype.map.call(object, function (element) {
        return element;
    });
};


function addLinksToElement(element, handlers) {
    if (element instanceof window.HTMLAnchorElement) {
        return;
    }
    
    log('Update:', element);
    var nodes = element.childNodes;
    
    for (var i = 0; i < nodes.length; ++i) {
        var node = nodes.item(i);
        
        if (node instanceof window.Element) {
            addLinksToElement(node, handlers);
        }
        else if (node instanceof window.Text) {
            var subNodes = [node];
            
            for (var j = 0; j < handlers.length; ++j) {
                var handler = handlers[j];
                var pattern = handler.pattern.source;
                
                log('Handler:', pattern);
                
                for (var k = 0; k < subNodes.length; ++k) {
                    var subNode = subNodes[k];
                    
                    if (subNode instanceof window.Text) {
                        var newNodes = [];
                        var textParts = subNode.nodeValue.split(
                            RegExp('(' + pattern + ')'));
                        
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
                            subNodes.splice.apply(subNodes,
                                [k, 1].concat(newNodes));
                            
                            k += newNodes.length;
                        }
                    }
                }
            }
            
            for (var j = 0; j < subNodes.length; ++j) {
                element.insertBefore(subNodes[j], node);
            }
            
            element.removeChild(node);
            i += subNodes.length;
        }
    }
}


function log(/* ... */) {
    opera.postError(Array.from(arguments).join(' '));
}


document.addEventListener('readystatechange', function() {
    var handlers = [httpHandler, mailtoHandler];
    addLinksToElement(document.body, handlers);
    
    document.addEventListener('DOMNodeInserted', function (event) {
        addLinksToElement(event.target, handlers);
    }, false);
}, false);
