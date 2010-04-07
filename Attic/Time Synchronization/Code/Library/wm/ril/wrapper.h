#ifndef __WM__RIL__WRAPPER__
#define __WM__RIL__WRAPPER__


#include "../Object.h"
#include <CELLCORE/OAK/INC/ril.h>


#if defined(API_FUNCTION_DEFINITION)
#   undef API_FUNCTION
#   define API_FUNCTION(return, name, args) \
        name##Function name = NULL
#elif defined(API_FUNCTION_LOADER)
#   undef API_FUNCTION
#   define API_FUNCTION(return, name, args) \
        (name = (name##Function) GetProcAddress(library, TEXT(#name)))
#elif defined(API_FUNCTION_UNLOADER)
#   undef API_FUNCTION
#   define API_FUNCTION(return, name, args) \
        (name = NULL)
#else
#   define API_FUNCTION(return ,name, args) \
        typedef return (*name##Function) args; \
        extern name##Function name
#endif


namespace wm {
namespace Api {
namespace Ril {
    HINSTANCE Load();
    bool Unload();
    
#if defined(API_FUNCTION_LOADER)
static void LoadFunctions(HINSTANCE library) {
#elif defined(API_FUNCTION_UNLOADER)
static void UnloadFunctions() {
#endif
    
    API_FUNCTION(HRESULT, RIL_Initialize, (DWORD dwIndex, RILRESULTCALLBACK pfnResult, RILNOTIFYCALLBACK pfnNotify, DWORD dwNotificationClasses, DWORD dwParam, HRIL* lphRil));
    API_FUNCTION(HRESULT, RIL_InitializeEmergency, (DWORD dwIndex, RILRESULTCALLBACK pfnResult, RILNOTIFYCALLBACK pfnNotify, DWORD dwNotificationClasses, DWORD dwParam, HRIL* lphRil));
    API_FUNCTION(HRESULT, RIL_Deinitialize, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_EnableNotifications, (HRIL hRil, DWORD dwNotificationClasses));
    API_FUNCTION(HRESULT, RIL_DisableNotifications, (HRIL hRil, DWORD dwNotificationClasses));
    API_FUNCTION(HRESULT, RIL_RegisterATCommandLogging, (HRIL hRil, BOOL fEnable));
    API_FUNCTION(HRESULT, RIL_ATCommandLogFile, (HRIL hRil, LPCTSTR pszFilename));
    API_FUNCTION(HRESULT, RIL_GetSerialPortHandle, (HRIL hRil, HANDLE* lphSerial));
    API_FUNCTION(HRESULT, RIL_GetVTSerialPortHandle, (HRIL hRil, HANDLE* lphSerial));
    API_FUNCTION(HRESULT, RIL_GetSerialPortHandleFromContextID, (HRIL hRil, DWORD dwContextID, HANDLE *lphSerial));
    API_FUNCTION(HRESULT, RIL_GetSerialPortStatistics, (HRIL hRil, RILSERIALPORTSTATS* lpSerialPortStats));
    API_FUNCTION(HRESULT, RIL_GetDriverVersion, (HRIL hRil, DWORD *pdwVersion));
    API_FUNCTION(HRESULT, RIL_GetSubscriberNumbers, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_GetOperatorList, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_GetAllOperatorsList, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_GetPreferredOperatorList, (HRIL hRil, DWORD dwFormat));
    API_FUNCTION(HRESULT, RIL_AddPreferredOperator, (HRIL hRil, DWORD dwIndex, const RILOPERATORNAMES* lpOperatorNames));
    API_FUNCTION(HRESULT, RIL_RemovePreferredOperator, (HRIL hRil, DWORD dwIndex));
    API_FUNCTION(HRESULT, RIL_GetCurrentOperator, (HRIL hRil, DWORD dwFormat));
    API_FUNCTION(HRESULT, RIL_RegisterOnNetwork, (HRIL hRil, DWORD dwMode, const RILOPERATORNAMES* lpOperatorNames));
    API_FUNCTION(HRESULT, RIL_UnregisterFromNetwork, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_GetRegistrationStatus, (HRIL hRil, DWORD * pdwRegStatus));
    API_FUNCTION(HRESULT, RIL_GetCallerIdSettings, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetCallerIdStatus, (HRIL hRil, DWORD dwStatus));
    API_FUNCTION(HRESULT, RIL_GetHideIdSettings, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetHideIdStatus, (HRIL hRil, DWORD dwStatus));
    API_FUNCTION(HRESULT, RIL_GetDialedIdSettings, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetDialedIdStatus, (HRIL hRil, DWORD dwStatus));
    API_FUNCTION(HRESULT, RIL_GetHideConnectedIdSettings, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetHideConnectedIdStatus, (HRIL hRil, DWORD dwStatus));
    API_FUNCTION(HRESULT, RIL_GetCCBSStatus, (HRIL hRil, DWORD dwCCBSIndex));
    API_FUNCTION(HRESULT, RIL_ClearCCBSRegistration, (HRIL hRil, DWORD dwCCBSIndex));
    API_FUNCTION(HRESULT, RIL_GetClosedGroupSettings, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetClosedGroupSettings, (HRIL hRil, const RILCLOSEDGROUPSETTINGS* lpSettings));
    API_FUNCTION(HRESULT, RIL_GetCallForwardingSettings, (HRIL hRil, DWORD dwReason, DWORD dwInfoClass));
    API_FUNCTION(HRESULT, RIL_AddCallForwarding, (HRIL hRil, DWORD dwReason, const RILCALLFORWARDINGSETTINGS* lpSettings));
    API_FUNCTION(HRESULT, RIL_RemoveCallForwarding, (HRIL hRil, DWORD dwReason, DWORD dwInfoClasses));
    API_FUNCTION(HRESULT, RIL_SetCallForwardingStatus, (HRIL hRil, DWORD dwReason, DWORD dwInfoClasses, DWORD dwStatus));
    API_FUNCTION(HRESULT, RIL_GetCallWaitingSettings, (HRIL hRil, DWORD dwInfoClass));
    API_FUNCTION(HRESULT, RIL_SetCallWaitingStatus, (HRIL hRil, DWORD dwInfoClasses, DWORD dwStatus));
    API_FUNCTION(HRESULT, RIL_SendSupServiceData, (HRIL hRil, LPCWSTR pwszData));
    API_FUNCTION(HRESULT, RIL_CancelSupServiceDataSession, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_GetCurrentAddressId, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetCurrentAddressId, (HRIL hRil, DWORD dwAddressId));
    API_FUNCTION(HRESULT, RIL_Dial, (HRIL hRil, LPCSTR lpszAddress, DWORD dwType, DWORD dwOptions));
    API_FUNCTION(HRESULT, RIL_Answer, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_Hangup, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SendDTMF, (HRIL hRil, LPCSTR lpszChars, DWORD dwDuration));
    API_FUNCTION(HRESULT, RIL_StartDTMF, (HRIL hRil, CHAR ch));
    API_FUNCTION(HRESULT, RIL_StopDTMF, (HRIL hRil, CHAR ch));
    API_FUNCTION(HRESULT, RIL_SetDTMFMonitoring, (HRIL hRil, BOOL fEnable));
    API_FUNCTION(HRESULT, RIL_ManageCalls, (HRIL hRil, DWORD dwCommand, DWORD dwID));
    API_FUNCTION(HRESULT, RIL_TransferCall, (HRIL hRil, const RILADDRESS* lpAddress, const RILSUBADDRESS* lpSubAddress));
    API_FUNCTION(HRESULT, RIL_GetAudioGain, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetAudioGain, (HRIL hRil, const RILGAININFO* lpGainInfo));
    API_FUNCTION(HRESULT, RIL_GetAudioDevices, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetAudioDevices, (HRIL hRil, const RILAUDIODEVICEINFO* lpAudioDeviceInfo));
    API_FUNCTION(HRESULT, RIL_GetAudioMuting, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetAudioMuting, (HRIL hRil, BOOL fEnable));
    API_FUNCTION(HRESULT, RIL_GetHSCSDOptions, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetHSCSDOptions, (HRIL hRil, const RILHSCSDINFO* lpHscsdInfo));
    API_FUNCTION(HRESULT, RIL_GetHSCSDCallSettings, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_GetDataCompression, (HRIL hRil, RILDATACOMPINFO * pDataCompInfo));
    API_FUNCTION(HRESULT, RIL_SetDataCompression, (HRIL hRil, const RILDATACOMPINFO* lpDataCompInfo));
    API_FUNCTION(HRESULT, RIL_GetErrorCorrection, (HRIL hRil, RILERRORCORRECTIONINFO * pErrorCorrectionInfo));
    API_FUNCTION(HRESULT, RIL_SetErrorCorrection, (HRIL hRil, const RILERRORCORRECTIONINFO* lpErrorCorrectionInfo));
    API_FUNCTION(HRESULT, RIL_GetBearerServiceOptions, (HRIL hRil, RILBEARERSVCINFO* pBearerServiceInfo));
    API_FUNCTION(HRESULT, RIL_SetBearerServiceOptions, (HRIL hRil, const RILBEARERSVCINFO* lpBearerServiceInfo));
    API_FUNCTION(HRESULT, RIL_GetRLPOptions, (HRIL hRil, RILRLPINFO* lpRlpInfo));
    API_FUNCTION(HRESULT, RIL_SetRLPOptions, (HRIL hRil, const RILRLPINFO* lpRlpInfo));
    API_FUNCTION(HRESULT, RIL_GetMsgServiceOptions, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetMsgServiceOptions, (HRIL hRil, const RILMSGSERVICEINFO* lpMsgServiceInfo));
    API_FUNCTION(HRESULT, RIL_GetMsgConfig, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetMsgConfig, (HRIL hRil, const RILMSGCONFIG* lpMsgConfigInfo));
    API_FUNCTION(HRESULT, RIL_GetCellBroadcastMsgConfig, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetCellBroadcastMsgConfig, (HRIL hRil, const RILCBMSGCONFIG* lpCbMsgConfigInfo));
    API_FUNCTION(HRESULT, RIL_ReadMsg, (HRIL hRil, DWORD dwIndex));
    API_FUNCTION(HRESULT, RIL_DeleteMsg, (HRIL hRil, DWORD dwIndex));
    API_FUNCTION(HRESULT, RIL_WriteMsg, (HRIL hRil, const RILMESSAGE* lpMessage, DWORD dwStatus));
    API_FUNCTION(HRESULT, RIL_SendMsg, (HRIL hRil, const RILMESSAGE* lpMessage, DWORD dwOptions));
    API_FUNCTION(HRESULT, RIL_SendStoredMsg, (HRIL hRil, DWORD dwIndex, DWORD dwOptions));
    API_FUNCTION(HRESULT, RIL_SendMsgAcknowledgement, (HRIL hRil, BOOL fSuccess));
    API_FUNCTION(HRESULT, RIL_GetUserIdentity, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_GetPhoneLockedState, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_UnlockPhone, (HRIL hRil, LPCSTR lpszPassword, LPCSTR lpszNewPassword));
    API_FUNCTION(HRESULT, RIL_GetLockingStatus, (HRIL hRil, DWORD dwFacility, LPCSTR lpszPassword));
    API_FUNCTION(HRESULT, RIL_SetLockingStatus, (HRIL hRil, DWORD dwFacility, LPCSTR lpszPassword, DWORD dwStatus));
    API_FUNCTION(HRESULT, RIL_ChangeLockingPassword, (HRIL hRil, DWORD dwFacility, DWORD dwOldPasswordType, LPCSTR lpszOldPassword, LPCSTR lpszNewPassword));
    API_FUNCTION(HRESULT, RIL_GetCallBarringStatus, (HRIL hRil, DWORD dwType, DWORD dwInfoClass, LPCSTR lpszPassword));
    API_FUNCTION(HRESULT, RIL_SetCallBarringStatus, (HRIL hRil, DWORD dwType, DWORD dwInfoClass, LPCSTR lpszPassword, DWORD dwStatus));
    API_FUNCTION(HRESULT, RIL_SendSecureMmiString, (HRIL hRil, LPCSTR lpszMmiString));
    API_FUNCTION(HRESULT, RIL_ChangeCallBarringPassword, (HRIL hRil, DWORD dwType, LPCSTR lpwszOldPassword, LPCSTR lpwszNewPassword));
    API_FUNCTION(HRESULT, RIL_GetEquipmentInfo, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_GetEquipmentState, (HRIL hRil, RILEQUIPMENTSTATE *pEqState));
    API_FUNCTION(HRESULT, RIL_SetEquipmentState, (HRIL hRil, DWORD dwEquipmentState));
    API_FUNCTION(HRESULT, RIL_GetRadioPresence, (HRIL hRIL, DWORD* dwRadioPresence));
    API_FUNCTION(HRESULT, RIL_GetPhonebookOptions, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetPhonebookOptions, (HRIL hRil, const RILPHONEBOOKINFO* lpPhonebookInfo));
    API_FUNCTION(HRESULT, RIL_ReadPhonebookEntries, (HRIL hRil, DWORD dwStartIndex, DWORD dwEndIndex));
    API_FUNCTION(HRESULT, RIL_WritePhonebookEntry, (HRIL hRil, const RILPHONEBOOKENTRY* lpEntry));
    API_FUNCTION(HRESULT, RIL_DeletePhonebookEntry, (HRIL hRil, DWORD dwIndex));
    API_FUNCTION(HRESULT, RIL_SendSimCmd, (HRIL hRil, const BYTE* lpbCommand, DWORD dwSize));
    API_FUNCTION(HRESULT, RIL_GetATR, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SendRestrictedSimCmd, (HRIL hRil, DWORD dwCommand, const RILSIMCMDPARAMETERS* lpParameters, const BYTE* lpbData, DWORD dwSize));
    API_FUNCTION(HRESULT, RIL_GetSimRecordStatus, (HRIL hRil, DWORD dwFileID));
    API_FUNCTION(HRESULT, RIL_GetSimToolkitProfile, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetSimToolkitProfile, (HRIL hRil, const BYTE* lpbProfile, DWORD dwSize));
    API_FUNCTION(HRESULT, RIL_SendSimToolkitEnvelopeCmd, (HRIL hRil, const BYTE* lpbCommand, DWORD dwSize));
    API_FUNCTION(HRESULT, RIL_FetchSimToolkitCmd, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SendSimToolkitCmdResponse, (HRIL hRil, const RILSIMTOOLKITRSP* pRsp, const BYTE *pDetails, DWORD dwDetailSize));
    API_FUNCTION(HRESULT, RIL_TerminateSimToolkitSession, (HRIL hRil, DWORD dwCause));
    API_FUNCTION(HRESULT, RIL_SendSimToolkitEventDownload, (HRIL hRil, const DWORD dwEvent, const BYTE *pData, DWORD dwDataSize));
    API_FUNCTION(HRESULT, RIL_GetCostInfo, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetCostInfo, (HRIL hRil, const RILCOSTINFO* lpCostInfo, LPCSTR lpszPassword));
    API_FUNCTION(HRESULT, RIL_GetCellTowerInfo, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_DevSpecific, (HRIL hRil, const BYTE* lpbParams, DWORD dwSize, BYTE* pbAsyncResults, DWORD dwAsyncResultsSize, DWORD* pcbAsyncResults, DWORD dwTimeOut));
    API_FUNCTION(HRESULT, RIL_GetDevCaps, (HRIL hRil, DWORD dwCapsType));
    API_FUNCTION(HRESULT, RIL_GetSystemTime, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_GetGPRSContextList, (HRIL hRil, RILGPRSCONTEXT * pGPRSContext, DWORD * pdwDataSize));
    API_FUNCTION(HRESULT, RIL_SetGPRSContext, (HRIL hRil, const RILGPRSCONTEXT* lpGprsContext));
    API_FUNCTION(HRESULT, RIL_DeleteGPRSContext, (HRIL hRil, DWORD dwContextID));
    API_FUNCTION(HRESULT, RIL_GetRequestedQualityOfServiceList, (HRIL hRil, RILGPRSQOSPROFILE * lpGprsQosProfile, DWORD * pdwDataSize));
    API_FUNCTION(HRESULT, RIL_SetRequestedQualityOfService, (HRIL hRil, const RILGPRSQOSPROFILE* lpGprsQosProfile));
    API_FUNCTION(HRESULT, RIL_DeleteRequestedQualityOfService, (HRIL hRil, DWORD dwContextID));
    API_FUNCTION(HRESULT, RIL_GetMinimumQualityOfServiceList, (HRIL hRil, RILGPRSQOSPROFILE * lpGprsQosProfile, DWORD * pdwDataSize));
    API_FUNCTION(HRESULT, RIL_SetMinimumQualityOfService, (HRIL hRil, const RILGPRSQOSPROFILE* lpGprsQosProfile));
    API_FUNCTION(HRESULT, RIL_DeleteMinimumQualityOfService, (HRIL hRil, DWORD dwContextID));
    API_FUNCTION(HRESULT, RIL_SetGPRSAttached, (HRIL hRil, BOOL fAttached));
    API_FUNCTION(HRESULT, RIL_GetGPRSAttached, (HRIL hRil, BOOL * pfGPRSAttached));
    API_FUNCTION(HRESULT, RIL_SetGPRSContextActivated, (HRIL hRil, DWORD dwContextID, BOOL fContextActivation));
    API_FUNCTION(HRESULT, RIL_GetGPRSContextActivatedList, (HRIL hRil, RILGPRSCONTEXTACTIVATED * pGPRSContextActivated, DWORD * pdwDataSize));
    API_FUNCTION(HRESULT, RIL_EnterGPRSDataMode, (HRIL hRil, const RILENTERGPRSDATAMODE* lpEnterGprsDataMode));
    API_FUNCTION(HRESULT, RIL_GetGPRSAddress, (HRIL hRil, DWORD dwContextID, WCHAR * pwszGPRSAddress, LPDWORD pGprsAddrCch));
    API_FUNCTION(HRESULT, RIL_GPRSAnswer, (HRIL hRil, const RILGPRSANSWER* lpGprsAnswer));
    API_FUNCTION(HRESULT, RIL_GetGPRSRegistrationStatus, (HRIL hRil, DWORD * pdwGPRSRegStatus));
    API_FUNCTION(HRESULT, RIL_GetGPRSClass, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetGPRSClass, (HRIL hRil, DWORD dwClass));
    API_FUNCTION(HRESULT, RIL_GetMOSMSService, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetMOSMSService, (HRIL hRil, DWORD dwMoSmsService));
    API_FUNCTION(HRESULT, RIL_GetPacketByteCount, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_ResetPacketByteCount, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_GetCurrentSystemType, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SendFlash, (HRIL hRil, LPRILADDRESS lpraRilAddress));
    API_FUNCTION(HRESULT, RIL_GetRoamingMode, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetRoamingMode, (HRIL hRil, DWORD dwRoamingMode));
    API_FUNCTION(HRESULT, RIL_GetPreferredPrivacyMode, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SetPreferredPrivacyMode, (HRIL hRil, DWORD dwPreferredPrivacyMode));
    API_FUNCTION(HRESULT, RIL_GetCurrentPrivacyStatus, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_SendAKey, (HRIL hRil, LPCSTR lpszChars));
    API_FUNCTION(HRESULT, RIL_GetCurrentLocationStatus, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_GetCurrentRoamingStatus, (HRIL hRil));
    API_FUNCTION(HRESULT, RIL_NDIS_SetGPRSContextActivated, (HRIL hRil, const RILNDISGPRSCONTEXT *lpNdisSetGprsContextActivated));
    API_FUNCTION(HRESULT, RIL_LogEventToRadio, (HRIL hRil, LPCSTR lpszChars));
    
#if defined(API_FUNCTION_LOADER) || defined(API_FUNCTION_UNLOADER)
}
#endif
}}}


#undef API_FUNCTION


#endif
