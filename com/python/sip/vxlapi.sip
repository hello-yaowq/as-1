/* Define the SIP wrapper to the vxlapi library. */  
%Module(name=vxlapi, language="C")  

//# 1 "vxlapi.h"
//# 1 "<built-in>"
//# 1 "vxlapi.h"
//# 238 "vxlapi.h"
enum e_XLevent_type {
  XL_NO_COMMAND = 0,
  XL_RECEIVE_MSG = 1,
  XL_CHIP_STATE = 4,
  XL_TRANSCEIVER = 6,
  XL_TIMER = 8,
  XL_TRANSMIT_MSG = 10,
  XL_SYNC_PULSE = 11,
  XL_APPLICATION_NOTIFICATION = 15,



  XL_LIN_MSG = 20,
  XL_LIN_ERRMSG = 21,
  XL_LIN_SYNCERR = 22,
  XL_LIN_NOANS = 23,
  XL_LIN_WAKEUP = 24,
  XL_LIN_SLEEP = 25,
  XL_LIN_CRCINFO = 26,


  XL_RECEIVE_DAIO_DATA = 32,

  XL_RECEIVE_DAIO_PIGGY = 34,
};
//# 448 "vxlapi.h"
typedef unsigned long XLuint64;

//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack8.h" 1 3

//#pragma pack(push,8)
//# 451 "vxlapi.h" 2







struct s_xl_application_notification {
  unsigned int notifyReason;
  //unsigned int reserved[7];
  unsigned int* reserved
  {
	  %GetCode
	  	  sipPy = PyList_New(7);
	  	  for(int i=0;i<7;i++)
	  	  {
	  		PyList_SetItem(i,PyLong_FromLong(sipCpp->reserved[i]));
	  	  }
	  %End
	  
	  %SetCode
	  	  if(PyList_Size(sipPy)!=7)
	  	  {
	  		  PyErr_SetString(PyExc_ValueError,"the reserved must be exactly 7 bytes");
	  		  sipErr = 1;
	  	  }
	  	  else
	  	  {
	  		  for(int i=0;i<7;i++)
	  		  {
	  			  sipCpp->reserved[i] = PyLong_AsLong(PyList_GetItem(sipPy,i));
	  		  }
	  	  }
	  %End
  };
} ;
typedef s_xl_application_notification XL_APPLICATION_NOTIFICATION_EV;
//# 471 "vxlapi.h"
struct s_xl_sync_pulse_ev {
  unsigned int triggerSource;
  unsigned int reserved;
  XLuint64 time;
} ;
typedef s_xl_sync_pulse_ev XL_SYNC_PULSE_EV;


//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack1.h" 1 3

//#pragma pack(push,1)
//# 480 "vxlapi.h" 2
struct s_xl_sync_pulse {
  unsigned char pulseCode;
  XLuint64 time;
};

//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 486 "vxlapi.h" 2




//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 491 "vxlapi.h" 2
//# 539 "vxlapi.h"
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack1.h" 1 3

//#pragma pack(push,1)
//# 540 "vxlapi.h" 2

typedef char *XLstringType;



typedef XLuint64 XLaccess;



typedef void* XLhandle;
//# 592 "vxlapi.h"
struct s_xl_lin_stat_par{
     unsigned int LINMode;
     int baudrate;
     unsigned int LINVersion;
     unsigned int reserved;
} ;
typedef s_xl_lin_stat_par XLlinStatPar;
//# 637 "vxlapi.h"
struct s_xl_can_msg {
         unsigned long id;
         unsigned short flags;
         unsigned short dlc;
         XLuint64 res1;
         //unsigned char data [8];
         unsigned char* data;
         XLuint64 res2;
       };
//# 660 "vxlapi.h"
struct s_xl_daio_data {
         unsigned short flags;
         unsigned int timestamp_correction;
         unsigned char mask_digital;
         unsigned char value_digital;
         unsigned char mask_analog;
         unsigned char reserved0;
         //unsigned short value_analog[4];
         unsigned short* value_analog
		 {
			  %GetCode
				  sipPy = PyList_New(4);
				  for(int i=0;i<4;i++)
				  {
					PyList_SetItem(i,PyLong_FromLong(sipCpp->value_analog[i]));
				  }
			  %End
			  
			  %SetCode
				  if(PyList_Size(sipPy)!=4)
				  {
					  PyErr_SetString(PyExc_ValueError,"the value_analog must be exactly 4 bytes");
					  sipErr = 1;
				  }
				  else
				  {
					  for(int i=0;i<4;i++)
					  {
						  sipCpp->value_analog[i] = PyLong_AsLong(PyList_GetItem(sipPy,i));
					  }
				  }
			  %End
		 };
         unsigned int pwm_frequency;
         unsigned short pwm_value;
         unsigned int reserved1;
         unsigned int reserved2;
};

struct s_xl_io_digital_data {
  unsigned int digitalInputData;
} ;
typedef s_xl_io_digital_data XL_IO_DIGITAL_DATA;
struct s_xl_io_analog_data {
  unsigned int measuredAnalogData0;
  unsigned int measuredAnalogData1;
  unsigned int measuredAnalogData2;
  unsigned int measuredAnalogData3;
} ;
typedef s_xl_io_analog_data XL_IO_ANALOG_DATA;

struct s_xl_daio_piggy_data {
  unsigned int daioEvtTag;
  unsigned int triggerType;
//  union {
		XL_IO_DIGITAL_DATA digital
		{
			%GetCode
			
			%End
			
			%SetCode
			
			%End
		};
		
		XL_IO_ANALOG_DATA analog	 
		{
			%GetCode
			
			%End
			
			%SetCode
			
			%End
		};
//  } data;
};
//# 703 "vxlapi.h"
struct s_xl_chip_state {
         unsigned char busStatus;
         unsigned char txErrorCounter;
         unsigned char rxErrorCounter;
       };
//# 716 "vxlapi.h"
struct s_xl_transceiver {
         unsigned char event_reason;
         unsigned char is_present;
       };
//# 737 "vxlapi.h"
struct s_xl_lin_msg {
  unsigned char id;
  unsigned char dlc;
  unsigned short flags;
  //unsigned char data[8];
  unsigned char* data
  {
	  %GetCode
		  sipPy = PyList_New(8);
		  for(int i=0;i<8;i++)
		  {
			PyList_SetItem(i,PyLong_FromLong(sipCpp->data[i]));
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
  unsigned char crc;
};
struct s_xl_lin_sleep {
  unsigned char flag;
};

struct s_xl_lin_no_ans {
  unsigned char id;
};

struct s_xl_lin_wake_up {
  unsigned char flag;
};

struct s_xl_lin_crc_info {
  unsigned char id;
  unsigned char flags;
};



//union s_xl_lin_msg_api {
//       struct s_xl_lin_msg linMsg;
//       struct s_xl_lin_no_ans linNoAns;
//       struct s_xl_lin_wake_up linWakeUp;
//       struct s_xl_lin_sleep linSleep;
//       struct s_xl_lin_crc_info linCRCinfo;
//};




//union s_xl_tag_data {
//        struct s_xl_can_msg msg;
//        struct s_xl_chip_state chipState;
//        union s_xl_lin_msg_api linMsgApi;
//        struct s_xl_sync_pulse syncPulse;
//        struct s_xl_daio_data daioData;
//        struct s_xl_transceiver transceiver;
//        struct s_xl_daio_piggy_data daioPiggyData;
//      };

typedef unsigned char XLeventTag;





struct s_xl_event {
         XLeventTag tag;
         unsigned char chanIndex;
         unsigned short transId;
         unsigned short portHandle;
         unsigned char flags;
         unsigned char reserved;
         XLuint64 timeStamp;
         //union s_xl_tag_data tagData;
		 struct s_xl_can_msg msg
		 {
			%GetCode
			
			%End
			
			%SetCode
			
			%End
		 };
		 struct s_xl_chip_state chipState
		 {
			%GetCode
			
			%End
			
			%SetCode
			
			%End
		 };		 
		 //union s_xl_lin_msg_api linMsgApi
		struct s_xl_lin_msg linMsg
		{
			%GetCode
			
			%End
			
			%SetCode
			
			%End
		 };			
		struct s_xl_lin_no_ans linNoAns
		{
			%GetCode
			
			%End
			
			%SetCode
			
			%End
		 };			
		struct s_xl_lin_wake_up linWakeUp
		{
			%GetCode
			
			%End
			
			%SetCode
			
			%End
		 };			
		struct s_xl_lin_sleep linSleep
		{
			%GetCode
			
			%End
			
			%SetCode
			
			%End
		 };			
		struct s_xl_lin_crc_info linCRCinfo
		{
			%GetCode
			
			%End
			
			%SetCode
			
			%End
		 };		 
		 struct s_xl_sync_pulse syncPulse
		 {
			%GetCode
			
			%End
			
			%SetCode
			
			%End
		 };
		 struct s_xl_daio_data daioData
		 {
			%GetCode
			
			%End
			
			%SetCode
			
			%End
		 };
		 struct s_xl_transceiver transceiver
		 {
			%GetCode
			
			%End
			
			%SetCode
			
			%End
		 };
		 struct s_xl_daio_piggy_data daioPiggyData
		 {
			%GetCode
			
			%End
			
			%SetCode
			
			%End
		 };
       };



typedef struct s_xl_event XLevent;
//# 818 "vxlapi.h"
typedef short XLstatus;
//# 862 "vxlapi.h"
struct XLcanFdConf_tag {
  unsigned int arbitrationBitRate;
  unsigned int sjwAbr;
  unsigned int tseg1Abr;
  unsigned int tseg2Abr;
  unsigned int dataBitRate;
  unsigned int sjwDbr;
  unsigned int tseg1Dbr;
  unsigned int tseg2Dbr;
  //unsigned int reserved[2];
  unsigned int* reserved
  {
	  %GetCode
		  sipPy = PyList_New(2);
		  for(int i=0;i<2;i++)
		  {
			PyList_SetItem(i,PyLong_FromLong(sipCpp->reserved[i]));
		  }
	  %End
	  
	  %SetCode
		  if(PyList_Size(sipPy)!=2)
		  {
			  PyErr_SetString(PyExc_ValueError,"the reserved must be exactly 2 bytes");
			  sipErr = 1;
		  }
		  else
		  {
			  for(int i=0;i<2;i++)
			  {
				  sipCpp->reserved[i] = PyLong_AsLong(PyList_GetItem(sipPy,i));
			  }
		  }
	  %End
  };
} ;
typedef XLcanFdConf_tag XLcanFdConf;


struct XLchipParams_tag {
          unsigned long bitRate;
          unsigned char sjw;
          unsigned char tseg1;
          unsigned char tseg2;
          unsigned char sam;
        } ;

typedef XLchipParams_tag XLchipParams;



struct XLbusParams_can{
  unsigned int bitRate;
  unsigned char sjw;
  unsigned char tseg1;
  unsigned char tseg2;
  unsigned char sam;
  unsigned char outputMode;
} ;
struct XLbusParams_most{
  unsigned int activeSpeedGrade;
  unsigned int compatibleSpeedGrade;
  unsigned int inicFwVersion;
};
struct XLbusParams_flexray{

  unsigned int status;
  unsigned int cfgMode;
  unsigned int baudrate;
};
struct XLbusParams_ethernet{
//  unsigned char macAddr[6];
	unsigned char* macAddr
  {
	  %GetCode
		  sipPy = PyList_New(6);
		  for(int i=0;i<6;i++)
		  {
			PyList_SetItem(i,PyLong_FromLong(sipCpp->macAddr[i]));
		  }
	  %End
	  
	  %SetCode
		  if(PyList_Size(sipPy)!=6)
		  {
			  PyErr_SetString(PyExc_ValueError,"the macAddr must be exactly 6 bytes");
			  sipErr = 1;
		  }
		  else
		  {
			  for(int i=0;i<6;i++)
			  {
				  sipCpp->macAddr[i] = PyLong_AsLong(PyList_GetItem(sipPy,i));
			  }
		  }
	  %End
  };
  unsigned char connector;
  unsigned char phy;
  unsigned char link;
  unsigned char speed;
  unsigned char clockMode;
  unsigned char bypass;
};
struct XLbusParams_tag{
  unsigned int busType;
//  union {
   XLbusParams_can  can;
   XLbusParams_most most;
   XLbusParams_flexray flexray;
   XLbusParams_ethernet ethernet;
//   unsigned char raw[32];
   unsigned char* raw
   {
	   %GetCode
	   
	   %End
	   
	   %SetCode
	   
	   %End
   };
//  } data;
};
typedef XLbusParams_tag XLbusParams;


typedef long XLportHandle/*, *pXLportHandle*/;
//# 939 "vxlapi.h"
struct s_xl_license_info {
  unsigned char bAvailable;
//  char licName[65];
  char *licName
  {
	  %GetCode
		 sipPy = PyString_FromStringAndSize(sipCpp->licName, 65);
	 %End

	 %SetCode
		 char *ptr;
		 int length;

		 if (PyString_AsStringAndSize(sipPy, &ptr, &length) == -1)
		 {
			 sipErr = 1;
		 }
		 else if (length != 65)
		 {
			 /*
			  * Raise an exception because the length isn't exactly
			  * right.
			  */

			 PyErr_SetString(PyExc_ValueError,
					 "an licName must be exactly 65 bytes");
			 sipErr = 1;
		 }
		 else
		 {
			 memcpy(sipCpp->buffer, ptr, 65);
		 }
	 %End
  };
} ;
typedef s_xl_license_info XL_LICENSE_INFO;
typedef XL_LICENSE_INFO XLlicenseInfo;


typedef struct s_xl_channel_config {
          char name [31 + 1];
          unsigned char hwType;
          unsigned char hwIndex;
          unsigned char hwChannel;
          unsigned short transceiverType;
          unsigned short transceiverState;
          unsigned short configError;
          unsigned char channelIndex;
          XLuint64 channelMask;
          unsigned int channelCapabilities;
          unsigned int channelBusCapabilities;



          unsigned char isOnBus;
          unsigned int connectedBusType;
          XLbusParams busParams;

          unsigned int driverVersion;
          unsigned int interfaceVersion;
          unsigned int raw_data[10];

          unsigned int serialNumber;
          unsigned int articleNumber;

          char transceiverName [31 + 1];

          unsigned int specialCabFlags;
          unsigned int dominantTimeout;
          unsigned char dominantRecessiveDelay;
          unsigned char recessiveDominantDelay;
          unsigned char connectionInfo;
          unsigned char currentlyAvailableTimestamps;
          unsigned short minimalSupplyVoltage;
          unsigned short maximalSupplyVoltage;
          unsigned int maximalBaudrate;
          unsigned char fpgaCoreCapabilities;
          unsigned char specialDeviceStatus;
          unsigned short channelBusActiveCapabilities;
          unsigned short breakOffset;
          unsigned short delimiterOffset;
          unsigned int reserved[3];
        } XL_CHANNEL_CONFIG;

typedef XL_CHANNEL_CONFIG XLchannelConfig;
typedef XL_CHANNEL_CONFIG *pXLchannelConfig;

typedef struct s_xl_driver_config {
          unsigned int dllVersion;
          unsigned int channelCount;
          unsigned int reserved[10];
          XLchannelConfig channel[64];
        } XL_DRIVER_CONFIG;

typedef XL_DRIVER_CONFIG XLdriverConfig;
typedef XL_DRIVER_CONFIG *pXLdriverConfig;
//# 1032 "vxlapi.h"
struct _XLacc_filt {
  unsigned char isSet;
  unsigned long code;
  unsigned long mask;
};
typedef struct _XLacc_filt XLaccFilt;


struct _XLacceptance {
  XLaccFilt std;
  XLaccFilt xtd;
};
typedef struct _XLacceptance XLacceptance;







//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 1053 "vxlapi.h" 2
//# 1102 "vxlapi.h"
typedef unsigned int XLremoteHandle;
typedef unsigned int XLdeviceAccess;
typedef unsigned int XLremoteStatus;

//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack8.h" 1 3

//#pragma pack(push,8)
//# 1107 "vxlapi.h" 2

typedef struct s_xl_ip_address {
    union {
      unsigned int v4;
      unsigned int v6[4];
    } ip;
    unsigned int prefixLength;
    unsigned int ipVersion;
    unsigned int configPort;
    unsigned int eventPort;
} XLipAddress;

typedef struct s_xl_remote_location_config {
    char hostName[64];
    char alias[64];
    XLipAddress ipAddress;
    XLipAddress userIpAddress;
    unsigned int deviceType;
    unsigned int serialNumber;
    unsigned int articleNumber;
    XLremoteHandle remoteHandle;
} XLremoteLocationConfig;

typedef struct s_xl_remote_device {
    char deviceName[32];
    unsigned int hwType;
    unsigned int articleNumber;
    unsigned int serialNumber;
    unsigned int reserved;
} XLremoteDevice;

typedef struct s_xl_remote_device_info {
    XLremoteLocationConfig locationConfig;
    unsigned int flags;
    unsigned int reserved;
    unsigned int nbrOfDevices;
    XLremoteDevice deviceInfo[16];
} XLremoteDeviceInfo;


//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 1148 "vxlapi.h" 2
//# 1408 "vxlapi.h"
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack8.h" 1 3

//#pragma pack(push,8)
//# 1409 "vxlapi.h" 2




typedef struct s_xl_most_ctrl_spy {
  unsigned int arbitration;
  unsigned short targetAddress;
  unsigned short sourceAddress;
  unsigned char ctrlType;
  unsigned char ctrlData[17];
  unsigned short crc;
  unsigned short txStatus;
  unsigned short ctrlRes;
  unsigned int spyRxStatus;
} XL_MOST_CTRL_SPY_EV;

typedef struct s_xl_most_ctrl_msg {
  unsigned char ctrlPrio;
  unsigned char ctrlType;
  unsigned short targetAddress;
  unsigned short sourceAddress;
  unsigned char ctrlData[17];
  unsigned char direction;
  unsigned int status;
} XL_MOST_CTRL_MSG_EV;

typedef struct s_xl_most_async_msg {
  unsigned int status;
  unsigned int crc;
  unsigned char arbitration;
  unsigned char length;
  unsigned short targetAddress;
  unsigned short sourceAddress;
  unsigned char asyncData[1018];
} XL_MOST_ASYNC_MSG_EV;

typedef struct s_xl_most_async_tx {
  unsigned char arbitration;
  unsigned char length;
  unsigned short targetAddress;
  unsigned short sourceAddress;
  unsigned char asyncData[1014];
} XL_MOST_ASYNC_TX_EV;

typedef struct s_xl_most_special_register {
  unsigned int changeMask;
  unsigned int lockStatus;
  unsigned char register_bNAH;
  unsigned char register_bNAL;
  unsigned char register_bGA;
  unsigned char register_bAPAH;
  unsigned char register_bAPAL;
  unsigned char register_bNPR;
  unsigned char register_bMPR;
  unsigned char register_bNDR;
  unsigned char register_bMDR;
  unsigned char register_bSBC;
  unsigned char register_bXTIM;
  unsigned char register_bXRTY;
} XL_MOST_SPECIAL_REGISTER_EV;

typedef struct s_xl_most_event_source {
  unsigned int mask;
  unsigned int state;
} XL_MOST_EVENT_SOURCE_EV;

typedef struct s_xl_most_all_bypass {
  unsigned int bypassState;
} XL_MOST_ALL_BYPASS_EV;

typedef struct s_xl_most_timing_mode {
  unsigned int timingmode;
} XL_MOST_TIMING_MODE_EV;

typedef struct s_xl_most_timing_mode_spdif {
  unsigned int timingmode;
} XL_MOST_TIMING_MODE_SPDIF_EV;

typedef struct s_xl_most_frequency {
  unsigned int frequency;
} XL_MOST_FREQUENCY_EV;

typedef struct s_xl_most_register_bytes {
  unsigned int number;
  unsigned int address;
  unsigned char value[16];
} XL_MOST_REGISTER_BYTES_EV;

typedef struct s_xl_most_register_bits {
  unsigned int address;
  unsigned int value;
  unsigned int mask;
} XL_MOST_REGISTER_BITS_EV;

typedef struct s_xl_most_sync_alloc {
  unsigned char allocTable[64];
} XL_MOST_SYNC_ALLOC_EV;

typedef struct s_xl_most_ctrl_sync_audio {
  unsigned int channelMask[4];
  unsigned int device;
  unsigned int mode;
} XL_MOST_CTRL_SYNC_AUDIO_EV;

typedef struct s_xl_most_ctrl_sync_audio_ex {
  unsigned int channelMask[16];
  unsigned int device;
  unsigned int mode;
} XL_MOST_CTRL_SYNC_AUDIO_EX_EV;

typedef struct s_xl_most_sync_volume_status {
  unsigned int device;
  unsigned int volume;
} XL_MOST_SYNC_VOLUME_STATUS_EV;

typedef struct s_xl_most_sync_mutes_status {
  unsigned int device;
  unsigned int mute;
} XL_MOST_SYNC_MUTES_STATUS_EV;

typedef struct s_xl_most_rx_light {
  unsigned int light;
} XL_MOST_RX_LIGHT_EV;

typedef struct s_xl_most_tx_light {
  unsigned int light;
} XL_MOST_TX_LIGHT_EV;

typedef struct s_xl_most_light_power {
  unsigned int lightPower;
} XL_MOST_LIGHT_POWER_EV;

typedef struct s_xl_most_lock_status {
  unsigned int lockStatus;
} XL_MOST_LOCK_STATUS_EV;

typedef struct s_xl_most_supervisor_lock_status {
  unsigned int supervisorLockStatus;
} XL_MOST_SUPERVISOR_LOCK_STATUS_EV;

typedef struct s_xl_most_gen_light_error {
  unsigned int lightOnTime;
  unsigned int lightOffTime;
  unsigned int repeat;
} XL_MOST_GEN_LIGHT_ERROR_EV;

typedef struct s_xl_most_gen_lock_error {
  unsigned int lockOnTime;
  unsigned int lockOffTime;
  unsigned int repeat;
} XL_MOST_GEN_LOCK_ERROR_EV;

typedef struct s_xl_most_rx_buffer {
  unsigned int mode;
} XL_MOST_RX_BUFFER_EV;

typedef struct s_xl_most_error {
  unsigned int errorCode;
  unsigned int parameter[3];
} XL_MOST_ERROR_EV;

typedef XL_SYNC_PULSE_EV XL_MOST_SYNC_PULSE_EV;

typedef struct s_xl_most_ctrl_busload {
  unsigned int busloadCtrlStarted;
} XL_MOST_CTRL_BUSLOAD_EV;

typedef struct s_xl_most_async_busload {
  unsigned int busloadAsyncStarted;
} XL_MOST_ASYNC_BUSLOAD_EV;

typedef struct s_xl_most_stream_state {
  unsigned int streamHandle;
  unsigned int streamState;
  unsigned int streamError;
  unsigned int reserved;
} XL_MOST_STREAM_STATE_EV;

typedef struct s_xl_most_stream_buffer {
  unsigned int streamHandle;
  unsigned char * pBuffer;
  unsigned int validBytes;
  unsigned int status;
  unsigned int pBuffer_highpart;
} XL_MOST_STREAM_BUFFER_EV;


typedef struct s_xl_most_sync_tx_underflow {
  unsigned int streamHandle;
  unsigned int reserved;
} XL_MOST_SYNC_TX_UNDERFLOW_EV;

typedef struct s_xl_most_sync_rx_overflow {
  unsigned int streamHandle;
  unsigned int reserved;
} XL_MOST_SYNC_RX_OVERFLOW_EV;






union s_xl_most_tag_data {
    XL_MOST_CTRL_SPY_EV mostCtrlSpy;
    XL_MOST_CTRL_MSG_EV mostCtrlMsg;
    XL_MOST_ASYNC_MSG_EV mostAsyncMsg;
    XL_MOST_ASYNC_TX_EV mostAsyncTx;
    XL_MOST_SPECIAL_REGISTER_EV mostSpecialRegister;
    XL_MOST_EVENT_SOURCE_EV mostEventSource;
    XL_MOST_ALL_BYPASS_EV mostAllBypass;
    XL_MOST_TIMING_MODE_EV mostTimingMode;
    XL_MOST_TIMING_MODE_SPDIF_EV mostTimingModeSpdif;
    XL_MOST_FREQUENCY_EV mostFrequency;
    XL_MOST_REGISTER_BYTES_EV mostRegisterBytes;
    XL_MOST_REGISTER_BITS_EV mostRegisterBits;
    XL_MOST_SYNC_ALLOC_EV mostSyncAlloc;
    XL_MOST_CTRL_SYNC_AUDIO_EV mostCtrlSyncAudio;
    XL_MOST_CTRL_SYNC_AUDIO_EX_EV mostCtrlSyncAudioEx;
    XL_MOST_SYNC_VOLUME_STATUS_EV mostSyncVolumeStatus;
    XL_MOST_SYNC_MUTES_STATUS_EV mostSyncMuteStatus;
    XL_MOST_RX_LIGHT_EV mostRxLight;
    XL_MOST_TX_LIGHT_EV mostTxLight;
    XL_MOST_LIGHT_POWER_EV mostLightPower;
    XL_MOST_LOCK_STATUS_EV mostLockStatus;
    XL_MOST_GEN_LIGHT_ERROR_EV mostGenLightError;
    XL_MOST_GEN_LOCK_ERROR_EV mostGenLockError;
    XL_MOST_RX_BUFFER_EV mostRxBuffer;
    XL_MOST_ERROR_EV mostError;
    XL_MOST_SYNC_PULSE_EV mostSyncPulse;
    XL_MOST_CTRL_BUSLOAD_EV mostCtrlBusload;
    XL_MOST_ASYNC_BUSLOAD_EV mostAsyncBusload;
    XL_MOST_STREAM_STATE_EV mostStreamState;
    XL_MOST_STREAM_BUFFER_EV mostStreamBuffer;
    XL_MOST_SYNC_TX_UNDERFLOW_EV mostSyncTxUnderflow;
    XL_MOST_SYNC_RX_OVERFLOW_EV mostSyncRxOverflow;
};


typedef unsigned short XLmostEventTag;

struct s_xl_most_event {
  unsigned int size;
  XLmostEventTag tag;
  unsigned short channelIndex;
  unsigned int userHandle;
  unsigned short flagsChip;
  unsigned short reserved;
  XLuint64 timeStamp;
  XLuint64 timeStampSync;


  union s_xl_most_tag_data tagData;
};

typedef struct s_xl_most_event XLmostEvent;

typedef XL_MOST_CTRL_MSG_EV XLmostCtrlMsg;
typedef XL_MOST_ASYNC_TX_EV XLmostAsyncMsg;

typedef struct s_xl_most_ctrl_busload_configuration {
  unsigned int transmissionRate;
  unsigned int counterType;
  unsigned int counterPosition;
  XL_MOST_CTRL_MSG_EV busloadCtrlMsg;
} XL_MOST_CTRL_BUSLOAD_CONFIGURATION;


typedef struct s_xl_most_async_busload_configuration {
  unsigned int transmissionRate;
  unsigned int counterType;
  unsigned int counterPosition;
  XL_MOST_ASYNC_TX_EV busloadAsyncMsg;
} XL_MOST_ASYNC_BUSLOAD_CONFIGURATION;

typedef XL_MOST_CTRL_BUSLOAD_CONFIGURATION XLmostCtrlBusloadConfiguration;
typedef XL_MOST_ASYNC_BUSLOAD_CONFIGURATION XLmostAsyncBusloadConfiguration;

typedef struct s_xl_most_device_state {
  unsigned int selectionMask;

  unsigned int lockState;
  unsigned int rxLight;
  unsigned int txLight;
  unsigned int txLightPower;

  unsigned char registerBunch1[16];

  unsigned int bypassState;
  unsigned int timingMode;
  unsigned int frequency;

  unsigned char registerBunch2[2];

  unsigned char registerBunch3[2];

  unsigned int volume[2];
  unsigned int mute[2];

  unsigned int eventSource;

  unsigned int rxBufferMode;

  unsigned char allocTable[64];

  unsigned int supervisorLockStatus;

  unsigned int broadcastedConfigStatus;
  unsigned int adrNetworkMaster;
  unsigned int abilityToWake;
} XL_MOST_DEVICE_STATE;

typedef XL_MOST_DEVICE_STATE XLmostDeviceState;

typedef struct s_xl_most_stream_open {
  unsigned int* pStreamHandle;
  unsigned int numSyncChannels;
  unsigned int direction;
  unsigned int options;
  unsigned int latency;
} XL_MOST_STREAM_OPEN;

typedef XL_MOST_STREAM_OPEN XLmostStreamOpen;

typedef struct s_xl_most_stream_info {
  unsigned int streamHandle;
  unsigned int numSyncChannels;
  unsigned int direction;
  unsigned int options;
  unsigned int latency;
  unsigned int streamState;
  unsigned int reserved;
  unsigned char syncChannels[60];
} XL_MOST_STREAM_INFO;

typedef XL_MOST_STREAM_INFO XLmostStreamInfo;


//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 1747 "vxlapi.h" 2
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack4.h" 1 3

//#pragma pack(push,4)
//# 1748 "vxlapi.h" 2
//# 1761 "vxlapi.h"
typedef struct s_xl_fr_cluster_configuration {

  unsigned int busGuardianEnable;
  unsigned int baudrate;
  unsigned int busGuardianTick;
  unsigned int externalClockCorrectionMode;
  unsigned int gColdStartAttempts;
  unsigned int gListenNoise;
  unsigned int gMacroPerCycle;
  unsigned int gMaxWithoutClockCorrectionFatal;
  unsigned int gMaxWithoutClockCorrectionPassive;
  unsigned int gNetworkManagementVectorLength;
  unsigned int gNumberOfMinislots;
  unsigned int gNumberOfStaticSlots;
  unsigned int gOffsetCorrectionStart;
  unsigned int gPayloadLengthStatic;
  unsigned int gSyncNodeMax;
  unsigned int gdActionPointOffset;
  unsigned int gdDynamicSlotIdlePhase;
  unsigned int gdMacrotick;
  unsigned int gdMinislot;
  unsigned int gdMiniSlotActionPointOffset;
  unsigned int gdNIT;
  unsigned int gdStaticSlot;
  unsigned int gdSymbolWindow;
  unsigned int gdTSSTransmitter;
  unsigned int gdWakeupSymbolRxIdle;
  unsigned int gdWakeupSymbolRxLow;
  unsigned int gdWakeupSymbolRxWindow;
  unsigned int gdWakeupSymbolTxIdle;
  unsigned int gdWakeupSymbolTxLow;
  unsigned int pAllowHaltDueToClock;
  unsigned int pAllowPassiveToActive;
  unsigned int pChannels;
  unsigned int pClusterDriftDamping;
  unsigned int pDecodingCorrection;
  unsigned int pDelayCompensationA;
  unsigned int pDelayCompensationB;
  unsigned int pExternOffsetCorrection;
  unsigned int pExternRateCorrection;
  unsigned int pKeySlotUsedForStartup;
  unsigned int pKeySlotUsedForSync;
  unsigned int pLatestTx;
  unsigned int pMacroInitialOffsetA;
  unsigned int pMacroInitialOffsetB;
  unsigned int pMaxPayloadLengthDynamic;
  unsigned int pMicroInitialOffsetA;
  unsigned int pMicroInitialOffsetB;
  unsigned int pMicroPerCycle;
  unsigned int pMicroPerMacroNom;
  unsigned int pOffsetCorrectionOut;
  unsigned int pRateCorrectionOut;
  unsigned int pSamplesPerMicrotick;
  unsigned int pSingleSlotEnabled;
  unsigned int pWakeupChannel;
  unsigned int pWakeupPattern;
  unsigned int pdAcceptedStartupRange;
  unsigned int pdListenTimeout;
  unsigned int pdMaxDrift;
  unsigned int pdMicrotick;
  unsigned int gdCASRxLowMax;
  unsigned int gChannels;
  unsigned int vExternOffsetControl;
  unsigned int vExternRateControl;
  unsigned int pChannelsMTS;

  unsigned int framePresetData;

  unsigned int reserved[15];
} XLfrClusterConfig;



typedef struct s_xl_fr_channel_config {
  unsigned int status;
  unsigned int cfgMode;
  unsigned int reserved[6];
  XLfrClusterConfig xlFrClusterConfig;
} XLfrChannelConfig;
//# 1866 "vxlapi.h"
typedef struct s_xl_fr_set_modes {
  unsigned int frMode;
  unsigned int frStartupAttributes;
  unsigned int reserved[30];
} XLfrMode;
//# 1890 "vxlapi.h"
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 1891 "vxlapi.h" 2

//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack8.h" 1 3

//#pragma pack(push,8)
//# 1893 "vxlapi.h" 2
//# 1908 "vxlapi.h"
typedef struct s_xl_fr_acceptance_filter {
  unsigned int filterStatus;
  unsigned int filterTypeMask;
  unsigned int filterFirstSlot;
  unsigned int filterLastSlot;
  unsigned int filterChannelMask;
} XLfrAcceptanceFilter;
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 1916 "vxlapi.h" 2
//# 2055 "vxlapi.h"
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack8.h" 1 3

//#pragma pack(push,8)
//# 2056 "vxlapi.h" 2
//# 2064 "vxlapi.h"
typedef struct s_xl_fr_start_cycle {
  unsigned int cycleCount;
  int vRateCorrection;
  int vOffsetCorrection;
  unsigned int vClockCorrectionFailed;
  unsigned int vAllowPassivToActive;
  unsigned int reserved[3];
} XL_FR_START_CYCLE_EV;

typedef struct s_xl_fr_rx_frame {
  unsigned short flags;
  unsigned short headerCRC;
  unsigned short slotID;
  unsigned char cycleCount;
  unsigned char payloadLength;
  unsigned char data[254];
} XL_FR_RX_FRAME_EV;

typedef struct s_xl_fr_tx_frame {
  unsigned short flags;
  unsigned short slotID;
  unsigned char offset;
  unsigned char repetition;
  unsigned char payloadLength;
  unsigned char txMode;
  unsigned char incrementSize;
  unsigned char incrementOffset;
  unsigned char reserved0;
  unsigned char reserved1;
  unsigned char data[254];
} XL_FR_TX_FRAME_EV;

typedef struct s_xl_fr_wakeup {
  unsigned char cycleCount;
  unsigned char wakeupStatus;
  unsigned char reserved[6];
} XL_FR_WAKEUP_EV;

typedef struct s_xl_fr_symbol_window {
  unsigned int symbol;
  unsigned int flags;
  unsigned char cycleCount;
  unsigned char reserved[7];
} XL_FR_SYMBOL_WINDOW_EV;

typedef struct s_xl_fr_status {
  unsigned int statusType;
  unsigned int reserved;
} XL_FR_STATUS_EV;

typedef struct s_xl_fr_nm_vector {
  unsigned char nmVector[12];
  unsigned char cycleCount;
  unsigned char reserved[3];
} XL_FR_NM_VECTOR_EV;

typedef XL_SYNC_PULSE_EV XL_FR_SYNC_PULSE_EV;

typedef struct s_xl_fr_error_poc_mode {
  unsigned char errorMode;
  unsigned char reserved[3];
} XL_FR_ERROR_POC_MODE_EV;

typedef struct s_xl_fr_error_sync_frames {
  unsigned short evenSyncFramesA;
  unsigned short oddSyncFramesA;
  unsigned short evenSyncFramesB;
  unsigned short oddSyncFramesB;
  unsigned int reserved;
} XL_FR_ERROR_SYNC_FRAMES_EV;

typedef struct s_xl_fr_error_clock_corr_failure {
  unsigned short evenSyncFramesA;
  unsigned short oddSyncFramesA;
  unsigned short evenSyncFramesB;
  unsigned short oddSyncFramesB;
  unsigned int flags;
  unsigned int clockCorrFailedCounter;
  unsigned int reserved;
} XL_FR_ERROR_CLOCK_CORR_FAILURE_EV;

typedef struct s_xl_fr_error_nit_failure {
  unsigned int flags;
  unsigned int reserved;
} XL_FR_ERROR_NIT_FAILURE_EV;

typedef struct s_xl_fr_error_cc_error {
  unsigned int ccError;
  unsigned int reserved;
} XL_FR_ERROR_CC_ERROR_EV;

union s_xl_fr_error_info {
  XL_FR_ERROR_POC_MODE_EV frPocMode;
  XL_FR_ERROR_SYNC_FRAMES_EV frSyncFramesBelowMin;
  XL_FR_ERROR_SYNC_FRAMES_EV frSyncFramesOverload;
  XL_FR_ERROR_CLOCK_CORR_FAILURE_EV frClockCorrectionFailure;
  XL_FR_ERROR_NIT_FAILURE_EV frNitFailure;
  XL_FR_ERROR_CC_ERROR_EV frCCError;
};

typedef struct s_xl_fr_error {
  unsigned char tag;
  unsigned char cycleCount;
  unsigned char reserved[6];
  union s_xl_fr_error_info errorInfo;
} XL_FR_ERROR_EV;

typedef struct s_xl_fr_spy_frame {
  unsigned int frameLength;
  unsigned char frameError;
  unsigned char tssLength;
  unsigned short headerFlags;
  unsigned short slotID;
  unsigned short headerCRC;
  unsigned char payloadLength;
  unsigned char cycleCount;
  unsigned short reserved;
  unsigned int frameCRC;
  unsigned char data[254];
} XL_FR_SPY_FRAME_EV;

typedef struct s_xl_fr_spy_symbol {
  unsigned short lowLength;
  unsigned short reserved;
 } XL_FR_SPY_SYMBOL_EV;






union s_xl_fr_tag_data {
  XL_FR_START_CYCLE_EV frStartCycle;
  XL_FR_RX_FRAME_EV frRxFrame;
  XL_FR_TX_FRAME_EV frTxFrame;
  XL_FR_WAKEUP_EV frWakeup;
  XL_FR_SYMBOL_WINDOW_EV frSymbolWindow;
  XL_FR_ERROR_EV frError;
  XL_FR_STATUS_EV frStatus;
  XL_FR_NM_VECTOR_EV frNmVector;
  XL_FR_SYNC_PULSE_EV frSyncPulse;
  XL_FR_SPY_FRAME_EV frSpyFrame;
  XL_FR_SPY_SYMBOL_EV frSpySymbol;

  XL_APPLICATION_NOTIFICATION_EV applicationNotification;

  unsigned char raw[512 - 32];
};

typedef unsigned short XLfrEventTag;

struct s_xl_fr_event {
  unsigned int size;
  XLfrEventTag tag;
  unsigned short channelIndex;
  unsigned int userHandle;
  unsigned short flagsChip;
  unsigned short reserved;
  XLuint64 timeStamp;
  XLuint64 timeStampSync;


  union s_xl_fr_tag_data tagData;
};

typedef struct s_xl_fr_event XLfrEvent;


//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 2233 "vxlapi.h" 2
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack8.h" 1 3

//#pragma pack(push,8)
//# 2234 "vxlapi.h" 2
//# 2249 "vxlapi.h"
typedef struct s_xl_daio_trigger_mode {
  unsigned int portTypeMask;
  unsigned int triggerType;

  union triggerTypeParams {
    unsigned int cycleTime;
    struct {
      unsigned int portMask;
      unsigned int type;
    } digital;
  } param;

} XLdaioTriggerMode;






typedef struct xl_daio_set_port{
 unsigned int portType;
 unsigned int portMask;
 unsigned int portFunction[8];
 unsigned int reserved[8];
} XLdaioSetPort;
//# 2293 "vxlapi.h"
typedef struct xl_daio_digital_params{
 unsigned int portMask;
 unsigned int valueMask;
} XLdaioDigitalParams;
//# 2310 "vxlapi.h"
typedef struct xl_daio_analog_params {
 unsigned int portMask;
 unsigned int value[8];
} XLdaioAnalogParams;
//# 2326 "vxlapi.h"
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 2327 "vxlapi.h" 2
//# 2471 "vxlapi.h"
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack1.h" 1 3

//#pragma pack(push,1)
//# 2472 "vxlapi.h" 2





typedef unsigned short XLethEventTag;

typedef struct s_xl_eth_frame {
  unsigned short etherType;
  unsigned char payload[(unsigned int) 1500];
} T_XL_ETH_FRAME;

typedef union s_xl_eth_framedata {
  unsigned char rawData[(unsigned int) 1600];
  T_XL_ETH_FRAME ethFrame;
} T_XL_ETH_FRAMEDATA;

typedef struct s_xl_eth_dataframe_rx {
  unsigned int frameIdentifier;
  unsigned int frameDuration;
  unsigned short dataLen;
  unsigned short reserved;
  unsigned int reserved2[3];
  unsigned int fcs;
  unsigned char destMAC[6];
  unsigned char sourceMAC[6];
  T_XL_ETH_FRAMEDATA frameData;
} T_XL_ETH_DATAFRAME_RX;

typedef struct s_xl_eth_dataframe_rxerror {
  unsigned int frameIdentifier;
  unsigned int frameDuration;
  unsigned int errorFlags;
  unsigned short dataLen;
  unsigned short reserved;
  unsigned int reserved2[3];
  unsigned int fcs;
  unsigned char destMAC[6];
  unsigned char sourceMAC[6];
  T_XL_ETH_FRAMEDATA frameData;
} T_XL_ETH_DATAFRAME_RX_ERROR;

typedef struct s_xl_eth_dataframe_tx {
  unsigned int frameIdentifier;
  unsigned int flags;
  unsigned short dataLen;
  unsigned short reserved;
  unsigned int reserved2[4];
  unsigned char destMAC[6];
  unsigned char sourceMAC[6];
  T_XL_ETH_FRAMEDATA frameData;
} T_XL_ETH_DATAFRAME_TX;

typedef struct s_xl_eth_dataframe_tx_event {
  unsigned int frameIdentifier;
  unsigned int flags;
  unsigned short dataLen;
  unsigned short reserved;
  unsigned int frameDuration;
  unsigned int reserved2[2];
  unsigned int fcs;
  unsigned char destMAC[6];
  unsigned char sourceMAC[6];
  T_XL_ETH_FRAMEDATA frameData;
} T_XL_ETH_DATAFRAME_TX_EVENT;

typedef T_XL_ETH_DATAFRAME_TX_EVENT T_XL_ETH_DATAFRAME_TXACK;
typedef T_XL_ETH_DATAFRAME_TX_EVENT T_XL_ETH_DATAFRAME_TXACK_SW;
typedef T_XL_ETH_DATAFRAME_TX_EVENT T_XL_ETH_DATAFRAME_TXACK_OTHERAPP;

typedef struct s_xl_eth_dataframe_txerror {
  unsigned int errorType;
  T_XL_ETH_DATAFRAME_TX_EVENT txFrame;
} T_XL_ETH_DATAFRAME_TX_ERROR;

typedef T_XL_ETH_DATAFRAME_TX_ERROR T_XL_ETH_DATAFRAME_TX_ERR_SW;
typedef T_XL_ETH_DATAFRAME_TX_ERROR T_XL_ETH_DATAFRAME_TX_ERR_OTHERAPP;

//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 2551 "vxlapi.h" 2
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack4.h" 1 3

//#pragma pack(push,4)
//# 2552 "vxlapi.h" 2

typedef struct s_xl_eth_config_result {
  unsigned int result;
} T_XL_ETH_CONFIG_RESULT;

typedef struct s_xl_eth_channel_status {
  unsigned int link;
  unsigned int speed;
  unsigned int duplex;
  unsigned int mdiType;
  unsigned int activeConnector;
  unsigned int activePhy;
  unsigned int clockMode;
  unsigned int brPairs;
} T_XL_ETH_CHANNEL_STATUS;

typedef struct s_xl_eth_lostevent {
  XLethEventTag eventTypeLost;
  unsigned short reserved;
  unsigned int reason;
  union {
    struct {
      unsigned int frameIdentifier;
      unsigned int fcs;
      unsigned char sourceMAC[6];
      unsigned char reserved[2];
    } txAck, txAckSw;
    struct {
      unsigned int errorType;
      unsigned int frameIdentifier;
      unsigned int fcs;
      unsigned char sourceMAC[6];
      unsigned char reserved[2];
    } txError, txErrorSw;
    unsigned int reserved[20];
  } eventInfo;
} T_XL_ETH_LOSTEVENT;

typedef struct s_xl_eth_event {
  unsigned int size;
  XLethEventTag tag;
  unsigned short channelIndex;
  unsigned int userHandle;
  unsigned short flagsChip;
  unsigned short reserved;
  XLuint64 reserved1;
  XLuint64 timeStampSync;



  union s_xl_eth_tag_data {
    unsigned char rawData[(unsigned int) 2048];
    T_XL_ETH_DATAFRAME_RX frameRxOk;
    T_XL_ETH_DATAFRAME_RX_ERROR frameRxError;
    T_XL_ETH_DATAFRAME_TXACK frameTxAck;
    T_XL_ETH_DATAFRAME_TXACK_SW frameTxAckSw;
    T_XL_ETH_DATAFRAME_TXACK_OTHERAPP frameTxAckOtherApp;
    T_XL_ETH_DATAFRAME_TX_ERROR frameTxError;
    T_XL_ETH_DATAFRAME_TX_ERR_SW frameTxErrorSw;
    T_XL_ETH_DATAFRAME_TX_ERR_OTHERAPP frameTxErrorOtherApp;
    T_XL_ETH_CONFIG_RESULT configResult;
    T_XL_ETH_CHANNEL_STATUS channelStatus;
    XL_SYNC_PULSE_EV syncPulse;
    T_XL_ETH_LOSTEVENT lostEvent;
  } tagData;
} T_XL_ETH_EVENT;

typedef struct {
  unsigned int speed;
  unsigned int duplex;
  unsigned int connector;
  unsigned int phy;
  unsigned int clockMode;
  unsigned int mdiMode;
  unsigned int brPairs;
} T_XL_ETH_CONFIG;

//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 2630 "vxlapi.h" 2
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack4.h" 1 3

//#pragma pack(push,4)
//# 2631 "vxlapi.h" 2
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 2632 "vxlapi.h" 2
//# 2980 "vxlapi.h"
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack1.h" 1 3

//#pragma pack(push,1)
//# 2981 "vxlapi.h" 2





typedef unsigned short XLmostEventTag;


typedef struct s_xl_most150_event_source{
  unsigned int sourceMask;
} XL_MOST150_EVENT_SOURCE_EV;

typedef struct s_xl_most150_device_mode {
  unsigned int deviceMode;
} XL_MOST150_DEVICE_MODE_EV;

typedef struct s_xl_most150_frequency {
  unsigned int frequency;
} XL_MOST150_FREQUENCY_EV;

typedef struct s_xl_most150_special_node_info{
  unsigned int changeMask;
  unsigned short nodeAddress;
  unsigned short groupAddress;
  unsigned char npr;
  unsigned char mpr;
  unsigned char sbc;
  unsigned char ctrlRetryTime;
  unsigned char ctrlSendAttempts;
  unsigned char asyncRetryTime;
  unsigned char asyncSendAttempts;
  unsigned char macAddr[6];
  unsigned char nprSpy;
  unsigned char mprSpy;
  unsigned char sbcSpy;
  unsigned char inicNIState;
  unsigned char reserved1[3];
  unsigned int reserved2[3];
} XL_MOST150_SPECIAL_NODE_INFO_EV;

typedef struct s_xl_most150_ctrl_rx {
  unsigned short targetAddress;
  unsigned short sourceAddress;
  unsigned char fblockId;
  unsigned char instId;
  unsigned short functionId;
  unsigned char opType;
  unsigned char telId;
  unsigned short telLen;
  unsigned char ctrlData[45];
} XL_MOST150_CTRL_RX_EV;

typedef struct s_xl_most150_ctrl_spy{
  unsigned int frameCount;
  unsigned int msgDuration;
  unsigned char priority;
  unsigned short targetAddress;
  unsigned char pAck;
  unsigned short ctrlDataLenAnnounced;
  unsigned char reserved0;
  unsigned char pIndex;
  unsigned short sourceAddress;
  unsigned short reserved1;
  unsigned short crc;
  unsigned short crcCalculated;
  unsigned char cAck;
  unsigned short ctrlDataLen;
  unsigned char reserved2;
  unsigned int status;
  unsigned int validMask;
  unsigned char ctrlData[51];
} XL_MOST150_CTRL_SPY_EV;

typedef struct s_xl_most150_async_rx_msg {
  unsigned short length;
  unsigned short targetAddress;
  unsigned short sourceAddress;
  unsigned char asyncData[1524];
} XL_MOST150_ASYNC_RX_EV;

typedef struct s_xl_most150_async_spy_msg {
  unsigned int frameCount;
  unsigned int pktDuration;
  unsigned short asyncDataLenAnnounced;
  unsigned short targetAddress;
  unsigned char pAck;
  unsigned char pIndex;
  unsigned short sourceAddress;
  unsigned int crc;
  unsigned int crcCalculated;
  unsigned char cAck;
  unsigned short asyncDataLen;
  unsigned char reserved;
  unsigned int status;
  unsigned int validMask;
  unsigned char asyncData[1524];
} XL_MOST150_ASYNC_SPY_EV;

typedef struct s_xl_most150_ethernet_rx {
  unsigned char sourceAddress[6];
  unsigned char targetAddress[6];
  unsigned int length;
  unsigned char ethernetData[1510];
} XL_MOST150_ETHERNET_RX_EV;

typedef struct s_xl_most150_ethernet_spy {
  unsigned int frameCount;
  unsigned int pktDuration;
  unsigned short ethernetDataLenAnnounced;
  unsigned char targetAddress[6];
  unsigned char pAck;
  unsigned char sourceAddress[6];
  unsigned char reserved0;
  unsigned int crc;
  unsigned int crcCalculated;
  unsigned char cAck;
  unsigned short ethernetDataLen;
  unsigned char reserved1;
  unsigned int status;
  unsigned int validMask;
  unsigned char ethernetData[1506];
} XL_MOST150_ETHERNET_SPY_EV;

typedef struct s_xl_most150_cl_info {
  unsigned short label;
  unsigned short channelWidth;
} XL_MOST150_CL_INFO;

typedef struct s_xl_most150_sync_alloc_info {
  XL_MOST150_CL_INFO allocTable[(unsigned int) 372];
} XL_MOST150_SYNC_ALLOC_INFO_EV;


typedef struct s_xl_most150_sync_volume_status {
 unsigned int device;
 unsigned int volume;
} XL_MOST150_SYNC_VOLUME_STATUS_EV;

typedef struct s_xl_most150_tx_light {
  unsigned int light;
} XL_MOST150_TX_LIGHT_EV;

typedef struct s_xl_most150_rx_light_lock_status {
  unsigned int status;
} XL_MOST150_RXLIGHT_LOCKSTATUS_EV;

typedef struct s_xl_most150_error {
  unsigned int errorCode;
  unsigned int parameter[3];
} XL_MOST150_ERROR_EV;

typedef struct s_xl_most150_configure_rx_buffer {
  unsigned int bufferType;
  unsigned int bufferMode;
} XL_MOST150_CONFIGURE_RX_BUFFER_EV;

typedef struct s_xl_most150_ctrl_sync_audio {
  unsigned int label;
  unsigned int width;
  unsigned int device;
  unsigned int mode;
} XL_MOST150_CTRL_SYNC_AUDIO_EV;

typedef struct s_xl_most150_sync_mute_status {
  unsigned int device;
  unsigned int mute;
} XL_MOST150_SYNC_MUTE_STATUS_EV;

typedef struct s_xl_most150_tx_light_power {
  unsigned int lightPower;
} XL_MOST150_LIGHT_POWER_EV;

typedef struct s_xl_most150_gen_light_error {
  unsigned int stressStarted;
} XL_MOST150_GEN_LIGHT_ERROR_EV;

typedef struct s_xl_most150_gen_lock_error {
  unsigned int stressStarted;
} XL_MOST150_GEN_LOCK_ERROR_EV;

typedef struct s_xl_most150_ctrl_busload {
  unsigned long busloadStarted;
} XL_MOST150_CTRL_BUSLOAD_EV;

typedef struct s_xl_most150_async_busload {
  unsigned long busloadStarted;
} XL_MOST150_ASYNC_BUSLOAD_EV;

typedef struct s_xl_most150_systemlock_flag {
  unsigned int state;
} XL_MOST150_SYSTEMLOCK_FLAG_EV;

typedef struct s_xl_most150_shutdown_flag {
  unsigned int state;
} XL_MOST150_SHUTDOWN_FLAG_EV;

typedef struct s_xl_most150_spdif_mode {
  unsigned int spdifMode;
  unsigned int spdifError;
} XL_MOST150_SPDIF_MODE_EV;

typedef struct s_xl_most150_ecl {
  unsigned int eclLineState;
} XL_MOST150_ECL_EV;

typedef struct s_xl_most150_ecl_termination {
  unsigned int resistorEnabled;
} XL_MOST150_ECL_TERMINATION_EV;

typedef struct s_xl_most150_nw_startup {
  unsigned int error;
  unsigned int errorInfo;
} XL_MOST150_NW_STARTUP_EV;

typedef struct s_xl_most150_nw_shutdown {
  unsigned int error;
  unsigned int errorInfo;
} XL_MOST150_NW_SHUTDOWN_EV;

typedef struct s_xl_most150_stream_state {
  unsigned int streamHandle;
  unsigned int streamState;
  unsigned int streamError;
} XL_MOST150_STREAM_STATE_EV;

typedef struct s_xl_most150_stream_tx_buffer {
  unsigned int streamHandle;
  unsigned int numberOfBytes;
  unsigned int status;
} XL_MOST150_STREAM_TX_BUFFER_EV;

typedef struct s_xl_most150_stream_rx_buffer {
  unsigned int streamHandle;
  unsigned int numberOfBytes;
  unsigned int status;
  unsigned int labelInfo;
} XL_MOST150_STREAM_RX_BUFFER_EV;

typedef struct s_xl_most150_stream_tx_underflow {
  unsigned int streamHandle;
  unsigned int reserved;
} XL_MOST150_STREAM_TX_UNDERFLOW_EV;

typedef struct s_xl_most150_stream_tx_label {
  unsigned int streamHandle;
  unsigned int errorInfo;
  unsigned int connLabel;
  unsigned int width;
} XL_MOST150_STREAM_TX_LABEL_EV;

typedef struct s_xl_most150_gen_bypass_stress {
  unsigned int stressStarted;
} XL_MOST150_GEN_BYPASS_STRESS_EV;

typedef struct s_xl_most150_ecl_sequence {
  unsigned int sequenceStarted;
} XL_MOST150_ECL_SEQUENCE_EV;

typedef struct s_xl_most150_ecl_glitch_filter {
  unsigned int duration;
} XL_MOST150_ECL_GLITCH_FILTER_EV;

typedef struct s_xl_most150_sso_result {
  unsigned int status;
} XL_MOST150_SSO_RESULT_EV;


typedef struct s_xl_most150_ctrl_tx_ack {
  unsigned short targetAddress;
  unsigned short sourceAddress;
  unsigned char ctrlPrio;
  unsigned char ctrlSendAttempts;
  unsigned char reserved[2];
  unsigned int status;
//# 3269 "vxlapi.h"
  unsigned char ctrlData[51];
} XL_MOST150_CTRL_TX_ACK_EV;

typedef struct s_xl_most150_async_tx_ack {
  unsigned char priority;
  unsigned char asyncSendAttempts;
  unsigned short length;
  unsigned short targetAddress;
  unsigned short sourceAddress;
  unsigned int status;
  unsigned char asyncData[1524];
} XL_MOST150_ASYNC_TX_ACK_EV;

typedef struct s_xl_most150_ethernet_tx {
  unsigned char priority;
  unsigned char ethSendAttempts;
  unsigned char sourceAddress[6];
  unsigned char targetAddress[6];
  unsigned char reserved[2];
  unsigned int length;
  unsigned char ethernetData[1510];
} XL_MOST150_ETHERNET_TX_ACK_EV;

typedef struct s_xl_most150_hw_sync {
  unsigned int pulseCode;
} XL_MOST150_HW_SYNC_EV;

typedef struct s_xl_event_most150 {

  unsigned int size;
  XLmostEventTag tag;
  unsigned short channelIndex;
  unsigned int userHandle;
  unsigned short flagsChip;
  unsigned short reserved;
  XLuint64 timeStamp;
  XLuint64 timeStampSync;


  union {
    unsigned char rawData[(unsigned int) 2048];
    XL_MOST150_EVENT_SOURCE_EV mostEventSource;
    XL_MOST150_DEVICE_MODE_EV mostDeviceMode;
    XL_MOST150_FREQUENCY_EV mostFrequency;
    XL_MOST150_SPECIAL_NODE_INFO_EV mostSpecialNodeInfo;
    XL_MOST150_CTRL_RX_EV mostCtrlRx;
    XL_MOST150_CTRL_TX_ACK_EV mostCtrlTxAck;
    XL_MOST150_ASYNC_SPY_EV mostAsyncSpy;
    XL_MOST150_ASYNC_RX_EV mostAsyncRx;
    XL_MOST150_SYNC_ALLOC_INFO_EV mostSyncAllocInfo;
    XL_MOST150_SYNC_VOLUME_STATUS_EV mostSyncVolumeStatus;
    XL_MOST150_TX_LIGHT_EV mostTxLight;
    XL_MOST150_RXLIGHT_LOCKSTATUS_EV mostRxLightLockStatus;
    XL_MOST150_ERROR_EV mostError;
    XL_MOST150_CONFIGURE_RX_BUFFER_EV mostConfigureRxBuffer;
    XL_MOST150_CTRL_SYNC_AUDIO_EV mostCtrlSyncAudio;
    XL_MOST150_SYNC_MUTE_STATUS_EV mostSyncMuteStatus;
    XL_MOST150_LIGHT_POWER_EV mostLightPower;
    XL_MOST150_GEN_LIGHT_ERROR_EV mostGenLightError;
    XL_MOST150_GEN_LOCK_ERROR_EV mostGenLockError;
    XL_MOST150_CTRL_BUSLOAD_EV mostCtrlBusload;
    XL_MOST150_ASYNC_BUSLOAD_EV mostAsyncBusload;
    XL_MOST150_ETHERNET_RX_EV mostEthernetRx;
    XL_MOST150_SYSTEMLOCK_FLAG_EV mostSystemLockFlag;
    XL_MOST150_SHUTDOWN_FLAG_EV mostShutdownFlag;
    XL_MOST150_SPDIF_MODE_EV mostSpdifMode;
    XL_MOST150_ECL_EV mostEclEvent;
    XL_MOST150_ECL_TERMINATION_EV mostEclTermination;
    XL_MOST150_CTRL_SPY_EV mostCtrlSpy;
    XL_MOST150_ASYNC_TX_ACK_EV mostAsyncTxAck;
    XL_MOST150_ETHERNET_SPY_EV mostEthernetSpy;
    XL_MOST150_ETHERNET_TX_ACK_EV mostEthernetTxAck;
    XL_MOST150_HW_SYNC_EV mostHWSync;
    XL_MOST150_NW_STARTUP_EV mostStartup;
    XL_MOST150_NW_SHUTDOWN_EV mostShutdown;
    XL_MOST150_STREAM_STATE_EV mostStreamState;
    XL_MOST150_STREAM_TX_BUFFER_EV mostStreamTxBuffer;
    XL_MOST150_STREAM_RX_BUFFER_EV mostStreamRxBuffer;
    XL_MOST150_STREAM_TX_UNDERFLOW_EV mostStreamTxUnderflow;
    XL_MOST150_STREAM_TX_LABEL_EV mostStreamTxLabel;
    XL_MOST150_GEN_BYPASS_STRESS_EV mostGenBypassStress;
    XL_MOST150_ECL_SEQUENCE_EV mostEclSequence;
    XL_MOST150_ECL_GLITCH_FILTER_EV mostEclGlitchFilter;
    XL_MOST150_SSO_RESULT_EV mostSsoResult;
  } tagData;
} XLmost150event;







typedef struct s_xl_set_most150_special_node_info {
  unsigned int changeMask;
  unsigned int nodeAddress;
  unsigned int groupAddress;
  unsigned int sbc;
  unsigned int ctrlRetryTime;
  unsigned int ctrlSendAttempts;
  unsigned int asyncRetryTime;
  unsigned int asyncSendAttempts;
  unsigned char macAddr[6];
} XLmost150SetSpecialNodeInfo;


typedef struct s_xl_most150_ctrl_tx_msg {
  unsigned int ctrlPrio;
  unsigned int ctrlSendAttempts;
  unsigned int targetAddress;
//# 3393 "vxlapi.h"
  unsigned char ctrlData[51];
} XLmost150CtrlTxMsg;


typedef struct s_xl_most150_async_tx_msg {
  unsigned int priority;
  unsigned int asyncSendAttempts;
  unsigned int length;
  unsigned int targetAddress;
  unsigned char asyncData[(unsigned short) 1600];
} XLmost150AsyncTxMsg;


typedef struct s_xl_most150_ethernet_tx_msg {
  unsigned int priority;
  unsigned int ethSendAttempts;
  unsigned char sourceAddress[6];
  unsigned char targetAddress[6];
  unsigned int length;
  unsigned char ethernetData[(unsigned short) 1600];
} XLmost150EthernetTxMsg;


typedef struct s_xl_most150_sync_audio_parameter {
  unsigned int label;
  unsigned int width;
  unsigned int device;
  unsigned int mode;
} XLmost150SyncAudioParameter;


typedef struct s_xl_most150_ctrl_busload_config {
  unsigned int transmissionRate;
  unsigned int counterType;
  unsigned int counterPosition;
  XLmost150CtrlTxMsg busloadCtrlMsg;
} XLmost150CtrlBusloadConfig;


typedef struct s_xl_most150_async_busload_config {
  unsigned int busloadType;
  unsigned int transmissionRate;
  unsigned int counterType;
  unsigned int counterPosition;
  union {
    unsigned char rawBusloadPkt[1540];
    XLmost150AsyncTxMsg busloadAsyncPkt;
    XLmost150EthernetTxMsg busloadEthernetPkt;
  } busloadPkt;
} XLmost150AsyncBusloadConfig;


typedef struct s_xl_most150_stream_open {
  unsigned int* pStreamHandle;
  unsigned int direction;
  unsigned int numBytesPerFrame;
  unsigned int reserved;
  unsigned int latency;
} XLmost150StreamOpen;


typedef struct s_xl_most150_stream_get_info {
  unsigned int streamHandle;
  unsigned int numBytesPerFrame;
  unsigned int direction;
  unsigned int reserved;
  unsigned int latency;
  unsigned int streamState;
  unsigned int connLabels[8];
} XLmost150StreamInfo;


//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 3466 "vxlapi.h" 2

//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/pshpack8.h" 1 3

//#pragma pack(push,8)
//# 3468 "vxlapi.h" 2
//# 3514 "vxlapi.h"
typedef struct {
  unsigned int canId;
  unsigned int msgFlags;
  unsigned char dlc;
  unsigned char reserved[7];
  unsigned char data[64];
} XL_CAN_TX_MSG;

typedef struct {
  unsigned short tag;
  unsigned short transId;
  unsigned char channelIndex;
  unsigned char reserved[3];

  union {
    XL_CAN_TX_MSG canMsg;
  } tagData;
} XLcanTxEvent;
//# 3540 "vxlapi.h"
typedef struct {
  unsigned int canId;
  unsigned int msgFlags;
  unsigned int crc;
  unsigned char reserved1[12];
  unsigned short totalBitCnt;
  unsigned char dlc;
  unsigned char reserved[5];
  unsigned char data[64];
} XL_CAN_EV_RX_MSG;

typedef struct {
  unsigned int canId;
  unsigned int msgFlags;
  unsigned char dlc;
  unsigned char reserved1;
  unsigned short reserved;
  unsigned char data[64];
} XL_CAN_EV_TX_REQUEST;



typedef struct {
  unsigned char busStatus;
  unsigned char txErrorCounter;
  unsigned char rxErrorCounter;
  unsigned char reserved;
  unsigned int reserved0;
} XL_CAN_EV_CHIP_STATE;


typedef XL_SYNC_PULSE_EV XL_CAN_EV_SYNC_PULSE;






typedef struct {
  unsigned char errorCode;
  unsigned char reserved[95];
} XL_CAN_EV_ERROR;
//# 3590 "vxlapi.h"
typedef struct {
  unsigned int size;
  unsigned short tag;
  unsigned char channelIndex;
  unsigned char reserved;
  unsigned int userHandle;
  unsigned short flagsChip;
  unsigned short reserved0;
  XLuint64 reserved1;
  XLuint64 timeStampSync;

  union {
    XL_CAN_EV_RX_MSG canRxOkMsg;
    XL_CAN_EV_RX_MSG canTxOkMsg;
    XL_CAN_EV_TX_REQUEST canTxRequest;
    XL_CAN_EV_ERROR canError;
    XL_CAN_EV_CHIP_STATE canChipState;
    XL_CAN_EV_SYNC_PULSE canSyncPulse;
  } tagData;
} XLcanRxEvent;
//# 1 "d:\\mingw\\bin\\../lib/gcc/mingw32/4.6.2/../../../../include/poppack.h" 1 3

//#pragma pack(pop)
//# 3611 "vxlapi.h" 2
//# 3626 "vxlapi.h"
  XLstatus   xlOpenDriver(void);
//# 3640 "vxlapi.h"
  XLstatus   xlCloseDriver(void);
//# 3660 "vxlapi.h"
XLstatus   xlGetApplConfig ( char *appName, unsigned int appChannel, unsigned int *pHwType, unsigned int *pHwIndex, unsigned int *pHwChannel, unsigned int busType )






                  ;

XLstatus   xlSetApplConfig ( char *appName, unsigned int appChannel, unsigned int hwType, unsigned int hwIndex, unsigned int hwChannel, unsigned int busType )






      ;
//# 3686 "vxlapi.h"
XLstatus   xlGetDriverConfig (XLdriverConfig *pDriverConfig);
//# 3711 "vxlapi.h"
  int   xlGetChannelIndex(
    int hwType,
    int hwIndex,
    int hwChannel
  );

  XLaccess   xlGetChannelMask(
    int hwType,
    int hwIndex,
    int hwChannel
  );
//# 3736 "vxlapi.h"
XLstatus   xlOpenPort ( XLportHandle *pPortHandle, char *userName, XLaccess accessMask, XLaccess *pPermissionMask, unsigned int rxQueueSize, unsigned int xlInterfaceVersion, unsigned int busType)







                 ;
//# 3756 "vxlapi.h"
XLstatus   xlSetTimerRate ( XLportHandle portHandle, unsigned long timerRate)


                   ;
//# 3782 "vxlapi.h"
XLstatus   xlSetTimerRateAndChannel ( XLportHandle portHandle, XLaccess *timerChannelMask, unsigned long *timerRate)



                   ;
//# 3795 "vxlapi.h"
XLstatus   xlResetClock (XLportHandle portHandle);
//# 3809 "vxlapi.h"
XLstatus   xlSetNotification ( XLportHandle portHandle, XLhandle *pHandle, int queueLevel)



                 ;
//# 3823 "vxlapi.h"
XLstatus   xlSetTimerBasedNotify ( XLportHandle portHandle, XLhandle *pHandle)


                   ;







XLstatus   xlFlushReceiveQueue (XLportHandle portHandle);







XLstatus   xlGetReceiveQueueLevel ( XLportHandle portHandle, int *level)


                 ;
//# 3854 "vxlapi.h"
XLstatus   xlActivateChannel ( XLportHandle portHandle, XLaccess accessMask, unsigned int busType, unsigned int flags)




                   ;
//# 3878 "vxlapi.h"
XLstatus   xlReceive ( XLportHandle portHandle, unsigned int *pEventCount, XLevent *pEvents)



                   ;
//# 3896 "vxlapi.h"
  XLstringType   xlGetErrorString( XLstatus err );
  XLstringType   xlGetEventString( XLevent *pEv );
  XLstringType   xlCanGetEventString( XLcanRxEvent *pEv );







XLstatus   xlOemContact (XLportHandle portHandle, unsigned long Channel, XLuint64 context1, XLuint64 *context2);
//# 3915 "vxlapi.h"
XLstatus   xlGetSyncTime ( XLportHandle portHandle, XLuint64 *pTime )


     ;







XLstatus   xlGetChannelTime ( XLportHandle portHandle, XLaccess accessMask, XLuint64 *pChannelTime )



     ;
//# 3940 "vxlapi.h"
XLstatus   xlGenerateSyncPulse ( XLportHandle portHandle, XLaccess accessMask)


                  ;






XLstatus   xlPopupHwConfig ( char *callSign, unsigned int waitForFinish)


                   ;
//# 3963 "vxlapi.h"
XLstatus   xlDeactivateChannel ( XLportHandle portHandle, XLaccess accessMask)


                  ;







XLstatus   xlClosePort ( XLportHandle portHandle)

                  ;
//# 3988 "vxlapi.h"
XLstatus   xlCanFlushTransmitQueue ( XLportHandle portHandle, XLaccess accessMask)

                   ;
//# 4000 "vxlapi.h"
XLstatus   xlCanSetChannelOutput ( XLportHandle portHandle, XLaccess accessMask, int mode)



                   ;
//# 4016 "vxlapi.h"
XLstatus   xlCanSetChannelMode ( XLportHandle portHandle, XLaccess accessMask, int tx, int txrq)




                  ;






XLstatus   xlCanSetReceiveMode ( XLportHandle Port, unsigned char ErrorFrame, unsigned char ChipState )




                  ;
//# 4047 "vxlapi.h"
XLstatus   xlCanSetChannelTransceiver ( XLportHandle portHandle, XLaccess accessMask, int type, int lineMode, int resNet)





                   ;
//# 4065 "vxlapi.h"
XLstatus   xlCanSetChannelParams ( XLportHandle portHandle, XLaccess accessMask, XLchipParams* pChipParams)



                   ;

XLstatus   xlCanSetChannelParamsC200 ( XLportHandle portHandle, XLaccess accessMask, unsigned char btr0, unsigned char btr1)




                   ;

XLstatus   xlCanSetChannelBitrate ( XLportHandle portHandle, XLaccess accessMask, unsigned long bitrate)



                  ;





XLstatus   xlCanFdSetConfiguration ( XLportHandle portHandle, XLaccess accessMask, XLcanFdConf* pCanFdConf)



                  ;





XLstatus   xlCanReceive ( XLportHandle portHandle, XLcanRxEvent* pXlCanRxEvt)


                  ;





XLstatus   xlCanTransmitEx ( XLportHandle portHandle, XLaccess accessMask, unsigned int msgCnt, unsigned int* pMsgCntSent, XLcanTxEvent* pXlCanTxEvt)





                  ;
//# 4124 "vxlapi.h"
XLstatus   xlCanSetChannelAcceptance ( XLportHandle portHandle, XLaccess accessMask, unsigned long code, unsigned long mask, unsigned int idRange)





                  ;
//# 4139 "vxlapi.h"
XLstatus   xlCanAddAcceptanceRange ( XLportHandle portHandle, XLaccess accessMask, unsigned long first_id, unsigned long last_id)




                   ;
XLstatus   xlCanRemoveAcceptanceRange ( XLportHandle portHandle, XLaccess accessMask, unsigned long first_id, unsigned long last_id)




                   ;
XLstatus   xlCanResetAcceptance ( XLportHandle portHandle, XLaccess accessMask, unsigned int idRange);



                   ;
//# 4164 "vxlapi.h"
XLstatus   xlCanRequestChipState ( XLportHandle portHandle, XLaccess accessMask)


                 ;
//# 4181 "vxlapi.h"
XLstatus   xlCanTransmit ( XLportHandle portHandle, XLaccess accessMask, unsigned int *pEventCount, void *pEvents)




                   ;






XLstatus   xlSetGlobalTimeSync (unsigned long newValue, unsigned long *previousValue)
                                                                                           ;
//# 4205 "vxlapi.h"
XLstatus   xlCheckLicense ( XLportHandle portHandle, XLaccess accessMask, unsigned long protectionCode)



                   ;
//# 4226 "vxlapi.h"
XLstatus   xlGetLicenseInfo ( XLaccess channelMask, XLlicenseInfo *pLicInfoArray, unsigned int licInfoArraySize)



                   ;






XLstatus   xlLinSetChannelParams (XLportHandle portHandle, XLaccess accessMask, XLlinStatPar vStatPar);
XLstatus   xlLinSetDLC (XLportHandle portHandle, XLaccess accessMask, unsigned char dlc[64]);
XLstatus   xlLinSetSlave (XLportHandle portHandle, XLaccess accessMask, unsigned char linId, unsigned char data[8], unsigned char dlc, unsigned short checksum);
XLstatus   xlLinSendRequest (XLportHandle portHandle, XLaccess accessMask, unsigned char linId, unsigned int flags);
XLstatus   xlLinSetSleepMode (XLportHandle portHandle, XLaccess accessMask, unsigned int flags, unsigned char linId);
XLstatus   xlLinWakeUp (XLportHandle portHandle, XLaccess accessMask);
XLstatus   xlLinSetChecksum (XLportHandle portHandle, XLaccess accessMask, unsigned char checksum[60]);
XLstatus   xlLinSwitchSlave (XLportHandle portHandle, XLaccess accessMask, unsigned char linID, unsigned char mode);





XLstatus   xlDAIOSetPWMOutput (XLportHandle portHandle, XLaccess accessMask, unsigned int frequency, unsigned int value);
XLstatus   xlDAIOSetDigitalOutput (XLportHandle portHandle, XLaccess accessMask, unsigned int outputMask, unsigned int valuePattern);
XLstatus   xlDAIOSetAnalogOutput (XLportHandle portHandle, XLaccess accessMask, unsigned int analogLine1, unsigned int analogLine2, unsigned int analogLine3, unsigned int analogLine4);
XLstatus   xlDAIORequestMeasurement (XLportHandle portHandle, XLaccess accessMask);
XLstatus   xlDAIOSetDigitalParameters (XLportHandle portHandle, XLaccess accessMask, unsigned int inputMask, unsigned int outputMask);
XLstatus   xlDAIOSetAnalogParameters (XLportHandle portHandle, XLaccess accessMask, unsigned int inputMask, unsigned int outputMask, unsigned int highRangeMask);
XLstatus   xlDAIOSetAnalogTrigger (XLportHandle portHandle, XLaccess accessMask, unsigned int triggerMask, unsigned int triggerLevel, unsigned int triggerEventMode);
XLstatus   xlDAIOSetMeasurementFrequency (XLportHandle portHandle, XLaccess accessMask, unsigned int measurementInterval);
XLstatus   xlDAIOSetDigitalTrigger (XLportHandle portHandle, XLaccess accessMask, unsigned int triggerMask);
//# 4313 "vxlapi.h"
XLstatus   xlMostReceive (XLportHandle portHandle, XLmostEvent* pEventBuffer);
//# 4323 "vxlapi.h"
XLstatus   xlMostSwitchEventSources (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned short sourceMask);







XLstatus   xlMostSetAllBypass (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned char bypassMode);






XLstatus   xlMostGetAllBypass (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);







XLstatus   xlMostSetTimingMode (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned char timingMode);







XLstatus   xlMostGetTimingMode (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);
//# 4363 "vxlapi.h"
XLstatus   xlMostSetFrequency (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned short frequency);
//# 4372 "vxlapi.h"
XLstatus   xlMostGetFrequency (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);
//# 4383 "vxlapi.h"
XLstatus   xlMostWriteRegister (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned short adr, unsigned char numBytes, unsigned char data[16]);
//# 4393 "vxlapi.h"
XLstatus   xlMostReadRegister (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned short adr, unsigned char numBytes);
//# 4404 "vxlapi.h"
XLstatus   xlMostWriteRegisterBit (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned short adr, unsigned char mask, unsigned char value);
//# 4416 "vxlapi.h"
XLstatus   xlMostCtrlTransmit (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmostCtrlMsg* pCtrlMsg);
//# 4429 "vxlapi.h"
XLstatus   xlMostAsyncTransmit (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmostAsyncMsg* pAsyncMsg);







XLstatus   xlMostSyncGetAllocTable (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);
//# 4449 "vxlapi.h"
XLstatus   xlMostCtrlSyncAudio (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int channel[4], unsigned int device, unsigned int mode);
//# 4461 "vxlapi.h"
XLstatus   xlMostCtrlSyncAudioEx (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int channel[16], unsigned int device, unsigned int mode);
//# 4470 "vxlapi.h"
XLstatus   xlMostSyncVolume (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int device, unsigned char volume);
//# 4479 "vxlapi.h"
XLstatus   xlMostSyncMute (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int device, unsigned char mute);
//# 4488 "vxlapi.h"
XLstatus   xlMostSyncGetVolumeStatus (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int device);
//# 4497 "vxlapi.h"
XLstatus   xlMostSyncGetMuteStatus (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int device);







XLstatus   xlMostGetRxLight (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);







XLstatus   xlMostSetTxLight (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned char txLight);







XLstatus   xlMostGetTxLight (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);







XLstatus   xlMostSetLightPower (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned char attenuation);
//# 4539 "vxlapi.h"
XLstatus   xlMostGetLockStatus (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);
//# 4550 "vxlapi.h"
XLstatus   xlMostGenerateLightError (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned long lightOffTime, unsigned long lightOnTime, unsigned short repeat);
//# 4561 "vxlapi.h"
XLstatus   xlMostGenerateLockError (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned long unmodTime, unsigned long modTime, unsigned short repeat);
//# 4571 "vxlapi.h"
XLstatus   xlMostCtrlRxBuffer (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned short bufferMode);





XLstatus   xlMostTwinklePowerLed (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);
//# 4590 "vxlapi.h"
XLstatus   xlMostCtrlConfigureBusload (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmostCtrlBusloadConfiguration* pCtrlBusloadConfiguration)
                                                                                                                                        ;
//# 4603 "vxlapi.h"
XLstatus   xlMostCtrlGenerateBusload (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned long numberCtrlFrames);
//# 4616 "vxlapi.h"
XLstatus   xlMostAsyncConfigureBusload (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmostAsyncBusloadConfiguration* pAsyncBusloadConfiguration)
                                                                                                                                            ;
//# 4629 "vxlapi.h"
XLstatus   xlMostAsyncGenerateBusload (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned long numberAsyncFrames);
//# 4643 "vxlapi.h"
XLstatus   xlMostStreamOpen (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmostStreamOpen* pStreamOpen);
//# 4655 "vxlapi.h"
XLstatus   xlMostStreamClose (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int streamHandle);
//# 4669 "vxlapi.h"
XLstatus   xlMostStreamStart (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int streamHandle, unsigned char syncChannels[60]);
//# 4682 "vxlapi.h"
XLstatus   xlMostStreamStop (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int streamHandle);
//# 4699 "vxlapi.h"
XLstatus   xlMostStreamBufferAllocate (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int streamHandle, unsigned char** ppBuffer, unsigned int* pBufferSize);
//# 4712 "vxlapi.h"
XLstatus   xlMostStreamBufferDeallocateAll (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int streamHandle);
//# 4726 "vxlapi.h"
XLstatus   xlMostStreamBufferSetNext (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int streamHandle, unsigned char* pBuffer, unsigned int filledBytes);
//# 4739 "vxlapi.h"
XLstatus   xlMostStreamGetInfo (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmostStreamInfo* pStreamInfo);
//# 4752 "vxlapi.h"
XLstatus   xlMostStreamBufferClearAll (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int streamHandle);
//# 4767 "vxlapi.h"
XLstatus   xlFrSetConfiguration (XLportHandle portHandle, XLaccess accessMask, XLfrClusterConfig *pxlClusterConfig);
//# 4778 "vxlapi.h"
XLstatus   xlFrGetChannelConfiguration (XLportHandle portHandle, XLaccess accessMask, XLfrChannelConfig* pxlFrChannelConfig);
//# 4788 "vxlapi.h"
XLstatus   xlFrSetMode (XLportHandle portHandle, XLaccess accessMask, XLfrMode *pxlFrMode);
//# 4798 "vxlapi.h"
XLstatus   xlFrInitStartupAndSync (XLportHandle portHandle, XLaccess accessMask, XLfrEvent *pEventBuffer);
//# 4809 "vxlapi.h"
XLstatus   xlFrSetupSymbolWindow (XLportHandle portHandle, XLaccess accessMask, unsigned int frChannel, unsigned int symbolWindowMask)
                                                                                                              ;
//# 4819 "vxlapi.h"
XLstatus   xlFrReceive (XLportHandle portHandle, XLfrEvent *pEventBuffer);
//# 4829 "vxlapi.h"
XLstatus   xlFrTransmit (XLportHandle portHandle, XLaccess accessMask, XLfrEvent *pEventBuffer);
//# 4840 "vxlapi.h"
XLstatus   xlFrSetTransceiverMode (XLportHandle portHandle, XLaccess accessMask, unsigned int frChannel, unsigned int mode);
//# 4850 "vxlapi.h"
XLstatus   xlFrSendSymbolWindow (XLportHandle portHandle, XLaccess accessMask, unsigned int symbolWindow);
//# 4860 "vxlapi.h"
XLstatus   xlFrActivateSpy (XLportHandle portHandle, XLaccess accessMask, unsigned int mode);
//# 4869 "vxlapi.h"
XLstatus   xlFrSetAcceptanceFilter (XLportHandle portHandle, XLaccess accessMask, XLfrAcceptanceFilter *pAcceptanceFilter)
                                                                                                                ;






XLstatus   xlGetRemoteDriverConfig (XLdriverConfig *pDriverConfig);
//# 4888 "vxlapi.h"
XLstatus   xlGetRemoteDeviceInfo (XLremoteDeviceInfo **deviceList, unsigned int *nbrOfRemoteDevices, unsigned int netSearch);





XLstatus   xlReleaseRemoteDeviceInfo (XLremoteDeviceInfo **deviceList);
//# 4904 "vxlapi.h"
XLstatus   xlAddRemoteDevice (XLremoteHandle remoteHandle, XLdeviceAccess deviceMask, unsigned int flags);
//# 4913 "vxlapi.h"
XLstatus   xlRemoveRemoteDevice (XLremoteHandle remoteHandle, XLdeviceAccess deviceMask, unsigned int flags);
//# 4922 "vxlapi.h"
XLstatus   xlUpdateRemoteDeviceInfo (XLremoteDeviceInfo *deviceList, unsigned int nbrOfRemoteDevices);
//# 4932 "vxlapi.h"
XLstatus   xlGetRemoteHwInfo (XLremoteHandle remoteHandle, int *hwType, int *hwIndex, int *isPresent);
//# 4942 "vxlapi.h"
XLstatus   xlRegisterRemoteDevice (int hwType, XLipAddress *ipAddress, unsigned int flags);
//# 4955 "vxlapi.h"
XLstatus   xlIoSetTriggerMode (XLportHandle portHandle, XLaccess accessMask, XLdaioTriggerMode* pxlDaioTriggerMode);






XLstatus   xlIoSetDigitalOutput (XLportHandle portHandle, XLaccess accessMask, XLdaioDigitalParams *pxlDaioDigitalParams);






XLstatus   xlIoConfigurePorts (XLportHandle portHandle, XLaccess accessMask, XLdaioSetPort *pxlDaioSetPort);






XLstatus   xlIoSetDigInThreshold (XLportHandle portHandle, XLaccess accessMask, unsigned int level);






XLstatus   xlIoSetDigOutLevel (XLportHandle portHandle, XLaccess accessMask, unsigned int level);






XLstatus   xlIoSetAnalogOutput (XLportHandle portHandle, XLaccess accessMask, XLdaioAnalogParams *pxlDaioAnalogParams);






XLstatus   xlIoStartSampling (XLportHandle portHandle, XLaccess accessMask, unsigned int portTypeMask);
//# 5031 "vxlapi.h"
XLstatus   xlMost150Receive (XLportHandle portHandle, XLmost150event* pEventBuffer);





XLstatus   xlMost150TwinklePowerLed (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);
//# 5046 "vxlapi.h"
XLstatus   xlMost150SwitchEventSources (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int sourceMask);
//# 5055 "vxlapi.h"
XLstatus   xlMost150SetDeviceMode (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int deviceMode);






XLstatus   xlMost150GetDeviceMode (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);







XLstatus   xlMost150SetSPDIFMode (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int spdifMode);







XLstatus   xlMost150GetSPDIFMode (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);







XLstatus   xlMost150SetSpecialNodeInfo (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmost150SetSpecialNodeInfo *pSpecialNodeInfo);






XLstatus   xlMost150GetSpecialNodeInfo (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int requestMask);






XLstatus   xlMost150SetFrequency (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int frequency);





XLstatus   xlMost150GetFrequency (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);






XLstatus   xlMost150CtrlTransmit (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmost150CtrlTxMsg *pCtrlTxMsg);






XLstatus   xlMost150AsyncTransmit (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmost150AsyncTxMsg *pAsyncTxMsg);






XLstatus   xlMost150EthernetTransmit (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmost150EthernetTxMsg *pEthernetTxMsg);





XLstatus   xlMost150GetSystemLockFlag (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);





XLstatus   xlMost150GetShutdownFlag (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);





XLstatus   xlMost150Shutdown (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);





XLstatus   xlMost150Startup (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);






XLstatus   xlMost150SyncGetAllocTable (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);






XLstatus   xlMost150CtrlSyncAudio (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmost150SyncAudioParameter *pSyncAudioParameter);







XLstatus   xlMost150SyncSetVolume (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int device, unsigned int volume);







XLstatus   xlMost150SyncGetVolume (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int device);







XLstatus   xlMost150SyncSetMute (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int device, unsigned int mute);






XLstatus   xlMost150SyncGetMute (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int device);






XLstatus   xlMost150GetRxLightLockStatus (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int fromSpy);







XLstatus   xlMost150SetTxLight (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int txLight);





XLstatus   xlMost150GetTxLight (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);







XLstatus   xlMost150SetTxLightPower (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int attenuation);
//# 5235 "vxlapi.h"
XLstatus   xlMost150GenerateLightError (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int lightOffTime, unsigned int lightOnTime, unsigned int repeat);
//# 5245 "vxlapi.h"
XLstatus   xlMost150GenerateLockError (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int unlockTime, unsigned int lockTime, unsigned int repeat);
//# 5254 "vxlapi.h"
XLstatus   xlMost150ConfigureRxBuffer (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int bufferType, unsigned int bufferMode);






XLstatus   xlMost150CtrlConfigureBusload (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmost150CtrlBusloadConfig *pCtrlBusLoad);
//# 5271 "vxlapi.h"
XLstatus   xlMost150CtrlGenerateBusload (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned long numberCtrlFrames);






XLstatus   xlMost150AsyncConfigureBusload (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmost150AsyncBusloadConfig *pAsyncBusLoad);
//# 5288 "vxlapi.h"
XLstatus   xlMost150AsyncGenerateBusload (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned long numberAsyncPackets);







XLstatus   xlMost150SetECLLine (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int eclLineState);
//# 5305 "vxlapi.h"
XLstatus   xlMost150SetECLTermination (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int eclLineTermination);






XLstatus   xlMost150GetECLInfo (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);
//# 5324 "vxlapi.h"
XLstatus   xlMost150StreamOpen (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmost150StreamOpen* pStreamOpen);
//# 5336 "vxlapi.h"
XLstatus   xlMost150StreamClose (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int streamHandle);
//# 5351 "vxlapi.h"
XLstatus   xlMost150StreamStart (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int streamHandle, unsigned int numConnLabels, unsigned int* pConnLabels)

                                                                                                 ;
//# 5366 "vxlapi.h"
XLstatus   xlMost150StreamStop (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int streamHandle);
//# 5381 "vxlapi.h"
XLstatus   xlMost150StreamTransmitData (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int streamHandle, unsigned char* pBuffer, unsigned int* pNumberOfBytes);
//# 5393 "vxlapi.h"
XLstatus   xlMost150StreamClearTxFifo (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int streamHandle);
//# 5405 "vxlapi.h"
XLstatus   xlMost150StreamGetInfo (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, XLmost150StreamInfo* pStreamInfo);
//# 5414 "vxlapi.h"
XLstatus   xlMost150StreamInitRxFifo (XLportHandle portHandle, XLaccess accessMask);
//# 5429 "vxlapi.h"
XLstatus   xlMost150StreamReceiveData (XLportHandle portHandle, XLaccess accessMask, unsigned char* pBuffer, unsigned int* pBufferSize)
                                                                                                                                ;
//# 5442 "vxlapi.h"
XLstatus   xlMost150GenerateBypassStress (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int bypassCloseTime, unsigned int bypassOpenTime, unsigned int repeat)
                                                                                                                                     ;
//# 5456 "vxlapi.h"
XLstatus   xlMost150EclConfigureSeq (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int numStates, unsigned int* pEclStates, unsigned int* pEclStatesDuration)

                                                                                                           ;
//# 5468 "vxlapi.h"
XLstatus   xlMost150EclGenerateSeq (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int start);
//# 5478 "vxlapi.h"
XLstatus   xlMost150SetECLGlitchFilter (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int duration);
//# 5490 "vxlapi.h"
XLstatus   xlMost150SetSSOResult (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int ssoCUStatus);
//# 5500 "vxlapi.h"
XLstatus   xlMost150GetSSOResult (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);
//# 5529 "vxlapi.h"
XLstatus   xlEthSetConfig (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, const T_XL_ETH_CONFIG *config);







XLstatus   xlEthGetConfig (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, T_XL_ETH_CONFIG *config);
//# 5551 "vxlapi.h"
XLstatus   xlEthReceive (XLportHandle portHandle, T_XL_ETH_EVENT *ethEventBuffer);







XLstatus   xlEthSetBypass (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, unsigned int mode);





XLstatus   xlEthTwinkleStatusLed (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle);







XLstatus   xlEthTransmit (XLportHandle portHandle, XLaccess accessMask, unsigned short userHandle, const T_XL_ETH_DATAFRAME_TX *data);
