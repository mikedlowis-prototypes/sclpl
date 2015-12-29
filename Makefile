#------------------------------------------------------------------------------
# Build Configuration
#------------------------------------------------------------------------------
# Update these variables according to your requirements.

# tools
CC = c99
LD = ${CC}
AR = ar

# completed flags
INCS      = -Isource/ -Itests/
CPPFLAGS  = -D_XOPEN_SOURCE=700
CFLAGS   += ${INCS} ${CPPFLAGS}
LDFLAGS  += ${LIBS}
ARFLAGS   = rcs

# Enable GCC/Clang debug symbols
#CFLAGS += -g

# Enable GCC coverage
#CFLAGS  += --coverage
#LDFLAGS += --coverage

#------------------------------------------------------------------------------
# Build Targets and Rules
#------------------------------------------------------------------------------
BIN  = sclpl
OBJS = source/main.o    \
       source/gc.o      \
       source/vec.o     \
       source/pprint.o  \
       source/lexer.o   \
       source/parser.o  \
       source/ast.o     \
       source/anf.o     \
       source/codegen.o

TESTBIN  = testsclpl
TESTOBJS = tests/atf.o        \
           tests/sclpl/main.o

all: options sclpl tests specs

options:
	@echo "Toolchain Configuration:"
	@echo "  CC       = ${CC}"
	@echo "  CFLAGS   = ${CFLAGS}"
	@echo "  LD       = ${LD}"
	@echo "  LDFLAGS  = ${LDFLAGS}"
	@echo "  AR       = ${AR}"
	@echo "  ARFLAGS  = ${ARFLAGS}"

lib${BIN}.a: ${OBJS}
	@echo AR $@ $*
	@${AR} ${ARFLAGS} $@ $^

${BIN}: lib${BIN}.a
	@echo LD $@
	@${LD} ${LDFLAGS} -o $@ $^

${TESTBIN}: ${TESTOBJS}
	@echo LD $@
	@${LD} ${LDFLAGS} -o $@ $^

tests: $(TESTBIN)
	@./$<

specs: $(BIN)
	@echo TEST $<
	@rspec --pattern 'spec/**{,/*/**}/*_spec.rb'

.c.o:
	@echo CC $<
	@${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f ${BIN} lib${BIN}.a
	@rm -f ${TESTBIN} ${TESTOBJS} ${TESTOBJS:.o=.gcda} ${TESTOBJS:.o=.gcno}
	@rm -f ${OBJS} ${OBJS:.o=.gcda} ${OBJS:.o=.gcno}

.PHONY: all options tests specs

