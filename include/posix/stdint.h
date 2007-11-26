#ifndef STDINT_H
#define STDINT_H


typedef char                        int8_t;
typedef short                       int16_t;
typedef int                         int32_t;
typedef long long                   int64_t;
typedef unsigned char               uint8_t;
typedef unsigned short              uint16_t;
typedef unsigned int                uint32_t;
typedef unsigned long long          uint64_t;

typedef int8_t                      __s8;
typedef int16_t                     __s16;
typedef int32_t                     __s32;
typedef int64_t                     __s64;
typedef uint8_t                     __u8;
typedef uint16_t                    __u16;
typedef uint32_t                    __u32;
typedef uint64_t                    __u64;

typedef volatile int8_t             vint8_t;
typedef volatile int16_t            vint16_t;
typedef volatile int32_t            vint32_t;
typedef volatile int64_t            vint64_t;
typedef volatile uint8_t            vuint8_t;
typedef volatile uint16_t           vuint16_t;
typedef volatile uint32_t           vuint32_t;
typedef volatile uint64_t           vuint64_t;

typedef int                         intptr_t;
typedef unsigned int                uintptr_t;


#endif
