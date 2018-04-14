#ifndef V4L2CAPTURE_H
#define V4L2CAPTURE_H

#include "types.h"

#define PIC_WIDTH IM_WIDTH
#define PIC_HEIGHT IM_HEIGHT

typedef enum 
{
  V4LC_COMPOSITE0 = 0,
  V4LC_COMPOSITE1,
  V4LC_COMPOSITE2,
  V4LC_SVIDEO,
  V4LC_SVIDEO200 = 1,
} V4LCaptureType; 

typedef enum {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
} io_method;

struct buffer {
        void *                  start;
        size_t                  length;
};


/**************************************************************
 *
 * V4l2Capture class 
 * Description: Sets up video capture
 **************************************************************
 */
class V4l2Capture
{
  public:
  /******************************************
    * ctor
    * Description: Sets defaults.
    ******************************************
    */  
    V4l2Capture();
    
  /******************************************
    * startCapture
    * Description: Starts capture from the frame grabber
    * Parameters:  devicePath - The path of the frame grabber
    ******************************************
    */
    void startCapture( CHAR *devicePath, V4LCaptureType v4lcaptureType );

   /******************************************
    * stopCapture
    * Description: Stops capture from the frame grabber
    ******************************************
    */        
    void stopCapture();
   
   /******************************************
    * readFrame
    * Description: Grabs a frame and places it in memory
    * Returns:     Pointer to the beginning of the image buffer
    ******************************************
    */         
    CHAR *readFrame();
   
   /******************************************
    * isRunning
    * Description: Returns true if capture is running
    * Returns:     true if capture is running
    ******************************************
    */           
    BOOL isRunning() { return running; }
  
  private:
    /* To capture images use the following three functions in order
    * to start the stream. */
    void open_device(void); /* Set dev_name = "/dev/v4l/video0" beforehand */
    void init_device(void);
    void start_capturing(void);
    
    /* Run the following function to capture a frame from the camera 
    * Note that the image is stored in buffers */
    int read_frame(void);
    
    /* To close down the stream, use the following functions in order */
    void stop_capturing(void);
    void uninit_device(void);
    void close_device(void);
    
    //Outputs the current input (by EDN)
    void v4lcInputInquiry();
    void v4lcSelectInput(int newInput);
    
    void errno_exit(const char *s);
    int  xioctl(int fd, int request, void *arg);
    void process_image(const void *p);
    void init_read(unsigned int buffer_size);
    void mainloop(void);
    void init_mmap(void);
    void init_userp(unsigned int buffer_size);
    void usage(FILE *fp, int argc, char **argv);   
    
  private:
  
    char             dev_name[100] ;
    io_method        io       ;
    int              fd       ;
    struct buffer *  buffers  ;
    unsigned int     n_buffers;
    BOOL running;

   
};

#endif
