# python language software component

# main file list
./can/vxlapi.py : this is the Vector XL Driver Library python interface wrapper which is based on pycanlibxl which is distributed on google code.
./sip/vxlapi.sip : this is the python sip file, try to wrapper vxlapi directly, but this is a hard job, I give up
./sip/can.sip : this is the python sip file, wrapper the vxlapi CAN access API only, this is more simple and easier to do, and I this is more fast than ./can/vxlapi.py.

In all, python ctypes is much more easier to be used to access DLL.


