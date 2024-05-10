obj-m		:= rootkit.o

KBUILD_DIR	:= /lib/modules/$(shell uname -r)/build

#KBUILD_EXTRA_SYMBOLS := /home/luis/Documents/kernel_puc/final-project/main/3.0_hiding_lkm/Module.symvers

default:
	$(MAKE) -C $(KBUILD_DIR) M=$(shell pwd)
clean:
	$(MAKE) -C $(KBUILD_DIR) M=$(shell pwd) clean