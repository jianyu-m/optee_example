#ifndef HELLO_WORLD_T_H__
#define HELLO_WORLD_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>

#include "tee_internal_api.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif


int inc_value(int a);
void do_string(char* buffer, int len);

TEE_Result do_ocall_string(char* buffer, int len);
TEE_Result do_inc(int* retval);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
