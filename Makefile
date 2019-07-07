CC=gcc

OUT=bin
OUTBIN=$(OUT)/ntp_client
ODIR=obj
SDIR=src
CDIR=config

_OBJS=main.o ntp_client.o json_proc.o display_module.o
OBJS=$(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: $(SDIR)/%.c 
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUTBIN): $(OBJS)
	$(CC) -pthread -o $(OUTBIN) $(OBJS)
	cp $(CDIR)/config.json $(OUT)/

.PHONY: clean

clean:
	- rm -f $(ODIR)/*.o
	- rm -f $(OUT)/*