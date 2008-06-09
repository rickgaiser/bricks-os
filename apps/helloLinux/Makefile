all: hello

clean:
	rm -f hello
	rm -f *.o

floppy:
	sudo mount /dev/fd0
	sudo cp hello /media/floppy/boot/kernel/hello
	sudo umount /dev/fd0

floppy-img:
	sudo mount ../bricks-os/bin/pc/floppy.img bin/pc/floppy -o loop
	sudo cp hello ../bricks-os/bin/pc/floppy/boot/kernel/hello
	sudo umount ../bricks-os/bin/pc/floppy

hello: hello.o
	ld -s -o hello hello.o

hello.o: hello.s
	gcc -I/usr/include/ -c hello.s
