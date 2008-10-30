#----------------------------------------------------------------------------
# GBA
#----------------------------------------------------------------------------
MY_INCL_PATH    +=\
                -I/D/dev/env/devkitPSX/include \

MY_LIBS_PATH    +=\
                -L/D/dev/env/devkitPSX/lib \

#----------------------------------------------------------------------------
CROSS_COMPILE   =mips
CFLAGS          +=\
                -Xlinker \
                -mpsx \
                -DPS1 \

CXXFLAGS        +=\
                -Xlinker \
                -mpsx \
                -DPS1 \

LINK_FLAGS      +=\
                -Xlinker \
                -mpsx \
                -Wl,-Map,$@.map \

MY_LIBS         +=\
                -lps \
                /D/dev/env/devkitPSX/lib/htsprint.o

#----------------------------------------------------------------------------
# Create .elf file from MY_OBJS + MY_LIBS
$(MY_ELF): $(MY_OBJS) $(MY_LIBS_DEP)
	@echo [CC ] $@
	$(CXX) $(LINK_FLAGS) $(MY_OBJS) $(MY_LIBS_PATH) $(MY_LIBS) -o $@
