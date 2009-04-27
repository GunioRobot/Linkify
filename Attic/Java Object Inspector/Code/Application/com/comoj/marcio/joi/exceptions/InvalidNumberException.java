package com.comoj.marcio.joi.exceptions;


@SuppressWarnings("serial")
public class InvalidNumberException extends RuntimeException {
    public InvalidNumberException() {
        super("Invalid number.");
    }
}
