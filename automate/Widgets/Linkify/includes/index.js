Array.from = function(object) {
    return Array.prototype.map.call(object, function (element) {
        return element;
    });
};


function addLinks() {
    log('Begin');
    
    var libraries = [
        'http://code.jquery.com/jquery.js',
        'https://raw.github.com/maranomynet/linkify/master/1.0/jquery.linkify-1.0.js',
    ];
    
    loadScripts(libraries, function() {
        log('Loaded');
        
        window.jQuery('body').linkify(function (links) {
            links.css({
                color: '#0082E0',
                textDecoration: 'underline'
            });
        });
        
        log('Done');
    });
    
    log('End');
}


function loadScript(url, onLoad) {
    var script = document.createElement('script');
    
    script.src = url;
    script.onload = onLoad;
    
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
