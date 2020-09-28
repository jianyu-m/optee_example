
#include "teec_enclave.h"

TEEC_Context ctx;
TEEC_Session sess;
TEEC_UUID uuid = { 0x8aaaf200, 0x2450, 0x11e4, \
		{ 0xab, 0xe2, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} }
;
tee_id_t eid;

tee_id_t global_eid = 0;

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

void ocall_add(char* ocall_buffer, void* ocall_table) {
	
}

void ocall_del(char* ocall_buffer) {

}