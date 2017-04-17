
ifeq ($(shell uname),Linux)
DD=-DUSE_LIBUSB=1
LL=-lusb
else
DD=-DUSE_WIN32=1
LL=-lsetupapi -lhid
endif

teensy_loader_cli : teensy_loader_cli.c
	gcc $(DD) teensy_loader_cli.c $(LL) -o teensy_loader_cli


clean:
	rm -f teensy_loader_cli
