#
# Makefile for msp430
#
# 'make' builds everything
# 'make clean' deletes everything except source files and Makefile
# You need to set TARGET, MCU and SOURCES for your project.
# TARGET is the name of the executable file to be produced 
# $(TARGET).elf $(TARGET).hex and $(TARGET).txt nad $(TARGET).map are all generated.
# The TXT file is used for BSL loading, the ELF can be used for JTAG use
# 
TARGET     = main
MCU        = msp430g2231
# List all the source files here
# eg if you have a source file foo.c then list it here
SOURCES = main.c

# GCC paths
GCC_BASE_DIR = /usr/local/msp430-uniarch-20110716
# GCC variant...for quick switching between mspgcc and TI/Redhat's msp430-gcc bin names
# Set GCC_VAR = elf- for the msp430-gcc binaries and leave blank for mspgcc
GCC_VAR = 

GCC_DIR = $(GCC_BASE_DIR)/bin
INCLUDE_PATH = -I $(GCC_BASE_DIR)/include
LIBRARY_PATH = -L $(GCC_BASE_DIR)/include
# Additional defines
DEFINES = 

# Add or subtract whatever MSPGCC flags you want. There are plenty more
#######################################################################################
CFLAGS   = -mmcu=$(MCU) -g -Os -Wall -Wunused $(DEFINES) $(INCLUDE_PATH)   
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=$(MCU) $(LIBRARY_PATH)
########################################################################################
CC       = $(GCC_DIR)/msp430-$(GCC_VAR)gcc
LD       = $(GCC_DIR)/msp430-$(GCC_VAR)ld
AR       = $(GCC_DIR)/msp430-$(GCC_VAR)ar
AS       = $(GCC_DIR)/msp430-$(GCC_VAR)gcc
GASP     = $(GCC_DIR)/msp430-$(GCC_VAR)gasp
NM       = $(GCC_DIR)/msp430-$(GCC_VAR)nm
OBJCOPY  = $(GCC_DIR)/msp430-$(GCC_VAR)objcopy
OBJDUMP  = $(GCC_DIR)/msp430-$(GCC_VAR)objdump
RANLIB   = $(GCC_DIR)/msp430-$(GCC_VAR)ranlib
STRIP    = $(GCC_DIR)/msp430-$(GCC_VAR)strip
SIZE     = $(GCC_DIR)/msp430-$(GCC_VAR)size
READELF  = $(GCC_DIR)/msp430-$(GCC_VAR)readelf
MSPDEBUG = mspdebug
CP       = cp -p
RM       = rm -f
MV       = mv
########################################################################################
# the file which will include dependencies
DEPEND = $(SOURCES:.c=.d)
# all the object files
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET).elf $(TARGET).hex $(TARGET).lst

$(TARGET).elf: $(OBJECTS)
	echo "Linking $@"
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	echo
	echo ">>>> Size of Firmware <<<<"
	$(SIZE) $(TARGET).elf
	echo

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

%.o: %.c
	echo "Compiling $<"
	$(CC) -c $(CFLAGS) -o $@ $<

# rule for making assembler source listing, to see the code
%.lst: %.elf
	$(OBJDUMP) -d $< >$@

# include the dependencies unless we're going to clean, then forget about them.
ifneq ($(MAKECMDGOALS), clean)
-include $(DEPEND)
endif

# dependencies file
# includes also considered, since some of these are our own
# (otherwise use -MM instead of -M)
%.d: %.c
	echo "Generating dependencies $@ from $<"
	$(CC) -M ${CFLAGS} $< >$@

install: $(TARGET).elf
	$(MSPDEBUG) rf2500 "prog $(TARGET).elf"

.SILENT:
.PHONY:	clean
clean:
	rm -rf *.elf *.lst *.d *.o *.hex *.lst *.map
