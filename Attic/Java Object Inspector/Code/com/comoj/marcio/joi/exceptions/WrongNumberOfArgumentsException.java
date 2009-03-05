package com.comoj.marcio.joi.exceptions;


@SuppressWarnings("serial")
public class WrongNumberOfArgumentsException extends RuntimeException {
    public WrongNumberOfArgumentsException() {
        super("Wrong number of arguments.");
    }
}
