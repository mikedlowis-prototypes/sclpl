#------------------------------------------------------------------------------
# Build Configuration
#------------------------------------------------------------------------------
# Update these variables according to your requirements.

# tools
CC = cc
LD = ${CC}

# completed flags
INCS      = -Isource/
CPPFLAGS  =
CFLAGS   += -O0 -g ${INCS} ${CPPFLAGS}
LDFLAGS  += ${LIBS}
ARFLAGS   = rcs

#------------------------------------------------------------------------------
# Build Targets and Rules
#------------------------------------------------------------------------------
BIN  = sclpl
OBJS = source/main.o    \
       source/vec.o     \
       source/pprint.o  \
       source/parser.o  \
       source/lexer.o   \
       source/ast.o     \
       source/types.o   \
       source/syms.o    \
       source/codegen.o

.PHONY: all tests specs
all: sclpl tests specs

lib${BIN}.a: ${OBJS}
	${AR} ${ARFLAGS} $@ $^

${BIN}: lib${BIN}.a
	${LD} ${LDFLAGS} -o $@ $^

specs: $(BIN)
	rspec --pattern 'spec/**{,/*/**}/*_spec.rb' --format documentation

.l.c:
	${LEX} -o $@ $<

.c.o:
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f ${BIN} lib${BIN}.a
	@rm -f ${TESTBIN} ${TESTOBJS} ${TESTOBJS:.o=.gcda} ${TESTOBJS:.o=.gcno}
	@rm -f ${OBJS} ${OBJS:.o=.gcda} ${OBJS:.o=.gcno} source/lexer.c
