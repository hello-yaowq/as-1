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
    else
      service = string.format("unknown(%X)",serviceid)
    end
    
    return service
end

function get_nrc_name(nrc)
    if nrc == 0x33 then
      name = "security access denied"
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

function M.transmit(channel,req)
  ercd = true
  response  = nil
  cantp.transmit(channel,req)
  while ercd == true do
    ercd,res = cantp.receive(channel)
    if ercd == true then
      if (req[1]|0x40 == res[1]) then
        -- positive response
        response  = res
      elseif (rawlen(res) == 3) and (res[1] == 0x7f) and (res[2] == req[0]) and (res[3] == 0x78) then
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

return dcm