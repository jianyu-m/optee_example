

#ifndef TEEC_ENCLAVE_H
#define TEEC_ENCLAVE_H

#include "tee_client_api.h"
#include "err.h"

typedef uint32_t tee_id_t;

extern TEEC_Context ctx;
extern TEEC_Session sess;
extern TEEC_UUID uuid;;
extern tee_id_t eid;

extern tee_id_t global_eid;

TEEC_Result sgx_create_enclave(char* name, int debug_flag, int* token, int *updated, tee_id_t *eid, void *unused);

void sgx_close_enclave(tee_id_t id);

void ocall_add(char* ocall_buffer, void** ocall_table);

void ocall_del(char* ocall_buffer);

#endif