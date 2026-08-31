#include "mokmockprovider.h"

QVector<MokKeyEntry> MokMockProvider::getMockKeys() {
    QVector<MokKeyEntry> mockCache;

    // 1. YOUR FIRST FAKE KEY LISTING (With the full 48-line text dump block)
    MokKeyEntry key1;
    key1.commonName = "KDE Secure Boot Manager Key";
    key1.expirationDate = "Jul 29 2126 GMT";
    key1.serialNumber = "69:A1:C5:DA:BA:A0:C8:98";
    key1.rawCertificate =
    "[key 2]\n"
    "Owner: 605dab50-e046-4300-abb6-3dd810dd8b23\n"
    "SHA1 Fingerprint: 09:36:01:84:cb:8f:34:a9:ff:cf:e8:98:79:b9:51:8f:5a:a0:38:98\n"
    "Certificate:\n"
    "    Data:\n"
    "        Version: 3 (0x2)\n"
    "        Serial Number:\n"
    "            69:a1:c5:da:ba:a0:c8:98:5c:3c:b3:6a:77:bc:aa:ec:78:b8:f2:17\n"
    "        Signature Algorithm: sha256WithRSAEncryption\n"
    "        Issuer: CN=My Custom Kernel Key\n"
    "        Validity\n"
    "            Not Before: Aug 22 01:44:25 2026 GMT\n"
    "            Not After : Jul 29 01:44:25 2126 GMT\n"
    "        Subject: CN=My Custom Kernel Key\n"
    "        Subject Public Key Info:\n"
    "            Public Key Algorithm: rsaEncryption\n"
    "                Public-Key: (2048 bit)\n"
    "                Modulus:\n"
    "                    00:ae:de:6a:25:e3:15:35:13:84:f1:ae:dc:ab:69:\n"
    "                    c8:58:d6:52:4b:3b:18:b9:77:45:21:67:db:70:0b:\n"
    "                    7e:b4:85:20:a6:dc:1e:f2:d8:e6:b2:ed:10:36:cc:\n"
    "                    a7:74:c8:29:0c:a7:c9:11:b5:90:ee:be:b0:19:b7:5a:\n"
    "                    e3:22:d4:d3:2b:cd:e3:dc:ac:e2:52:fe:9a:83:de:\n"
    "                    eb:d9:7a:4f:0c:6c:b8:29:cb:b9:0c:25:e2:dc:d0:\n"
    "                    1e:c1:d5:ac:d7:29:d3:ca:81:32:7f:aa:c4:24:e6:\n"
    "                    b3:91:7b:2a:4d:9c:74:38:90:96:3c:00:cd:df:cf:\n"
    "                    8c:b6:26:d9:7d:49:2f:66:22:bf:ee:c5:11:b6:e5:\n"
    "                    85:8e:47:d1:62:a9:2b:91:95:87:8c:0e:66:c5:74:\n"
    "                    fb:f8:a3:9c:a0:65:19:48:82:c8:af:10:06:fe:11:\n"
    "                    af:b7:8d:e7:27:33:cc:f9:a4:cf:17:a2:84:f7:8c:\n"
    "                    92:22:12:42:ce:68:09:d4:42:63:74:bd:91:d2:45:\n"
    "                    00:70:d9:02:b5:b0:34:16:98:de:c0:60:15:c2:1e:\n"
    "                    e3:82:58:50:46:4a:d2:50:ed:11:23:51:18:82:49:\n"
    "                    2b:01:f0:c1:5f:ea:e8:ea:e2:28:73:58:48:8e:17:\n"
    "                    bf:5d:22:16:36:2a:26:fb:e3:30:8a:97:f0:29:21:\n"
    "                    8d:65\n"
    "                Exponent: 65537 (0x10001)\n"
    "        X509v3 extensions:\n"
    "            X509v3 Subject Key Identifier:\n"
    "                B1:BB:9F:FA:BF:08:BE:13:C5:9C:00:C3:EE:B7:91:1F:AD:FA:35:61\n"
    "            X509v3 Authority Key Identifier:\n"
    "                B1:BB:9F:FA:BF:08:BE:13:C5:9C:00:C3:EE:B7:91:1F:AD:FA:35:61\n"
    "            X509v3 Basic Constraints: critical\n"
    "                CA:TRUE\n"
    "    Signature Algorithm: sha256WithRSAEncryption\n"
    "    Signature Value:\n"
    "        29:3e:c2:f5:cd:eb:b1:9d:68:b8:c6:a5:31:7c:b0:d5:76:d2:\n"
    "        d5:6e:25:cb:c7:14:88:14:fb:cb:7f:57:fe:a8:7a:80:23:50:\n"
    "        70:a9:29:50:cf:a9:c3:88:7b:54:5c:02:2f:52:a7:ba:64:12:\n"
    "        21:d9:0e:f6:5d:97:8f:3b:88:d7:28:27:6f:01:e8:cc:94:a2:\n"
    "        1b:6d:6f:a2:47:eb:92:7d:c6:90:a2:6c:ec:23:70:f7:3e:48:\n"
    "        86:8b:b4:c6:9e:48:f0:34:42:af:6f:3b:5d:be:9f:1c:77:4b:\n"
    "        92:e4:f6:ee:12:01:cb:b9:bf:d0:43:1a:99:d9:10:0b:86:a6:";
    mockCache.append(key1);

    // 2. RESTORING YOUR SECOND FAKE KEY LISTING
    MokKeyEntry key2;
    key2.commonName = "Canonical Ltd. Master Certificate Authority";
    key2.expirationDate = "Dec 31 2046 GMT";
    key2.serialNumber = "46:2A:9F:BC:11:02:88:E3:44:A2";
    key2.rawCertificate =
    "[key 1]\n"
    "SHA1 Fingerprint: 46:2A:9F:BC:11:02:88:E3:44:A2:81:45:9B:21\n"
    "Certificate:\n"
    "    Data:\n"
    "        Version: 3 (0x2)\n"
    "        Issuer: CN=Ubuntu Secure Boot Signer\n"
    "        Validity\n"
    "            Not Before: Dec 01 09:00:00 2016 GMT\n"
    "            Not After : Dec 31 23:59:59 2046 GMT";
    mockCache.append(key2);

    return mockCache;
}
