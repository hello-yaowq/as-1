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

-- place this file under LAU_PATH=/path/to/this/script/?.lua

-- ===================== [ INCLUDE  ] ================================
require("cantp")
-- ===================== [ MACRO    ] ================================

-- ===================== [ LOCAL    ] ================================
local M = {}
local runtime = {}
-- ===================== [ DATA     ] ================================
dcm = M 

-- ===================== [ FUNCTION ] ================================
function M.init(channel,bus,rxid,txid)
  cantp.init(channel,bus,rxid,txid)
end

function get_service_name(serviceid)
    if serviceid == 0x10 then
      service = "session control"
    elseif serviceid == 0x27 then
      service = "security access"
    elseif serviceid == 0x31 then
      service = "routine control"  
    elseif serviceid == 0x34 then
      service = "request download"        
    elseif serviceid == 0x35 then
      service = "request upload"
    elseif serviceid == 0x36 then
      service = "transfer data"
    elseif serviceid == 0x37 then
      service = "request transfer exit"        
    else
      service = string.format("unknown(%X)",serviceid)
    end
    
    return service
end

function get_nrc_name(nrc)
  if nrc == 0x10 then
    name = "general reject"
  elseif nrc == 0x11 then
    name = "service not supported"
  elseif nrc == 0x12 then
    name = "sub function not supported"
  elseif nrc == 0x13 then
    name = "incorrect message length or invalid format"
  elseif nrc == 0x22 then
    name = "condition not ok"
  elseif nrc == 0x24 then
    name = "request sequence error"
  elseif nrc == 0x31 then
    name = "request out of range"    
  elseif nrc == 0x33 then
    name = "security access denied"
  elseif nrc == 0x35 then
    name = "invalid key"
  elseif nrc == 0x72 then
    name = "general programming failure"
  elseif nrc == 0x7f then
    name = "service not supported in active session"
  else
    name = string.format("unknown(%X)",nrc)
  end
  
  return name
end

local function show_negative_response(res)
  if (res[1] == 0x7f) and (rawlen(res) == 3) then
    service = get_service_name(res[2])
    nrc = get_nrc_name(res[3])
    print(string.format("  >> service '%s' negative response '%s' ",service,nrc))
  else
    print("unknown response")
  end
  
end

local function show_request(req)
  ss = "  >> dcm request  = ["
  len = rawlen(req)
  if len > 16 then
    len = 16
  end
  for i=1,len,1 do
    ss = string.format("%s%02X,",ss,req[i])
  end
  ss = string.format("%s],",ss)
  print(ss)
end

local function show_response(res)
  ss = "  >> dcm response = ["
  len = rawlen(res)
  if len > 16 then
    len = 16
  end
  for i=1,len,1 do
    ss = string.format("%s%02X,",ss,res[i])
  end
  ss = string.format("%s],",ss)
  print(ss)
end

function M.transmit(channel,req)
  ercd = true
  response  = nil
  show_request(req)
  cantp.transmit(channel,req)
  while ercd == true do
    ercd,res = cantp.receive(channel)
    if ercd == true then
      show_response(res)
      if (req[1]|0x40 == res[1]) then
        -- positive response
        response  = res
        break
      elseif (rawlen(res) == 3) and (res[1] == 0x7f) and (res[2] == req[1]) and (res[3] == 0x78) then
        -- response is pending as server is busy
        -- continue
      else
        show_negative_response(res)
        ercd = false 
      end
    else
      ercd = false
    end
  end

  return ercd,response
end
-- ===================== [ INSTANCE ] ================================
return dcm