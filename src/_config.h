#ifndef _JRMY_CONFIG
#define _JRMY_CONFIG

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct PrinterConfigStruct
    {
        char SSID[32] = "";
        char APPW[32] = "";
        char printerIP[16] = "";
        char accessCode[9] = "";
        char serialNumber[16] = "";

    } PrinterConfig;

    PrinterConfig printerConfig;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif