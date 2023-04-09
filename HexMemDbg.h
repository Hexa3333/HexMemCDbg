/*  hexmemdbg - v0.1 public domain standard c memory debugger
*   Use:
*       #define HEXMEMDBG_IMPLEMENTATION
*     before including this fileName in a C/C++ fileName to create the implementation.
*
*   By defining HEXMEMDBG_OUTPUT to a string literal you can also print all of the data into a file.
*   If undefined, all of the data will be printed onto stdout.
*       #define HEXMEMDBG_OUTPUT "MyOutput.txt"
*
*
*   Other Macros/Flags:
*       HEXMEMDBG_TRACE - Default Mode
*
*/


#ifndef HEXMEM_DBG_H
#define HEXMEM_DBG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

static unsigned int hexmemdbg_allocationCounter = 0;
static unsigned int hexmemdbg_deallocationCounter = 0;

#ifdef HEXMEMDBG_OUTPUT
const const char* hexmemdbg_outputFileName = HEXMEMDBG_OUTPUT;
#endif

typedef enum
{
    MALLOC, CALLOC, REALLOC, FREE
} hexmemdbg_Function_Type;

typedef struct
{
    hexmemdbg_Function_Type fType;
    uint32_t allocated;
    uint16_t line;
    const char* fileName;
} hexmemdbg_Job;

hexmemdbg_Job hexmemdbg_Job_new(hexmemdbg_Function_Type fType, uint32_t allocated, uint16_t line, const char* fileName)
{
    hexmemdbg_Job Job = {
        .fType = fType,
        .allocated = allocated,
        .line = line,
        .fileName = fileName
    };
    return Job;
}

#ifdef __cplusplus
extern "C" {
#endif


void hexmemdbg_PrintToStream(FILE* stream)
{
    fprintf(stream, "Allocs: %i\nDeallocs: %i\n", hexmemdbg_allocationCounter, hexmemdbg_deallocationCounter);    
}

#define __hexmemdbg_Print(...) hexmemdbg_PrintToStream(__VA_ARGS__);

// TRACE DECLARATIONS
extern void* hexmemdbg_malloc_trace(size_t n, uint16_t line, const char* fileName);
extern void* hexmemdbg_calloc_trace(size_t n, size_t s, uint16_t line, const char* fileName);
extern void* hexmemdbg_realloc_trace(void* n, size_t s, uint16_t line, const char* fileName);
extern void hexmemdbg_free_trace(void* n, uint16_t line, const char* fileName);

#ifdef __cplusplus
}
#endif


#ifdef HEXMEMDBG_IMPLEMENTATION

// TRACE DEFINITIONS
void* hexmemdbg_malloc_trace(size_t allocated, uint16_t line, const char* fileName)
{
    hexmemdbg_Job myJob = hexmemdbg_Job_new(MALLOC, allocated, line, fileName);
    
    char traceData[512];
    int offset;

    // Type     FileName        Line        Size
    sprintf(traceData, "Malloc %s:%i (%i)\nCall Stack:\n", fileName, line, allocated);
    offset = strlen(traceData);
    
    if (hexmemdbg_outputFileName)
    {
        FILE* fp = fopen(hexmemdbg_outputFileName, "a");
        fprintf(fp, "%s\n", traceData);
        fclose(fp);
    }

    hexmemdbg_allocationCounter++;
    return malloc(allocated);
}

void* hexmemdbg_calloc_trace(size_t n, size_t s, uint16_t line, const char* fileName)
{
    hexmemdbg_allocationCounter++;
    return calloc(n, s);
}

void* hexmemdbg_realloc_trace(void* n, size_t allocated, uint16_t line, const char* fileName)
{
    return realloc(n, allocated);
}

void hexmemdbg_free_trace(void* n, uint16_t line, const char* fileName)
{
    hexmemdbg_deallocationCounter++;
    free(n);
}


    // Trace Shortcuts
    #define malloc(n) hexmemdbg_malloc_trace(n, __LINE__, __FILE__)
    #define calloc(n, s) hexmemdbg_calloc_trace(n, s, __LINE__, __FILE__)
    #define realloc(n, s) hexmemdbg_realloc_trace(n, s, __LINE__, __FILE__)
    #define free(n) hexmemdbg_free_trace(n, __LINE__, __FILE__)

#endif // HEXMEMDBG_IMPLEMENTATION


#endif // HEXMEMDBG_H