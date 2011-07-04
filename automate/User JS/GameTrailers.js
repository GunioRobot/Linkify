// ==UserScript==
// @include http://www.gametrailers.com/player*
// @include http://www.gametrailers.com/video*
// ==/UserScript==


function openDownloadMenu() {
    var styles = {
        'MediaDownload': 'inline',
        'media_dl': 'block',
    };
    
    for (var id in styles) {
        try {
            document.getElementById(id).style.display = styles[id];
            opera.removeEventListener('AfterScript', openDownloadMenu, false);
            break;
        }
        catch (exception) {
            continue;
        }
    }
}


opera.addEventListener('AfterScript', openDownloadMenu, false);
