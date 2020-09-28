#ifndef HELLO_WORLD_U_H__
#define HELLO_WORLD_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "teec_enclave.h"
#include "tee_client_api.h" /* for sgx_satus_t etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

void do_ocall_string (char* buffer, int len);

TEEC_Result inc_value(tee_id_t eid, int* retval, int a);
TEEC_Result do_string(tee_id_t eid, char* buffer, int len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
