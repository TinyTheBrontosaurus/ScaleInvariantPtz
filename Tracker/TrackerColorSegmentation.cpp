#include <stdio.h>
#include <highgui.h>
#include "TrackerColorSegmentation.h"


/*****************************************************************
 * TrackerColorSegmentation ctor
 *****************************************************************
 */
TrackerColorSegmentation::TrackerColorSegmentation() {
  
  //Initialize lookup table to all zeros
  for( UINT16 hCounter = 0; hCounter <= HUE_RANGE; hCounter++ ) {
    for( UINT16 sCounter = 0; sCounter <= SATURATION_RANGE; sCounter++ ) {
      lookup[hCounter][sCounter] = 0;
    }
  }
  
  // initialize image pointers to zero
  img_filt = NEW_IMAGE;
  img_hsv = NEW_IMAGE;
  img_bw = NEW_IMAGE_BW;
  img_cont = NEW_IMAGE_BW;
  density_map = NEW_IMAGE_DM;/*cvCreateImage( 
                  cvSize(IM_WIDTH/CSEG_DENSITYMAP,IM_HEIGHT/CSEG_DENSITYMAP), 
                  IPL_DEPTH_8U, 1 );*/
  //Formerly img_temp in connect
  img_connect = NEW_IMAGE_DM;/*cvCreateImage( 
                  cvSize(IM_WIDTH/CSEG_DENSITYMAP,IM_HEIGHT/CSEG_DENSITYMAP), 
                  8, 1 );*/
  
  // create erosion and dilation element 
  // 3x3 cross
  filterElement = 
    cvCreateStructuringElementEx( 3, 3, 2, 2, CV_SHAPE_CROSS, NULL );
  connectElement = 
    cvCreateStructuringElementEx( 2, 2, 1, 1, CV_SHAPE_RECT, 0 );
    
  storage = cvCreateMemStorage(0);    
}
 

/*****************************************************************
 * TrackerColorSegmentation dtor
 *****************************************************************
 */
TrackerColorSegmentation::~TrackerColorSegmentation() {
  cvReleaseImage( &img_filt );
  cvReleaseImage( &img_hsv );
  cvReleaseImage( &img_bw );
  cvReleaseImage( &img_cont );
  cvReleaseImage( &density_map );
  

  cvReleaseStructuringElement( &filterElement );
  cvReleaseStructuringElement( &connectElement );
  
  cvReleaseMemStorage( &storage );
}

/*****************************************************************
 * TrackerColorSegmentation::train
 *****************************************************************
 */
UINT8 TrackerColorSegmentation::train(CHAR *dir, CHAR *pic_list) {

  FILE *list_pics;
  CHAR temp[100];
  CHAR cur_pic[100];
  IplImage *rgbTrain, *hsvTrain;
  UINT16 ret_val = 1;
  
  list_pics = fopen(pic_list, "r");
  
  if( list_pics != NULL) {
    
    while(fscanf(list_pics, "%s", temp) != EOF)  {
      
      sprintf(cur_pic, "%s%s", dir, temp);
      rgbTrain = cvLoadImage(cur_pic);
      hsvTrain = cvCreateImage( cvGetSize(rgbTrain), IPL_DEPTH_8U, 3 );
      cvCvtColor( rgbTrain, hsvTrain, CV_RGB2HSV );
      updateLookup( hsvTrain );
      cvReleaseImage( &hsvTrain );
      cvReleaseImage( &rgbTrain );
    }
    fclose( list_pics );
  }
  else {
    printf("Error: %s does not exist!  System not trained!\n", pic_list);
    ret_val = 0;
  }
  return ret_val;
}

/*****************************************************************
 * TrackerColorSegmentation::train
 *****************************************************************
 */
void TrackerColorSegmentation::train( CHAR *pic ) {
  IplImage *rgbTrain, *hsvTrain;
  rgbTrain = cvLoadImage(pic);
  hsvTrain = cvCreateImage( cvGetSize(rgbTrain), IPL_DEPTH_8U, 3 );
  cvCvtColor( rgbTrain, hsvTrain, CV_RGB2HSV );
  updateLookup( hsvTrain );
  cvReleaseImage( &hsvTrain );
  cvReleaseImage( &rgbTrain );
}

/*****************************************************************
 * TrackerColorSegmentation::updateLookup
 *****************************************************************
 */
void TrackerColorSegmentation::updateLookup(IplImage *img) {

  UINT16 height = img->height; // height of the image
  UINT16 width = img->width * img->nChannels; // width of the image
  UINT16 step = img->widthStep;

  UINT8 *data = reinterpret_cast<UINT8 *>(img->imageData);
  for(UINT16 i = 0; i < height; i++) {
    for(UINT16 j = 0; j < width; j+=img->nChannels) {
      //Intensity filter
      if( (UINT16)data[j+2] >= CSEG_INTENSITY_MIN && 
        (UINT16)data[j+2] <= CSEG_INTENSITY_MAX ) { 
	lookup[(UINT16)data[j]][(UINT16)data[j+1]] = 1;
      }
    }
    data += step; // next line
  }
}


/*****************************************************************
 * TrackerColorSegmentation::writeLookup
 *****************************************************************
 */
void TrackerColorSegmentation::writeLookup(CHAR *dest_file) {
  FILE *dest;
  
  dest = fopen(dest_file, "w");

  fprintf(dest, "%dx%d\n", HUE_RANGE, SATURATION_RANGE );
  
  for( UINT16 i = 0; i <= HUE_RANGE; i++ ) {
    for( UINT16 j = 0; j <= SATURATION_RANGE; j++ ) {
      fprintf(dest, "%d ", lookup[i][j]);
    }
  }
  fprintf(dest, "\n");
  
  fclose(dest);
}


/*****************************************************************
 * TrackerColorSegmentation::filter
 *****************************************************************
 */
void TrackerColorSegmentation::filter() {
  UINT16 height = img_rgb->height;
  UINT16 channels = img_rgb->nChannels;
  UINT16 width = img_rgb->width * channels;
  UINT16 step = img_rgb->widthStep;

  UINT16 dm_wth = 0;
  UINT16 dm_wth_cnt = 0;
  UINT16 dm_hgt_cnt = 0;

  // create erosion and dilation element 
  // 3x3 cross
  cvZero( img_filt );
  cvZero( density_map );
  cvCopy( img_rgb, img_filt, NULL );

  // point to the data of the rgb image
  UINT8 *data_filt = 
    reinterpret_cast<UINT8 *>(img_filt->imageData);
  // point to the data of the hsv image
  UINT8 *data_hsv = 
    reinterpret_cast<UINT8 *>(img_hsv->imageData);
  UINT8 *data_dm = 
    reinterpret_cast<UINT8 *>(density_map->imageData);
  UINT16 step_dm = density_map->widthStep;

  //Iterate through image
  for( UINT16 i = 0; i < height; i++ ) {
    dm_wth = 0;
    for( UINT16 j = 0; j < width; j+=channels ) {
      // if the hsv value is in the lookup table it is a skin tone
      // so keep it the same value.  if it is not in the lookup table
      // make the pixel black
      if( lookup[(UINT16)data_hsv[j]][(UINT16)data_hsv[j+1]] == 0 ||
          (UINT16)data_hsv[j+2] < CSEG_INTENSITY_MIN || 
          (UINT16)data_hsv[j+2] > CSEG_INTENSITY_MAX )  
      {
	data_filt[j] = (UINT8)0;
	data_filt[j+1] = (UINT8)0;
	data_filt[j+2] = (UINT8)0;
      } else {
	data_dm[dm_wth] += 1;
      }
      if( dm_wth_cnt < CSEG_DENSITYMAP - 1 ) {
	dm_wth_cnt += 1;
      } else {
	dm_wth_cnt = 0;
	dm_wth += 1;
      }
    }
    if( dm_hgt_cnt < CSEG_DENSITYMAP-1 ) {
      dm_hgt_cnt += 1;
    } else {
      dm_hgt_cnt = 0;
      data_dm += step_dm;
    }
    
    data_hsv += step;
    data_filt += step;
  }
  // need to check threshold value
  cvThreshold( density_map, density_map, CSEG_DENSITYMAP*CSEG_DENSITYMAP/2, 255,
    CV_THRESH_BINARY );
  
  
}


/*****************************************************************
 * TrackerColorSegmentation::connect
 *****************************************************************
 */
void TrackerColorSegmentation::connect( UINT8 *num_boxes, CvRect boxes[] ) {
  //  printf( "inside connect\n" );
  
  // defines up-left(pt0), up-right(pt1), bottom-right(pt2), bottom-left(pt3)
  UINT16 x0 = 0, y0 = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
  // defines up-left(pt0), bottom-right(pt2) of element in boxes
  UINT16 xi0 = 0, yi0 = 0, xi2 = 0, yi2 = 0;
  // the index of the box which the current one is touching
  SINT16 inscribed;
    
  //The area of the largest box
  UINT32 maxArea = 0;
  UINT32 areaOfNewBox;
  UINT8 indexOfMaxAreaBox;
    
  cvCopy( density_map, img_connect, NULL );

  
  CvRect temp;
  // white

  //  printf( "2\n" );

  CvScalar color = CV_RGB( 255, 255, 255 );
  // init images
  contours = 0;
  cvZero( img_bw );
  cvZero( img_cont );

  *num_boxes = 0;

  // convert the image to grayscale
  cvCvtColor( img_filt, img_bw, CV_RGB2GRAY );

  // threshold the image to bring out the important features
  cvThreshold( img_bw, img_bw, 1, 255, CV_THRESH_BINARY );

  cvErode( img_connect, img_connect, connectElement, 1 );
  cvDilate( img_connect, img_connect, connectElement, 1 );

  cvFindContours( img_connect, storage, &contours, sizeof(CvContour),
            CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

  // for each contour
  for( ; contours != 0 && *num_boxes < MAX_ROIS; 
       contours = contours->h_next ) {
    // draw the contour on the contour image
    cvDrawContours( img_cont, contours, color, color, -1, CV_FILLED, 8 );
    inscribed = -1;
    temp = cvBoundingRect( contours, 0 );
    temp.x = temp.x * CSEG_DENSITYMAP-1;
    temp.y = temp.y * CSEG_DENSITYMAP-1; //OBOE somewhere
    temp.width = temp.width * CSEG_DENSITYMAP;
    temp.height = temp.height * CSEG_DENSITYMAP;
    // make sure the rectangle is at least 20x20
    if( temp.width >= CSEG_MIN_ROI_W && temp.height >= CSEG_MIN_ROI_H ) {
      // find all the corners of the new rectangle 
      // starting at the top-left and moving clockwise
      x0 = temp.x; y0 = temp.y;  // up-left (pt0)
      x1 = x0 + temp.width; y1 = y0;  // up-right (pt1)
      x2 = x1; y2 = y0 + temp.height; // bottom-right (pt2)
      x3 = x0; y3 = y2;          // bottom-left (pt3)
      // iterate through all the currently existing boxes to see if the new
      // one touches any
      for( UINT16 i = 0; i < *num_boxes && inscribed == -1; i++ ) {
	xi0 = boxes[i].x; yi0 = boxes[i].y;
	xi2 = xi0 + boxes[i].width; yi2 = yi0 + boxes[i].height;
	// if the top-left x coordinate is between the current box's x 
	// coordinates && if the top-left y coordinate is near the 
	// current box's bottom y coordinate
	if( x0 >= xi0 && x0 <= xi2 ) {
	  if( y0 >= yi0 && y0 <= yi2 )
	    inscribed = i;
	  else if( abs(y0 - yi2) <= CSEG_MAX_DIST )  
	    inscribed = i;
	} else if( x1 >= xi0 && x1 <= xi2 ) {
	  if( y1 >= yi0 && y1 <= yi2 ) 
	    inscribed = i;
	  else if( abs(y1 - yi2) <= CSEG_MAX_DIST ) 
	    inscribed = i;
	} else if( x2 >= xi0 && x2 <= xi2 ) {
	  if( y2 >= yi0 && y2 <= yi2 ) 
	    inscribed = i;
	  else if( abs(y2 - yi0) <= CSEG_MAX_DIST ) 
	    inscribed = i;
	} else if( x3 >= xi0 && x3 <= xi2 ) {
	  if( y3 >= yi0 && y3 <= yi2 ) 
	    inscribed = i;
	  else if( abs(y3 - yi0) <= CSEG_MAX_DIST )
	    inscribed = i;
	} else if( y0 >= yi0 && y0 <= yi2 ) {
	  if( x0 >= xi0 && x0 <= xi2 ) 
	    inscribed = i;
	  else if( abs(x0 - xi0) <= CSEG_MAX_DIST )
	    inscribed = i;
	} else if( y1 >= yi0 && y0 <= yi2 ) {
	  if( x1 >= xi0 && x1 <= xi2 ) 
	    inscribed = i;
	  else if( abs(x1 - xi2) <= CSEG_MAX_DIST ) 
	    inscribed = i;
	} else if( y2 >= yi0 && y2 <= yi2 ) {
	  if( x2 >= xi0 && x2 <= xi2 ) 
	    inscribed = i;
	  else if( abs(x2 - xi2) <= CSEG_MAX_DIST ) 
	    inscribed = i;
	} else if( y3 >= yi0 && y3 <= yi2 ) {
	  if( x3 >= xi0 && x3 <= xi2 ) 
	    inscribed = i;
	  else if( abs(x3 - xi0) <= CSEG_MAX_DIST ) 
	    inscribed = i; 
	}
      }
      // if the new box does not touch any other box
      if( inscribed == -1 ) {
        // new box!
        
        //Keep track of the largest box
        areaOfNewBox = temp.height * temp.width;
        if( areaOfNewBox > maxArea )
        {
          maxArea = areaOfNewBox;
          indexOfMaxAreaBox = *num_boxes;          
        }
        
        //Save the box        
	boxes[*num_boxes] = temp;
	*num_boxes += 1;
        
        
        
      } else {
	// find bounding box of two boxes and set boxes[inscribed] to bounding
	// box
	boxes[inscribed] = cvMaxRect( &boxes[inscribed], &temp );
      }
    }
  }
  
  //Put the largest box at the front of the array
  if( *num_boxes > 0 )
  {
    temp = boxes[0];
    boxes[0] = boxes[indexOfMaxAreaBox];
    boxes[indexOfMaxAreaBox] = temp;
  }
  
  cvClearMemStorage( storage );

}


/*****************************************************************
 * TrackerColorSegmentation::track
 *****************************************************************
 */
void TrackerColorSegmentation::track( IplImage *frame, UINT8 *numROI, CvRect *roi )
{
  //Save the frame
  img_rgb = frame;
  //Convert to HSV
  cvCvtColor(img_rgb, img_hsv, CV_RGB2HSV);
  //Filter the frame       
  filter();
  //Run connected components
  connect( numROI, roi );
  
}

/*****************************************************************
 * TrackerColorSegmentation::registerVideo
 *****************************************************************
 */
void TrackerColorSegmentation::registerVideo( VideoStream *vStream,
  CHAR *descriptor )
{
  CHAR curDescriptor[VIDSTR_MAX_DESC_LENGTH];
  
  sprintf( curDescriptor, "%s%s", descriptor, "CSEG: Filtered");
  vStream->registerVideo(img_filt, curDescriptor);
  sprintf( curDescriptor, "%s%s", descriptor, "CSEG: Black and white");
  vStream->registerVideo(img_bw, curDescriptor);
  sprintf( curDescriptor, "%s%s", descriptor, "CSEG: Contoured");
  vStream->registerVideo(img_cont, curDescriptor);
  sprintf( curDescriptor, "%s%s", descriptor, "CSEG: Density map");
  vStream->registerVideo(density_map, curDescriptor);
  sprintf( curDescriptor, "%s%s", descriptor, 
    "CSEG: Connected components");
  vStream->registerVideo(img_connect, curDescriptor);
  sprintf( curDescriptor, "%s%s", descriptor, 
    "CSEG: Hue Saturation Values");
  vStream->registerVideo(img_hsv, curDescriptor);

}

/*
 * cseg.cpp
 *
 * Implementation of cseg.h
 *
 * Author: Jared Holsopple
 *         Justin Hnatow
 *
 * Version:
 *     $Id: TrackerColorSegmentation.cpp,v 1.13 2005/06/30 14:46:28 edn2065 Exp $
 *
 * Revisions:
 *     $Log: TrackerColorSegmentation.cpp,v $
 *     Revision 1.13  2005/06/30 14:46:28  edn2065
 *     overnight checkin
 *
 *     Revision 1.12  2005/06/28 18:33:26  edn2065
 *     Completed addition of blue and red trackers
 *
 *     Revision 1.11  2005/06/28 15:09:40  edn2065
 *     Added multiple tracker color option to kernel
 *
 *     Revision 1.10  2005/06/13 18:47:14  edn2065
 *     Calibration implemented without principle point finder.
 *
 *     Revision 1.9  2005/06/09 13:48:03  edn2065
 *     Fixed synchronization errors caused by not initializing Barrier in SIKernal
 *
 *     Revision 1.8  2005/05/27 14:57:27  edn2065
 *     Fix VideoSource memory leak and possible OBOE in CSEG
 *
 *     Revision 1.7  2005/05/26 19:39:18  edn2065
 *     Tracking works
 *
 *     Revision 1.6  2005/05/26 19:15:02  edn2065
 *     Added VideoStream. tested. Still need to get tracker working
 *
 *     Revision 1.5  2005/05/26 14:08:11  edn2065
 *     Tracker compiles, but doesnt build because of execute in Menu.h
 *
 *     Revision 1.4  2005/05/26 13:34:49  edn2065
 *     Added Tracker.h and finished changing names.
 *
 *     Revision 1.3  2005/05/25 19:35:18  edn2065
 *     Checking in for night. need to test still
 *
 *     Revision 1.1  2005/05/25 17:12:03  edn2065
 *     Adding JJJ versions to repository
 *
 *     Revision 1.16  2005/03/09 21:17:46  edn2065
 *     added intensity filter
 *
 *     Revision 1.15  2005/03/09 21:01:13  edn2065
 *     Removed call to cvClearSeq because it was causing segmentation
 *     fault because cvReleaseMemStorage already did the memory
 *     freeing for that memory.
 *
 *     Revision 1.14  2005/03/09 20:17:13  edn2065
 *     Changed threshold level of density map
 *
 *     Revision 1.13  2005/03/09 20:15:11  edn2065
 *     resized density map to 1
 *
 *     Revision 1.12  2005/03/09 18:13:44  edn2065
 *     read only for some reason
 *
 *     Revision 1.11  2005/01/25 02:01:22  jmh1170
 *     density map working in filter function.  histogram needs modification before integrating into connected components algorithm
 *
 *     Revision 1.10  2005/01/24 18:27:09  jmh1170
 *     tried adding density map but causing seg fault.  checking in to keep working build
 *
 *     Revision 1.9  2005/01/19 01:16:24  jmh1170
 *     changed lookup table to 360 elements. verified proper functinoality
 *
 *     Revision 1.8  2005/01/18 22:26:13  jmh1170
 *     removed 2d lookup table and made 1d
 *
 *     Revision 1.7  2005/01/18 22:04:54  jmh1170
 *     attempted HUE_RANGE & SATURATION_RANGE sized lookup table (2d vs 1d)
 *     performance worse than with 1d
 *     neewds more training but reverting to 1d
 *
 *     Revision 1.6  2005/01/18 20:37:57  jmh1170
 *     done with commenting.  about to modify lookup table to be 2d
 *
 *     Revision 1.5  2005/01/18 19:10:43  jmh1170
 *     modified box nearness algorithm
 *
 *     Revision 1.4  2005/01/18 15:39:11  jmh1170
 *     adding comments
 *
 */
