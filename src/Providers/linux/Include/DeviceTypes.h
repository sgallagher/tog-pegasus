//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
/* This file was automatically generated
 * using ./generateDeviceTypes.pl from /usr/share/pci.ids
 * This was performed on Tue Oct 23 15:37:22 2001
 * Do not edit by hand
 */
#define Device_UnclassifiedDevice 0x0000
#define Device_UnclassifiedDevice_NonVGAUnclassifiedDevice 0x0000
#define Device_UnclassifiedDevice_VGACompatibleUnclassifiedDevice 0x0001
#define Device_MassStorageController 0x0001
#define Device_MassStorageController_SCSIStorageController 0x0000
#define Device_MassStorageController_IDEInterface 0x0001
#define Device_MassStorageController_FloppyDiskController 0x0002
#define Device_MassStorageController_IPIBusController 0x0003
#define Device_MassStorageController_RAIDBusController 0x0004
#define Device_MassStorageController_UnknownMassStorageController 0x0080
#define Device_NetworkController 0x0002
#define Device_NetworkController_EthernetController 0x0000
#define Device_NetworkController_TokenRingNetworkController 0x0001
#define Device_NetworkController_FDDINetworkController 0x0002
#define Device_NetworkController_ATMNetworkController 0x0003
#define Device_NetworkController_ISDNController 0x0004
#define Device_NetworkController_NetworkController 0x0080
#define Device_DisplayController 0x0003
#define Device_DisplayController_VGACompatibleController 0x0000
#define Device_DisplayController_VGACompatibleController_VGA 0x0000
#define Device_DisplayController_VGACompatibleController_8514 0x0001
#define Device_DisplayController_XGACompatibleController 0x0001
#define Device_DisplayController_3DController 0x0002
#define Device_DisplayController_DisplayController 0x0080
#define Device_MultimediaController 0x0004
#define Device_MultimediaController_MultimediaVideoController 0x0000
#define Device_MultimediaController_MultimediaAudioController 0x0001
#define Device_MultimediaController_ComputerTelephonyDevice 0x0002
#define Device_MultimediaController_MultimediaController 0x0080
#define Device_MemoryController 0x0005
#define Device_MemoryController_RAMMemory 0x0000
#define Device_MemoryController_FLASHMemory 0x0001
#define Device_MemoryController_MemoryController 0x0080
#define Device_Bridge 0x0006
#define Device_Bridge_HostBridge 0x0000
#define Device_Bridge_ISABridge 0x0001
#define Device_Bridge_EISABridge 0x0002
#define Device_Bridge_MicroChannelBridge 0x0003
#define Device_Bridge_PCIBridge 0x0004
#define Device_Bridge_PCIBridge_NormalDecode 0x0000
#define Device_Bridge_PCIBridge_SubtractiveDecode 0x0001
#define Device_Bridge_PCMCIABridge 0x0005
#define Device_Bridge_NuBusBridge 0x0006
#define Device_Bridge_CardBusBridge 0x0007
#define Device_Bridge_RACEwayBridge 0x0008
#define Device_Bridge_RACEwayBridge_TransparentMode 0x0000
#define Device_Bridge_RACEwayBridge_EndpointMode 0x0001
#define Device_Bridge_SemiTransparentPCIToPCIBridge 0x0009
#define Device_Bridge_SemiTransparentPCIToPCIBridge_PrimaryBusTowardsHostCPU 0x0040
#define Device_Bridge_SemiTransparentPCIToPCIBridge_SecondaryBusTowardsHostCPU 0x0080
#define Device_Bridge_InfiniBandToPCIHostBridge 0x000a
#define Device_Bridge_Bridge 0x0080
#define Device_CommunicationController 0x0007
#define Device_CommunicationController_SerialController 0x0000
#define Device_CommunicationController_SerialController_8250 0x0000
#define Device_CommunicationController_SerialController_16450 0x0001
#define Device_CommunicationController_SerialController_16550 0x0002
#define Device_CommunicationController_SerialController_16650 0x0003
#define Device_CommunicationController_SerialController_16750 0x0004
#define Device_CommunicationController_SerialController_16850 0x0005
#define Device_CommunicationController_SerialController_16950 0x0006
#define Device_CommunicationController_ParallelController 0x0001
#define Device_CommunicationController_ParallelController_SPP 0x0000
#define Device_CommunicationController_ParallelController_BiDir 0x0001
#define Device_CommunicationController_ParallelController_ECP 0x0002
#define Device_CommunicationController_ParallelController_IEEE1284 0x0003
#define Device_CommunicationController_ParallelController_IEEE1284Target 0x00fe
#define Device_CommunicationController_MultiportSerialController 0x0002
#define Device_CommunicationController_Modem 0x0003
#define Device_CommunicationController_Modem_Generic 0x0000
#define Device_CommunicationController_Modem_Hayes16450 0x0001
#define Device_CommunicationController_Modem_Hayes16550 0x0002
#define Device_CommunicationController_Modem_Hayes16650 0x0003
#define Device_CommunicationController_Modem_Hayes16750 0x0004
#define Device_CommunicationController_CommunicationController 0x0080
#define Device_GenericSystemPeripheral 0x0008
#define Device_GenericSystemPeripheral_PIC 0x0000
#define Device_GenericSystemPeripheral_PIC_8259 0x0000
#define Device_GenericSystemPeripheral_PIC_ISAPIC 0x0001
#define Device_GenericSystemPeripheral_PIC_EISAPIC 0x0002
#define Device_GenericSystemPeripheral_PIC_IOAPIC 0x0010
#define Device_GenericSystemPeripheral_PIC_IOXAPIC 0x0020
#define Device_GenericSystemPeripheral_DMAController 0x0001
#define Device_GenericSystemPeripheral_DMAController_8237 0x0000
#define Device_GenericSystemPeripheral_DMAController_ISADMA 0x0001
#define Device_GenericSystemPeripheral_DMAController_EISADMA 0x0002
#define Device_GenericSystemPeripheral_Timer 0x0002
#define Device_GenericSystemPeripheral_Timer_8254 0x0000
#define Device_GenericSystemPeripheral_Timer_ISATimer 0x0001
#define Device_GenericSystemPeripheral_Timer_EISATimers 0x0002
#define Device_GenericSystemPeripheral_RTC 0x0003
#define Device_GenericSystemPeripheral_RTC_Generic 0x0000
#define Device_GenericSystemPeripheral_RTC_ISARTC 0x0001
#define Device_GenericSystemPeripheral_PCIHotPlugController 0x0004
#define Device_GenericSystemPeripheral_SystemPeripheral 0x0080
#define Device_InputDeviceController 0x0009
#define Device_InputDeviceController_KeyboardController 0x0000
#define Device_InputDeviceController_DigitizerPen 0x0001
#define Device_InputDeviceController_MouseController 0x0002
#define Device_InputDeviceController_ScannerController 0x0003
#define Device_InputDeviceController_GameportController 0x0004
#define Device_InputDeviceController_GameportController_Generic 0x0000
#define Device_InputDeviceController_GameportController_Extended 0x0010
#define Device_InputDeviceController_InputDeviceController 0x0080
#define Device_DockingStation 0x000a
#define Device_DockingStation_GenericDockingStation 0x0000
#define Device_DockingStation_DockingStation 0x0080
#define Device_Processor 0x000b
#define Device_Processor_386 0x0000
#define Device_Processor_486 0x0001
#define Device_Processor_Pentium 0x0002
#define Device_Processor_Alpha 0x0010
#define Device_Processor_PowerPC 0x0020
#define Device_Processor_MIPS 0x0030
#define Device_Processor_CoProcessor 0x0040
#define Device_SerialBusController 0x000c
#define Device_SerialBusController_FireWireIEEE1394 0x0000
#define Device_SerialBusController_FireWireIEEE1394_Generic 0x0000
#define Device_SerialBusController_FireWireIEEE1394_OHCI 0x0010
#define Device_SerialBusController_ACCESSBus 0x0001
#define Device_SerialBusController_SSA 0x0002
#define Device_SerialBusController_USBController 0x0003
#define Device_SerialBusController_USBController_UHCI 0x0000
#define Device_SerialBusController_USBController_OHCI 0x0010
#define Device_SerialBusController_USBController_Unspecified 0x0080
#define Device_SerialBusController_USBController_USBDevice 0x00Fe
#define Device_SerialBusController_FiberChannel 0x0004
#define Device_SerialBusController_SMBus 0x0005
#define Device_SerialBusController_InfiniBand 0x0006
#define Device_WirelessController 0x000d
#define Device_WirelessController_IRDAController 0x0000
#define Device_WirelessController_ConsumerIRController 0x0001
#define Device_WirelessController_RFController 0x0010
#define Device_WirelessController_WirelessController 0x0080
#define Device_IntelligentController 0x000e
#define Device_IntelligentController_I2O 0x0000
#define Device_SatelliteCommunicationsController 0x000f
#define Device_SatelliteCommunicationsController_SatelliteTVController 0x0000
#define Device_SatelliteCommunicationsController_SatelliteAudioCommunicationController 0x0001
#define Device_SatelliteCommunicationsController_SatelliteVoiceCommunicationController 0x0003
#define Device_SatelliteCommunicationsController_SatelliteDataCommunicationController 0x0004
#define Device_EncryptionController 0x0010
#define Device_EncryptionController_NetworkAndComputingEncryptionDevice 0x0000
#define Device_EncryptionController_EntertainmentEncryptionDevice 0x0010
#define Device_EncryptionController_EncryptionController 0x0080
#define Device_SignalProcessingController 0x0011
#define Device_SignalProcessingController_DPIOModule 0x0000
#define Device_SignalProcessingController_PerformanceCounters 0x0001
#define Device_SignalProcessingController_CommunicationSynchronizer 0x0010
#define Device_SignalProcessingController_SignalProcessingController 0x0080
#define Device_MassStorageDevice 0xff00
#define Device_MassStorageDevice_Disk 0xff00
#define Device_MassStorageDevice_Floppy 0xff01
#define Device_MassStorageDevice_CDROM 0xff02
#define Device_MassStorageDevice_Worm 0xff03
#define Device_MassStorageDevice_MagnetoOptical 0xff04
#define Device_MassStorageDevice_Tape 0xff05
#define Device_MassStorageDevice_DVD 0xff06
#define Device_Ports 0xff01
#define Device_Ports_Serial 0xff00
#define Device_Ports_Parallel 0xff01
#define Device_SystemResources 0xff02
#define Device_SystemResources_IRQ 0xff00
#define Device_SystemResources_DMA 0xff01
#define Device_SystemResources_IOPort 0xff02
#define Device_SystemResources_IOMemory 0xff03
#define Device_Process 0xff03
#define Device_Process_Process 0xff00
#define Device_Process_KernelThread 0xff01
#define Device_IPV4NetworkSettings 0xff04
#define Device_IPV4NetworkSettings_IPRoutes 0xff00
#define Device_IPV4NetworkSettings_Interfaces 0xff01
#define Device_IPV4NetworkSettings_Interfaces_LocalLoopback 0xff00
#define Device_IPV4NetworkSettings_Interfaces_Ethernet 0xff01
#define Device_IPV4NetworkSettings_Interfaces_TokenRing 0xff02
#define Device_IPV6NetworkSettings 0xff05
#define Device_Volumes 0xff06
#define Device_Volumes_Partitions 0xff00
#define Device_Volumes_Mounts 0xff01
#define Device_Volumes_Mounts_Local 0xff00
#define Device_Volumes_Mounts_Remote 0xff01
