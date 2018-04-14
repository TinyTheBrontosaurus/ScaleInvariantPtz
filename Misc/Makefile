include ../shared/MakefileOptions
OUTDIR=.
                                                                  
TARGET=./MiscTester
                                       
SRCS=LookupTable.cpp LookupTableTest.cpp PartialBox.cpp Matrix.cpp
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

ODOBJS=$(addprefix $(OUTDIR)/, $(OBJS)) 
               
$(OUTDIR)/%.o: %.cpp 
	$(XCC) $(INC) -c $(CPPFLAGS) $(WXFLGS) -o $@ $<  
	                                                       
all: $(TARGET)                           
                                                                                  
$(TARGET): $(ODOBJS) 
	$(XCC) -o $@ $(ODOBJS) $(WXLIBS) $(LINKFLAGS)
	cp *.o ../shared
	cp *.h ../shared
	rm -f ../shared/LookupTableTest.o
        
clean: 
	rm -f $(OUTDIR)/*.o
	rm -f $(TARGET)
	rm -f ../shared/LookupTable*.o
	rm -f ../shared/PartialBox*.o        
	rm -f ../shared/BoundedValue*.o
        
