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
require("os")

local function hexs(data)
  ss = "["
  len = rawlen(data)
  for i=1,len,1 do
    ss = string.format("%s%02X,",ss,data[i])
  end
  ss = string.format("%s],",ss)
  return ss
end

function test_can_peak()
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

function test_can_serail()
  data = {}
  as.can_open(0,"serial",3,115200)
  while true do
	as.can_read(0,0x732)
  end
end

function test_can_vcan()
  data = {}
  as.can_open(0,"socket",32,115200)
  while true do
	ercd,canid,data = as.can_read(0,-1)
	if ercd == true then
	  print(string.format("canid=%03X, data=[%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X]",canid,
		data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8]))
	end
  end
end

function test_can_amb()
  data = {}
  as.can_open(0,"socket",0,1000000)
  while true do
	ercd = as.can_write(0,0x100,{0x00,0x01,0x00,0x02,0x00,0x03,0x00,0x04})
  os.usleep(100)
  end
end

test_can_amb()
