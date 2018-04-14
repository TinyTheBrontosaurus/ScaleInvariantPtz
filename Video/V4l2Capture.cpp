#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>       /* getopt_long() */

#include <fcntl.h>        /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>    /* for videodev2.h */

#include <linux/videodev2.h>
#include "V4l2Capture.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

/*****************************************************************
 * V4l2Capture ctor
 *****************************************************************
 */
V4l2Capture::V4l2Capture()
{
    io  = IO_METHOD_MMAP/*IO_METHOD_READ*/;
    fd  = -1;
    buffers = NULL;
    n_buffers = 0;
    running = false;
}

/*****************************************************************
 * V4l2Capture ctor
 *****************************************************************
 */
void V4l2Capture::startCapture( CHAR *devicePath, V4LCaptureType v4lcaptureType )
{
  strcpy(dev_name, devicePath);
  if( running )
  {
    stopCapture();
  }
  open_device();
  init_device();
  start_capturing();
  
  //Set the capture type. 
  v4lcSelectInput(v4lcaptureType);
  v4lcInputInquiry();
  
  running = true; 
}

/*****************************************************************
 * V4l2Capture ctor
 *****************************************************************
 */
void V4l2Capture::stopCapture()
{
  //Uninit the capturing, if running
  if( running)
  {
    stop_capturing();
    uninit_device();
    close_device();
  }
  running = false;
}

/*****************************************************************
 * V4l2Capture ctor
 *****************************************************************
 */
CHAR *V4l2Capture::readFrame()
{
  int i;
  i = read_frame();  
  return (CHAR*)buffers[i].start;
}
 
/*****************************************************************
 * V4l2Capture::errno_exit
 *****************************************************************
 */
void V4l2Capture::errno_exit(const char *s)
{
  fprintf (stderr, "%s error %d, %s\n",
     s, errno, strerror (errno));

  exit (EXIT_FAILURE);
}

/*****************************************************************
 * V4l2Capture::errno_exit
 *****************************************************************
 */
int V4l2Capture::xioctl(int fd, int request, void *arg)
{
  int r;

  do r = ioctl (fd, request, arg);
  while (-1 == r && EINTR == errno);

  return r;
}

/*****************************************************************
 * V4l2Capture::process_image
 *****************************************************************
 */
void V4l2Capture::process_image(const void *p)
{
  /*fputc ('.', stdout);
    fflush (stdout);*/
}

/*****************************************************************
 * V4l2Capture::read_frame
 *****************************************************************
 */
int V4l2Capture::read_frame(void)
{
  struct v4l2_buffer buf;
  unsigned int i;
	
	int ret_val = 0;

  switch (io) {
  case IO_METHOD_READ:
    if (-1 == read (fd, buffers[0].start, buffers[0].length)) {
      switch (errno) {
      case EAGAIN:
        return 0;

      case EIO:
        /* Could ignore EIO, see spec. */

        /* fall through */

      default:
        errno_exit ("read");
      }
    }

    process_image (buffers[0].start);
		ret_val = 0;
    break;

  case IO_METHOD_MMAP:
    CLEAR (buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl (fd, VIDIOC_DQBUF, &buf)) {
      switch (errno) {
      case EAGAIN:
        return 0;

      case EIO:
        /* Could ignore EIO, see spec. */

        /* fall through */

      default:
        errno_exit ("VIDIOC_DQBUF");
      }
    }

    assert (buf.index < n_buffers);

    process_image (buffers[buf.index].start);

		ret_val = buf.index;
    
		if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
      errno_exit ("VIDIOC_QBUF");

    break;

  case IO_METHOD_USERPTR:
    CLEAR (buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl (fd, VIDIOC_DQBUF, &buf)) {
      switch (errno) {
      case EAGAIN:
        return 0;

      case EIO:
        /* Could ignore EIO, see spec. */

        /* fall through */

      default:
        errno_exit ("VIDIOC_DQBUF");
      }
    }

    for (i = 0; i < n_buffers; ++i)
      if (buf.m.userptr == (unsigned long) buffers[i].start
          && buf.length == buffers[i].length)
        break;

    assert (i < n_buffers);

    process_image ((void *) buf.m.userptr);

    if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
      errno_exit ("VIDIOC_QBUF");

    break;
  }

  return ret_val;
}

/*****************************************************************
 * V4l2Capture::mainloop
 *****************************************************************
 */
void V4l2Capture::mainloop(void)
{
  unsigned int count;

  count = 100;

  while (count-- > 0) {
    for (;;) {
      fd_set fds;
      struct timeval tv;
      int r;

      FD_ZERO (&fds);
      FD_SET (fd, &fds);

      /* Timeout. */
      tv.tv_sec = 2;
      tv.tv_usec = 0;

      r = select (fd + 1, &fds, NULL, NULL, &tv);

      if (-1 == r) {
        if (EINTR == errno)
          continue;

        errno_exit ("select");
      }

      if (0 == r) {
        fprintf (stderr, "select timeout\n");
        exit (EXIT_FAILURE);
      }

      if (read_frame ())
        break;
  
      /* EAGAIN - continue select loop. */
    }
  }
}

/*****************************************************************
 * V4l2Capture::stop_capturing
 *****************************************************************
 */
void V4l2Capture::stop_capturing(void)
{
  enum v4l2_buf_type type;

  switch (io) {
  case IO_METHOD_READ:
    /* Nothing to do. */
    break;

  case IO_METHOD_MMAP:
  case IO_METHOD_USERPTR:
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl (fd, VIDIOC_STREAMOFF, &type))
      errno_exit ("VIDIOC_STREAMOFF");

    break;
  }
}

/*****************************************************************
 * V4l2Capture::start_capturing
 *****************************************************************
 */
void V4l2Capture::start_capturing(void)
{
  unsigned int i;
  enum v4l2_buf_type type;

  switch (io) {
  case IO_METHOD_READ:
    /* Nothing to do. */
    break;

  case IO_METHOD_MMAP:
    for (i = 0; i < n_buffers; ++i) {
      struct v4l2_buffer buf;

      CLEAR (buf);

      buf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory      = V4L2_MEMORY_MMAP;
      buf.index       = i;

      if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
        errno_exit ("VIDIOC_QBUF");
    }
    
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
      errno_exit ("VIDIOC_STREAMON");

    break;

  case IO_METHOD_USERPTR:
    for (i = 0; i < n_buffers; ++i) {
      struct v4l2_buffer buf;

      CLEAR (buf);

      buf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory      = V4L2_MEMORY_USERPTR;
      buf.m.userptr   = (unsigned long) buffers[i].start;
      buf.length      = buffers[i].length;

      if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
        errno_exit ("VIDIOC_QBUF");
    }


    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
      errno_exit ("VIDIOC_STREAMON");

    break;
  }      
}

/*****************************************************************
 * V4l2Capture::uninit_device
 *****************************************************************
 */
void V4l2Capture::uninit_device(void)
{
  unsigned int i;

  switch (io) {
  case IO_METHOD_READ:
    free (buffers[0].start);
    break;

  case IO_METHOD_MMAP:
    for (i = 0; i < n_buffers; ++i)
      if (-1 == munmap (buffers[i].start, buffers[i].length))
        errno_exit ("munmap");
    break;

  case IO_METHOD_USERPTR:
    for (i = 0; i < n_buffers; ++i)
      free (buffers[i].start);
    break;
  }

  free (buffers);
}

/*****************************************************************
 * V4l2Capture::init_read
 *****************************************************************
 */
void V4l2Capture::init_read(unsigned int buffer_size)
{
  buffers = (buffer *)calloc (1, sizeof (*buffers));

  if (!buffers) {
    fprintf (stderr, "Out of memory\n");
    exit (EXIT_FAILURE);
  }

  buffers[0].length = buffer_size;
  buffers[0].start = malloc (buffer_size);

  if (!buffers[0].start) {
    fprintf (stderr, "Out of memory\n");
    exit (EXIT_FAILURE);
  }
}

/*****************************************************************
 * V4l2Capture::init_mmap
 *****************************************************************
 */
void V4l2Capture::init_mmap(void)
{
  struct v4l2_requestbuffers req;

  CLEAR (req);

  req.count         = 4;
  req.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory        = V4L2_MEMORY_MMAP;

  if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req)) {
    if (EINVAL == errno) {
      fprintf (stderr, "%s does not support "
         "memory mapping\n", dev_name);
      exit (EXIT_FAILURE);
    } else {
      errno_exit ("VIDIOC_REQBUFS");
    }
  }

  if (req.count < 2) {
    fprintf (stderr, "Insufficient buffer memory on %s\n",
       dev_name);
    exit (EXIT_FAILURE);
  }

  buffers = (buffer *)calloc (req.count, sizeof (*buffers));

  if (!buffers) {
    fprintf (stderr, "Out of memory\n");
    exit (EXIT_FAILURE);
  }

  for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
    struct v4l2_buffer buf;

    CLEAR (buf);

    buf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory      = V4L2_MEMORY_MMAP;
    buf.index       = n_buffers;

    if (-1 == xioctl (fd, VIDIOC_QUERYBUF, &buf))
      errno_exit ("VIDIOC_QUERYBUF");

    buffers[n_buffers].length = buf.length;
    buffers[n_buffers].start =
      mmap (NULL /* start anywhere */,
            buf.length,
            PROT_READ | PROT_WRITE /* required */,
            MAP_SHARED /* recommended */,
            fd, buf.m.offset);

    if (MAP_FAILED == buffers[n_buffers].start)
      errno_exit ("mmap");
  }
}

/*****************************************************************
 * V4l2Capture::init_userp
 *****************************************************************
 */
void V4l2Capture::init_userp(unsigned int buffer_size)
{
  struct v4l2_requestbuffers req;

  CLEAR (req);

  req.count         = 4;
  req.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory        = V4L2_MEMORY_USERPTR;

  if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req)) {
    if (EINVAL == errno) {
      fprintf (stderr, "%s does not support "
         "user pointer i/o\n", dev_name);
      exit (EXIT_FAILURE);
    } else {
      errno_exit ("VIDIOC_REQBUFS");
    }
  }

  buffers = (buffer *)calloc (4, sizeof (*buffers));

  if (!buffers) {
    fprintf (stderr, "Out of memory\n");
    exit (EXIT_FAILURE);
  }

  for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
    buffers[n_buffers].length = buffer_size;
    buffers[n_buffers].start = malloc (buffer_size);

    if (!buffers[n_buffers].start) {
      fprintf (stderr, "Out of memory\n");
      exit (EXIT_FAILURE);
    }
  }
}

/*****************************************************************
 * V4l2Capture::init_device
 *****************************************************************
 */
void V4l2Capture::init_device(void)
{
  struct v4l2_capability cap;
  struct v4l2_cropcap cropcap;
  struct v4l2_crop crop;
  struct v4l2_format fmt;
  unsigned int min;
	v4l2_std_id std; // defines a video standard set

  if (-1 == xioctl (fd, VIDIOC_QUERYCAP, &cap)) {
    if (EINVAL == errno) {
      fprintf (stderr, "%s is no V4L2 device\n",
         dev_name);
      exit (EXIT_FAILURE);
    } else {
      errno_exit ("VIDIOC_QUERYCAP");
    }
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    fprintf (stderr, "%s is no video capture device\n",
       dev_name);
    exit (EXIT_FAILURE);
  }

  switch (io) {
  case IO_METHOD_READ:
    if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
      fprintf (stderr, "%s does not support read i/o\n",
         dev_name);
      exit (EXIT_FAILURE);
    }

    break;

  case IO_METHOD_MMAP:
  case IO_METHOD_USERPTR:
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
      fprintf (stderr, "%s does not support streaming i/o\n",
         dev_name);
      exit (EXIT_FAILURE);
    }

    break;
  }

  /* Select video input, video standard and tune here. */
  cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	// set the video standard
	std = V4L2_STD_NTSC_M;
	xioctl( fd, VIDIOC_S_STD, &std );


  if (-1 == xioctl (fd, VIDIOC_CROPCAP, &cropcap)) {
    /* Errors ignored. */
  }

  crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  crop.c = cropcap.defrect; /* reset to default */

  if (-1 == xioctl (fd, VIDIOC_S_CROP, &crop)) {
    switch (errno) {
    case EINVAL:
      /* Cropping not supported. */
      break;
    default:
      /* Errors ignored. */
      break;
    }
  }

  CLEAR (fmt);

  fmt.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width       = PIC_WIDTH; 
  fmt.fmt.pix.height      = PIC_HEIGHT;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;/*V4L2_PIX_FMT_YUYV;*/
  fmt.fmt.pix.field       = IM_INTERLACING;

  if (-1 == xioctl (fd, VIDIOC_S_FMT, &fmt))
    errno_exit ("VIDIOC_S_FMT");

  /* Note VIDIOC_S_FMT may change width and height. */

  /* Buggy driver paranoia. */
  min = fmt.fmt.pix.width * 2;
  if (fmt.fmt.pix.bytesperline < min)
    fmt.fmt.pix.bytesperline = min;
  min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
  if (fmt.fmt.pix.sizeimage < min)
    fmt.fmt.pix.sizeimage = min;

  switch (io) {
  case IO_METHOD_READ:
    init_read (fmt.fmt.pix.sizeimage);
    break;

  case IO_METHOD_MMAP:
    init_mmap ();
    break;

  case IO_METHOD_USERPTR:
    init_userp (fmt.fmt.pix.sizeimage);
    break;
  }
}

/*****************************************************************
 * V4l2Capture::close_device
 *****************************************************************
 */
void V4l2Capture::close_device(void)
{
  if (-1 == close(fd))
    errno_exit ("close");

  fd = -1;
}

/*****************************************************************
 * V4l2Capture::open_device
 *****************************************************************
 */
void V4l2Capture::open_device(void)
{
  struct stat st; 

  if (-1 == stat (dev_name, &st)) {
    fprintf (stderr, "Cannot identify '%s': %d, %s\n",
       dev_name, errno, strerror (errno));
    exit (EXIT_FAILURE);
  }

  if (!S_ISCHR (st.st_mode)) {
    fprintf (stderr, "%s is no device\n", dev_name);
    exit (EXIT_FAILURE);
  }

  fd = open (dev_name, O_RDWR /* required *//* | O_NONBLOCK, 0*/);

  if (-1 == fd) {
    fprintf (stderr, "Cannot open '%s': %d, %s\n",
       dev_name, errno, strerror (errno));
    exit (EXIT_FAILURE);
  }
}

/*****************************************************************
 * V4l2Capture::usage
 *****************************************************************
 */
void V4l2Capture::usage(FILE *fp,
         int        argc,
         char **    argv)
{
  fprintf (fp,
     "Usage: %s [options]\n\n"
     "Options:\n"
     "-d | --device name   Video device name [/dev/video]\n"
     "-h | --help    Print this message\n"
     "-m | --mmap    Use memory mapped buffers\n"
     "-r | --read    Use read() calls\n"
     "-u | --userp   Use application allocated buffers\n"
     "",
     argv[0]);
}

const char short_options [] = "d:hmru";

const struct option
long_options [] = {
  { "device",     required_argument,      NULL,     'd' },
  { "help",       no_argument,      NULL,     'h' },
  { "mmap",       no_argument,      NULL,     'm' },
  { "read",       no_argument,      NULL,     'r' },
  { "userp",      no_argument,      NULL,     'u' },
  { 0, 0, 0, 0 }
};

/*
int
main          (int        argc,
         char **    argv)
{
  dev_name = "/dev/v4l/video0";

  for (;;) {
    int index;
    int c;
    
    c = getopt_long (argc, argv,
         short_options, long_options,
         &index);

    if (-1 == c)
      break;

    switch (c) {
    case 0:
      break;

    case 'd':
      dev_name = optarg;
      break;

    case 'h':
      usage (stdout, argc, argv);
      exit (EXIT_SUCCESS);

    case 'm':
      io = IO_METHOD_MMAP;
      break;

    case 'r':
      io = IO_METHOD_READ;
      break;

    case 'u':
      io = IO_METHOD_USERPTR;
      break;

    default:
      usage (stderr, argc, argv);
      exit (EXIT_FAILURE);
    }
  }

  open_device ();

  init_device ();

  start_capturing ();

  mainloop ();

  stop_capturing ();

  uninit_device ();

  close_device ();

  exit (EXIT_SUCCESS);

  return 0;
}
*/

/*****************************************************************
 * V4l2Capture::v4lcInputInquiry
 *****************************************************************
 */
void V4l2Capture::v4lcInputInquiry()
{

  struct v4l2_input input;
  int index;
  
  if (-1 == ioctl (fd, VIDIOC_G_INPUT, &index)) {
    perror ("VIDIOC_G_INPUT");
    exit (EXIT_FAILURE);
  }
  
  memset (&input, 0, sizeof (input));
  input.index = index;
  
  if (-1 == ioctl (fd, VIDIOC_ENUMINPUT, &input)) {
    perror ("VIDIOC_ENUMINPUT");
    exit (EXIT_FAILURE);
  }
  
  printf ("Current input: %s (%d)\n", input.name, input.index);  
}
  
/*****************************************************************
 * V4l2Capture::v4lcSelectInput
 *****************************************************************
 */
void V4l2Capture::v4lcSelectInput( int newInput )
{

  if (-1 == ioctl (fd, VIDIOC_S_INPUT, &newInput)) {
    //If S video fails, try using the alternate S video for the Osprey 200
    if( newInput == V4LC_SVIDEO )
    {
      v4lcSelectInput( V4LC_SVIDEO200 );
    }
    else
    {
      perror ("VIDIOC_S_INPUT");
      exit (EXIT_FAILURE);
    }
  }
}

// File: $Id: V4l2Capture.cpp,v 1.2 2005/08/10 02:48:03 edn2065 Exp $
// Author: Eric D Nelson
// Description: The options for an object in the Controller class
// Revisions:
// $Log: V4l2Capture.cpp,v $
// Revision 1.2  2005/08/10 02:48:03  edn2065
// Changed to allow warning and error free ICC compile
//
// Revision 1.1  2005/08/09 23:32:34  edn2065
// Turned v4lcapture into a class
//
//

/*
 *  V4L2 video capture example
 *
 * Originally from: http://v4l2spec.bytesex.org/spec/capture-example.html
 *
 * Modified by JJJ: Changed scope of functions decleared in v4l2capture.h
 */

