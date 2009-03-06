package com.comoj.marcio.joi.values.java.lang;


import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.comoj.marcio.joi.exceptions.InvalidSyntaxException;
import com.comoj.marcio.joi.exceptions.UnsupportedTypeException;


/**
 * A string that can be inspected.
 */
public class StringValue extends ObjectValue {
    /**
     * Escapes a string.
     * 
     * @param string string to be escaped
     * @return an escaped string
     */
    public static String escape(String string) {
        StringBuffer buffer = new StringBuffer(string.length());
        
        for (char ch : string.toCharArray()) {
            buffer.append(CharacterValue.escape(ch));
        }
        
        return '"' + buffer.toString() + '"';
    }
    

    /**
     * Unescapes a string.
     * 
     * @param string string to be escaped
     * @return the unescaped string
     * @throws InvalidSyntaxException if the string syntax is invalid
     */
    public static String unescape(String string) {
        String charRegex = "([^\"\\\\]|\\\\.)";
        Pattern charSyntax = Pattern.compile(charRegex);
        Pattern strSyntax = Pattern.compile("^\"(" + charRegex + "*)\"$");
        Matcher strMatcher = strSyntax.matcher(string);
        
        if (!strMatcher.find()) {
            throw new InvalidSyntaxException();
        }
        
        string = (strMatcher.group(1) == null) ? "" : strMatcher.group(1);
        
        Matcher charMatcher = charSyntax.matcher(string);
        StringBuffer buffer = new StringBuffer(string.length());
        
        while (charMatcher.find()) {
            String ch = charMatcher.group(1);
            
            if (ch.equals("'")) {
                ch = "\\'";
            }
            
            buffer.append(CharacterValue.unescape(ch));
        }
        
        return buffer.toString();
    }
    

    public StringValue(String string) {
        super(string);
    }
    

    public String getValue() {
        return (String) super.getValue();
    }
    

    public String getValueToOutput() {
        String string = getValue();
        
        if (string == null) {
            return super.getValueToOutput();
        }
        else {
            return escape(string);
        }
    }
    

    public void setValue(Object newValue) {
        if ((newValue == null) || (newValue instanceof String)) {
            super.setValue(newValue);
        }
        else {
            throw new UnsupportedTypeException();
        }
    }
    

    public void setValueFromInput(String input) {
        try {
            setValue(unescape(input));
        }
        catch (InvalidSyntaxException error) {
            super.setValueFromInput(input);
        }
    }
}
