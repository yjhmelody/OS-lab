obj-m:=test_hello2.o
KDIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

# 其中：KDIR 是内核源码目录，该目录通过当前运行内核使用的模块目录中的 build 符号链接指定,
# 或者直接给出源码目录也可以，如：KDIR:=/usr/src/linux-headers-4.4.0-36-generic.

default:
	make -C $(KDIR) M=$(PWD) modules # -C 指定内核源码目录，M 指定模块源码目录
clean:
	make -C $(KDIR) M=$(PWD) clean   