AS=ca65 -I ../includes
LD=ld65

BUILD=./build

OBJS=tx-rep uartlib
OBJS2=$(addprefix $(BUILD)/, $(addsuffix .o, $(OBJS)))
DEPS=
INCS=$(wildcard ../includes/*.inc)
HOSTFS=~/hostfs/beebkeyjam

TARGET=tx-rep

X:=$(shell mkdir -p $(BUILD))


all::	$(addprefix $(BUILD)/, $(addsuffix .bin, $(TARGET)))

hostfs:: all
	$(shell mkdir -p $(HOSTFS))
	cp $(BUILD)/$(TARGET).bin* $(HOSTFS)

$(BUILD)/%.o: 	%.asm $(DEPS) $(INCS)
	$(AS) -o $@ -g -l $(basename $@).lst $<

$(BUILD)/%.o: 	../%.asm $(DEPS) $(INCS)
	$(AS) -o $@ -g -l $(basename $@).lst $<


$(BUILD)/$(TARGET).bin: $(OBJS2) $(TARGET).cfg
	$(LD) -vm -Ln $(basename $@).sy2 -m $(basename $@).map -o $@ -C $(TARGET).cfg $(OBJS2) $(LIBS)
	echo "$$.TXREP 1D00 1D00\r" > $(BUILD)/$(TARGET).bin.inf

clean::
	-rm -Rf build
