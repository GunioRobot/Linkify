// ==UserScript==
// @include http://*
// ==/UserScript==


document.addEventListener('DOMContentLoaded', function() {
    if (document.title == '500 Internal Server Error') {
        location.reload();
    }
}, false);
