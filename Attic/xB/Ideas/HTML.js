var HTML = new function() {
    /**
     * Cookies interface. (Singleton.)
     */
    this.Cookies = new function() {
        var self = this;
        
        /**
         * Checks if cookies are enabled.
         *
         * @return true if cookies are enabled or false otherwise
         * @type Boolean
         */
        this.enabled = function() {
            // Don't trust the navigator.cookieEnabled property as some browsers
            // can set it incorrectly.
            
            var name = 'enabled', previousValue = self.get(name);
            
            self.set(name, 'true');
            var enabled = (self.get(name) !== null);
            
            if (previousValue !== null) {
                // Restore previous cookie.
                self.set(name, previousValue);
            }
            else {
                // Remove test cookie.
                self.remove(name);
            }
            
            return enabled;
        };
        
        /**
         * Gets a cookie.
         *
         * @param {String} name name of the cookie to retrieve its value
         * @return the cookie value or null if it doesn't exist
         * @type String
         */
        this.get = function(name) {
            var target = new RegExp(encodeURIComponent(name) + '=([^;]*)\s*');
            var matches = document.cookie.match(target);
            return (matches === null) ? null : decodeURIComponent(matches[1]);
        };
        
        /**
         * Removes a cookie.
         *
         * @param {String} name name of the cookie to be removed
         */
        this.remove = function(name) {
            self.set(encodeURIComponent(name), '', {expires: -1});
        };
        
        /**
         * Sets a new cookie. The options object may have the following
         * attributes:
         * <ul>
         *   <li>domain: if not defined defaults to ''.</li>
         *   <li>expires: if not defined the cookie will last as long as the
         *       current session, otherwise is the:
         *       <ul>
         *         <li>Expiration date: if a string in the format
         *             'YYYY,MM,DD'.</li>
         *         <li>Life span in days: if a number.</li>
         *       </ul></li>
         *   <li>path: if not defined defaults to '/'.</li>
         *   <li>secure: if not defined defaults to ''.</li>
         * </ul>
         *
         * @param {String} name cookie name
         * @param value cookie value (will be converted to a string)
         * @param {Object} options optional cookie creation options
         * @return a string with the cookie
         * @type String
         */
        this.set = function(name, value, options) {
            var expires = null, path = '/', domain = null, secure = null, date;
            
            if (defined(options)) {
                expires = options.expires || expires;
                path    = options.path    || path;
                domain  = options.domain  || domain;
                secure  = options.secure  || secure;
            }
            
            switch (typeof(expires)) {
            case 'string':
                date = new Date(expires).toGMTString();
                break;
            case 'number':
                date = new Date();
                date.setTime(date.getTime() + (expires * 24 * 60 * 60 * 1000));
                date = date.toGMTString();
                break;
            default:
                expires = null;
                break;
            }
            
            path    = '; path=' + path;
            expires = (expires === null) ? '' : ('; expires=' + date);
            domain  = (domain === null)  ? '' : ('; domain='  + domain);
            secure  = (secure === null)  ? '' : ('; secure='  + secure);
            
            var cookie = encodeURIComponent(name) + '='
                         + encodeURIComponent(value) + path + expires + domain
                         + secure;
            document.cookie = cookie;
            
            return cookie;
        };
    };
    
    /**
     * XHTML Document interface. (Singleton.)
     */
    this.Document = new function() {
        /** Associative array with all parameters from the query string. */
        this.queryString = {};
        
        /** Associative array of onload event handlers for each IFRAME id. */
        this._frames = {};
        
        /** Head element. */
        var head = document.getElementsByTagName('head')[0];
        
        var self = this;
        
        /**
         * Initial properties of the window object so that a dynamic script load
         * via an IFRAME copies only new properties.
         */
        var windowProperties = {onload: null};
        
        /**
         * Loads the given JavaScript script file.
         *
         * @param {String} file file name of the JavaScript script file to load
         * @param {Function} onLoad optional function to be called on a
         *                          successful load
         */
        this.loadScript = function(file, onLoad) {
            var script = document.createElement('script');
            script.type = 'text/javascript';
            script.src = file;
            
            if ( !defined(onLoad)) {
                head.appendChild(script);
                return;
            }
            
            if ((Object.hasProperty(script, 'onload')
                && !Object.hasProperty(window, 'konqueror'))
                || (Object.hasProperty(window, 'GetAttention')
                    && navigator.userAgent.match(/Netscape/i)))
            {
                script.onload = onLoad;
                head.appendChild(script);
            }
            else if (Object.hasProperty(script, 'onreadystatechange')) {
                script.onreadystatechange = onLoad;
                head.appendChild(script);
            }
            else if (Object.hasProperty(window, 'opera')
                     && navigator.userAgent.match(/Opera/i))
            {
                // Opera loads the script synchronously.
                head.appendChild(script);
                onLoad();
            }
            else {
                // Last alternative: create a hidden IFRAME element with the
                // script to be loaded in its HTML code and an onload event
                // handler for the BODY element that calls the onLoad function.
                var frame = document.createElement('iframe');
                var id = '_iframe' + (Math.floor(Math.random() * 1000000) + 1);
                
                frame.id = frame.name = id;
                frame.src = 'about:blank';
                frame.style.borderWidth = 0;
                frame.style.width = frame.style.height = 0;
                
                HTML.Element.body.appendChild(frame);
                self._frames[id] = function(frameWindow) {
                    for (var property in frameWindow) {
                        if ( !defined(windowProperties[property])) {
                            // Copy all functions and variables from the frame
                            // window that aren't defined in this window.
                            window[property] = frameWindow[property];
                        }
                    }
                    
                    // After leaving this function (which is running inside the
                    // frame scope), remove the frame to not pollute the HTML
                    // code of the document's body.
                    setTimeout(function() {
                        HTML.Element.body.removeChild(frame);
                        onLoad();
                    }, 10);
                };
                
                var doc = frames[id].document;
                var bodyOnload = "parent.window.HTML.Document._frames['"
                                 + id + "'](this)";
                var html = '<html><head><title></title></head>'
                           + '<body onload="' + bodyOnload + '">'
                           + '<script src="' + file + '" type="text/javascript">'
                           + '</script></body></html>';
                
                doc.open();
                doc.write(html);
                doc.close();
            }
        };
        
        function constructor() {
            // Parse query string.
            var queryString = document.location.href.match(/\?(.*)(#.*)?$/);
            
            if (queryString !== null) {
                var parameters = queryString[1].split('&');
                
                for (var i = 0; i < parameters.length; ++i) {
                    var parameter = parameters[i].match(/^([^=]+)=([^=]*)$/);
                    if (parameter === null) {
                        continue;
                    }
                    
                    var key = decodeURIComponent(parameter[1]);
                    var value = decodeURIComponent(parameter[2]);
                    self.queryString[key] = value;
                }
            }
            
            // Save initial window properties.
            for (var property in window) {
                windowProperties[property] = null;
            }
        }
        
        constructor();
    };
    
    /**
     * Element interface. (Singleton.)
     */
    this.Element = new function() {
        /** Document's body element. */
        this.body = document.getElementsByTagName('body')[0] || null;
        
        /** Text node (node type). */
        var TEXT_NODE = 3;
        
        var html = document.getElementsByTagName('html')[0];
        
        var self = this;
        
        /**
         * Adds the given class name to the given element's list of class names.
         *
         * @param {Object} elem element for which to add a class name
         * @param {String} className class name to add
         * @return the same element
         * @type Object
         */
        this.addClassName = function(elem, className) {
            elem.className = elem.className.split(/\s+/).each(function(c) {
                return (c != className) ? c : void(0);
            }).add(className).join(' ');
            
            return elem;
        };
        
        /**
         * Appends the given child to the given parent.
         *
         * @param {Object} parent parent node
         * @param {Object} child child node
         * @return the same parent element
         * @type Object
         */
        this.appendChild = function(parent, child) {
            parent._appendChild(child);
            return parent;
        };
        
        /**
         * Creates a new element of the given type.
         *
         * @param {String} tagName tag name of the element type to create
         * @return a new element of the given type
         * @type Object
         */
        this.create = function(tagName) {
            return attachElementMethods(document.createElement(tagName));
        };
        
        /**
         * Creates a new text node.
         *
         * @param {String} text text to be set for the new text node
         * @return a new text node
         * @type Object
         */
        this.createTextNode = function(text) {
            return document.createTextNode(text);
        };
        
        /**
         * Creates a sub-tree. Each element value may be another element object,
         * an array, or an actual DOM element. For example, to create a
         * paragraph with class names "Greeting" and "Text", with some text and
         * everything inside a div, use this object:
         *   {div: {'p [class=Greeting, Text]': ['Hello ',
         *                                       {b: 'World!'}]}}
         *
         * @param {Object} root sub-tree root element object
         * @return the element that is the root of the created tree
         * @type {Object}
         */
        this.createTree = function(root) {
            if (typeof(root) == 'string') {
                return self.createTextNode(root);
            }
            else if (defined(root.nodeName) && root.nodeName.match(/#?\w+/)) {
                return root;
            }
            
            var element = Object.keys(root)[0];
            var tagName = element.match(/^(\w+)/)[1];
            var tree = self.create(tagName);
            
            // Extract attributes and apply them.
            Array.from(element.match(/(\[[^[]+\])/g)).each(function(attr) {
                var name = attr.match(/^\[(\w+)=/)[1];
                var value = attr.match(/=(.+)\]$/)[1];
                
                if (name == 'class') {
                    value = value.split(/\s*,\s*/).join(' ');
                    tree.setAttribute('className', value);    // IE needs this.
                }
                
                tree.setAttribute(name, value);
            });
            
            if (root[element] instanceof Array) {
                root[element].each(function(child) {
                    self.appendChild(tree, self.createTree(child));
                });
            }
            else {
                self.appendChild(tree, self.createTree(root[element]));
            }
            
            return tree;
        };
        
        /**
         * Gets all childs of the given element.
         *
         * @param {Object} element element for which to get its childs
         * @return an array with all childs of the given element
         * @type Array
         */
        this.getChilds = function(element) {
            return Array.from(element.childNodes || element.children);
        };
        
        /**
         * Gets the given element's list of class names.
         *
         * @param {Object} element element for which to get the list of
         *                         class names
         * @return an array with the element's class names
         * @type Array
         */
        this.getClassNames = function(element) {
            return element.className.split(/\s+/);
        };
        
        /**
         * Gets the elements with the given class name.
         *
         * @param {String} className class name of the elements to be retrieved
         * @param {Object} subTree optional sub-tree in which to search (if
         *                         omitted searches in the document's body)
         * @return an array with all the elements with the given class name
         * @type Array
         */
        this.getElementsByClassName = function(className, subTree) {
            subTree = subTree || self.body;
            var elements = [];
            
            if (defined(subTree.className)
                && self.hasClassName(subTree, className))
            {
                elements.push(subTree);
            }
            
            self.getChilds(subTree).each(function(child) {
                var childs = self.getElementsByClassName(className, child);
                elements = elements.concat(childs);
            });
            
            return elements;
        };
        
        /**
         * Gets the element with the given id.
         *
         * @param {String} id id of the element to be retrieved
         * @return the element with the given id or null if it doesn't
         *          exist
         * @type Object
         */
        this.getElementById = function(id) {
            var element = document.getElementById(id);
            if (element === null) {
                return null;
            }
            
            // Check if the element is still in the DOM tree.
            var ancestor = element.parentNode;
            while ((ancestor !== null) && (ancestor != html)) {
                ancestor = ancestor.parentNode;
            }
            if (ancestor != html) {
                return null;    // Not in the DOM tree.
            }
            
            // The element's id must be checked because some browsers
            // incorrectly return an element with 'name = <id>'.
            if (element.id == id) {
                return element;
            }
            
            function search(element) {
                var childs = self.getChilds(element);
                if (childs.length == 0) {
                    return (element.id == id) ? element : null;
                }
                
                for (var i = 0; i < childs.length; ++i) {
                    var element = search(childs[i]);
                    if (element !== null) {
                        return element;
                    }
                }
                
                return null;
            }
            
            // Search the entire DOM tree for an element with the given id.
            element = search(self.body);
            if (element === null) {
                return null;
            }
            
            return element;
        };
        
        /**
         * Gets the text of the given element or text node. If it's an element,
         * returns the concatenation of the text of all childs.
         *
         * @param {Object} element element or text node from which to get its
         *                         text
         * @return the text of the given element or text node
         * @type String
         */
        this.getText = function(element) {
            return (element.nodeType == TEXT_NODE) ?
                   element.nodeValue :
                   self.getChilds(element).each(self.getText).join('');
        };
        
        /**
         * Checks if the given element has a class name.
         *
         * @param {Object} elem element
         * @param {String} className class name to check for existence
         * @return true if the element has the given class name or false
         *          otherwise
         * @type Boolean
         */
        this.hasClassName = function(elem, className) {
            return elem.className.split(/\s+/).indexOf(className) >= 0;
        };
        
        /**
         * Removes the given child element from the given parent.
         *
         * @param {Object} parent parent element
         * @param {Object} child child element to remove
         * @return the same parent element
         * @type Object
         */
        this.removeChild = function(parent, child) {
            parent._removeChild(child);
            return parent;
        };
        
        /**
         * Removes the given class name to the given element's list of class
         * names.
         *
         * @param {Object} el element for which to remove a class name
         * @param {String} cName class name to remove
         * @return the same element
         * @type Object
         */
        this.removeClassName = function(el, cName) {
            el.className = el.className.split(/\s+/).without(cName).join(' ');
            return el;
        };
        
        /**
         * Sets the text of the given text node.
         *
         * @param {Object} textNode text node for which to set its text
         * @param {String} text new text to be set for the given text node
         * @return the same text node
         * @type Object
         */
        this.setText = function(textNode, text) {
            textNode.nodeValue = text;
            return textNode;
        };
        
        function constructor() {
            if (self.body === null) {
                // If it couldn't find the body element, then it's because this
                // script was loaded inside the head element.
                createFakeBody();
            }
            else {
                attachElementMethods(self.body);
            }
        }
        
        /**
         * Attaches all element functions available via the Element interface as
         * methods.
         *
         * @param {Object} element element for which to attach element methods
         * @return the same element but with the element methods attached to it
         * @type Object
         */
        function attachElementMethods(element) {
            element.addClassName = function() {
                return self.addClassName.bind(this).apply(this, arguments);
            };
            
            // Save native method.
            element._appendChild = element.appendChild;
            element.appendChild = function() {
                return self.appendChild.bind(this).apply(this, arguments);
            };
            
            element.getChilds = function() {
                return self.getChilds.bind(this).apply(this, arguments);
            };
            element.getClassNames = function() {
                return self.getClassNames.bind(this).apply(this, arguments);
            };
            element.getElementsByClassName = function(className) {
                return self.getElementsByClassName(className, this);
            };
            element.hasClassName = function() {
                return self.hasClassName.bind(this).apply(this, arguments);
            };
            
            // Save native method.
            element._removeChild = element.removeChild;
            element.removeChild = function() {
                return self.removeChild.bind(this).apply(this, arguments);
            };
            
            element.removeClassName = function() {
                return self.removeClassName.bind(this).apply(this, arguments);
            };
            
            return element;
        }
        
        /**
         * Creates a temporary fake body until the real one gets loaded.
         */
        function createFakeBody() {
            self.body = new function() {
                var body = self.create('div');
                
                // Array of actions to be applied later.
                var actions = [];
                
                this.addClassName = function() {
                    addAction('addClassName', arguments);
                    return body.addClassName.apply(body, arguments);
                };
                
                this.appendChild = function() {
                    addAction('appendChild', arguments);
                    return body.appendChild.apply(body, arguments);
                };
                
                this.getChilds = function() {
                    addAction('getChilds', arguments);
                    return body.getChilds.apply(body, arguments);
                };
                
                this.getClassNames = function() {
                    addAction('getClassNames', arguments);
                    return body.getClassNames.apply(body, arguments);
                };
                
                this.getElementsByClassName = function(className) {
                    addAction('getElementsByClassName', arguments);
                    return body.getElementsByClassName(className);
                };
                
                this.hasClassName = function() {
                    addAction('hasClassName', arguments);
                    return body.hasClassName.apply(body, arguments);
                };
                
                this.removeChild = function() {
                    addAction('removeChild', arguments);
                    return body.removeChild.apply(body, arguments);
                };
                
                this.removeClassName = function() {
                    addAction('removeClassName', arguments);
                    return body.removeClassName.apply(body, arguments);
                };
                
                /**
                 * Adds an action to be applied later to the real body element.
                 *
                 * @param {String} methodName name of the method to be applied
                 * @param {Object} args arguments of the called method
                 */
                function addAction(methodName, args) {
                    actions.push({
                        method: methodName,
                        args: Array.from(args)
                    });
                }
                
                // Poll to get the body element as soon as it loads.
                var bodyPoll = setInterval(function() {
                    var actualBody = document.getElementsByTagName('body')[0];
                    
                    if (defined(actualBody)) {
                        clearInterval(bodyPoll);
                        delete body;
                        self.body = actualBody;
                        attachElementMethods(self.body);
                        
                        // Apply deferred actions.
                        actions.each(function(action) {
                            var method = action.method, args = action.args;
                            self.body[method].apply(self.body, args);
                        });
                    }
                }, 250);
            };
        }
        
        constructor();
    };
};
