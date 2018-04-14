include ../shared/MakefileOptions
OUTDIR=.
                                                                  
TARGET=./MFSTester
                                       
SRCS=MeasureFinalSizeLogger.cpp MeasureFinalSizeLoggerTester.cpp
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

ODOBJS=$(addprefix $(OUTDIR)/, $(OBJS)) 
               
$(OUTDIR)/%.o: %.cpp 
	$(XCC) $(INC) -c $(CPPFLAGS) $(WXFLGS) -o $@ $<  
	                                                       
all: $(TARGET)                           
                                                                                  
$(TARGET): $(ODOBJS) 
	$(XCC) -o $@ $(ODOBJS) $(WXLIBS) $(LINKFLAGS)
	cp *.o ../shared
	cp *.h ../shared
	rm -f ../shared/MeasureFinalSizeLoggerTester.o
        
clean: 
	rm -f $(OUTDIR)/*.o
	rm -f $(TARGET)
	rm -f ../shared/MeasureFinalSizeLogger*.o

        
