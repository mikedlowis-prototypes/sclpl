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
CFLAGS   += ${INCS} ${CPPFLAGS}
LDFLAGS  += ${LIBS}

#------------------------------------------------------------------------------
# Build Targets and Rules
#------------------------------------------------------------------------------
SRCS = source/main.c    \
       source/grammar.c \
       source/lexer.c   \
       source/parser.c  \
       source/opts.c    \
       source/pprint.c  \
       source/gc.c      \
       source/vec.c     \
       source/ast.c
OBJS = ${SRCS:.c=.o}

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
	@rm -f sclpl ${OBJS}

.PHONY: all options test

