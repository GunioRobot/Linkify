Array.from = function(object) {
    return Array.prototype.map.call(object, function (element) {
        return element;
    });
};


Function.prototype.subClass = function(BaseClass, methods) {
    // Unless an intermediate class is used, the prototype of the sub class
    // would also be the same of the base class. If so, any changes in the
    // prototype of the sub class would also be reflected in the base class.
    function InheritanceLink() {
    }
    
    InheritanceLink.prototype = BaseClass.prototype;
    this.prototype = new InheritanceLink();
    this.prototype.baseClass = InheritanceLink.prototype;
    this.prototype.constructor = this;
    
    if (methods != undefined) {
        for (method in methods) {
            this.prototype[method] = methods[method];
        }
    }
    
    return this;
};


function Handler() {
    this.pattern = null;
}

Handler.prototype.replacement = function (href, caption) {
    var anchor = document.createElement('a');
    
    anchor.href = href;
    anchor.textContent = (caption == undefined) ? href : caption;
    
    return anchor;
};


function UrlHandler() {
    this.forceAbsoluteUrls = true;
    
    this.ip4Address = /(?:25[0-5]|2[0-4][0-9]|[0-1]?[0-9]{1,2}\.){3}(?:25[0-5]|2[0-4][0-9]|[0-1]?[0-9]{1,2})/;
    
    this.domainPattern = RegExp(
        '(?:www\\d{0,3}\\.)|(?:[a-z0-9.\\-]+\\.[a-z]{2,4}/)|'
            + this.ip4Address.source,
        'i');
    
    this.protocolPattern = /[a-z][\w-]+:(?:\/{1,3}|[a-z0-9%](?![\w-]*\s*=\s*"))/i;
    
    /**
     * @see http://daringfireball.net/2010/07/improved_regex_for_matching_urls
     */
    this.pattern = RegExp(
        '\\b(?:'
            + this.protocolPattern.source
            + '|'
            + this.domainPattern.source
            + ')(?:[^\\s()<>]+|\\((?:[^\\s()<>]+|(?:\\([^\\s()<>]+\\)))*\\))+(?:\\(?:(?:[^\\s()<>]+|(?:\\([^\\s()<>]+\\)))*\\)|[^\\s`!()\\[\\]{};:\'".,<>?«»“”‘’])',
        'i');
};

UrlHandler.subClass(Handler, {
    replacement: function (url) {
        var caption = url;
        
        if (this.forceAbsoluteUrls
            && !this.protocolPattern.test(url)
            && this.domainPattern.test(url))
        {
            url = 'http://' + url;
        }
        
        return this.baseClass.replacement.call(this, url, caption);
    }
});


function EmailAddressHandler() {
    /**
     * @see http://docs.jquery.com/Plugins/Validation/Methods/email
     */
    this.pattern = /\b(?:(?:(?:[a-z]|\d|[!#\$%&'\*\+\-\/=\?\^_`{\|}~]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])+(?:\.(?:[a-z]|\d|[!#\$%&'\*\+\-\/=\?\^_`{\|}~]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])+)*)|(?:(?:\x22)(?:(?:(?:(?:\x20|\x09)*(?:\x0d\x0a))?(?:\x20|\x09)+)?(?:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x7f]|\x21|[\x23-\x5b]|[\x5d-\x7e]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(?:\\(?:[\x01-\x09\x0b\x0c\x0d-\x7f]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF]))))*(?:(?:(?:\x20|\x09)*(?:\x0d\x0a))?(?:\x20|\x09)+)?(?:\x22)))@(?:(?:(?:[a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(?:(?:[a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])(?:[a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])*(?:[a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])))\.)+(?:(?:[a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(?:(?:[a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])(?:[a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])*(?:[a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])))+/i;
};

EmailAddressHandler.subClass(Handler, {
    replacement: function (email) {
        return this.baseClass.replacement.call(this, 'mailto:' + email, email);
    }
});


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


document.addEventListener('readystatechange', function() {
    var options = {
        excludedTags: /^(?:a|applet|area|button|embed|frame|frameset|head|iframe|img|input|link|map|meta|object|option|param|script|select|style|textarea|title)$/i,
        handlers: [new UrlHandler(), new EmailAddressHandler()]
    };
    
    addLinksToElement(document.documentElement, options);
    
    document.addEventListener('DOMNodeInserted', function (event) {
        addLinksToElement(event.target, options);
    }, false);
}, false);
