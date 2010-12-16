// ==UserScript==
// @include http://my.opera.com/*
// @include http://my.*.opera.com/*
// ==/UserScript==


function removeTabIndex() {
    var tags = ['a', 'li'];
    
    while (tags.length > 0) {
        var elements = document.getElementsByTagName(tags.shift());
        
        for (var i = 0; i < elements.length; ++i) {
            var element = elements.item(i);
            
            if (element.getAttribute('tabindex') == '0') {
                element.removeAttribute('tabindex');
            }
        }
    }
}


document.addEventListener('DOMContentLoaded', removeTabIndex, false);
