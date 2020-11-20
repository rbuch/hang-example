-include ../../../common.mk
CHARMC?=../../../../bin/charmc $(OPTS)

OBJS = hello.o

all: hello

hello: $(OBJS)
	$(CHARMC) -language charm++ -o hello $(OBJS) -module CommonLBs

hello.decl.h: hello.ci
	$(CHARMC)  hello.ci

clean:
	rm -f *.decl.h *.def.h conv-host *.o hello charmrun charmrun.exe hello.exe hello.pdb hello.ilk

hello.o: hello.C hello.decl.h
	$(CHARMC) -c hello.C

test: all
	$(call run, ./hello +p4 10 )
