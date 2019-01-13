CXX=g++
CXXFLAGS=-std=c++11 -Wall -g
FC=gfortran

OUTDIR=bin
BINTARGET=test
OPTIONS= -DDEBUG

DEPS=fortranformat.hpp

OBJDIR=obj
CPPOBJ=$(OBJDIR)/fortranformat.o \
	   $(OBJDIR)/test.o

FOBJ=$(OBJDIR)/test.of

$(OBJDIR)/%.o: %.cpp $(DEPS)
	mkdir -p $(OBJDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(OPTIONS)

$(BINTARGET): $(CPPOBJ)
	mkdir -p $(OUTDIR)
	$(CXX) -o $(OUTDIR)/$@.exe $^ $(CXXFLAGS) $(OPTIONS)

all: $(BINTARGET)


$(OBJDIR)/%.of: %.f90
	mkdir -p $(OBJDIR)
	$(FC) -c -o $@ $< $(FFLAGS) $(FOPTIONS)

testf : $(FOBJ)
	mkdir -p $(OUTDIR)
	$(FC) -o $(OUTDIR)/$@.exe $^ $(FFLAGS) $(FOPTIONS)


.PHONY : clean

clean:
	rm $(OUTDIR)/$(BINTARGET).exe
	rm $(OBJDIR)/*.o