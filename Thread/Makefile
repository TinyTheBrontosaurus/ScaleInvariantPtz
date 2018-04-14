include ../shared/MakefileOptions
OUTDIR=./
                                                                  
TARGET=./tester
                  
                                                              
SRCS=Thread.cpp Barrier.cpp Semaphore.cpp ThreadTester.cpp
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

ODOBJS=$(addprefix $(OUTDIR)/, $(OBJS))
               
$(OUTDIR)/%.o: %.cpp 
	$(XCC) $(INC) -c $(CPPFLAGS) $(WXFLGS) -o $@ $<  
                                                       
all: $(TARGET)                           
                                                                                  
$(TARGET): $(ODOBJS) 
	$(XCC) -o $@ $(ODOBJS) $(WXLIBS) $(LINKFLAGS)
	cp *.o ../shared
	cp *.h ../shared
	rm -f ../shared/ThreadTester.o

clean: 
	rm -f $(OUTDIR)/*.o
	rm -f ../shared/Thread*.o
	

	
                                                                               

