obj-m += kb-driver.o
INCLUDES_DIR := /includes
HEADERS := $(wildcard $(INCLUDES_DIR)/*.h)

EXTRA_CFLAGS += -I/$(PWD)/$(INCLUDES_DIR)

.PHONY: build clean load unload reload

build:
	@echo "Building driver..."
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 
	
clean:
	@echo "Cleaning driver..."
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


load:
	sudo insmod $(obj-m:.o=.ko)

unload:
	sudo rmmod $(obj-m:.o=.ko)

reload:
	sudo rmmod $(obj-m:.o=.ko)
	sudo insmod $(obj-m:.o=.ko)


