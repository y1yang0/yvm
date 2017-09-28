package ycloader;

import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;
import ycloader.exception.ClassLoadingException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import java.io.IOException;
import java.util.ArrayList;

class RTSearchXMLParser {
    static final short TYPE_JARS = 0;
    static final short TYPE_CLASS_PATH = 1;
    private static final String RT_SEARCH_LIST_XML = "./conf/rtsearch.xml";
    private Document document;
    private NodeList root;

    private boolean isReady;

    RTSearchXMLParser() {
        isReady = false;
    }

    private void prepare() throws ClassLoadingException {
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        DocumentBuilder db = null;
        try {
            db = dbf.newDocumentBuilder();
            document = db.parse(RT_SEARCH_LIST_XML);
            root = document.getChildNodes();
            isReady = true;
        } catch (ParserConfigurationException | SAXException | IOException e) {
            throw new ClassLoadingException("config file not found (./conf/rtsearch.xml)");
        }
    }

    private String poorNormalizePath(String path) {
        String res = path.trim();
        res = res.replace("\\\\", "/");
        res = res.replace("\\", "/");
        res = res.replace("\"", "");
        if (!res.endsWith("/")) {
            res += "/";
        }
        return res;
    }

    ArrayList<String> getStringArray(short NODE_TYPE) throws ClassLoadingException {
        if (!isReady) {
            prepare();
        }
        ArrayList<String> res = null;

        switch (NODE_TYPE) {
            case TYPE_JARS:
                for (int i = 0; i < root.getLength(); i++) {
                    if (root.item(i).getNodeName().equals("searchPath")) {
                        NodeList nodeList = root.item(i).getChildNodes();
                        for (int k = 0; k < nodeList.getLength(); k++) {
                            if (nodeList.item(k).getNodeName().equals("jars")) {
                                res = new ArrayList<>();
                                NodeList pNode = nodeList.item(k).getChildNodes();
                                for (int t = 0; t < pNode.getLength(); t++) {
                                    if (pNode.item(t).getNodeName().equals("p")) {
                                        String path = pNode.item(t).getTextContent();
                                        res.add(poorNormalizePath(path));
                                    }
                                }
                            }
                        }
                    }
                }
                return res;
            case TYPE_CLASS_PATH:
                for (int i = 0; i < root.getLength(); i++) {
                    if (root.item(i).getNodeName().equals("searchPath")) {
                        NodeList nodeList = root.item(i).getChildNodes();
                        for (int k = 0; k < nodeList.getLength(); k++) {
                            if (nodeList.item(k).getNodeName().equals("classPath")) {
                                res = new ArrayList<>();
                                NodeList pNode = nodeList.item(k).getChildNodes();
                                for (int t = 0; t < pNode.getLength(); t++) {
                                    if (pNode.item(t).getNodeName().equals("p")) {
                                        String path = pNode.item(t).getTextContent();
                                        res.add(poorNormalizePath(path));
                                    }
                                }
                            }
                        }
                    }
                }
                return res;
            default:
                return null;
        }
    }
}
