import sys,os
if(os.path.exists('../as.tool/config.infrastructure.system/third_party')):
    sys.path.append(os.path.abspath('../as.tool/config.infrastructure.system/third_party'))
elif(os.path.exists('./config.infrastructure.system/third_party')):
    sys.path.append(os.path.abspath('./config.infrastructure.system/third_party'))
else:
    raise Exception("can't locate config.infrastructure.system")

import autosar
def main():
    ws=autosar.workspace()
    
    dataTypes=ws.createPackage('DataType')
    dataTypes.createSubPackage('DataTypeSemantics', role='CompuMethod')
    dataTypes.createSubPackage('DataTypeUnits', role='Unit')
    
    dataTypes.createIntegerDataType('InactiveActive_T', valueTable=[
         'InactiveActive_Inactive',
         'InactiveActive_Active',
         'InactiveActive_Error',
         'InactiveActive_NotAvailable'])
    dataTypes.createIntegerDataType('OnOff_T', valueTable=[
        "OnOff_Off",
        "OnOff_On",
        "OnOff_1Hz",
        "OnOff_2Hz",
        "OnOff_3Hz"])
    
    ttList=['TPMS','LowOil','PosLamp','TurnLeft','TurnRight','AutoCruise','HighBeam',
            'SeatbeltDriver','SeatbeltPassenger','Airbag']
    
    package=ws.createPackage('Constant', role='Constant')
    for tt in ttList:
        package.createConstant('C_%sStatus_IV'%(tt), '/DataType/InactiveActive_T', 3)
        package.createConstant('C_Telltale%sStatus_IV'%(tt), '/DataType/OnOff_T', 0)
    
    package = ws.createPackage('PortInterface', role='PortInterface')
    for tt in ttList:
        package.createSenderReceiverInterface("%sStatus_I"%(tt), autosar.DataElement('%sStatus'%(tt), '/DataType/InactiveActive_T'))
        package.createSenderReceiverInterface("Telltale%sStatus_I"%(tt), autosar.DataElement('Telltale%sStatus'%(tt), '/DataType/OnOff_T'))

    package=ws.createPackage("ComponentType", role="ComponentType")
    swc = package.createApplicationSoftwareComponent('Telltale')
    for tt in ttList:
        swc.createRequirePort('%sStatusPort'%(tt), '%sStatus_I'%(tt), initValueRef='C_%sStatus_IV'%(tt))
        swc.createProvidePort('Telltale%sStatusPort'%(tt), 'Telltale%sStatus_I'%(tt), initValueRef='C_Telltale%sStatus_IV'%(tt))
    
    portAccessList = []
    for tt in ttList:
        portAccessList.append('%sStatusPort'%(tt))
        portAccessList.append('Telltale%sStatusPort'%(tt))
    swc.behavior.createRunnable('Telltale_run', portAccess=portAccessList)
    swc.behavior.createTimingEvent('Telltale_run', period=20)
    rtegen = autosar.RteGenerator()
    rtegen.writeComponentHeaders(swc, 'swc/telltale')

if(__name__ == '__main__'):
    main()