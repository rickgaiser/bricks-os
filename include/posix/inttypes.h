#ifndef INTTYPES_H
#define INTTYPES_H

#ifdef __cplusplus
extern "C" {
#endif


typedef char                        int8_t;
typedef short                       int16_t;
typedef long                        int32_t;
typedef long long                   int64_t;
typedef unsigned char               uint8_t;
typedef unsigned short              uint16_t;
typedef unsigned long               uint32_t;
typedef unsigned long long          uint64_t;

typedef volatile char               vint8_t;
typedef volatile short              vint16_t;
typedef volatile long               vint32_t;
typedef volatile long long          vint64_t;
typedef volatile unsigned char      vuint8_t;
typedef volatile unsigned short     vuint16_t;
typedef volatile unsigned long      vuint32_t;
typedef volatile unsigned long long vuint64_t;

typedef int                         intptr_t;
typedef unsigned int                uintptr_t;


#ifdef __cplusplus
}
#endif

#endif