# Software used
GCCPATH     := /opt/ti/ccsv8/tools/compiler/msp430-gcc-7.3.2.154_linux64
COMPILER    := $(GCCPATH)/bin/msp430-elf-gcc
ARCHIVER    := $(GCCPATH)/bin/msp430-elf-ar
ASSEMBLER   := $(GCCPATH)/bin/msp430-elf-as
LINKER      := $(GCCPATH)/bin/msp430-elf-ld
OBJCOPY     := $(GCCPATH)/bin/msp430-elf-objcopy
GDB		    := $(GCCPATH)/bin/msp430-elf-gdb
MSPDEBUG     := /usr/bin/mspdebug
FIND        := /usr/bin/find
HEX2TXT     := $(GCCPATH)/bin/hex2txt

# Directories: Source, Includes, Objects, Binary
SRCDIR      := src
INCDIR      := inc
OBJDIR      := obj
BINDIR      := bin
ASMEXT      := S
SRCEXT      := c
OBJEXT      := o

# Device used
DEVICE      := msp430f5529

# Compiler Flags (Optimization (-On), Debug info (-g))
CFLAGS      := -I $(INCDIR)          \
               -I $(GCCPATH)\include \
               -mmcu=$(DEVICE)       \
               -O2                   \
               -g

# Assembler Flags
AFLAGS      := -D_GNU_ASSEMBLER_ -Wall -Os -g \
               -T $(DEVICE).ld                \
               -mmcu=$(DEVICE)                \
               -fdata-sections                \
               -ffunction-sections            \
               -nostdlib                      \
               -I $(INCDIR)                   \
               -I $(GCCPATH)\include          \
               -L $(GCCPATH)\include          \


# Linker Flags
LFLAGS		:= -L $(GCCPATH)\include -T $(GCCPATH)\include\$(DEVICE).ld

#---------------------------------------------------------------------------------

# Automatically get all source files
ASMFILES    := $(shell $(FIND) $(SRCDIR) -type f -name *.$(ASMEXT))
CFILES      := $(shell $(FIND) $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(CFILES:.$(SRCEXT)=.$(OBJEXT))) \
               $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(ASMFILES:.$(ASMEXT)=.s))

# Link
$(BINDIR)\$(DEVICE).elf: $(BINDIR) $(OBJECTS)
#	$(ARCHIVER) r $(BINDIR)/$(DEVICE) $(OBJECTS) $(LIB)
	$(LINKER) $(LFLAGS) $(OBJECTS) -o $(BINDIR)\$(DEVICE).elf

# Compile
$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)\%.$(SRCEXT) $(OBJDIR)
	$(COMPILER) -c -o $@ $(CFLAGS) $<

# Assemble
$(OBJDIR)/%.s: $(SRCDIR)\%.$(ASMEXT) $(OBJDIR)
	$(COMPILER) -c -o $@ $(AFLAGS) $<

# Make Directories
$(OBJDIR):
	mkdir $(OBJDIR)

$(BINDIR):
	mkdir $(BINDIR)

# Flash
flash: $(BINDIR)\$(DEVICE).hex
	$(MSPDEBUG) -w $(BINDIR)\$(DEVICE).hex -v -g -z [VCC]

debug: $(BINDIR)\$(DEVICE).elf
	$(GDB) $(BINDIR)\$(DEVICE)

# Clean project
clean:
	@-rmdir /s /q $(BINDIR)
	@-rmdir /s /q $(OBJDIR)

#Non-File Targets
.PHONY: all clean flash
