package ycloader;

import ycloader.exception.ClassLoadingException;

import java.io.IOException;

public interface Stuffable {
    void stuffing() throws IOException, ClassLoadingException;
}
