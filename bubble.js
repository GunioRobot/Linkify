// ==UserScript==
// @include *
// ==/UserScript==


document.addEventListener('DOMContentLoaded', function() {
    var options = {
        bubbleStyle: {
            position: 'absolute',
            borderRadius: '999px',
            MozBorderRadius: '999px',
            WebkitBorderRadius: '999px',
            backgroundColor: 'rgba(82, 157, 255, 0.2)'
        },
        exceptions: {
            hostname: /^my\.opera\.com$/i,
            pathname: /\.(aspx?|cgi|php|pl|py)$/i,
            protocol: /^(data|https|javascript|mailto):$/i
        },
        outlineStyle: {
            outline: '0.3em solid rgba(82, 157, 255, 0.4)'
        },
        prefetchInterval: 1 * 1000,
        showBubble: false,
        showOutline: true,
        toggleBubbleKeys: ['b', 'B']
    };
    
    var state = {
        bubble: document.createElement('div'),
        cachedUrls: {},
        closest: undefined,
        links: [],
        style: {}
    };
    
    
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
    
    
    function log(/* ... */) {
        opera.postError(Array.prototype.join.call(arguments, ' '));
    }
    
    
    function urlToLink(url) {
        var anchor = document.createElement('a');
        
        anchor.href = url;
        return anchor;
    }
    
    
    function prefetch() {
        if ((window !== window.parent) || (state.closest == undefined)) {
            return;
        }
        
        var link = state.closest.link;
        
        if (link.href in state.cachedUrls) {
            return;
        }
        
        state.cachedUrls[link.href] = true;
        
        if ((link.href.length == 0) || (link.pathname == location.pathname)) {
            return;
        }
        
        for (component in options.exceptions) {
            if (options.exceptions[component].test(link[component])) {
                log('Skip:', link.href);
                return;
            }
        }
        
        log('Prefetch:', link.href);
    }
    
    
    function toggleBubble(event) {
        var key = String.fromCharCode((event.keyCode != undefined)
            ? event.keyCode
            : event.which);
        
        for (var i = 0; i < options.toggleBubbleKeys.length; ++i) {
            if (key == options.toggleBubbleKeys[i]) {
                options.showBubble = !options.showBubble;
                state.bubble.style.display = options.showBubble
                    ? 'block' : 'none';
                
                break;
            }
        }
    }
    
    
    function updateBubble(event) {
        var closestDistance = 9999;
        var closest;
        
        for (var i = 0; i < state.links.length; ++i) {
            var link = state.links[i];
            var distance = distanceToRect(event.pageX, event.pageY, link);
            
            if (distance < closestDistance) {
                closestDistance = distance;
                closest = link;
            }
        }
        
        if (closest !== state.closest) {
            if (options.showOutline) {
                if (state.closest != undefined) {
                    for (style in state.style) {
                        state.closest.link.style[style] = state.style[style];
                    }
                    state.style = {};
                }
                
                for (var style in options.outlineStyle) {
                    state.style[style] = closest.link.style[style];
                    closest.link.style[style] = options.outlineStyle[style];
                }
            }
            
            state.closest = closest;
        }
        
        state.bubble.style.width = 2 * closestDistance + 'px';
        state.bubble.style.height = 2 * closestDistance + 'px';
        state.bubble.style.top = event.pageY - closestDistance + 'px';
        state.bubble.style.left = event.pageX - closestDistance + 'px';
    }
    
    
    var metas = document.getElementsByTagName('meta');
    
    for (var i = 0; i < metas.length; ++i) {
        var meta = metas[i];
        
        // http://www.w3.org/TR/html4/appendix/notes.html#h-B.4.1.2
        if ((meta.name.toLowerCase() == 'robots')
            && /nofollow/i.test(meta.getAttribute('content')))
        {
            log('Prefetch disabled.');
            return;
        }
    }
    
    var anchors = document.getElementsByTagName('a');
    
    for (var i = 0; i < anchors.length; ++i) {
        var anchor = anchors[i];
        var offset = getElementOffset(anchor);
        
        state.links.push({
            link: anchor,
            bottom: offset.top + anchor.offsetHeight,
            left: offset.left,
            right: offset.left + anchor.offsetWidth,
            top: offset.top
        });
    }
    
    for (var style in options.bubbleStyle) {
        state.bubble.style[style] = options.bubbleStyle[style];
    }
    
    state.bubble.style.display = options.showBubble ? 'block' : 'none';
    document.body.appendChild(state.bubble);
    document.addEventListener('keypress', toggleBubble, false);
    document.addEventListener('mousemove', updateBubble, false);
    setInterval(prefetch, options.prefetchInterval);
}, false);
