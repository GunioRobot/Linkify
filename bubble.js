// ==UserScript==
// @include *
// ==/UserScript==


// TODO: Add documentation.
// TODO: Check rel="nofollow"?
// TODO: Prevent frame detection? E.g. opera.defineMagicVariable


document.addEventListener('DOMContentLoaded', function() {
    var options = {
        bubbleStyle: {
            position: 'absolute',
            borderRadius: '999px',
            MozBorderRadius: '999px',
            WebkitBorderRadius: '999px',
            backgroundColor: 'rgba(82, 157, 255, 0.2)'
        },
        enableBubbleCursor: false,
        exceptions: {
            hostname: /^(my\.opera\.com|stackoverflow\.com)$/i,
            pathname: /\.(aspx?|cgi|exe|m3u|pdf|php|p[ly]|torrent|zip)$/i,
            protocol: /^(data|https|javascript|mailto):$/i
        },
        fixLinkHashScroll: true,
        outlineStyle: '0.3em solid rgba(82, 157, 255, 0.4)',
        prefetchInterval: 1 * 1000,
        toggleBubbleKeys: ['b', 'B']
    };
    
    var state = {
        bubble: document.createElement('div'),
        cachedUrls: undefined,
        closest: undefined,
        links: [],
        outlineStyle: {}
    };
    
    
    function Storage() {
        if (typeof sessionStorage != 'undefined') {
            this.get = function(key) {
                return sessionStorage.getItem(key);
            };
            
            this.set = function(key, value) {
                sessionStorage.setItem(key, value);
            };
        }
        else {
            this.cache = {};
            
            this.get = function(key) {
                return this.cache[key];
            };
            
            this.set = function(key, value) {
                this.cache[key] = value;
            };
        }
    }
    
    
    function clickBubbleCursor(event) {
        if ((state.closest != undefined) && options.enableBubbleCursor) {
            location.href = state.closest.link.href;
        }
    }
    
    
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
    
    
    function loadLink(link) {
        var iframe = document.createElement('iframe');
        var url = link.href;
        
        if (options.fixLinkHashScroll) {
            var hash = link.hash;
            
            if (hash.length > 0) {
                link.hash = '';
                url = link.href;
                link.hash = hash;
            }
        }
        
        iframe.src = url;
        iframe.style.display = 'none';
        document.body.appendChild(iframe);
        
        iframe.addEventListener('readystatechange', function() {
            document.body.removeChild(iframe);
        }, false);
    }
    
    
    function log(/* ... */) {
        if (typeof opera != 'undefined') {
            opera.postError(Array.prototype.join.call(arguments, ' '));
        }
    }
    
    
    function prefetch() {
        if ((window !== window.parent) || (state.closest == undefined)) {
            return;
        }
        
        var link = state.closest.link;
        if (state.cachedUrls.get(link) != undefined) {
            return;
        }
        
        state.cachedUrls.set(link.href, true);
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
        loadLink(link);
    }
    
    
    function toggleBubble(event) {
        var key = String.fromCharCode((event.keyCode != undefined)
            ? event.keyCode
            : event.which);
        
        for (var i = 0; i < options.toggleBubbleKeys.length; ++i) {
            if (key == options.toggleBubbleKeys[i]) {
                options.enableBubbleCursor = !options.enableBubbleCursor;
                state.bubble.style.display = options.enableBubbleCursor
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
            if (state.closest != undefined) {
                state.closest.link.style.outline = state.outlineStyle;
            }
            
            state.outlineStyle = closest.link.style.outline;
            closest.link.style.outline = options.outlineStyle;
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
        
        if (anchor.href.length > 0) {
            state.links.push({
                link: anchor,
                bottom: offset.top + anchor.offsetHeight,
                left: offset.left,
                right: offset.left + anchor.offsetWidth,
                top: offset.top
            });
        }
    }
    
    for (var style in options.bubbleStyle) {
        state.bubble.style[style] = options.bubbleStyle[style];
    }
    
    state.bubble.style.display = options.enableBubbleCursor ? 'block' : 'none';
    state.cachedUrls = new Storage();
    
    document.body.appendChild(state.bubble);
    document.addEventListener('keypress', toggleBubble, false);
    document.addEventListener('mousemove', updateBubble, false);
    document.addEventListener('click', clickBubbleCursor, false);
    
    setInterval(prefetch, options.prefetchInterval);
}, false);
