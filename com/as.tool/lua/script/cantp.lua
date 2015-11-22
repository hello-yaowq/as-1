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
require("as")
require("os")
-- ===================== [ MACRO    ] ================================
local ISO15765_TPCI_MASK =  0x30
local ISO15765_TPCI_SF = 0x00         --/* Single Frame */
local ISO15765_TPCI_FF = 0x10         --/* First Frame */
local ISO15765_TPCI_CF = 0x20         --/* Consecutive Frame */
local ISO15765_TPCI_FC = 0x30         --/* Flow Control */
local ISO15765_TPCI_DL = 0x7          --/* Single frame data length mask */
local ISO15765_TPCI_FS_MASK = 0x0F    --/* Flow control status mask */


local ISO15765_FLOW_CONTROL_STATUS_CTS    =    0
local ISO15765_FLOW_CONTROL_STATUS_WAIT   =    1
local ISO15765_FLOW_CONTROL_STATUS_OVFLW  =    2

local CANTP_ST_IDLE = 0
local CANTP_ST_START_TO_SEND = 1
local CANTP_ST_SENDING = 2
local CANTP_ST_WAIT_FC = 3
local CANTP_ST_WAIT_CF = 4
local CANTP_ST_SEND_CF = 5
local CANTP_ST_SEND_FC = 6

local cfgSTmin = 10
local cfgBS    = 8
-- ===================== [ LOCAL    ] ================================
local M = {}
local runtime = {}
-- ===================== [ DATA     ] ================================
cantp = M 
-- ===================== [ FUNCTION ] ================================
function M.init(channel,bus,rxid,txid)
  runtime[channel] = {}
  runtime[channel]["bus"]  = bus
  runtime[channel]["rxid"] = rxid
  runtime[channel]["txid"] = txid
  runtime[channel]["rxbuffer"] = {}
  runtime[channel]["t_size"] = 0 -- already transmit size
  runtime[channel]["SN"] = 0
  runtime[channel]["BS"] = 0
  runtime[channel]["STmin"] = 0
  runtime[channel]["state"] = CANTP_ST_IDLE
end

local function sendSF(channel,data)
  length = rawlen(data)
  bus = runtime[channel]["bus"]
  txid = runtime[channel]["txid"]
  pdu = {}
  pdu[1] = ISO15765_TPCI_SF | length
  for i=1,length,1 do
    pdu[i+1] = data[i]
  end
  for i=length+1,8,1 do
    pdu[i] = 0x55
  end

  return as.can_write(bus,txid,pdu)
end

local function sendFF(channel,data)
  
  bus = runtime[channel]["bus"]
  txid = runtime[channel]["txid"]
  pdu = {}
  pdu[1] = ISO15765_TPCI_FF | ((length>>8)&0x0F)
  pdu[2] = length&0xFF
  
  for i=3,8,1 do
    pdu[i] = data[i-2]
  end
  
  runtime[channel]["SN"] = 0
  runtime[channel]["t_size"] = 6
  runtime[channel]["state"] = CANTP_ST_WAIT_FC
  
  return as.can_write(bus,txid,pdu)
end

local function elapsed(pre)

  gap = os.clock() -pre

  return gap
end

-- wait a response frame
local function waitRF(channel)
  ercd = false
  data=nil
  rxid = runtime[channel]["rxid"]
  pre = os.clock()
  while (elapsed(pre) < 1.0) and (ercd == false) do   -- 1s timeout
    result,canid,data = as.can_read(channel)
    if result and (canid == rxid) then
      ercd = true
      break
    else
      os.usleep(10) -- sleep a while 
    end
  end
  
  if false == ercd then
    print("cantp timeout when receiving a frame! elapsed time = ",elapsed(pre)," s!")
  end
  
  return ercd,data
end

local function sendCF(channel,request) 
  bus = runtime[channel]["bus"]
  txid = runtime[channel]["txid"]
  t_size = runtime[channel]["t_size"]
  pdu = {}
  
  runtime[channel]["SN"] = runtime[channel]["SN"] + 1
  if runtime[channel]["SN"] > 15 then
    runtime[channel]["SN"] = 0
  end
  l_size = rawlen(request) - t_size  --  left size 
  if l_size > 7 then
    l_size = 7
  end
  
  pdu[1] = ISO15765_TPCI_CF | runtime[channel]["SN"]
  
  for i=1,l_size,1 do
    pdu[1+i] = request[t_size+i]
  end
  
  runtime[channel]["t_size"] = runtime[channel]["t_size"] + l_size
  runtime[channel]["state"] = CANTP_ST_WAIT_FC
  
  return as.can_write(bus,txid,pdu)
end

local function handleFC(channel,request)
  ercd,data = waitRF(channel)
  if true == ercd then
    print("FC:",channel,table.concat(data, ":"))
  
    if (data[1]&ISO15765_TPCI_FS_MASK) == ISO15765_FLOW_CONTROL_STATUS_CTS then
      runtime[channel]["BS"] = data[2]
      runtime[channel]["STmin"] = data[3]
      ercd = sendCF(channel,request)
    elseif (data[1]&ISO15765_TPCI_FS_MASK) == ISO15765_FLOW_CONTROL_STATUS_WAIT then
      runtime[channel]["state"] = CANTP_ST_WAIT_FC
    elseif (data[1]&ISO15765_TPCI_FS_MASK) == ISO15765_FLOW_CONTROL_STATUS_OVFLW then
      print("cantp buffer over-flow, cancel...")
      ercd = false
    else
      print(string.format("FC error as reason %X",data[1]))
      ercd = false
    end 
  end
  return ercd
end

local function ScheduleTx(channel,request)

  length = rawlen(request)

  ercd = sendFF(channel,request)  -- FF sends 6 bytes
  
  if true == ercd then
    while(runtime[channel]["t_size"] < length) do
      if runtime[channel]["state"] == CANTP_ST_WAIT_FC then
        ercd = handleFC(channel,request);
      end
      
      if ercd == false then
        break
      end
    end
  end
  
  return ercd
end

function M.transmit(channel,data)
  print(channel,table.concat(data, ":"))
  if rawlen(data) < 7 then
    sendSF(channel,data)
  else
    ScheduleTx(channel,data)
  end
end

return cantp