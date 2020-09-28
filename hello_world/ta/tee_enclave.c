
#include "tee_enclave.h"

int tee_ocall(char* status) {
    volatile char* v_status = (volatile char*) status;
    *v_status = 1;
    while (*v_status != 1) {
        return (int)(*v_status);
    }
}

void ocall_add(char* ocall_buffer, void* ocall_table) {

}

void ocall_del(char* ocall_buffer) {

}