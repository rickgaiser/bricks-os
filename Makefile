DIRS		=\
		 stl \
		 libc \
		 arch \
		 kernel \
#		 tests \

LIBS		=\
		 arch \
		 stl \
		 c \
		 kernel \

ELF		=Bricks


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
