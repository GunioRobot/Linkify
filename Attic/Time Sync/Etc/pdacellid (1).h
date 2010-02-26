// http://forum.xda-developers.com/showpost.php?p=687097&postcount=24

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PDACELLID_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PDACELLID_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef PDACELLID_EXPORTS
#define PDACELLID_API __declspec(dllexport)
#else
#define PDACELLID_API __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif

PDACELLID_API long fnGetCell();
#ifdef __cplusplus
}
#endif
