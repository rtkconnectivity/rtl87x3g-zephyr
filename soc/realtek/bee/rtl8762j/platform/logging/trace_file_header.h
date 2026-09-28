#ifndef _TRACE_HEADER_H_
#define _TRACE_HEADER_H_

#include<stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct __trace_header
{
    char name[8];
    uint32_t start;
    uint32_t end;
    uint8_t version;
    uint8_t reserved[12];
    char  end_string[3];
} TRACE_HEADER;

#define TRACE_DATA_FIRST __attribute__((section(".TRACE_HEADER"))) __attribute__((aligned(4))) __attribute__((used))


#ifdef __cplusplus
}
#endif

#endif /* _TRACE_HEADER_H_ */