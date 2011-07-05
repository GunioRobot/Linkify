Array.from = function(object) {
    return Array.prototype.map.call(object, function (element) {
        return element;
    });
};


function addLinks() {
    var optimize = true;
    var libraries = [
        'https://raw.github.com/maranomynet/linkify/master/1.0/jquery.linkify-1.0'
            + (optimize ? '-min' : '') + '.js',
    ];
    
    if (window.jQuery == undefined) {
        libraries.unshift(
            'http://code.jquery.com/jquery' + (optimize ? '.min' : '') + '.js');
    }
    
    loadScripts(libraries, function() {
        addLinksToElement(document.body);
        
        document.addEventListener('DOMNodeInserted', function (event) {
            var element = event.target;
            
            if ((element.nodeType == window.Node.ELEMENT_NODE)
                && !(element instanceof window.HTMLAnchorElement))
            {
                addLinksToElement(event.target);
            }
        }, false);
    });
}


function addLinksToElement(element) {
    log('Linkify:', element);
    
    window.jQuery(element).linkify(function (links) {
        links.css({
            color: '#0082E0',
            textDecoration: 'underline'
        });
    });
}


function loadScript(url, onLoad) {
    var script = document.createElement('script');
    
    script.onload = onLoad;
    script.src = url;
    
    log('Load:', url);
    document.body.appendChild(script);
}


function loadScripts(urls, onLoad) {
    var url = urls.shift();
    
    loadScript(url, (urls.length == 0) ? onLoad : function() {
        loadScripts(urls, onLoad);
    });
}


function log(/* ... */) {
    opera.postError(Array.from(arguments).join(' '));
}


document.addEventListener('readystatechange', addLinks, false);
