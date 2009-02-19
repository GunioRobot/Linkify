if (window.addEventListener) {
    window.addEventListener('load', initialize, false);
}
else if (document.addEventListener) {
    document.addEventListener('load', initialize, false);
}
else if (window.attachEvent) {
    window.attachEvent('onload', initialize);
}
else {
    window.onload = initialize;
}


function getEmail() {
    return eval("'m.' + (function(x) { return 'fau' + x + 'ino'; })('st')"
        + "+ '@' + 'liamg'.split('').reverse().join('') + '.'"
        + "+ String.fromCharCode(109, 111, 99).split('').reverse().join('');");
}


function initialize() {
    var anchors = document.getElementsByTagName('a');
    
    for (var i = 0; i < anchors.length; ++i) {
        var anchor = anchors[i];
        
        if (anchor.className && anchor.className.match(/Email/)) {
            anchor.href = 'mailto:' + getEmail();
            
            if ((anchor.innerHTML == '') || (anchor.innerHTML == '…')) {
                anchor.innerHTML = getEmail();
            }
        }
    }
}
