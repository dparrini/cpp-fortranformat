CXX=g++
CXXFLAGS=-std=gnu++11 -Wall -g
INC=-I./include

OUTDIR=bin
BINTARGET=example

DEPS=include/fortranformat.hpp

OBJDIR=obj
CPPOBJ=$(OBJDIR)/fortranformat.o
EXAMPLE_OBJS=$(CPPOBJ) $(OBJDIR)/example.o
TEST_OBJS=$(CPPOBJ) $(OBJDIR)/test.o


# module and example
$(OBJDIR)/%.o: include/%.cpp $(DEPS)
	mkdir -p $(OBJDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(OPTIONS) $(INC)


$(OBJDIR)/%.o: tests/%.cpp $(DEPS)
	mkdir -p $(OBJDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(OPTIONS) $(INC)


$(BINTARGET): $(EXAMPLE_OBJS)
	mkdir -p $(OUTDIR)
	$(CXX) -o $(OUTDIR)/$@.exe $^ $(CXXFLAGS)


all: $(BINTARGET)


# CPP tests
test: $(TEST_OBJS)
	mkdir -p $(OUTDIR)
	$(CXX) -o $(OUTDIR)/$@.exe $^ $(CXXFLAGS) -DDEBUG


.PHONY : clean


clean:
	rm -f $(OUTDIR)/test.exe
	rm -f $(OUTDIR)/testf.exe
	rm -f $(OUTDIR)/$(BINTARGET).exe
	rm -f $(OBJDIR)/*.o
	rm -f $(OBJDIR)/*.of