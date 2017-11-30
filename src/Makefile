ifneq ($(KERNELRELEASE),)
obj-m := bbb-gpio-2.o
else
KDIR := /home/galadriel/Desktop/BBB_3_8/kernel/kernel
all:
	$(MAKE) -C $(KDIR) M=$$PWD modules
endif
