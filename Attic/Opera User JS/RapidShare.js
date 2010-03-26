// ==UserScript==
// @include http://rapidshare.com/files/*
// ==/UserScript==


function freeUser() {
    var download = document.getElementById('ff');
    
    if (download != null) {
        opera.removeEventListener('BeforeExternalScript', freeUser, false);
        download.submit();
    }
}


opera.addEventListener('AfterScript', freeUser, false);
