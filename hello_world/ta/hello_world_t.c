#include "hello_world_t.h"

#include "tee_internal_api.h"
#include "tee_ext_api.h"
#include "tee_log.h"
#include "securec.h"
#include "tee_enclave.h"
#include <string.h> /* for memcpy etc */
#include <stdlib.h> /* for malloc/free etc */

typedef TEE_Result (*ecall_invoke_entry) (uint32_t, TEE_Param[4]);

TEE_Param ocall_param;

TEE_Result TA_CreateEntryPoint(void) {
	addcaller_ca_exec("/vendor/bin/test_ca", "root");
	addcaller_ca_exec("/vendor/bin/teec_hello", "root");
return TEE_SUCCESS; }

void TA_DestroyEntryPoint(void) {}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
	TEE_Param params[4],
	void **sess_ctx) {
	/* Unused parameters */
	(void)&param_types;
	(void)&params;
	(void)&sess_ctx;
	return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void *sess_ctx) { 
	(void)&sess_ctx; /* Unused parameter */ 
} 



typedef struct ms_inc_value_t {
	int ms_retval;
	int ms_a;
} ms_inc_value_t;

typedef struct ms_do_string_t {
	char* ms_buffer;
	int ms_len;
} ms_do_string_t;

typedef struct ms_do_ocall_string_t {
	char* ms_buffer;
	int ms_len;
} ms_do_ocall_string_t;

typedef struct ms_do_inc_t {
	int ms_retval;
} ms_do_inc_t;

static TEE_Result tee_inc_value(uint32_t param_types,
	TEE_Param params[4])
{
	(void)&param_types;
	ms_inc_value_t* ms = SGX_CAST(ms_inc_value_t*, params[0].memref.buffer);
	char* buffer_start = params[0].memref.buffer + sizeof(ms_inc_value_t);

	TEE_Result status = TEE_SUCCESS;


	ms->ms_retval = inc_value(ms->ms_a);


	return status;
}

static TEE_Result tee_do_string(uint32_t param_types,
	TEE_Param params[4])
{
	(void)&param_types;
	ms_do_string_t* ms = SGX_CAST(ms_do_string_t*, params[0].memref.buffer);
	char* buffer_start = params[0].memref.buffer + sizeof(ms_do_string_t);

	TEE_Result status = TEE_SUCCESS;
	int _tmp_len = ms->ms_len;
	size_t _len_buffer = _tmp_len;
	char* _tmp_buffer = buffer_start + 0;
	char* _in_buffer = NULL;


	if (_tmp_buffer != NULL) {
		_in_buffer = (char*)malloc(_len_buffer);
		if (_in_buffer == NULL) {
			status = TEE_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_buffer, _tmp_buffer, _len_buffer);
	}
	do_string(_in_buffer, _tmp_len);
err:
	if (_in_buffer) {
		memcpy(_tmp_buffer, _in_buffer, _len_buffer);
		free(_in_buffer);
	}

	return status;
}

const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv;} ecall_table[2];
} g_ecall_table = {
	2,
	{
		{(void*)(uintptr_t)tee_inc_value, 0},
		{(void*)(uintptr_t)tee_do_string, 0},
	}
};

const struct {
	size_t nr_ocall;
	uint8_t entry_table[2][2];
} g_dyn_entry_table = {
	2,
	{
		{0, 0, },
		{0, 0, },
	}
};


TEE_Result do_ocall_string(char* buffer, int len)
{
	TEE_Result status = TEE_SUCCESS;
	size_t _len_buffer = len;

	ms_do_ocall_string_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_do_ocall_string_t);
	void *buffer_start = NULL;
	int* ocall_id;
	char* ocall_status;

	ocalloc_size += (buffer != NULL) ? _len_buffer : 0;

	ocall_status = ocall_param.memref.buffer;
	ocall_id = ocall_param.memref.buffer + sizeof(char);
	buffer_start = ocall_param.memref.buffer + sizeof(int) + sizeof(char);
	ms = (ms_do_ocall_string_t*)buffer_start;
	buffer_start += sizeof(ms_do_ocall_string_t);

	if (buffer != NULL) {
		ms->ms_buffer = (char*)buffer_start;
		memcpy(buffer_start, buffer, _len_buffer);
	} else if (buffer == NULL) {
		ms->ms_buffer = NULL;
	} else {
		return TEE_ERROR_BAD_PARAMETERS;
	}
	
	*ocall_id = 0;
	status = tee_ocall(ocall_status);

	if (buffer) memcpy((void*)buffer, buffer_start, _len_buffer);

	return status;
}

TEE_Result do_inc(int* retval)
{
	TEE_Result status = TEE_SUCCESS;

	ms_do_inc_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_do_inc_t);
	void *buffer_start = NULL;
	int* ocall_id;
	char* ocall_status;


	ocall_status = ocall_param.memref.buffer;
	ocall_id = ocall_param.memref.buffer + sizeof(char);
	buffer_start = ocall_param.memref.buffer + sizeof(int) + sizeof(char);
	ms = (ms_do_inc_t*)buffer_start;
	buffer_start += sizeof(ms_do_inc_t);

	
	*ocall_id = 1;
	status = tee_ocall(ocall_status);

	if (retval) *retval = ms->ms_retval;

	return status;
}


TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
	uint32_t cmd_id,
	uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx; /* Unused parameter */
	ocall_param = params[1];
	ecall_invoke_entry entry = SGX_CAST(ecall_invoke_entry, g_ecall_table.ecall_table[cmd_id].ecall_addr);
	return (*entry)(param_types, params);
}
