//#include <fstream>
#include <npapi.h>
#include <npupp.h>
#include <npruntime.h>
//#include <purple.h>
#include <string>
#include "npMessaging.h"


static NPObject* _pluginObj = NULL;
static NPNetscapeFuncs* _browser = NULL;
//static std::ofstream _log("npMessaging.log");


static bool hasMethod(NPObject* obj, NPIdentifier name) {
    //_log << "hasMethod" << std::endl;
    return true;
}


static bool invokeDefault(NPObject* obj, const NPVariant* argv, uint32_t argc, NPVariant* result) {
    //_log << "invokeDefault" << std::endl;

    result->type = NPVariantType_Int32;
    result->value.intValue = 1024;
    return true;
}


static bool invoke(NPObject* obj, NPIdentifier name, const NPVariant* argv, uint32_t argc, NPVariant* result) {
    //_log << "invoke" << std::endl;
    std::string cname = _browser->utf8fromidentifier(name);
    
    if (cname == "foo") {
        return invokeDefault(obj, argv, argc, result);
    }
    else {
        _browser->setexception(obj, "Exception during invocation.");
        return false;
    }
}


static bool hasProperty(NPObject* obj, NPIdentifier name) {
    //_log << "hasProperty" << std::endl;
    return false;
}


static bool getProperty(NPObject* obj, NPIdentifier name, NPVariant* result) {
    //_log << "getProperty" << std::endl;
    return false;
}


static bool setProperty(NPObject* obj, NPIdentifier name, const NPVariant* value) {
    //_log << "setProperty" << std::endl;
    return false;
}


static bool removeProperty(NPObject* obj, NPIdentifier name) {
    //_log << "removeProperty" << std::endl;
    return false;
}


static NPClass _pluginClass = {
    NP_CLASS_STRUCT_VERSION,
    NULL,
    NULL,
    NULL,
    hasMethod,
    invoke,
    invokeDefault,
    hasProperty,
    getProperty,
    setProperty,
    removeProperty,
};


static NPError create(NPMIMEType type, NPP instance, uint16 mode, int16 argc, char* argn[], char* argv[], NPSavedData* data) {
    //_log << "create" << std::endl;
    return NPERR_NO_ERROR;
}


static NPError destroy(NPP instance, NPSavedData** data) {
    //_log << "destroy" << std::endl;
    
    if (_pluginObj != NULL) {
        _browser->releaseobject(_pluginObj);
        _pluginObj = NULL;
    }
    
    return NPERR_NO_ERROR;
}


static NPError getValue(NPP instance, NPPVariable what, void* value) {
    //_log << "getValue" << std::endl;
    
    switch (what) {
    case NPPVpluginNameString:
        *(char**) value = PLUGIN_NAME;
        break;
    case NPPVpluginDescriptionString:
        *(char**) value = PLUGIN_DESCRIPTION;
        break;
    case NPPVpluginScriptableNPObject:
        if (_pluginObj == NULL) {
            _pluginObj = _browser->createobject(instance, &_pluginClass);
        }
        _browser->retainobject(_pluginObj);
        *(NPObject**) value = _pluginObj;
        break;
    case NPPVpluginNeedsXEmbed:
        *(PRBool*) value = PR_FALSE;
        break;
    default:
        return NPERR_GENERIC_ERROR;
    }
    
    return NPERR_NO_ERROR;
}


static NPError handleEvent(NPP instance, void* event) {
    //_log << "handleEvent" << std::endl;
    return NPERR_NO_ERROR;
}


static NPError setWindow(NPP instance, NPWindow* window) {
    //_log << "setWindow" << std::endl;
    return NPERR_NO_ERROR;
}


extern "C" NPError OSCALL NP_GetEntryPoints(NPPluginFuncs* plugin) {
    //_log << "NP_GetEntryPoints" << std::endl;
    
    plugin->version = (NP_VERSION_MAJOR << 8) | NP_VERSION_MINOR;
    plugin->newp = create;
    plugin->destroy = destroy;
    plugin->getvalue = getValue;
    plugin->event = handleEvent;
    plugin->setwindow = setWindow;
    
    return NPERR_NO_ERROR;
}


extern "C" NPError OSCALL NP_Initialize(NPNetscapeFuncs* browser) {
    //_log << "NP_Initialize" << std::endl;

    if (browser == NULL) {
        return NPERR_INVALID_FUNCTABLE_ERROR;
    }
    if (((browser->version >> 8) & 0xFF) > NP_VERSION_MAJOR) {
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
    
    _browser = browser;
    return NPERR_NO_ERROR;
}


extern "C" NPError OSCALL NP_Shutdown() {
    //_log << "NP_Shutdown" << std::endl;
    _browser = NULL;
    return NPERR_NO_ERROR;
}


extern "C" char* NP_GetMIMEDescription() {
    //_log << "NP_GetMIMEDescription" << std::endl;
    return PLUGIN_MIME_TYPE "::";
}
