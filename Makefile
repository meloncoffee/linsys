KERNEL_DIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)
SRC_DIR  := $(PWD)/src/kernel
BUILD_DIR := $(PWD)/build

all:
	@mkdir -p $(BUILD_DIR)
	$(MAKE) -C $(KERNEL_DIR) M=$(SRC_DIR) modules
	@mv -f $(SRC_DIR)/*.ko $(BUILD_DIR)/

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(SRC_DIR) clean
	@rm -rf $(BUILD_DIR)
