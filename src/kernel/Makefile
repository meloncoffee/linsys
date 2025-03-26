DEBUG ?= 0
obj-m := $(KERNEL_MODULE_NAME).o

ccflags-y += -Wall -I$(src)/../../include -DLINSYS_DEFENDER
ifeq ($(DEBUG), 1)
    ccflags-y += -DDEBUG -g
endif
