
#include "PduR.h"
#include "MemMap.h"

const PduR_RamBufCfgType PduR_RamBufCfg =
{
	.NTpBuffers=0,
	.NTpRouteBuffers=0,
	.NTxBuffers=0
};

 
SECTION_POSTBUILD_DATA const PduR_PBConfigType PduR_Config = {
	.PduRConfigurationId = 0,
	.RoutingPaths = NULL,
	.NRoutingPaths = 0,
	.DefaultValues = NULL,
	.DefaultValueLengths = NULL,
};


