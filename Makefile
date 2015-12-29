#------------------------------------------------------------------------------
# Build Configuration
#------------------------------------------------------------------------------
# Update these variables according to your requirements.

# tools
CC = c99
LD = ${CC}

# flags
INCS      = -Isource/
CPPFLAGS  = -D_XOPEN_SOURCE=700
CFLAGS   += ${INCS} ${CPPFLAGS} -g --coverage
LDFLAGS  += --coverage ${LIBS}

#------------------------------------------------------------------------------
# Build Targets and Rules
#------------------------------------------------------------------------------
OBJS = source/main.o    \
       source/gc.o      \
       source/vec.o     \
       source/pprint.o  \
       source/lexer.o   \
       source/parser.o  \
       source/ast.o     \
       source/anf.o     \
       source/codegen.o

all: options sclpl test

options:
	@echo "Toolchain Configuration:"
	@echo "  CC       = ${CC}"
	@echo "  CFLAGS   = ${CFLAGS}"
	@echo "  LD       = ${LD}"
	@echo "  LDFLAGS  = ${LDFLAGS}"

sclpl: ${OBJS}
	@echo LD $@
	@${LD} ${LDFLAGS} -o $@ ${OBJS}

test: sclpl
	@echo TEST $<
	@rspec --pattern 'spec/**{,/*/**}/*_spec.rb'

.c.o:
	@echo CC $<
	@${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f sclpl ${OBJS} ${OBJS:.o=.gcda} ${OBJS:.o=.gcno}

.PHONY: all options test

