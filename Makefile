-include ../../../common.mk
CHARMC?=../../../../bin/charmc

OPTS=-g

OBJS = hello.o

all: hello

hello: $(OBJS)
	$(CHARMC) $(OPTS) -language charm++ -o hello $(OBJS) -module CommonLBs

hello.decl.h: hello.ci
	$(CHARMC) $(OPTS) hello.ci

clean:
	rm -f *.decl.h *.def.h conv-host *.o hello charmrun charmrun.exe hello.exe hello.pdb hello.ilk

hello.o: hello.C hello.decl.h
	$(CHARMC) $(OPTS)  -c hello.C

test: all
	$(call run, ./hello +p4 10 )
