
KERNELDIR := /home/hjy/imx6ull_kernel
CURRENT_PATH := $(shell pwd)
CROSS_COMPILE := arm-linux-gnueabihf-
ARCH := arm

obj-m := dht11.o

all: build

build:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules

clean:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
