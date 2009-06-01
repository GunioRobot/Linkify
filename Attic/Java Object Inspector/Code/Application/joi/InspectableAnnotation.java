package joi;


import java.lang.annotation.Annotation;
import java.lang.reflect.Modifier;


/**
 * An annotation that can be inspected.
 */
public class InspectableAnnotation extends InspectableClass {
    public InspectableAnnotation(Annotation annotation) {
        super(annotation.annotationType());
    }
    

    public String describe() {
        String mods = Modifier.toString(getValue().getModifiers()).replaceFirst(
            "interface", "@interface");
        
        return mods + (mods.length() > 0 ? " " : "") + getClassName();
    }
}
