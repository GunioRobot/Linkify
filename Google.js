// ==UserScript==
// @include http://www.google.*/
// ==/UserScript==


document.addEventListener('DOMContentLoaded', function() {
    var disableFadeIn = document.createElement('style');
    
    disableFadeIn.appendChild(document.createTextNode('\
#fctr, #ghead, #pmocntr, #sbl, #tba, #tbe, .fade, .gbh {\
    opacity: 1 !important;\
}'));
    
    disableFadeIn.type = 'text/css';
    document.querySelector('html > head').appendChild(disableFadeIn);
}, false);
