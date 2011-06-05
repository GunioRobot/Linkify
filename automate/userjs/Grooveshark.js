// ==UserScript==
// @include http://grooveshark.com/*
// ==/UserScript==


function removeAdvertisements() {
    var fullWidth = document.createElement('style');
    
    fullWidth.appendChild(document.createTextNode(
        '#application {margin-right: 0 !important;}'));
    
    fullWidth.type = 'text/css';
    document.querySelector('html > head').appendChild(fullWidth);
    
    var ad = document.getElementById('capital');
    ad.parentNode.removeChild(ad);
}


document.addEventListener('DOMContentLoaded', removeAdvertisements, false);
