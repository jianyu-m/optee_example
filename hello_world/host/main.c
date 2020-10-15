/*
 * Copyright (c) 2016, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <stdio.h>
#include <string.h>

#include "teec_enclave.h"
#include "hello_world_u.h"

void do_ocall_string (char* buffer, int len) {
	buffer[1] = 'g';
}

int do_inc() {
	return 5;
}

int main(void)
{
	int token = 0;
	int updated = 0;
	tee_id_t eid;

	if (sgx_create_enclave("", 1, &token, &updated, &eid, NULL) != TEEC_SUCCESS) {
		printf("error\n");
		return -1;
	}
	
	int value = 0;
	int ret;
	if (ret = inc_value(eid, &value, 20) != TEEC_SUCCESS) {
		printf("error in ecall\n");
		return -1;
	}

	printf("return from inc_value %d\n", value);

	char hello[100];
	memcpy(hello, "hello world\n", strlen("hello world\n") + 1);

	if (ret = do_string(eid, hello, strlen(hello) + 1) != TEEC_SUCCESS) {
		printf("error in ecall\n");
		return -1;
	}

	printf("return %s\n", hello);

	sgx_close_enclave(eid);

	return 0;
}
