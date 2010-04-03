#include "messages.h"


static const TCHAR* _messages[] = {
    NULL,
    TEXT("Unspecified phone failure."), // RIL_E_PHONEFAILURE
    TEXT("RIL has no connection to the phone."), // RIL_E_NOCONNECTION
    TEXT("RIL's link to the phone is reserved."), // RIL_E_LINKRESERVED
    TEXT("Attempted operation isn't allowed."), // RIL_E_OPNOTALLOWED
    TEXT("Attempted operation isn't supported."), // RIL_E_OPNOTSUPPORTED
    TEXT("PH-SIM PIN is required to perform this operation."), // RIL_E_PHSIMPINREQUIRED
    TEXT("PH-FSIM PIN is required to perform this operation."), // RIL_E_PHFSIMPINREQUIRED
    TEXT("PH-FSIM PUK is required to perform this operation."), // RIL_E_PHFSIMPUKREQUIRED
    TEXT("SIM isn't inserted into the phone."), // RIL_E_SIMNOTINSERTED
    TEXT("SIM PIN is required to perform this operation."), // RIL_E_SIMPINREQUIRED
    TEXT("SIM PUK is required to perform this operation."), // RIL_E_SIMPUKREQUIRED
    TEXT("SIM failure was detected."), // RIL_E_SIMFAILURE
    TEXT("SIM is busy."), // RIL_E_SIMBUSY
    TEXT("Inorrect SIM was inserted."), // RIL_E_SIMWRONG
    TEXT("Incorrect password was supplied."), // RIL_E_INCORRECTPASSWORD
    TEXT("SIM PIN2 is required to perform this operation."), // RIL_E_SIMPIN2REQUIRED
    TEXT("SIM PUK2 is required to perform this operation."), // RIL_E_SIMPUK2REQUIRED
    TEXT("Storage memory is full."), // RIL_E_MEMORYFULL
    TEXT("Invalid storage index was supplied."), // RIL_E_INVALIDINDEX
    TEXT("A requested storage entry was not found."), // RIL_E_NOTFOUND
    TEXT("Storage memory failure."), // RIL_E_MEMORYFAILURE
    TEXT("Supplied text string is too long."), // RIL_E_TEXTSTRINGTOOLONG
    TEXT("Supplied text string contains invalid characters."), // RIL_E_INVALIDTEXTSTRING
    TEXT("Supplied dial string is too long."), // RIL_E_DIALSTRINGTOOLONG
    TEXT("Supplied dial string contains invalid characters."), // RIL_E_INVALIDDIALSTRING
    TEXT("Network service isn't available."), // RIL_E_NONETWORKSVC
    TEXT("Network operation timed out."), // RIL_E_NETWORKTIMEOUT
    TEXT("Network can only be used for emergency calls."), // RIL_E_EMERGENCYONLY
    TEXT("Network Personalization PIN is required to perform this operation."), // RIL_E_NETWKPINREQUIRED
    TEXT("Network Personalization PUK is required to perform this operation."), // RIL_E_NETWKPUKREQUIRED
    TEXT("Network Subset Personalization PIN is required to perform this operation."), // RIL_E_SUBSETPINREQUIRED
    TEXT("Network Subset Personalization PUK is required to perform this operation."), // RIL_E_SUBSETPUKREQUIRED
    TEXT("Service Provider Personalization PIN is required to perform this operation."), // RIL_E_SVCPINREQUIRED
    TEXT("Service Provider Personalization PUK is required to perform this operation."), // RIL_E_SVCPUKREQUIRED
    TEXT("Corporate Personalization PIN is required to perform this operation."), // RIL_E_CORPPINREQUIRED
    TEXT("Corporate Personalization PUK is required to perform this operation."), // RIL_E_CORPPUKREQUIRED
    TEXT("Telematic interworking isn't supported."), // RIL_E_TELEMATICIWUNSUPPORTED
    TEXT("Type 0 messages aren't supported."), // RIL_E_SMTYPE0UNSUPPORTED
    TEXT("Existing message cannot be replaced."), // RIL_E_CANTREPLACEMSG
    TEXT("Uspecified error related to the message Protocol ID."), // RIL_E_PROTOCOLIDERROR
    TEXT("Specified message Data Coding Scheme isn't supported."), // RIL_E_DCSUNSUPPORTED
    TEXT("Specified message class isn't supported."), // RIL_E_MSGCLASSUNSUPPORTED
    TEXT("Unspecified error related to the message Data Coding Scheme."), // RIL_E_DCSERROR
    TEXT("Specified message Command cannot be executed."), // RIL_E_CMDCANTBEACTIONED
    TEXT("Specified message Command isn't supported."), // RIL_E_CMDUNSUPPORTED
    TEXT("Unspecified error related to the message Command."), // RIL_E_CMDERROR
    TEXT("Unspecified error related to the message Body or Header."), // RIL_E_MSGBODYHEADERERROR
    TEXT("Message Service Center is busy."), // RIL_E_SCBUSY
    TEXT("No message Service Center subscription."), // RIL_E_NOSCSUBSCRIPTION
    TEXT("Message service Center system failure occurred."), // RIL_E_SCSYSTEMFAILURE
    TEXT("Specified address is invalid."), // RIL_E_INVALIDADDRESS
    TEXT("Message destination is barred."), // RIL_E_DESTINATIONBARRED
    TEXT("Duplicate message was rejected."), // RIL_E_REJECTEDDUPLICATE
    TEXT("Specified message Validity Period Format isn't supported."), // RIL_E_VPFUNSUPPORTED
    TEXT("Specified message Validity Period isn't supported."), // RIL_E_VPUNSUPPORTED
    TEXT("Message storage on the SIM is full."), // RIL_E_SIMMSGSTORAGEFULL
    TEXT("SIM isn't capable of storing messages."), // RIL_E_NOSIMMSGSTORAGE
    TEXT("SIM Application Toolkit is busy."), // RIL_E_SIMTOOLKITBUSY
    TEXT("SIM data download error."), // RIL_E_SIMDOWNLOADERROR
    TEXT("Messaging service is reserved."), // RIL_E_MSGSVCRESERVED
    TEXT("One of the message parameters is invalid."), // RIL_E_INVALIDMSGPARAM
    TEXT("Unknown message Service Center address was specified."), // RIL_E_UNKNOWNSCADDRESS
    TEXT("Specified message destination address is a currently unassigned phone number."), // RIL_E_UNASSIGNEDNUMBER
    TEXT("Message sending was barred by an operator."), // RIL_E_MSGBARREDBYOPERATOR
    TEXT("Message sending was prevented by outgoing calls barring."), // RIL_E_MSGCALLBARRED
    TEXT("Sent message has been rejected by the receiving equipment."), // RIL_E_MSGXFERREJECTED
    TEXT("Message could not be delivered because destination equipment is out of service."), // RIL_E_DESTINATIONOUTOFSVC
    TEXT("Sender's mobile ID isn't registered."), // RIL_E_UNIDENTIFIEDSUBCRIBER
    TEXT("Requested messaging service isn't supported."), // RIL_E_SVCUNSUPPORTED
    TEXT("Sender isn't recognized by the network."), // RIL_E_UNKNOWNSUBSCRIBER
    TEXT("Long-term network failure."), // RIL_E_NETWKOUTOFORDER
    TEXT("Short-term network failure."), // RIL_E_NETWKTEMPFAILURE
    TEXT("Operation failed because of the high network traffic."), // RIL_E_CONGESTION
    TEXT("Unspecified resources weren't available."), // RIL_E_RESOURCESUNAVAILABLE
    TEXT("Sender isn't subscribed for the requested messaging service."), // RIL_E_SVCNOTSUBSCRIBED
    TEXT("Requested messaging service isn't implemented on the network."), // RIL_E_SVCNOTIMPLEMENTED
    TEXT("Imvalid message reference value was used."), // RIL_E_INVALIDMSGREFERENCE
    TEXT("Message was determined to be invalid for unspecified reasons."), // RIL_E_INVALIDMSG
    TEXT("Mandatory message information is invalid or missing."), // RIL_E_INVALIDMANDATORYINFO
    TEXT("The message type is unsupported."), // RIL_E_MSGTYPEUNSUPPORTED
    TEXT("Sent message isn't compatible with the network."), // RIL_E_ICOMPATIBLEMSG
    TEXT("An information element specified in the message isn't supported."), // RIL_E_INFOELEMENTUNSUPPORTED
    TEXT("Unspefied protocol error."), // RIL_E_PROTOCOLERROR
    TEXT("Unspecified network error."), // RIL_E_NETWORKERROR
    TEXT("Unspecified messaging error."), // RIL_E_MESSAGINGERROR
    TEXT("RIL isn't yet ready to perform the requested operation."), // RIL_E_NOTREADY
    TEXT("Operation timed out."), // RIL_E_TIMEDOUT
    TEXT("Operation was cancelled."), // RIL_E_CANCELLED
    TEXT("Requested operation requires an RIL notification callback, which wasn't provided."), // RIL_E_NONOTIFYCALLBACK
    TEXT("Operator format isn't available."), // RIL_E_OPFMTUNAVAILABLE
    TEXT("Dial operation hasn't received a response for a long time."), // RIL_E_NORESPONSETODIAL
    TEXT("Security failure."), // RIL_E_SECURITYFAILURE
    TEXT("Radio failed to initialize correctly."), // RIL_E_RADIOFAILEDINIT
    TEXT("There was a problem initializing the radio driver."), // RIL_E_DRIVERINITFAILED
    TEXT("The Radio is not present."), // RIL_E_RADIONOTPRESENT
    TEXT("The Radio is in Off mode."), // RIL_E_RADIOOFF
    TEXT("Illegal MS."), // RIL_E_ILLEGALMS
    TEXT("Illegal ME."), // RIL_E_ILLEGALME
    TEXT("GPRS Service not allowed."), // RIL_E_GPRSSERVICENOTALLOWED
    TEXT("PLMN not allowed."), // RIL_E_PLMNNOTALLOWED
    TEXT("Location area not allowed."), // RIL_E_LOCATIONAREANOTALLOWED
    TEXT("Roaming not allowed in this location area."), // RIL_E_ROAMINGNOTALLOWEDINTHISLOCATIONAREA
    TEXT("Service option not supported."), // RIL_E_SERVICEOPTIONNOTSUPPORTED
    TEXT("Requested service option not subscribed."), // RIL_E_REQUESTEDSERVICEOPTIONNOTSUBSCRIBED
    TEXT("Service option temporarily out of order."), // RIL_E_SERVICEOPTIONTEMPORARILYOUTOFORDER
    TEXT("PDP authentication failure."), // RIL_E_PDPAUTHENTICATIONFAILURE
    TEXT("Invalid mobile class."), // RIL_E_INVALIDMOBILECLASS
    TEXT("Unspecific GPRS error."), // RIL_E_UNSPECIFIEDGPRSERROR
    TEXT("The command failed because the radio reset itself unexpectedly."), // RIL_E_RADIOREBOOTED
    TEXT("The command failed because the requested context state is invalid."), // RIL_E_INVALIDCONTEXTSTATE
    TEXT("The command failed because there are no more radio contexts."), // RIL_E_MAXCONTEXTS
    TEXT("The cached notification data is not present."), // RIL_E_SYNCHRONOUS_DATA_UNAVAILABLE
    TEXT("The RIL driver has issued an invalid asynchronous command response (hr == 0)."), // RIL_E_INVALIDASYNCCOMMANDRESPONSE
    NULL
};


namespace Wm {
namespace Api {
namespace Ril {
    const TCHAR* GetErrorMessage(HRESULT result) {
        if (HRESULT_FACILITY(result) == FACILITY_RIL) {
            return _messages[HRESULT_CODE(result) & 0xFF];
        }
        else {
            return NULL;
        }
    }
}}}
