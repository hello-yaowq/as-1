-- /**
-- * AS - the open source Automotive Software on https://github.com/parai
-- *
-- * Copyright (C) 2015  AS <parai@foxmail.com>
-- *
-- * This source code is free software; you can redistribute it and/or modify it
-- * under the terms of the GNU General Public License version 2 as published by the
-- * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
-- *
-- * This program is distributed in the hope that it will be useful, but
-- * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
-- * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
-- * for more details.
-- */
require("cantp")
require("as")
require("s19")

local function hexs(data)
  ss = "["
  len = rawlen(data)
  for i=1,len,1 do
    ss = string.format("%s%02X,",ss,data[i])
  end
  ss = string.format("%s],",ss)
  return ss
end

function test_can()
  data = {}
  as.can_open(0,"peak",0,1000000)
  cantp.init(0,0,0x732,0x731)
  for i=1,256,1 do
    data[i] = i;
  end
  cantp.transmit(0,data)
  response = cantp.receive(0)
end

function test_s19()
  srecord = s19.open("D:/repository/as/release/ascore/out/stm32f107vc.s19")
  secnbr = rawlen(srecord)
  for i=1,secnbr,1 do
    ss = srecord[i]
    print(string.format("addr=%X,size=%X,data=%s",ss['addr'],ss['size'],hexs(ss['data'])))
  end
end

test_s19()
