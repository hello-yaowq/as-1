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
-- ===================== [ INCLUDE  ] ================================
require("dcm")
require("as")
require("s19")
require("math")
-- ===================== [ MACRO    ] ================================

-- ===================== [ LOCAL    ] ================================
local can_bus = 0

local FLASH_WRITE_SIZE = 512
-- ===================== [ DATA     ] ================================
-- ===================== [ FUNCTION ] ================================
function enter_extend_session()
  ercd,res = dcm.transmit(can_bus,{0x10,0x03})
  
  if (false == ercd) then
    print("  >> enter extend session failed!")
  else
    print("  >> enter extend session ok!")
  end
  
  return ercd
  
end

function enter_program_session()
  ercd,res = dcm.transmit(can_bus,{0x10,0x02})
  
  if (false == ercd) then
    print("  >> enter program session failed!")
  else
    print("  >> enter program session ok!")
  end
  
  return ercd
  
end

function security_extds_access()
  -- level 1 
  ercd,res = dcm.transmit(can_bus,{0x27,0x01})
  
  if (false == ercd) then
    print("  >> security access request seed failed!")
  else
    print("  >> security access request seed ok!")
    seed = (res[3]<<24) + (res[4]<<16) + (res[5]<<8) +(res[6]<<0)
    key = seed ~ 0x78934673
    print(type(key),key)
    ercd,res = dcm.transmit(can_bus,{0x27,0x02,(key>>24)&0xFF,(key>>16)&0xFF,(key>>8)&0xFF,(key>>0)&0xFF})
    if (false == ercd) then
      print("  >> security access send key failed!")
    else
      print("  >> security access send key ok!")
    end
  end
  
  return ercd
  
end

function security_prgs_access()
  -- level 2
  ercd,res = dcm.transmit(can_bus,{0x27,0x03})
  
  if (false == ercd) then
    print("  >> security access request seed failed!")
  else
    print("  >> security access request seed ok!")
    seed = (res[3]<<24) + (res[4]<<16) + (res[5]<<8) +(res[6]<<0)
    key = seed ~ 0x94586792
    ercd,res = dcm.transmit(can_bus,{0x27,0x04,(key>>24)&0xFF,(key>>16)&0xFF,(key>>8)&0xFF,(key>>0)&0xFF})
    if (false == ercd) then
      print("  >> security access send key failed!")
    else
      print("  >> security access send key ok!")
    end
  end
  
  return ercd
  
end


function routine_erase_flash()
  ercd,res = dcm.transmit(can_bus,{0x31,0x01,0xFF,0x01,0x00,0x01,0x00,0x00,0x00,0x0F,0x00,0x00,0xFF})
  -- start address = 0x00010000 = 64K 
  -- end   address = 0x00100000 = 1M, so size =  0x000F0000
  -- identifier 0xFF
  
  if (false == ercd) then
    print("  >> routine erase flash failed!")
  else
    print("  >> routine erase flash ok!")
  end
  
  return ercd
  
end
function request_download(addr,size)
  data = {}
  data[1] = 0x34
  data[2] = 0x00 -- data format identifier
  data[3] = 0x44 -- address and length format
  data[4] = (addr>>24)&0xFF
  data[5] = (addr>>16)&0xFF
  data[6] = (addr>>8)&0xFF
  data[7] = (addr>>0)&0xFF
  data[8] = (size>>24)&0xFF
  data[9] = (size>>16)&0xFF
  data[10] = (size>>8)&0xFF
  data[11] = (size>>0)&0xFF
  data[12] = 0xFF -- memory identifier
 
  ercd,res = dcm.transmit(can_bus,data)
  
  if (false == ercd) then
    print("  >> request download failed!")
  else
    print("  >> request download ok!")
  end
  
  return ercd
end

function request_transfer_exit()
  ercd,res = dcm.transmit(can_bus,{0x37})
  
  if (false == ercd) then
    print("  >> request_transfer_exit failed!")
  else
    print("  >> request_transfer_exit ok!")
  end
  
  return ercd
end

function download_one_record(addr,size,data)
  ercd = request_download(addr,size)
  
  -- download application
  blockSequenceCounter = 1
  left_size = size
  pos = 1
  ability = math.floor((4096-4)/FLASH_WRITE_SIZE) * FLASH_WRITE_SIZE

  while (left_size > 0) and (true== ercd) do
    req ={}
    req[1] = 0x36
    req[2] = blockSequenceCounter
    req[3] = 0
    req[4] = 0xFF
    
    if (left_size > ability) then
      sz = ability
      left_size = left_size - ability
    else
      sz = math.floor((left_size+FLASH_WRITE_SIZE-1)/FLASH_WRITE_SIZE)*FLASH_WRITE_SIZE
      left_size = 0
    end
    for i=pos,size,1 do
      req[5+i-pos] = data[i]
      
      if((i-pos+1) >= sz) then
        break
      end
    end

    ercd,res = dcm.transmit(can_bus,req)
    
    pos = pos + sz

    blockSequenceCounter = (blockSequenceCounter + 1)&0xFF
 
  end
  
  if (true == ercd) then
    ercd = request_transfer_exit()
  end
  
  return ercd
end

function download_application()

  srecord = s19.open("D:/repository/as/release/ascore/out/stm32f107vc.s19")
  
  if( nil == srecord ) then
    print("  >> invalid application srecord file!")
    return false
  end
  
  secnbr = rawlen(srecord)
  for i=1,secnbr,1 do
    ss = srecord[i]
    addr =  0x00010000 + ss['addr'] - srecord[1]['addr']
    ercd = download_one_record(addr,ss['size'],ss['data'])
    if (false == ercd) then
      break
    end
  end
 
  if (false == ercd) then
    print("  >> download application failed!")
  else
    print("  >> download application ok!")
  end
  
  return ercd
end

operation_list = {enter_extend_session, security_extds_access,
                  enter_program_session,security_prgs_access,
                  routine_erase_flash, download_application
}

function main()
  data = {}
  -- as.can_open(can_bus,"rpmsg",0,1000000)
  as.can_open(can_bus,"serial",3,115200)	-- COM4
  dcm.init(can_bus,0,0x732,0x731)
  
  
  for i=1,rawlen(operation_list),1 do
    ercd = operation_list[i]()
    if false == ercd then
      break
    end
  end
end

main()