// ==UserScript==
// @include *
// ==/UserScript==


document.addEventListener('DOMContentLoaded', function() {
    function distance(x1, y1, x2, y2) {
        var x = x1 - x2;
        var y = y1 - y2;
        return Math.sqrt(x * x + y * y);
    }
    
    
    function distanceToRect(x, y, rect) {
        var horizontal = (x >= rect.left) && (x <= rect.right);
        var vertical = (y >= rect.top) && (y <= rect.bottom);
        
        if (horizontal && vertical) {
            // Inside.
            return 0;
        }
        else if (horizontal) {
            // Edges are closest.
            return (y < rect.top) ? (rect.top - y) : (y - rect.bottom);
        }
        else if (vertical) {
            return (x < rect.left) ? (rect.left - x) : (x - rect.right);
        }
        
        // Corners.
        if ((y < rect.top) && (x < rect.left)) {
            return distance(rect.left, rect.top, x, y);         // Top left.
        }
        else if ((y < rect.top) && (x > rect.right)) {
            return distance(rect.right, rect.top, x, y);        // Top right.
        }
        else if ((y > rect.bottom) && (x > rect.right)) {
            return distance(rect.right, rect.bottom, x, y);     // Bottom right.
        }
        else if ((y > rect.bottom) && (x < rect.left)) {
            return distance(rect.left, rect.bottom, x, y);      // Bottom left.
        }
        
        throw Error('Distance to rectangle.');
    }
    
    
    // Stolen from jQuery (version?).
    function getElementOffset(element) {
        var box = element.getBoundingClientRect();
        var document = element.ownerDocument;
        var body = document.body;
        var documentElement = document.documentElement;
        
        var clientTop = documentElement.clientTop || body.clientTop || 0;
        var clientLeft = documentElement.clientLeft || body.clientLeft || 0;
        
        var top = box.top - clientTop
            + (documentElement.scrollTop || body.scrollTop);
        var left = box.left - clientLeft
            + (documentElement.scrollLeft || body.scrollLeft);
        
        return {top: top, left: left};
    }
    
    
    var options = {
        showBubble: true
    };
    
    // Setup
    var bubble = document.createElement('div');
    var props = {
        position: 'absolute',
        borderRadius: '999px',
        MozBorderRadius: '999px',
        WebkitBorderRadius: '999px',
        backgroundColor: 'rgba(128,128,128,0.4)',
        display: options.showBubble ? 'block' : 'none'
    };
    
    for (var prop in props) {
        bubble.style[prop] = props[prop];
    }

    document.body.appendChild(bubble);

    // [{top:0, left:0, right:0, bottom:0, a:<anchor>}, ...]
    var links = [];

    var as = document.getElementsByTagName('a');
    for (var i = 0; i < as.length; ++i) {
        var a = as[i];
        var offset = getElementOffset(a);
        
        var link = {
            a: a,
            left: offset.left,
            top: offset.top,
            right: offset.left + a.offsetWidth,
            bottom: offset.top + a.offsetHeight
        };

        links.push(link);
    }

    var prevClosest = links[0];

    document.addEventListener('mousemove', function(event) {
        var closest;
        var closeDist = 9999;
        
        for (var i = 0; i < links.length; ++i) {
            var l = links[i];
            var dist = distanceToRect(event.pageX, event.pageY, l);
            
            if (dist < closeDist) {
                closest = links[i];
                closeDist = dist;
            }
        }
        
        if (closest !== prevClosest) {
            prevClosest.a.style.outline = '';
            closest.a.style.outline = '3px solid #529DFF';
            prevClosest = closest;
        }
        
        bubble.style.width = 2 * closeDist + 'px';
        bubble.style.height = 2 * closeDist + 'px';
        bubble.style.top = event.pageY - closeDist + 'px';
        bubble.style.left = event.pageX - closeDist + 'px';
    }, false);
    
    document.addEventListener('keypress', function(event) {
        var code = event.keyCode != undefined ? event.keyCode : event.which;
        
        if ((code == 66) || (code == 98)) {
            options.showBubble = !options.showBubble;
            bubble.style.display = options.showBubble ? 'block' : 'none';
            return false;
        }
    }, false);
}, false);
