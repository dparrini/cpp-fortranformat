CXX=g++
CXXFLAGS=-std=gnu++11 -Wall -g
FC=gfortran

OUTDIR=bin
BINTARGET=example

DEPS=fortranformat.hpp

OBJDIR=obj
CPPOBJ=$(OBJDIR)/fortranformat.o
EXAMPLE_OBJS=$(CPPOBJ) $(OBJDIR)/example.o
TEST_OBJS=$(CPPOBJ) $(OBJDIR)/test.o

FOBJ=$(OBJDIR)/test.of

# module and example
$(OBJDIR)/%.o: %.cpp $(DEPS)
	mkdir -p $(OBJDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(OPTIONS)

$(OBJDIR)/%.o: tests/%.cpp $(DEPS)
	mkdir -p $(OBJDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(OPTIONS)

$(BINTARGET): $(EXAMPLE_OBJS)
	mkdir -p $(OUTDIR)
	$(CXX) -o $(OUTDIR)/$@.exe $^ $(CXXFLAGS)

all: $(BINTARGET)


# CPP tests
test: $(TEST_OBJS)
	mkdir -p $(OUTDIR)
	$(CXX) -o $(OUTDIR)/$@.exe $^ $(CXXFLAGS) -DDEBUG

# fortran tests
$(OBJDIR)/%.of: tests/%.f90
	mkdir -p $(OBJDIR)
	$(FC) -c -o $@ $< $(FFLAGS) $(FOPTIONS)

testf : $(FOBJ)
	mkdir -p $(OUTDIR)
	$(FC) -o $(OUTDIR)/$@.exe $^ $(FFLAGS) $(FOPTIONS)


.PHONY : clean

clean:
	rm -f $(OUTDIR)/test.exe
	rm -f $(OUTDIR)/testf.exe
	rm -f $(OUTDIR)/$(BINTARGET).exe
	rm -f $(OBJDIR)/*.o
	rm -f $(OBJDIR)/*.of