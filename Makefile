CC=g++
OBJDIR=obj
EXECUTABLE=SIM
DEBUG=TRUE

ifeq ($(DEBUG), TRUE)
	DEBUGARG=-g
	OUTDIR=debug
else
	OUTDIR=release
endif

SOURCES := $(shell find ./ -name '*.cpp')
HEADERS := $(shell find ./ -name '*.h')
OBJFILES := $(patsubst ./%.cpp,$(OBJDIR)/%.o,$(SOURCES))

# Check headers and sources
$(OBJDIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(OBJDIR)
	$(CC) -c $(DEBUGARG) -o $@ $< $(CFLAGS)

$(EXECUTABLE): $(OBJFILES)
	@mkdir -p $(OUTDIR)
	$(CC) $(OBJFILES) -o $(OUTDIR)/$@

clean:
	rm -rf $(OUTDIR)/ $(OBJDIR)/