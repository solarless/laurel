SOURCEDIR = src
BUILDDIR = build
TARGET = laurel

SOURCES = $(wildcard $(SOURCEDIR)/*.c)
OBJECTS = $(addprefix $(BUILDDIR)/, $(notdir $(SOURCES:.c=.o)))

CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CPU = -mcpu=cortex-m3

CFLAGS = $(CPU) -mthumb
CFLAGS += -std=gnu99 -fno-common -ffunction-sections -fdata-sections -ffile-prefix-map=src/=
CFLAGS += -Wall -Werror
CFLAGS += -Iinclude -Ilibopencm3/include
CFLAGS += -DSTM32F1

LDFLAGS = $(CPU) -nostartfiles -Llibopencm3/lib -specs=nano.specs
LDFLAGS += -Tstm32f103c8.ld -Wl,-Map=$(BUILDDIR)/$(TARGET).map,--cref -Wl,--gc-sections -flto
LDLIBS = -lc -lopencm3_stm32f1

.PHONY: all
all: $(BUILDDIR)/$(TARGET).elf $(BUILDDIR)/$(TARGET).bin $(BUILDDIR)/$(TARGET).hex

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/$(TARGET).elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(BUILDDIR)/$(TARGET).bin: $(BUILDDIR)/$(TARGET).elf
	$(OBJCOPY) -O binary -S $^ $@

$(BUILDDIR)/$(TARGET).hex: $(BUILDDIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex -S $^ $@

$(BUILDDIR):
	mkdir $@

.PHONY: clean
clean:
	rm -rf build
