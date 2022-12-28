AS=ca65 -I ../includes
LD=ld65

BUILD=./build

OBJS=test-uart
OBJS2=$(addprefix $(BUILD)/, $(addsuffix .o, $(OBJS)))
DEPS=
INCS=$(wildcard ../includes/*.inc)
HOSTFS=~/hostfs/beebkeyjam

TARGETS=test-uart

X:=$(shell mkdir -p $(BUILD))


all::	$(addprefix $(BUILD)/, $(addsuffix .bin, $(TARGETS)))

hostfs:: all
	$(shell mkdir -p $(HOSTFS))
	cp $(BUILD)/test-uart.bin* $(HOSTFS)

$(BUILD)/%.o: 	%.asm $(DEPS) $(INCS)
	$(AS) -o $@ -g -l $(basename $@).lst $<

$(BUILD)/test-uart.bin: $(OBJS2) test-uart.cfg
	$(LD) -vm -Ln $(basename $@).sy2 -m $(basename $@).map -o $@ -C test-uart.cfg $(OBJS2) $(LIBS)
	echo "$$.TSTUART 1D00 1D00\r" > $(BUILD)/test-uart.bin.inf

clean::
	-rm -Rf build
