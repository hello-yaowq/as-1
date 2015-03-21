/* Define the SIP wrapper to the vxlapi library. */  
%Module(name=vxlapi, language="C")  

typedef unsigned long XLuint64;
typedef XLuint64 XLaccess;
typedef short XLstatus;
typedef long XLportHandle;


struct s_xl_can_msg {
	unsigned long id;
	unsigned short flags;
	unsigned short dlc;
	XLuint64 res1;
	//unsigned char data [8];
	unsigned char* data
	{
		%GetCode
			sipPy = PyList_New(8);
			for(int i=0;i<8;i++)
			{
				PyList_SetItem(sipPy,i,PyLong_FromLong(sipCpp->data[i]));
			}
		%End
		
		%SetCode
			if(PyList_Size(sipPy)!=8)
			{
				PyErr_SetString(PyExc_ValueError,"the data must be exactly 8 bytes");
				sipErr = 1;
			}
			else
			{
				for(int i=0;i<8;i++)
				{
					sipCpp->data[i] = PyLong_AsLong(PyList_GetItem(sipPy,i));
				}
			}
		%End
	};
	XLuint64 res2;
};

//typedef unsigned char XLeventTag;
//
//struct s_xl_event {
//	XLeventTag tag;
//	unsigned char chanIndex;
//	unsigned short transId;
//	unsigned short portHandle;
//	unsigned char flags;
//	unsigned char reserved;
//	XLuint64 timeStamp;
//	//union s_xl_tag_data tagData;
//	s_xl_can_msg can_msg;
//};
//
//
//
//typedef s_xl_event XLevent;

XLstatus   xlOpenDriver(void);
XLstatus   xlCloseDriver(void);
/*
 * (status,portHandle,permissionMask) = xlOpenPort( userName,	// bytearray(b'port0')
 * 											accessMask,
 * 											permissionMask,
 * 											rxQueueSize=512,
 * 											xlInterfaceVersion=XL_INTERFACE_VERSION,
 * 											busType=XL_BUS_TYPE_CAN )
 */
XLstatus   xlOpenPort ( XLportHandle *pPortHandle, char *userName, XLaccess accessMask, 
					XLaccess *pPermissionMask /In,Out/,
					unsigned int rxQueueSize=512, 
					unsigned int xlInterfaceVersion=XL_INTERFACE_VERSION, 
					unsigned int busType=XL_BUS_TYPE_CAN);

XLstatus   xlCanSetChannelBitrate ( XLportHandle portHandle=0, XLaccess accessMask=1,
					unsigned long bitrate=500000);
XLstatus   xlActivateChannel ( XLportHandle portHandle=0, XLaccess accessMask=1, 
					unsigned int busType=XL_BUS_TYPE_CAN, 
					unsigned int flags=XL_ACTIVATE_RESET_CLOCK);
XLstatus   xlDeactivateChannel ( XLportHandle portHandle=0, XLaccess accessMask=1);
//XLstatus   xlReceive ( XLportHandle portHandle, 
//					unsigned int *pEventCount /In,Out/, 
//					XLevent *pEvents /Out/)
