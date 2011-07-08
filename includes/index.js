Array.from = function(object) {
    return Array.prototype.map.call(object, function (element) {
        return element;
    });
};


function Handler() {
    this.pattern = null;
    
    this.replacement = function (url) {
        var anchor = document.createElement('a');
        
        anchor.href = url;
        anchor.textContent = url;
        
        return anchor;
    };
}


function UrlHandler() {
    /** @see http://daringfireball.net/2010/07/improved_regex_for_matching_urls */
    this.pattern = /\b(?:[a-z][\w-]+:(?:\/{1,3}|[a-z0-9%])|www\d{0,3}[.]|[a-z0-9.\-]+[.][a-z]{2,4}\/)(?:[^\s()<>]+|\((?:[^\s()<>]+|(?:\([^\s()<>]+\)))*\))+(?:\(?:(?:[^\s()<>]+|(?:\([^\s()<>]+\)))*\)|[^\s`!()\[\]{};:'".,<>?«»“”‘’])/i;
};

UrlHandler.prototype = new Handler();


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
        
        for (var j = 0; j < nodes.length; ++j) {
            var node = nodes[j];
            
            if (node instanceof window.Text) {
                var splitNodes = splitTextNodeByHandler(node, handler);
                
                if (splitNodes.length > 0) {
                    nodes.splice.apply(nodes, [j, 1].concat(splitNodes));
                    j += splitNodes.length;
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


function splitTextNodeByHandler(textNode, handler) {
    var nodes = [];
    var textParts = textNode.nodeValue.split(
        RegExp('(' + handler.pattern.source + ')'));
    
    if ((textParts.length == 1) && (textParts[0] === textNode.nodeValue)) {
        return nodes;
    }
    
    for (var i = 0; i < textParts.length; ++i) {
        var text = textParts[i];
        
        if (text.length > 0) {
            if ((i % 2) == 0) {
                nodes.push(document.createTextNode(text));
            }
            else {
                nodes.push(handler.replacement(text));
            }
        }
    }
    
    return nodes;
}


function log(/* ... */) {
    opera.postError(Array.from(arguments).join(' '));
}


opera.extension.addEventListener('message', function(event) {
    var widget = event.data;
    var options = {
        excludedTags: /^(?:a|applet|area|button|embed|frame|frameset|iframe|img|input|map|object|option|param|script|select|style|textarea)$/i,
        handlers: [new UrlHandler()]
    };
    
    document.addEventListener('readystatechange', function() {
        log(widget.name + ':', document.body.nodeName, document.body);
        addLinksToElement(document.body, options);
        
        document.addEventListener('DOMNodeInserted', function (event) {
            log(widget.name + ':', event.target.nodeName, event.target);
            addLinksToElement(event.target, options);
        }, false);
    }, false);
}, false);
