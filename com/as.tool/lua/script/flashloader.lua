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
require("dcm")
require("as")
local can_bus = 0

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
  ercd,res = dcm.transmit(can_bus,{0x31,0x01,0xFF,0x01,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00})
  
  if (false == ercd) then
    print("  >> routine erase flash failed!")
  else
    print("  >> routine erase flash ok!")
  end
  
  return ercd
  
end

operation_list = {enter_extend_session, security_extds_access,
                  enter_program_session,security_prgs_access,
                  routine_erase_flash
}

function main()
  data = {}
  as.can_open(can_bus,"rpmsg",0,1000000)
  dcm.init(can_bus,0,0x732,0x731)
  
  
  for i=1,rawlen(operation_list),1 do
    ercd = operation_list[i]()
    if false == ercd then
      break
    end
  end
end

main()