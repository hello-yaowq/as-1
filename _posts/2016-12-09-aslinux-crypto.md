---
layout: post
title: aslinux crypto
category: linux
comments: true
---

start from [kernel/crypto/tcrypt.c](https://git.kernel.org/cgit/linux/kernel/git/stable/linux-stable.git/tree/crypto/tcrypt.c?id=refs/tags/v3.18.45) which is the test case of module crypto of linux kernel.

but for the purpose to know some base knowledge of what is crypto, see some use case of crypto in python. It was somehow a check of crypto test case listed in file [kernel/crypto/testmgr.h](https://git.kernel.org/cgit/linux/kernel/git/stable/linux-stable.git/tree/crypto/testmgr.h?id=refs/tags/v3.18.45).

```python
#! /bin/python
#! /bin/python
import array,os
from Crypto.Cipher import AES
from Crypto.Util import Counter

def loghex(ss):
    print "\\x".join("{:02x}".format(ord(c)) for c in ss)

def aes_cbc_enc_tv_templat(key,iv,input,result):
    print("key:");loghex(key);print("iv:");loghex(iv);print("input: '%s'"%(input));loghex(input);
    obj = AES.new(key, AES.MODE_CBC, iv )
    ciphertext = obj.encrypt(input)
    print("result:");loghex(ciphertext)
    if(ciphertext != result): assert(0)
    # reverse check
    obj = AES.new(key, AES.MODE_CBC, iv )
    if(obj.decrypt(result) != input): assert(0)

    
def aes_ctr_dec_tv_template(key,iv,input,result):
    print("key:");loghex(key);print("iv:");loghex(iv);print("input: '%s'"%(input));loghex(input);
    ctr_e = Counter.new(128, initial_value=long(iv.encode("hex"), 16))
    obj = AES.new(key, AES.MODE_CTR, counter=ctr_e)
    ciphertext = obj.decrypt(input)
    print("result:");loghex(ciphertext)
    if(ciphertext != result): assert(0)
    # reverse check
    ctr_e = Counter.new(128, initial_value=long(iv.encode("hex"), 16))
    obj = AES.new(key, AES.MODE_CTR, counter=ctr_e)
    if(obj.encrypt(result) != input): assert(0)

aes_cbc_enc_tv_templat("\x06\xa9\x21\x40\x36\xb8\xa1\x5b\x51\x2e\x03\xd5\x34\x12\x00\x06",
                       "\x3d\xaf\xba\x42\x9d\x9e\xb4\x30\xb4\x22\xda\x80\x2c\x9f\xac\x41",
                       "Single block msg",
                       "\xe3\x53\x77\x9c\x10\x79\xae\xb8\x27\x08\x94\x2d\xbe\x77\x18\x1a")

aes_cbc_enc_tv_templat("\xc2\x86\x69\x6d\x88\x7c\x9a\xa0\x61\x1b\xbb\x3e\x20\x25\xa4\x5a",
                       "\x56\x2e\x17\x99\x6d\x09\x3d\x28\xdd\xb3\xba\x69\x5a\x2e\x6f\x58",
                       "\x00\x01\x02\x03\x04\x05\x06\x07"
              "\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
              "\x10\x11\x12\x13\x14\x15\x16\x17"
              "\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f",
            "\xd2\x96\xcd\x94\xc2\xcc\xcf\x8a"
              "\x3a\x86\x30\x28\xb5\xe1\xdc\x0a"
              "\x75\x86\x60\x2d\x25\x3c\xff\xf9"
              "\x1b\x82\x66\xbe\xa6\xd6\x1a\xb1")

aes_ctr_dec_tv_template( "\x2b\x7e\x15\x16\x28\xae\xd2\xa6"
              "\xab\xf7\x15\x88\x09\xcf\x4f\x3c",
               "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7"
              "\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff",
              "\x87\x4d\x61\x91\xb6\x20\xe3\x26"
              "\x1b\xef\x68\x64\x99\x0d\xb6\xce"
              "\x98\x06\xf6\x6b\x79\x70\xfd\xff"
              "\x86\x17\x18\x7b\xb9\xff\xfd\xff"
              "\x5a\xe4\xdf\x3e\xdb\xd5\xd3\x5e"
              "\x5b\x4f\x09\x02\x0d\xb0\x3e\xab"
              "\x1e\x03\x1d\xda\x2f\xbe\x03\xd1"
              "\x79\x21\x70\xa0\xf3\x00\x9c\xee",
              "\x6b\xc1\xbe\xe2\x2e\x40\x9f\x96"
              "\xe9\x3d\x7e\x11\x73\x93\x17\x2a"
              "\xae\x2d\x8a\x57\x1e\x03\xac\x9c"
              "\x9e\xb7\x6f\xac\x45\xaf\x8e\x51"
              "\x30\xc8\x1c\x46\xa3\x5c\xe4\x11"
              "\xe5\xfb\xc1\x19\x1a\x0a\x52\xef"
              "\xf6\x9f\x24\x45\xdf\x4f\x9b\x17"
              "\xad\x2b\x41\x7b\xe6\x6c\x37\x10")
```

so from this first sight of view, crypto is encryption and decryption of message with key and initialization vector, but there is a lot of different method to be used and which one is the best is hard to say, so I decide to not to learn the algorithm, that's all.

okay, there is a famous library named [libtom](http://www.libtom.org/) which is really a collection of all the kind of most used crypto algorithm.

