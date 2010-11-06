// ==UserScript==
// @include http://listen.grooveshark.com/*
// ==/UserScript==


document.addEventListener('DOMContentLoaded', function() {
    function $(/* ... */) {
        return document.getElementById.apply(document, arguments);
    }
    
    var ad = $('adBar') || $('sidebar');
    
    ad.parentNode.removeChild(ad);
    $('mainContentWrapper').style.width = '100%';
}, false);
