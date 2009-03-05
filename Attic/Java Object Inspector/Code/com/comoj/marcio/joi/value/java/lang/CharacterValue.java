package com.comoj.marcio.joi.value.java.lang;


import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.comoj.marcio.joi.exceptions.InvalidSyntaxException;
import com.comoj.marcio.joi.exceptions.UnsupportedTypeException;


/**
 * A character that can be inspected.
 */
public class CharacterValue extends ObjectValue {
    private static final Map<String, Character> _UNESCAPE;
    private static final Map<Character, String> _ESCAPE;
    
    
    static {
        _UNESCAPE = new HashMap<String, Character>();
        _ESCAPE = new HashMap<Character, String>();
        
        _UNESCAPE.put("\\b", '\b');
        _UNESCAPE.put("\\t", '\t');
        _UNESCAPE.put("\\n", '\n');
        _UNESCAPE.put("\\f", '\f');
        _UNESCAPE.put("\\r", '\r');
        _UNESCAPE.put("\\\"", '\"');
        _UNESCAPE.put("\\\'", '\'');
        _UNESCAPE.put("\\\\", '\\');
        
        for (String ch : _UNESCAPE.keySet()) {
            _ESCAPE.put(_UNESCAPE.get(ch), ch);
        }
    }
    

    /**
     * Escapes a character.
     * 
     * @param ch character to be escaped
     * @return a string that represents the escaped character (without quotes)
     */
    public static String escape(Character ch) {
        if (_ESCAPE.containsKey(ch)) {
            return _ESCAPE.get(ch);
        }
        else {
            return String.valueOf(ch);
        }
    }
    

    /**
     * Unescapes a string representing an escaped character.
     * 
     * @param string character to be escaped (without quotes)
     * @return the unescaped character
     * @throws InvalidSyntaxException if the character syntax is invalid
     */
    public static Character unescape(String string) {
        Pattern charSyntax = Pattern.compile("^([^'\\\\]|\\\\.)$");
        Matcher matcher = charSyntax.matcher(string);
        
        if (!matcher.find()) {
            throw new InvalidSyntaxException();
        }
        
        String ch = matcher.group(1);
        
        if (_UNESCAPE.containsKey(ch)) {
            return _UNESCAPE.get(ch);
        }
        else if (ch.length() == 1) {
            return ch.charAt(0);
        }
        else {
            throw new InvalidSyntaxException();
        }
    }
    

    public CharacterValue(Character ch) {
        super(ch);
    }
    

    public Character getValue() {
        return (Character) super.getValue();
    }
    

    public String getValueToOutput() {
        Character ch = getValue();
        
        if (ch == null) {
            return super.getValueToOutput();
        }
        else {
            return "'" + escape(ch) + "'";
        }
    }
    

    public void setValue(Object newValue) {
        if ((newValue == null) || (newValue instanceof Character)) {
            super.setValue(newValue);
        }
        else {
            throw new UnsupportedTypeException();
        }
    }
    

    public void setValueFromInput(String input) {
        try {
            setValue(unescape(input.replaceAll("(^')|('$)", "")));
        }
        catch (InvalidSyntaxException error) {
            super.setValueFromInput(input);
        }
    }
}
