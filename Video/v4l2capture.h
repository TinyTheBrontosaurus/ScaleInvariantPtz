#ifndef V4L2CAPTURE
#define V4L2CAPTURE

#include "types.h"

#define PIC_WIDTH IM_WIDTH
#define PIC_HEIGHT IM_HEIGHT

#define V4LC_COMPOSITE0 0
#define V4LC_COMPOSITE1 1
#define V4LC_COMPOSITE2 2
#define V4LC_SVIDEO 3

struct buffer {
        void *                  start;
        size_t                  length;
};

typedef enum {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
} io_method;

extern struct buffer *         buffers;
extern char *                  dev_name;
extern io_method               io;

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

#endif
