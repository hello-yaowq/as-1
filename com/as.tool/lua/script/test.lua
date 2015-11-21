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
require("as")
require("math")
require("os")
function wait(time)
  pre = os.clock()
  
  now = os.clock()
  
  while((now-pre)<time) do
    now = os.clock()
  end
end

function main()

    print "Hello World!"
    print "Welcome to arLua, hope better AUTOSAR!"
    print("sin(pi/2)=" , math.sin(math.pi/2))
 
    as.can_write(0,0x731,{0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55})
    wait(100)
	result,canid,data = as.can_read(0)
	print("can_read:",result,canid,data)
	as.can_write(0,0x731,{0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55})
    wait(100)
	result,canid,data = as.can_read(0)
	print("can_read:",result,canid,data[0],data[1],data[2])
	result,canid,data = as.can_read(0)
	print("can_read:",result,canid,data)
end

main()
