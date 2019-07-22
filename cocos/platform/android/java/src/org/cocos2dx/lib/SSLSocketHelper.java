package org.cocos2dx.lib;

import android.net.http.SslCertificate;
import android.os.Bundle;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;

public class SSLSocketHelper {


    public static String getSSLCertSHA256FromCert(InputStream certIn){
        try {
            CertificateFactory cf = CertificateFactory.getInstance("X.509");
            Certificate ca = cf.generateCertificate(certIn);
            MessageDigest sha256 = MessageDigest.getInstance("SHA-256");
            byte[] value = sha256.digest(((X509Certificate)ca).getEncoded());
            return byteToHex(value);
        } catch (CertificateException e) {
            e.printStackTrace();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        return null;
    }

    public static String getSSLCertSHA256FromServer(SslCertificate cert){
        Bundle bundle = SslCertificate.saveState(cert);
        if(bundle != null){
            byte[] bytes = bundle.getByteArray("x509-certificate");
            if(bytes != null){
                try {
                    CertificateFactory cf = CertificateFactory.getInstance("X.509");
                    Certificate ca = cf.generateCertificate(new ByteArrayInputStream(bytes));
                    MessageDigest sha256 = MessageDigest.getInstance("SHA-256");
                    byte[] value = sha256.digest(((X509Certificate)ca).getEncoded());
                    return byteToHex(value);
                } catch (CertificateException e) {
                    e.printStackTrace();
                } catch (NoSuchAlgorithmException e) {
                    e.printStackTrace();
                }
            }
        }
        return null;
    }

    public static String byteToHex(byte[] bytes){
        final char[] hexArray = {'0', '1', '2', '3', '4', '5', '6', '7', '8',
                '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        char[] hexChars = new char[bytes.length*2];
        int v;
        for(int i = 0;i<bytes.length;i++){
            v = bytes[i]&0xff;
            hexChars[i*2] = hexArray[v>>>4];
            hexChars[i*2+1] = hexArray[v&0x0f];
        }
        return new String(hexChars);

    }


}
