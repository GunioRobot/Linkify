// ==UserScript==
// @include http://listen.grooveshark.com/
// ==/UserScript==


document.addEventListener('DOMContentLoaded', function() {
    var adBar = document.getElementById('adBar');
    var mainContentWrapper = document.getElementById('mainContentWrapper');
    
    adBar.parentNode.removeChild(adBar);
    mainContentWrapper.style.width = '100%';
}, false);
