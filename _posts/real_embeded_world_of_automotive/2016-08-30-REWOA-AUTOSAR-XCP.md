---
layout: post
title: AUTOSAR XCP
category: real embeded world of automotive
comments: true
---

## Test Command List:
Connect:     cansend can0 554#FF00
Disconnect:  cansend can0 554#FE
GetStatus:   cansend can0 554#FD
GetID:       cansend can0 554#FA00

### Calibrating Parameters in the Slave

SetMTA:      cansend can0 554#F6000000607D8900
Download:    cansend can0 554#F00400000041
ShortUpload: cansend can0 554#F4040000607D8900
Upload:      cansend can0 554#F506


### reference:

1. [ASAM+MCD-1+XCP](https://wiki.asam.net/display/STANDARDS/ASAM+MCD-1+XCP)

2. [XCP\_ReferenceBook\_V1.0\_EN.pdf](http://vector.com/portal/medien/solutions_for/xcp/XCP_ReferenceBook_V1.0_EN.pdf)

3. [AUTOSAR\_Monitoring\_HanserAutomotive\_SH\_201111\_PressArticle\_EN.pdf](http://vector.com/portal/medien/cmc/press/Vector/AUTOSAR_Monitoring_HanserAutomotive_SH_201111_PressArticle_EN.pdf)

4. [xcp-book](http://www.vector.com/xcp-book)