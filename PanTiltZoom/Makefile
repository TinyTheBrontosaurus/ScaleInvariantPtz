include ../shared/MakefileOptions

OUTDIR=.
                                                                  
TARGET=./PTZTester
                                       
SRCS=PanTilt.cpp PTZTester.cpp ZoomOptical.cpp CameraComms.cpp ZoomDigital.cpp ZoomHybrid.cpp CameraRxThread.cpp CameraTxThread.cpp CommandLevel.cpp
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

SRCSC=1stLevel.c 2ndLevel.c  
OBJSC=$(patsubst %.c,%.o,$(SRCSC))

ODOBJS=$(addprefix $(OUTDIR)/, $(OBJS)) $(addprefix $(OUTDIR)/, $(OBJSC)) ../shared/*.o
               
$(OUTDIR)/%.o: %.cpp  
	$(XCC) $(INC) -c $(CPPFLAGS) $< $(WXFLGS) -o $@   
        
$(OUTDIR)/%.o: %.c 
	$(GCC) -c $< -o $@          
	                                                       
all: $(TARGET)                           
                                                                                  
$(TARGET): $(ODOBJS) 
	$(XCC) -o $@ $(ODOBJS) $(WXLIBS) $(LINKFLAGS)
	cp *.o ../shared
	cp *.h ../shared
	rm -f ../shared/PTZTester.o
	
clean: 
	rm -f ../shared/Zoom*.o
	rm -f ../shared/PanTilt*.o
	rm -f ../shared/CameraComms*.o        
	rm -f ../shared/1stLevel.o
	rm -f ../shared/2ndLevel.o
	rm -f ../shared/CommandLevel.o                       
	rm -f ../shared/CameraRxThread.o
	rm -f ../shared/CameraTxThread.o
	rm -f $(OUTDIR)/*.o
	rm -f $(TARGET)

