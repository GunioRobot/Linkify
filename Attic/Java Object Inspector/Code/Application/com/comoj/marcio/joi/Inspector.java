package com.comoj.marcio.joi;


import com.comoj.marcio.joi.ui.text.InspectorConsole;


/**
 * Java object inspector.
 */
public class Inspector {
    /**
     * Inspects an object.
     * 
     * @param object object to be inspected
     * @throws NullPointerException if the object is null
     */
    public void inspect(Object object) {
        if (object == null) {
            throw new NullPointerException("Can't inspect null.");
        }
        
        new InspectorConsole().start(object);
    }
}
