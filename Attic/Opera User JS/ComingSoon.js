// ==UserScript==
// @include http://www.comingsoon.net/*
// @include http://cms.springboard.gorillanation.com/embed_code_lightbox/*
// ==/UserScript==


opera.defineMagicVariable('adContentDART',
    function (currentValue) {
        // Disable advertisement.
        return undefined;
    },
    null
);


function showLink() {
    var links = document.selectNodes("//a[contains(@href, 'start_lightbox_cs002')]");
    
    if (links.length != 1) {
        return;
    }
    
    var link = links.item(0);
    var id = link.href.match(/(\d+)\/cs002/).pop();
    var url = 'http://cdn.springboard.gorillanation.com/storage/comingsoon.net/conversion/' + id + '.flv';
    var br = document.createElement('br');
    var download = document.createElement('a');
    
    download.href = url;
    download.textContent = 'Download';
    
    link.parentNode.appendChild(br);
    link.parentNode.appendChild(download);
    
    opera.removeEventListener('BeforeExternalScript', showLink, false);
}


opera.addEventListener('BeforeExternalScript', showLink, false);
