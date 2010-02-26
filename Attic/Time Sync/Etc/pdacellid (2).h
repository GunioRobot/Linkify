// http://forum.xda-developers.com/showpost.php?p=729533&postcount=30

#ifdef PDACELLID_EXPORTS
#define PDACELLID_API __declspec(dllexport)
#else
#define PDACELLID_API __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif

PDACELLID_API long fnGetCell(LPTSTR outData);
PDACELLID_API long fnGetCell2(LPTSTR outData);
#ifdef __cplusplus
}
#endif
