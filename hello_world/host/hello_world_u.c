#include "hello_world_u.h"
#include <errno.h>

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

static TEEC_Result hello_world_do_ocall_string(void* pms)
{
	ms_do_ocall_string_t* ms = SGX_CAST(ms_do_ocall_string_t*, pms);
	ms->ms_buffer = pms + sizeof(ms_do_ocall_string_t);

	do_ocall_string(ms->ms_buffer, ms->ms_len);

	return TEEC_SUCCESS;
}

static TEEC_Result hello_world_do_inc(void* pms)
{
	ms_do_inc_t* ms = SGX_CAST(ms_do_inc_t*, pms);

	ms->ms_retval = do_inc();

	return TEEC_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[2];
} ocall_table_hello_world = {
	2,
	{
		(void*)hello_world_do_ocall_string,
		(void*)hello_world_do_inc,
	}
};
TEEC_Result inc_value(tee_id_t eid, int* retval, int a)
{
	TEEC_Result status;
	TEEC_Operation op;
	uint32_t ret_orig;
	TEEC_SharedMemory input_sm;
	TEEC_SharedMemory output_sm;

	input_sm.size = sizeof(ms_inc_value_t);
	input_sm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
	output_sm.size = 10240;
	output_sm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
	status = TEEC_AllocateSharedMemory(&ctx, &input_sm);
	if (status != TEEC_SUCCESS) 	{
		return status;
	}
	status = TEEC_AllocateSharedMemory(&ctx, &output_sm);
	if (status != TEEC_SUCCESS) 	{
		return status;
	}


	ms_inc_value_t* ms = SGX_CAST(ms_inc_value_t*, input_sm.buffer);;
	

	ms->ms_a = a;
	
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_WHOLE, TEEC_MEMREF_WHOLE, TEEC_NONE, TEEC_NONE);
	op.params[0].memref.parent = &input_sm;
	op.params[0].memref.offset = 0;
	op.params[0].memref.size = sizeof(ms_inc_value_t);
	op.params[1].memref.parent = &output_sm;
	op.params[1].memref.offset = 0;
	op.params[1].memref.size = 10240;

	ocall_add(output_sm.buffer, ocall_table_hello_world.table);
	status = TEEC_InvokeCommand(&sess, 0, &op, &ret_orig);
	ocall_del(output_sm.buffer);
	if (status == TEEC_SUCCESS && retval) {
		*retval = ms->ms_retval;
	}
	return status;
}

TEEC_Result do_string(tee_id_t eid, char* buffer, int len)
{
	TEEC_Result status;
	TEEC_Operation op;
	uint32_t ret_orig;
	TEEC_SharedMemory input_sm;
	TEEC_SharedMemory output_sm;

	input_sm.size = sizeof(ms_do_string_t) + len;
	input_sm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
	output_sm.size = 10240;
	output_sm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
	status = TEEC_AllocateSharedMemory(&ctx, &input_sm);
	if (status != TEEC_SUCCESS) 	{
		return status;
	}
	status = TEEC_AllocateSharedMemory(&ctx, &output_sm);
	if (status != TEEC_SUCCESS) 	{
		return status;
	}


	ms_do_string_t* ms = SGX_CAST(ms_do_string_t*, input_sm.buffer);;
	
	memcpy(input_sm.buffer + sizeof(ms_do_string_t), buffer, len);

	ms->ms_buffer = buffer;
	ms->ms_len = len;
	
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_WHOLE, TEEC_MEMREF_WHOLE, TEEC_NONE, TEEC_NONE);
	op.params[0].memref.parent = &input_sm;
	op.params[0].memref.offset = 0;
	op.params[0].memref.size = sizeof(ms_do_string_t) + len;
	op.params[1].memref.parent = &output_sm;
	op.params[1].memref.offset = 0;
	op.params[1].memref.size = 10240;

	ocall_add(output_sm.buffer, ocall_table_hello_world.table);
	status = TEEC_InvokeCommand(&sess, 1, &op, &ret_orig);
	ocall_del(output_sm.buffer);
	if (status == TEEC_SUCCESS) {
		memcpy(buffer, input_sm.buffer + sizeof(ms_do_string_t), len);
	}
	return status;
}

