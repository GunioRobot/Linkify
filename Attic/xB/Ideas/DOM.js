/**
 * @fileoverview Ensures DOM conformance.
 * @author Márcio Faustino
 * @version 2010-04-15
 * @requires E262
 * @see http://www.w3.org/TR/REC-DOM-Level-1/
 */


var DOM = new function() {
    var self = this;
    
    
    /**
     * Level number.
     */
    this.level = 1;
    
    this.INDEX_SIZE_ERR = 1;
    this.DOMSTRING_SIZE_ERR = 2;
    this.HIERARCHY_REQUEST_ERR = 3;
    this.WRONG_DOCUMENT_ERR = 4;
    this.INVALID_CHARACTER_ERR = 5;
    this.NO_DATA_ALLOWED_ERR = 6;
    this.NO_MODIFICATION_ALLOWED_ERR = 7;
    this.NOT_FOUND_ERR = 8;
    this.NOT_SUPPORTED_ERR = 9;
    this.INUSE_ATTRIBUTE_ERR = 10;
    
    
    this.DOMException = function(code) {
        if ((code < 1) || (code > 10) || !Number.isInteger(code)) {
            var errorMessage = 'Invalid error code: ' + code;
            throw new TypeError('DOMException: ' + errorMessage);
        }
        
        var codeMessages = {
            1: 'Index or size is negative, or greater than the allowed value.',
            2: 'The specified range of text does not fit into a DOMString.',
            3: 'Node cannot be inserted where it does not belong to.',
            4: 'Node cannot be used in a different document than the one that created it.',
            5: 'Invalid character specified.',
            6: 'Data specified for a node which does not support data.',
            7: 'Object does not allow modifications.',
            8: 'Referenced node does not exist in the specified context.',
            9: 'Unsupported object type.',
            10: 'Attribute is already in use elsewhere.',
        };
        
        Error.call(this, codeMessages[code]);
        this.code = code;
    };
    
    Class.extend(this.DOMException, Error);
    this.DOMException.prototype.name = 'DOMException';
};
