global-incdirs-y += include
srcs-y += hello_world_t.c
srcs-y += hello_world.c
srcs-y += tee_enclave.c

# To remove a certain compiler flag, add a line like this
#cflags-template_ta.c-y += -Wno-strict-prototypes
