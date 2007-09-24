MAKEFLAGS	+=--no-print-directory -s


all:
	make -f Makefile all ARCH=mips TARGET=ps1

clean:
	make -f Makefile clean ARCH=mips TARGET=ps1
