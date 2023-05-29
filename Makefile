COMPILER=gcc
RM=rm
SRC_EXTENSION=c
OBJ_EXTENSION=o
RUNNER=

INCDIR=include
SRCDIR=src
OBJDIR=obj
BUILDDIR=build
BINDIR=bin
SRCS = $(wildcard $(SRCDIR)/*.$(SRC_EXTENSION))
OBJS = $(patsubst $(SRCDIR)/%.$(SRC_EXTENSION), $(OBJDIR)/%.$(OBJ_EXTENSION), $(SRCS))

CFLAGS=-I"./$(INCDIR)"
LDFLAGS=-fPIC -lm

EXECNAME=main
ARGS=

run: build
	@echo RUNNING $(RUNNER) ./$(BINDIR)/$(EXECNAME) $(ARGS)
	@echo ================
	@$(RUNNER) ./$(BINDIR)/$(EXECNAME) $(ARGS)
.PHONY: run

build: $(OBJS)
	@$(COMPILER) $^ -o $(BINDIR)/$(EXECNAME) $(LDFLAGS)
	@echo COMPILE $<
.PHONY: build

clean:
	@$(RM) -f $(OBJS)
	@echo RM $(OBJS)
.PHONY: clean

$(OBJDIR)/%.$(OBJ_EXTENSION): $(SRCDIR)/%.$(SRC_EXTENSION)
	@$(COMPILER) $(CFLAGS) -c $< -o $@
	@echo COMPILE $<

init:
	@echo Initializing Folders
	@mkdir $(INCDIR)
	@mkdir $(SRCDIR)
	@mkdir $(OBJDIR)
	@mkdir $(BUILDDIR)
	@mkdir $(BINDIR)
	@echo Initialization Completed
.PHONY: init

remove: check_remove
	@$(RM) -rf $(INCDIR)
	@$(RM) -rf $(SRCDIR)
	@$(RM) -rf $(OBJDIR)
	@$(RM) -rf $(BUILDDIR)	
	@$(RM) -rf $(BINDIR)
.PHONY: remove


check_remove:
	@echo Removing folders
	@echo -n "Are you sure? [y/N] " && read ans && [ $${ans:-N} = y ]

.PHONY: remove check_remove
