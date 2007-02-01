DIRS		=\
		 stl \
		 libc \
		 arch \
		 kernel \

LIBS		=\
		 arch \
		 stl \
		 c \
		 kernel \

ELF		=Bricks


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
