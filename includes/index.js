/**
 * @fileOverview Adds event listeners for page loads and DOM updates.
 */


/**
 * @private
 */
Array.from = function(object) {
    return Array.prototype.map.call(object, function (element) {
        return element;
    });
};


/**
 * @private
 */
Function.prototype.subClass = function(BaseClass, methods) {
    // Unless an intermediate class is used, the prototype of the sub class
    // would also be the same of the base class. If so, any changes in the
    // prototype of the sub class would also be reflected in the base class.
    function InheritanceLink() {
    }

    InheritanceLink.prototype = BaseClass.prototype;
    this.baseClass = InheritanceLink.prototype;
    this.prototype = new InheritanceLink();
    this.prototype.constructor = this;

    if (methods != undefined) {
        for (method in methods) {
            this.prototype[method] = methods[method];
        }
    }

    return this;
};


/**
 * @class Text handler abstract base class.
 */
function Handler() {
    /**
     * Search pattern.
     *
     * @type RegExp
     * @default null
     */
    this.pattern = null;
}


/**
 * Creates an HTML anchor element as the replacement node.
 *
 * @param {String} href link
 * @param {String} [caption=href] caption text
 * @returns {HTMLAnchorElement}
 */
Handler.prototype.replacement = function (href, caption) {
    var anchor = document.createElement('a');

    anchor.href = href;
    anchor.textContent = (caption == undefined) ? href : caption;

    return anchor;
};


/**
 * @class URL style text handler.
 * @augments Handler
 */
function UrlHandler() {
    /**
     * Whether or not URL's without a protocol should be modified to use HTTP.
     *
     * @default true
     * @type Boolean
     */
    this.forceAbsoluteUrls = true;

    this.protocolPattern = /[a-z][\w-]+:(?:\/{1,3}|[a-z0-9%](?![\w-]*\s*=\s*"))/i;
    this.domainPattern = RegExp(
        /(?:www\d{0,3}\.)|(?:[a-z0-9.\-]+\.[a-z]{2,4}\/)|/.source
            + this.constructor.ipv4AddressPattern.source + /(?::\d+)?\//.source,
        'i');

    /**
     * URL search pattern.
     *
     * @type RegExp
     * @see http://daringfireball.net/2010/07/improved_regex_for_matching_urls
     */
    this.pattern = RegExp(
        '\\b(?:'
            + this.protocolPattern.source
            + '|'
            + this.domainPattern.source
            + ')'
            + /(?:[^\s()<>"]+|\((?:[^\s()<>"]+|(?:\([^\s()<>"]+\)))*\))+(?:\((?:[^\s()<>"]+|(?:\([^\s()<>"]+\)))*\)|[^\s()<>"\[\]{};:'`.,!?«»“”‘’])/.source,
        'gi');
};


/**
 * @see http://search.cpan.org/perldoc?Regexp::Common::net
 */
UrlHandler.ipv4AddressPattern = /(?:25[0-5]|2[0-4][0-9]|[0-1]?[0-9]{1,2}\.){3}(?:25[0-5]|2[0-4][0-9]|[0-1]?[0-9]{1,2})/;


/**
 * @lends UrlHandler#
 */
UrlHandler.subClass(Handler, {
    /**
     * Creates an HTML anchor element for an URL.
     *
     * @param {String} url URL found
     * @returns {HTMLAnchorElement}
     */
    replacement: function (url) {
        var caption = url;

        if (this.forceAbsoluteUrls
            && !this.protocolPattern.test(url)
            && this.domainPattern.test(url))
        {
            url = 'http://' + url;
        }

        return UrlHandler.baseClass.replacement.call(this, url, caption);
    }
});


/**
 * @class E-mail address text handler.
 * @augments UrlHandler
 */
function EmailAddressHandler() {
    /**
     * E-mail address search pattern.
     *
     * @type RegExp
     * @see http://docs.jquery.com/Plugins/Validation/Methods/email
     */
    this.pattern = RegExp(
        /\b(?:(?:(?:[a-z]|\d|[!#\$%&'\*\+\-\/=\?\^_`{\|}~]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])+(?:\.(?:[a-z]|\d|[!#\$%&'\*\+\-\/=\?\^_`{\|}~]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])+)*)|(?:(?:\x22)(?:(?:(?:(?:\x20|\x09)*(?:\x0d\x0a))?(?:\x20|\x09)+)?(?:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x7f]|\x21|[\x23-\x5b]|[\x5d-\x7e]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(?:\\(?:[\x01-\x09\x0b\x0c\x0d-\x7f]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF]))))*(?:(?:(?:\x20|\x09)*(?:\x0d\x0a))?(?:\x20|\x09)+)?(?:\x22)))/.source
            + '@(?:'
            + /(?:(?:(?:(?:[a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(?:(?:[a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])(?:[a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])*(?:[a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])))\.)+(?:(?:[a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(?:(?:[a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])(?:[a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])*(?:[a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])))+)/.source
            + '|(?:'
            + UrlHandler.ipv4AddressPattern.source
            + '))',
        'gi');
};


/**
 * @lends EmailAddressHandler#
 */
EmailAddressHandler.subClass(UrlHandler, {
    /**
     * Creates an HTML anchor element for an e-mail address.
     *
     * @param {String} email e-mail address found
     * @returns {HTMLAnchorElement}
     */
    replacement: function (email) {
        return UrlHandler.baseClass.replacement.call(this,
            'mailto:' + email, email);
    }
});


/**
 * Recursively applies handlers to text nodes.
 *
 * @param {Element} element root element where to start the conversion
 * @param {Object} options
 * @param {RegExp} options.excludedTags HTML elements to be excluded
 * @param {Handler[]} options.handlers handlers to be applied
 */
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


/**
 * @private
 */
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


/**
 * @private
 */
function splitTextNodeByHandler(textNode, handler) {
    var separator = '\0';
    var matches = [];
    var nodes = [];

    var textParts = textNode.nodeValue.replace(handler.pattern, function () {
        matches.push(handler.replacement.apply(handler, arguments));
        return separator;
    }).split(separator);

    if (matches.length == 0) {
        return nodes;
    }

    while (textParts.length > 0) {
        var text = textParts.shift();
        var match = matches.shift();

        if (text.length > 0) {
            nodes.push(document.createTextNode(text));
        }

        if (match instanceof Array) {
            nodes.push.apply(nodes, match);
        }
        else if (match != undefined) {
            nodes.push(match);
        }
    }

    return nodes;
}


/**
 * @private
 */
function log(/* ... */) {
    opera.postError(Array.from(arguments).join(' '));
}


document.addEventListener('readystatechange', function() {
    /**
     * @see https://addons.opera.com/addons/extensions/details/popup-statusbar/
     */
    var options = {
        excludedTags: /^(?:a|applet|area|button|embed|frame|frameset|head|iframe|img|input|link|map|meta|object|option|param|script|select|statusbar|style|textarea|title)$/i,
        handlers: [new UrlHandler(), new EmailAddressHandler()]
    };

    addLinksToElement(document.documentElement, options);

    document.addEventListener('DOMNodeInserted', function (event) {
        addLinksToElement(event.target, options);
    }, false);
}, false);
