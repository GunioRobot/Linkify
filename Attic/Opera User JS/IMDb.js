// ==UserScript==
// @include http://www.imdb.com/title/*
// ==/UserScript==


function addParentsGuideWindow() {
    var movie = location.href.match(/title\/([^/]+)/).pop();
    var container = document.createElement('div');
    var title = document.createElement('span');
    var contents = document.createElement('iframe');
    
    with (container) {
        style.border = '0.1em solid black';
        style.backgroundColor = 'white';
        style.position = 'fixed';
        style.zIndex = 1000;
        style.bottom = 0;
        style.right = 0;
    }
    
    with (title) {
        style.padding = '0.1em 0.2em 0.1em 0.2em';
        style.fontSize = 'smaller';
        style.fontWeight = 'bold';
    }
    
    with (contents) {
        style.display = 'none';
        style.border = 'none';
    }
    
    container.addEventListener('mouseover', function() {
        title.style.display = 'none';
        contents.style.display = 'block';
    }, false);
    
    container.addEventListener('mouseout', function() {
        title.style.display = 'inline';
        contents.style.display = 'none';
    }, false);
    
    contents.src = 'http://www.imdb.com/title/' + movie + '/parentalguide?userjs';
    title.textContent = 'Parents Guide';
    
    container.appendChild(title);
    container.appendChild(contents);
    document.body.appendChild(container);
}


function cleanUpParentsGuide() {
    var contents = document.getElementById('swiki.2');
    contents.parentNode.removeChild(contents);
    
    with (contents) {
        style.marginTop = '-1.1em';
        style.marginBottom = '-1em';
        style.paddingLeft = '0.1em';
        style.paddingRight = '0.1em';
    }
    
    while (document.body.firstChild) {
        document.body.removeChild(document.body.firstChild);
    }
    
    document.body.appendChild(contents);
    document.body.style.fontSize = 'smaller';
    
    for (var i = 1; i <= 5; ++i) {
        var tab = document.getElementById('control.2.' + i + '.tab');
        tab.parentNode.removeChild(tab);
    }
}


if (!location.pathname.match(/parentalguide$/)) {
    document.addEventListener('DOMContentLoaded', addParentsGuideWindow, false);
}
else if (location.search == '?userjs') {
    document.addEventListener('DOMContentLoaded', cleanUpParentsGuide, false);
}
