include ../shared/MakefileOptions
OUTDIR=.
                                                                  
TARGET=./tester
                                                         
SRCS=SISingleCam.cpp SISingleCamOptions.cpp SICameraComms.cpp SIKalman.cpp SITordoff.cpp SISingleCamTester.cpp SIDualCam.cpp SIDualCamOptions.cpp
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

ODOBJS=$(addprefix $(OUTDIR)/, $(OBJS)) ../shared/*.o
               
$(OUTDIR)/%.o: %.cpp 
	$(XCC) $(INC) -c $(CPPFLAGS) $(WXFLGS) -o $@ $<  
	                                                       
all: $(TARGET)                           
                                                                                  
$(TARGET): $(ODOBJS) 
	$(XCC) -o $@ $(ODOBJS) $(WXLIBS) $(LINKFLAGS)
	cp *.o ../shared
	cp *.h ../shared
	rm -f ../shared/SISingleCamTester.o
	
clean: 
	rm -f $(OUTDIR)/*.o
	rm -f ../shared/SISingleCam*.o


	
                                                                               

