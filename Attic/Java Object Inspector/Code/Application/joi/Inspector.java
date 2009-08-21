package joi;


import joi.ui.text.InspectorConsole;


/**
 * Java object inspector.
 */
public class Inspector {
    /**
     * Inspects an object.
     * 
     * @param object object to inspect
     */
    public void inspect(Object object) {
        new InspectorConsole().start(object);
    }
}
