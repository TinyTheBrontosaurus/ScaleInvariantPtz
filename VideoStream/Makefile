include ../shared/MakefileOptions
OUTDIR=.
                                                                  
TARGET=./VideoStreamTester

                                        
SRCS=VideoStream.cpp VideoStreamTester.cpp
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

ODOBJS=$(addprefix $(OUTDIR)/, $(OBJS)) ../shared/*.o
               
$(OUTDIR)/%.o: %.cpp 
	$(XCC) $(INC) -c $(CPPFLAGS) $(WXFLGS) -o $@ $<  
	                                                       
all: $(TARGET)                           
                                                                                  
$(TARGET): $(ODOBJS) 
	$(XCC) -o $@ $(ODOBJS) $(WXLIBS) $(LINKFLAGS)
	cp *.o ../shared
	cp *.h ../shared
	rm -f ../shared/VideoStreamTester.o

	
clean: 
	rm -f $(OUTDIR)/*.o
	rm -f ../shared/VideoStream*.o
