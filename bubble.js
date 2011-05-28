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
        bubbleStyle: {
            position: 'absolute',
            borderRadius: '999px',
            MozBorderRadius: '999px',
            WebkitBorderRadius: '999px',
            backgroundColor: 'rgba(128, 128, 128, 0.4)'
        },
        outlineStyle: {
            outline: '3px solid #529DFF'
        },
        showBubble: true,
        showOutline: true
    };
    
    var bubble = document.createElement('div');
    
    for (var style in options.bubbleStyle) {
        bubble.style[style] = options.bubbleStyle[style];
    }
    
    bubble.style.display = options.showBubble ? 'block' : 'none';
    document.body.appendChild(bubble);
    
    var anchors = document.getElementsByTagName('a');
    var links = [];
    
    for (var i = 0; i < anchors.length; ++i) {
        var anchor = anchors[i];
        var offset = getElementOffset(anchor);
        
        links.push({
            anchor: anchor,
            bottom: offset.top + anchor.offsetHeight,
            left: offset.left,
            right: offset.left + anchor.offsetWidth,
            top: offset.top
        });
    }
    
    var previousClosest;
    
    document.addEventListener('mousemove', function(event) {
        var closestDistance = 9999;
        var closest;
        
        for (var i = 0; i < links.length; ++i) {
            var link = links[i];
            var distance = distanceToRect(event.pageX, event.pageY, link);
            
            if (distance < closestDistance) {
                closestDistance = distance;
                closest = link;
            }
        }
        
        if (closest !== previousClosest) {
            if (options.showOutline) {
                if (previousClosest != undefined) {
                    previousClosest.anchor.style.outline = '';
                }
                
                for (var style in options.outlineStyle) {
                    closest.anchor.style[style] = options.outlineStyle[style];
                }
            }
            
            previousClosest = closest;
        }
        
        bubble.style.width = 2 * closestDistance + 'px';
        bubble.style.height = 2 * closestDistance + 'px';
        bubble.style.top = event.pageY - closestDistance + 'px';
        bubble.style.left = event.pageX - closestDistance + 'px';
    }, false);
    
    document.addEventListener('keypress', function(event) {
        var code = (event.keyCode != undefined) ? event.keyCode : event.which;
        
        if ((code == 66) || (code == 98)) {
            options.showBubble = !options.showBubble;
            bubble.style.display = options.showBubble ? 'block' : 'none';
            return false;
        }
    }, false);
}, false);
