#----------------------------------------------------------------------------
# GBA
#----------------------------------------------------------------------------
MY_INCL_PATH    +=\
                -I/D/dev/env/devkitPSX/include \

MY_LIBS_PATH    +=\
                -L/D/dev/env/devkitPSX/lib \

#----------------------------------------------------------------------------
CROSS_COMPILE   =mips
CFLAGS          +=-Xlinker -mpsx
CXXFLAGS        +=-Xlinker -mpsx
LFLAGS          +=-Xlinker -mpsx $(MY_LIBS_PATH) -lps /D/dev/env/devkitPSX/lib/htsprint.o

#----------------------------------------------------------------------------
# Create .elf file from MY_OBJS + MY_LIBS
$(MY_ELF): $(MY_OBJS)
	@echo [CC ] $@
	$(CXX) $(LFLAGS) $^ $(MY_LIBS) -o $@
