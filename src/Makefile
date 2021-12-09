CC:=gcc
BIN:=openvpn-manager
LDFLAGS:=-largp -lubox -lubus -luci -ljson-c
SRC:= $(shell find $(SOURCEDIR) -name '*.c')
OBJS:=$(SRC:.c=.o)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(BIN) $(OBJS)
