#------------------------------------------------------------------------------
# Build Configuration
#------------------------------------------------------------------------------
# Update these variables according to your requirements.

# tools
CC = c99
LD = ${CC}

# completed flags
INCS      = -Isource/ -Itests/
CPPFLAGS  = -D_XOPEN_SOURCE=700
CFLAGS   += ${INCS} ${CPPFLAGS}
LDFLAGS  += ${LIBS}
ARFLAGS   = rcs

#------------------------------------------------------------------------------
# Build Targets and Rules
#------------------------------------------------------------------------------
BIN  = sclpl
OBJS = source/main.o    \
       source/gc.o      \
       source/vec.o     \
       source/pprint.o  \
       source/parser.o  \
       source/lexer.o   \
       source/ast.o

TESTBIN  = testsclpl
TESTOBJS = tests/atf.o        \
           tests/sclpl/main.o

.PHONY: all tests specs
all: sclpl tests specs

lib${BIN}.a: ${OBJS}
	${AR} ${ARFLAGS} $@ $^

${BIN}: lib${BIN}.a
	${LD} ${LDFLAGS} -o $@ $^

#${TESTBIN}: ${TESTOBJS}
#	${LD} ${LDFLAGS} -o $@ $^

#tests: ${TESTBIN}
#	./$<

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
