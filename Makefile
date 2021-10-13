CC := gcc
CFLAGS := -Wall
CFLAGS += -g3
CFLAGS += -static
ODIR := .OBJ
BF_DEPS := BF.h
HP_DEPS := HP.h
HT_DEPS := HT.h
SHT_DEPS := SHT.h
HP_SOURCE := $(ODIR)/HP.o $(ODIR)/HP_main.o
HT_SOURCE := $(ODIR)/HT.o $(ODIR)/HT_main.o
SHT_SOURCE := $(ODIR)/SHT.o $(ODIR)/SHT_main.o
OBJECT_DEPS := BF_64.a
all: HPprog HTprog SHTprog
$(ODIR):
	@mkdir $(ODIR)
$(ODIR)/HP.o: $(ODIR) $(HP_DEPS) $(BF_DEPS)
	$(CC) -c $(CFLAGS) HP.c -o $(ODIR)/HP.o

$(ODIR)/HT.o: $(ODIR) $(HT_DEPS) $(BF_DEPS)
	$(CC) -c $(CFLAGS) HT.c -o $(ODIR)/HT.o

$(ODIR)/HP_main.o: $(ODIR) $(HP_DEPS) $(BF_DEPS)
	$(CC) -c $(CFLAGS) HP_main.c -o $(ODIR)/HP_main.o

$(ODIR)/HT_main.o: $(ODIR) $(HT_DEPS) $(BF_DEPS)
	$(CC) -c $(CFLAGS) HT_main.c -o $(ODIR)/HT_main.o

HPprog: $(ODIR) $(HP_DEPS) $(BF_DEPS) $(OBJECT_DEPS) $(HP_SOURCE)
	$(CC)  $(CFLAGS) $(HP_SOURCE) $(OBJECT_DEPS) -o HPprog

HTprog: $(ODIR) $(HT_DEPS) $(BF_DEPS) $(OBJECT_DEPS) $(HT_SOURCE)
	$(CC)  $(CFLAGS) $(HT_SOURCE) $(OBJECT_DEPS) -o HTprog

$(ODIR)/SHT.o: $(ODIR) $(SHT_DEPS) $(HT_DEPS) $(BF_DEPS)
	$(CC) -c $(CFLAGS) SHT.c -o $(ODIR)/SHT.o

$(ODIR)/SHT_main.o: $(ODIR) $(SHT_DEPS) $(HT_DEPS) $(BF_DEPS)
	$(CC) -c $(CFLAGS) SHT_main.c -o $(ODIR)/SHT_main.o\

SHTprog: $(ODIR) $(SHT_DEPS) $(HT_DEPS) $(BF_DEPS) $(OBJECT_DEPS) $(SHT_SOURCE) $(HT_SOURCE)
	$(CC)  $(CFLAGS) $(SHT_SOURCE) $(OBJECT_DEPS) $(ODIR)/HT.o -o SHTprog

clean:
	rm -rf .OBJ HPprog HTprog test_hp test_ht stest_ht
