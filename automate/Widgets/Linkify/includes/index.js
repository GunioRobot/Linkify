function addLinks() {
    var libraries = [
        'http://code.jquery.com/jquery.js',
        'https://raw.github.com/maranomynet/linkify/master/1.0/jquery.linkify-1.0.js',
    ];
    
    loadScripts(libraries, function () {
        window.jQuery('body').linkify({
            handleLinks: function (links) {
                links.css({
                    color: '#0082E0',
                    textDecoration: 'underline'
                });
            }
        });
    });
}


function loadScript(url, onLoad) {
    var script = document.createElement('script');
    
    script.src = url;
    script.onload = onLoad;
    
    document.body.appendChild(script);
}


function loadScripts(urls, onLoad) {
    var url = urls.shift();
    
    loadScript(url, (urls.length == 0) ? onLoad : function () {
        loadScripts(urls, onLoad);
    });
}


document.addEventListener('DOMContentLoaded', addLinks, false);
