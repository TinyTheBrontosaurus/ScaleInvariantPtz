include ../shared/MakefileOptions
OUTDIR=.
                                                                  
TARGET=./SITrackerMenu
                                        
SRCS=Menu.cpp MenuTester.cpp
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

ODOBJS=$(addprefix $(OUTDIR)/, $(OBJS)) ../shared/*.o
               
$(OUTDIR)/%.o: %.cpp 
	$(XCC) $(INC) -c $(CPPFLAGS) $(WXFLGS) -o $@ $<  
	                                                       
all: $(TARGET)                           
                                                                                  
$(TARGET): $(ODOBJS) 
	$(XCC) -o $@ $(ODOBJS) $(WXLIBS) $(LINKFLAGS)
	cp *.o ../shared
	cp *.h ../shared
	rm -f ../shared/MenuTester.o
	cp $(TARGET) ../
	
clean: 
	rm -f $(OUTDIR)/*.o
	rm -f ../shared/Menu*.o
