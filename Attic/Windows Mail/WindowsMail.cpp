/**
 * @file
 * @brief Windows Mail (formerly Outlook Express) interface
 * @see http://msdn.microsoft.com/en-us/library/ms709546.aspx
 * @see http://www.codeproject.com/KB/COM/Outlook_Express_Messages.aspx
 *
 * Exposes a simpler interface for listing folders, opening messages, etc.
 */


#include <cstdio>
#include <cstring>
#include <initguid.h>    // Must be included once and before the API.
#include "msoeapi.h"


#ifdef _MSC_VER
    /* Supress "strcpy" deprecation warning. */
    #pragma warning (disable: 4996)
#endif


#define PUBLIC EXTERN_C __declspec(dllexport)


static IStoreNamespace* _storeNamespace = NULL;

/** Identifier of the root folder, for the enumeration starting point. */
PUBLIC const STOREFOLDERID ROOT_FOLDER_ID = FOLDERID_ROOT;

/** Maximum size (in bytes) of a folder name. */
PUBLIC const size_t FOLDER_NAME_MAX_SIZE = CCHMAX_FOLDER_NAME * sizeof(char);


/**
 * Performs clean-up actions.
 *
 * @return S_OK if successful, otherwise the corresponding status code
 */
PUBLIC HRESULT STDAPICALLTYPE Finalize() {
    _storeNamespace->Release();
    CoUninitialize();
    return S_OK;
}


/**
 * Gets the name of a folder.
 *
 * @param [in] folder folder identifier
 * @param [out] name folder name
 * @return S_OK if successful, otherwise the corresponding status code
 */
PUBLIC HRESULT STDAPICALLTYPE GetFolderName(STOREFOLDERID folder, char* name) {
    FOLDERPROPS properties;
    HRESULT result;
    
    properties.cbSize = sizeof(FOLDERPROPS);
    result = _storeNamespace->GetFolderProps(folder, 0, &properties);
    
    if (SUCCEEDED(result)) {
        strcpy(name, properties.szName);
        result = S_OK;
    }
    
    return result;
}


/**
 * Gets the raw contents of a message.
 *
 * @param [in] folder folder identifier
 * @param [in] message message identifier
 * @param [in] callback event handler function
 * @return S_OK if successful, otherwise the corresponding status code
 */
PUBLIC HRESULT STDAPICALLTYPE GetMessageData(
    STOREFOLDERID folder,
    MESSAGEID message,
    void (*callback)(const char* chunk))
{
    IStoreFolder* storeFolder;
    HRESULT result = _storeNamespace->OpenFolder(folder, 0, &storeFolder);
    
    if (FAILED(result)) {
        return result;
    }
    
    IStream* stream;
    result = storeFolder->OpenMessage(message, IID_IStream, (LPVOID*) &stream);
    
    if (SUCCEEDED(result)) {
        char buffer[BUFSIZ];
        ULONG bytesRead;
        
        do {
            result = stream->Read(buffer, sizeof(buffer) - 1, &bytesRead);
            
            if (FAILED(result)) {
                bytesRead = 0;
            }
            else {
                buffer[bytesRead] = '\0';
                callback(buffer);
            }
        }
        while (bytesRead > 0);
        
        stream->Release();
    }
    
    storeFolder->Release();
    return result;
}


/**
 * Prepares this library for further interactions.
 *
 * @return S_OK if successful, otherwise the corresponding status code
 */
PUBLIC HRESULT STDAPICALLTYPE Initialize() {
    HRESULT result;
    
    if (FAILED(result = CoInitialize(NULL))) {
        return result;
    }
    
    result = CoCreateInstance(CLSID_StoreNamespace, NULL, CLSCTX_SERVER,
                              IID_IStoreNamespace, (LPVOID*) &_storeNamespace);
    
    if (FAILED(result)) {
        return result;
    }
    if (FAILED(result = _storeNamespace->Initialize(NULL, 0))) {
        return result;
    }
    
    return S_OK;
}


/**
 * Lists folders.
 *
 * @param [in] parent identifier of the parent folder
 * @param [in] callback event handler function
 * @return S_OK if successful, otherwise the corresponding status code
 */
PUBLIC HRESULT STDAPICALLTYPE ListFolders(
    STOREFOLDERID parent,
    void (*callback)(STOREFOLDERID))
{
    FOLDERPROPS properties;
    HENUMSTORE iterator;
    HRESULT result;
    
    properties.cbSize = sizeof(FOLDERPROPS);
    result = _storeNamespace->GetFirstSubFolder(parent, &properties, &iterator);
    
    while (SUCCEEDED(result) && (result != S_FALSE)) {
        callback(properties.dwFolderId);
        result = _storeNamespace->GetNextSubFolder(iterator, &properties);
    }
    
    if (SUCCEEDED(result)) {
        result = S_OK;
    }
    if (iterator != NULL) {
        _storeNamespace->GetSubFolderClose(iterator);
    }
    
    return result;
}


/**
 * Lists messages.
 *
 * @param [in] folder identifier of the containing folder
 * @param [in] callback event handler function
 * @return S_OK if successful, otherwise the corresponding status code
 */
PUBLIC HRESULT STDAPICALLTYPE ListMessages(
    STOREFOLDERID folder,
    void (*callback)(MESSAGEID))
{
    IStoreFolder* storeFolder;
    HRESULT result = _storeNamespace->OpenFolder(folder, 0, &storeFolder);
    
    if (FAILED(result)) {
        return result;
    }
    
    MESSAGEPROPS properties;
    HENUMSTORE iterator;
    
    properties.cbSize = sizeof(MESSAGEPROPS);
    
    // An empty folder yields an unspecified error.
    result = storeFolder->GetFirstMessage(
        MSGPROPS_FAST, 0, MESSAGEID_FIRST, &properties, &iterator);
    
    while (SUCCEEDED(result) && (result != S_FALSE)) {
        callback(properties.dwMessageId);
        storeFolder->FreeMessageProps(&properties);
        
        result = storeFolder->GetNextMessage(
            iterator, MSGPROPS_FAST, &properties);
    }
    
    if (SUCCEEDED(result) || (result == E_FAIL)) {
        result = S_OK;
    }
    if (iterator != NULL) {
        storeFolder->GetMessageClose(iterator);
    }
    
    storeFolder->Release();
    return result;
}
