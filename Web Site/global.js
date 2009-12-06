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
    var c = [
        109, 111, 99, 46, 108, 105, 97, 109, 103, 64, 111, 110, 105, 116, 115,
        117, 97, 102, 98, 109, 111, 105, 99, 114, 97, 109
    ];
    
    return eval('String.fromCharCode.apply(null, c.reverse())');
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
