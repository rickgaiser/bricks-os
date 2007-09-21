DIRS            =\
                kernel \
                libtace \
                srr \

LIBS            =\
                arch \
                kernel \
                tace \
                srr \

ELF             =Bricks


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
