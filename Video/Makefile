include ../shared/MakefileOptions
OUTDIR=.
                                                                  
TARGET=$(OUTDIR)/exe
                                                                                      
SRCS=VideoSourceFrameGrabber.cpp VideoDestination.cpp VideoDestinationScreen.cpp VideoDestinationFile.cpp VideoSourceFile.cpp test.cpp V4l2Capture.cpp VideoDestinationScreenThread.cpp VideoDestinationFileRamDrive.cpp
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

ODOBJS=$(addprefix $(OUTDIR)/, $(OBJS)) ../shared/*.o
               
$(OUTDIR)/%.o: %.cpp 
	$(XCC) $(INC) -c $(CPPFLAGS) $(WXFLGS) -o $@ $<  
	                                                       
all: $(TARGET)                            
                                               
$(TARGET): $(ODOBJS) 
	$(XCC) -o $@ $(ODOBJS) $(WXLIBS) $(LINKFLAGS)
	cp Video*.h ../shared
	cp Video*.o ../shared
	cp V4l2Capture.h ../shared
	cp V4l2Capture.o ../shared

clean: 
	rm -f $(OUTDIR)/*.o
	rm -f ../shared/Video*.o
	rm -f ../shared/V4l2Capture.o
	
                                                                               

