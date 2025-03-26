DEBUG ?= 0
KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)
KERNEL_SRC_DIR  := $(PWD)/src/kernel
USER_SRC_DIR := $(PWD)/src/user
BUILD_DIR := $(PWD)/build

KERNEL_MODULE_NAME := linsys_defender
USER_MODULE_NAME := linsys_manager

all: kernel user

kernel:
	@mkdir -p $(BUILD_DIR)
	$(MAKE) -C $(KDIR) M=$(KERNEL_SRC_DIR) modules \
		KERNEL_MODULE_NAME=$(KERNEL_MODULE_NAME) \
		DEBUG=$(DEBUG)
	@cp -f $(KERNEL_SRC_DIR)/*.ko $(BUILD_DIR)/

user:
	@mkdir -p $(BUILD_DIR)
	$(MAKE) -C $(USER_SRC_DIR) \
		USER_MODULE_NAME=$(USER_MODULE_NAME) \
		DEBUG=$(DEBUG)
	@cp -f $(USER_SRC_DIR)/$(USER_MODULE_NAME) $(BUILD_DIR)/

clean:
	$(MAKE) -C $(KDIR) M=$(KERNEL_SRC_DIR) clean
	$(MAKE) -C $(USER_SRC_DIR) clean \
		USER_MODULE_NAME=$(USER_MODULE_NAME)
	@rm -rf $(BUILD_DIR)
