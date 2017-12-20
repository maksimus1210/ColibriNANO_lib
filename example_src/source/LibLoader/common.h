#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef float Real;

typedef struct
{
    Real re;
    Real im;
}Complex;

typedef void* Descriptor;

typedef enum
{
    Sr_48kHz = 0,
    Sr_96kHz,
    Sr_192kHz,
    Sr_384kHz,
    Sr_768kHz,
    Sr_1536kHz,
    Sr_1920kHz,
    Sr_2560kHz,
    Sr_3072kHz,

    SampleRatesCount
}SampleRateIndex;

typedef bool (*pCallbackRx)(Complex *pSrc, uint32_t len, bool adcOverload, void *pUserData);


#endif // COMMON_H
