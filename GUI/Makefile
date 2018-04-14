XCC=g++
OUTDIR=./
                                                                  
TARGET=./SITracker
                  
LINKFLAGS=-lcurses -l cv -l cvaux -l cxcore -l highgui -l stdc++ -pthread -lm -lrt -O1 
INC=-I /usr/local/include/opencv -I /usr/src/linux/include -I ../shared

WXFLGS=$(shell wx-config --cxxflags)
WXLIBS=$(shell wx-config --libs)
                                                                        
SRCS=gui.cpp main.cpp
OBJS=$(patsubst %.cpp,%.o,$(SRCS)) 

ODOBJS=$(addprefix $(OUTDIR)/, $(OBJS)) ../shared/*.o                



$(OUTDIR)/%.o: %.cpp
	$(XCC) -c $(CPPFLAGS) $(INC) $(WXFLGS) -o $@ $<  
                                                         
all: $(TARGET)                           
                                     
                                                              
$(TARGET): $(OUTDIR) $(ODOBJS) 
	$(XCC) -o $@ $(ODOBJS) $(WXLIBS) $(LINKFLAGS)
	cp $(TARGET) ../

$(OUTDIR):
	mkdir $(OUTDIR)	
	
clean: 
	rm *.o
	rm $(TARGET)
	
                                                                               

