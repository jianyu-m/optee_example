
#include "teec_enclave.h"
#include <stdio.h>
#include <pthread.h>
#include <malloc.h>

TEEC_Context ctx;
TEEC_Session sess;
TEEC_UUID uuid = { 0x8aaaf200, 0x2450, 0x11e4, \
		{ 0xab, 0xe2, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} }
;
tee_id_t eid;

tee_id_t global_eid = 0;

typedef TEEC_Result (*ocall_func_entry) (void* pms);

TEEC_Result sgx_create_enclave(char* name, int debug_flag, int* token, int *updated, tee_id_t *eid, void *unused) {
    
    uint32_t err_origin;

    TEEC_Result res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/*
	 * Open a session to the "hello world" TA, the TA will print "hello
	 * world!" in the log when the session is created.
	 */
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

    *eid = ++global_eid;

    return res;
}

void sgx_close_enclave(tee_id_t id) {
    TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);
}

typedef struct thread_data_t {
	char *buffer;
	void **ocall_table;
} thread_data;

void * thread_func(void * arg)
{
	thread_data *td = (thread_data*)arg;
    char* buffer = td->buffer;
	volatile char *status = buffer;
	int *ocall_idx_ptr = (int*)(buffer + sizeof(char));
	void **ocall_table = (void**)td->ocall_table;
	printf("ocall thread entered\n");
	while (*status != 111) {
		if (*status == 1) {
			// if there are 
			printf("ocall %d\n", *ocall_idx_ptr);
			ocall_func_entry entry = (ocall_func_entry) ocall_table[*ocall_idx_ptr];
			TEEC_Result r = (*entry)(buffer + sizeof(char) + sizeof(int));
			*status = (char)r;
		}
	}
	printf("ocall thread exited\n");
	return NULL;
}

void ocall_add(char* ocall_buffer, void** ocall_table) {
	thread_data *td = (thread_data*)malloc(sizeof(thread_data));
	td->buffer = ocall_buffer;
	td->ocall_table = ocall_table;
	pthread_t thd_idx;
	// Create a thread that will function threadFunc()
	int err = pthread_create(&thd_idx, NULL, &thread_func, td);
	if (err) {
		printf("error in creating ocall thread\n");
	}
}

void ocall_del(char* ocall_buffer) {
	volatile char *status = ocall_buffer;
	// stop the ocall thread
	*status = 111;
}