package ycloader;

import ycloader.adt.u1;
import ycloader.adt.u2;
import ycloader.adt.u4;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ClassFileReader {
    private DataInputStream in;
    private String javaClass;
    private String[] rtJarsList;
    private String[] classPathList;

    ClassFileReader(String javaClass) {
        this.javaClass = javaClass;
        rtJarsList = new String[]{"E:/Program Files/Java/jdk1.8.0_101/jre/lib/rt.jar"};
        classPathList = new String[]{"C:/Users/asus/Desktop/yvm/out/production/yvm/"};
    }

    boolean openDataInputStream() {
        try {
            String parsedFileName = parseFileName(javaClass);
            //1. search in rt.jar
            for (String jar : rtJarsList) {
                JarFile j = new JarFile(jar);
                Enumeration<JarEntry> entry = j.entries();
                while (entry.hasMoreElements()) {
                    JarEntry e = entry.nextElement();
                    if (e.toString().compareTo(parsedFileName) == 0) {
                        in = new DataInputStream(j.getInputStream(e));
                        return true;
                    }
                }
            }
            //2. search in class path
            for (String path : classPathList) {
                File f = new File(path + parsedFileName);
                if (f.exists()) {
                    in = new DataInputStream(new FileInputStream(f));
                    return true;
                }
            }

            return false;
        } catch (IOException e) {
            return false;
        }
    }

    private String parseFileName(String str) throws IOException {
        if (!isValidFileName(str)) {
            throw new IOException("malformed class file name");
        }

        return str.endsWith(".class") ? str : str + ".class";
    }

    private boolean isValidFileName(String str) {
        Pattern pattern = Pattern.compile("([a-zA-Z]+)(\\.class{0,1}|(/[a-zA-Z]+)*)(\\.class){0,1}");
        Matcher matcher = pattern.matcher(str);
        return matcher.matches();
    }

    public boolean isEOF() throws IOException {
        return in.available() == 0;
    }

    public u1 read1Byte() throws IOException {
        int a = in.readUnsignedByte();
        return new u1(a);
    }

    public u2 read2Bytes() throws IOException {
        int a = in.readUnsignedShort();
        return new u2(a);
    }

    public u4 read4Bytes() throws IOException {
        int a = in.readInt();
        return new u4(a);
    }
}

