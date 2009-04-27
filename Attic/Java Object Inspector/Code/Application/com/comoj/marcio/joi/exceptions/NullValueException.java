package com.comoj.marcio.joi.exceptions;


@SuppressWarnings("serial")
public class NullValueException extends RuntimeException {
    public NullValueException() {
        super("null isn't a primitive value.");
    }
}
