// ==UserScript==
// @include http://*.ign.com/*
// ==/UserScript==


function skipAdvertisement() {
    var elements = document.getElementsByClassName('prestitialText2');
    
    for (var i = 0; i < elements.length; ++i) {
        var element = elements.item(i);
        
        if (element.nodeName.toLowerCase() == 'a') {
            element.click();
            break;
        }
    }
}


document.addEventListener('DOMContentLoaded', skipAdvertisement, false);
