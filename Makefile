ifeq ($(OS),Windows_NT)
DIRSEP = $(subst /,\\,/)
BASEDIR = $(subst /,\\,$(CURDIR))
BINEXT = .exe
MKDIR = mkdir
RMDIR = rmdir /s /q
else
DIRSEP = /
BASEDIR = $(CURDIR)
BINEXT =
MKDIR = mkdir -p
RMDIR = rm -r
endif

UTILDIR = $(BASEDIR)$(DIRSEP)util
UTILDIRS = $(UTILDIR)$(DIRSEP)
BINDIR = $(BASEDIR)$(DIRSEP)bin
BINDIRS = $(BINDIR)$(DIRSEP)

FLSHMSRCDIR = $(BASEDIR)$(DIRSEP)src
FLSHMC = $(FLSHMSRCDIR)$(DIRSEP)flshm.c
FLSHMI = -I$(FLSHMSRCDIR)

CFLAGS = -std=c99 -D_XOPEN_SOURCE=600 \
	-g -O0 -Wall -Wextra -Wno-unused-parameter -pedantic


all: flshmopenclose \
	flshmdump \
	flshmconnectionnamevalid \
	flshmconnectionlist \
	flshmconnectionadd \
	flshmconnectionremove \
	flshmmessagegenerateticks \
	flshmmessagetick \
	flshmmessageread \
	flshmmessagewrite \
	flshmmessageclear \
	flshmchatbot

clean:
	$(RMDIR) $(BINDIR)

$(BINDIR):
	$(MKDIR) $(BINDIR)

flshmopenclose: $(BINDIR)
	$(CC) $(CFLAGS) $(FLSHMI) -o$(BINDIRS)$@$(BINEXT) $(UTILDIRS)$@.c $(FLSHMC)

flshmdump: $(BINDIR)
	$(CC) $(CFLAGS) $(FLSHMI) -o$(BINDIRS)$@$(BINEXT) $(UTILDIRS)$@.c $(FLSHMC)

flshmconnectionnamevalid: $(BINDIR)
	$(CC) $(CFLAGS) $(FLSHMI) -o$(BINDIRS)$@$(BINEXT) $(UTILDIRS)$@.c $(FLSHMC)

flshmconnectionlist: $(BINDIR)
	$(CC) $(CFLAGS) $(FLSHMI) -o$(BINDIRS)$@$(BINEXT) $(UTILDIRS)$@.c $(FLSHMC)

flshmconnectionadd: $(BINDIR)
	$(CC) $(CFLAGS) $(FLSHMI) -o$(BINDIRS)$@$(BINEXT) $(UTILDIRS)$@.c $(FLSHMC)

flshmconnectionremove: $(BINDIR)
	$(CC) $(CFLAGS) $(FLSHMI) -o$(BINDIRS)$@$(BINEXT) $(UTILDIRS)$@.c $(FLSHMC)

flshmmessagegenerateticks: $(BINDIR)
	$(CC) $(CFLAGS) $(FLSHMI) -o$(BINDIRS)$@$(BINEXT) $(UTILDIRS)$@.c $(FLSHMC)

flshmmessagetick: $(BINDIR)
	$(CC) $(CFLAGS) $(FLSHMI) -o$(BINDIRS)$@$(BINEXT) $(UTILDIRS)$@.c $(FLSHMC)

flshmmessageread: $(BINDIR)
	$(CC) $(CFLAGS) $(FLSHMI) -o$(BINDIRS)$@$(BINEXT) $(UTILDIRS)$@.c $(FLSHMC)

flshmmessagewrite: $(BINDIR)
	$(CC) $(CFLAGS) $(FLSHMI) -o$(BINDIRS)$@$(BINEXT) $(UTILDIRS)$@.c $(FLSHMC)

flshmmessageclear: $(BINDIR)
	$(CC) $(CFLAGS) $(FLSHMI) -o$(BINDIRS)$@$(BINEXT) $(UTILDIRS)$@.c $(FLSHMC)

flshmchatbot: $(BINDIR)
	$(CC) $(CFLAGS) $(FLSHMI) -o$(BINDIRS)$@$(BINEXT) $(UTILDIRS)$@.c $(FLSHMC)
