#ifndef TRACKERCOLORSEGMENTATION_H
#define TRACKERCOLORSEGMENTATION_H

#include "types.h"
#include "Tracker.h"
#include "VideoStream.h"
#include "cv.h"

/**************************************************************
 *
 * TrackerColorSegmentation class 
 * Description: Tracks objects by segmenting based on HSV values
 *
 *
 **************************************************************
 */

class TrackerColorSegmentation : public Tracker {
 
  public:
   /******************************************
    * ctor
    * Description: Constructor that creates a lookup table of the specified size initialized 
    *              to zero.
    ******************************************
    */
    TrackerColorSegmentation();
    
   /******************************************
    * Constructor that creates a lookup table based on the specified input file
    *
    * Parameters: char *lookup_file - the file to read from
    * 
    * This expects the lookup file to be in the following format:
    * <RANGE_HxRANGE_S>
    * <val[1][1]> <val[1][2]> ... <val[RANGE_H-1][RANGE_S-1>
    *
    * where val[x] is either a 0 or 1, there only needs to be whitespace
    * between each value
    ******************************************
    */    
    TrackerColorSegmentation(CHAR *lookup_file);

   /******************************************
    * dtor
    * Description: Deallocates memory
    ******************************************
    */        
    ~TrackerColorSegmentation();
   
    /*********************************************
    * train
    *
    * This sets the value of the lookup tables based on the hue value
    * of each pixel in the picture.  It takes the hsv value of each pixel in each
    * picture listed in dir/pic_list and updates the lookup table with a 1.
    *
    * Parameters:
    *  char *dir - the directory the pictures are in
    *  char *pic_list - a file that lists the pictures to process in dir
    *                   This file is NOT assumed to be in dir
    *
    * Modifies:
    *  lookup
    *
    * Returns:
    *  1 if all pics successfully processed, 0 otherwise
    ************************************************
    */    
    UINT8 train(CHAR *dir, CHAR *pic_list);
    
   /************************************************
    * train
    *
    * This sets the value of the lookup tables based on the hue value
    * of each pixel in the picture. 
    *
    * Parameters:
    *  char *pic - file name of picture 
    *
    * Modifies
    *  lookup table
    ************************************************
    */  
    void train(CHAR *pic);

   /****************************************************
    * writeLookup
    *
    * This writes the lookup table to a file in the format that it needs to read
    *
    * Parameters
    *  char *dest_file - the file in which to dump the lookup table
    *
    * Modifies
    *  the destination file
    *
    **************************************************** 
    */    
    void writeLookup(CHAR *dest_file);

   /****************************************************
    * segment
    *
    * combines segmentation and connected components algorithms
    *
    * Parameters
    *  frame - the current frame
    *  numROI - the number of regions of interest
    *  roi - the regions of interest
    *
    *******************************************************
    */       
    virtual void track( IplImage *frame, UINT8 *numROI, CvRect *roi );
    
   /****************************************************
    * registerVideo
    *
    * Tells the tracker to register all of its videostreams
    *
    * Parameters
    *  vStream - The stream in which the videos will be registered
    *  descriptor - The string to add to the beginning of each 
    *               string representing the streams added. 
    *
    *******************************************************
    */ 
    void registerVideo( VideoStream *vStream, CHAR *descriptor  );


  private:

   /*************************************************************
    * filter
    *
    * This returns the filtered image based on the lookup table
    *
    * Parameters
    *
    * Modifies
    *  img_filt
    *************************************************************
    */                    
    void filter();
    
   /****************************************************************
    * connect
    *
    * connects the segmented regions and creates bounding boxes for each region
    * http://www710.univ-lyon1.fr/~bouakaz/OpenCV-0.9.5/docs/ref/OpenCVRef_ImageProcessing.htm#decl_CvConnectedComp
    * 
    * Parameters
    *  IplImage *img_seg - an rgb segmented image
    *  int *num_boxes - the number of regions found
    *  CvRect boxes[] - the regions found
    *
    * Modifies
    *  img_bw
    *  img_cont
    *  num_boxes
    *  boxes
    *
    * Returns
    *  img_bw
    *  num_boxes
    *  boxes
    *****************************************************************
    */    
    void connect( UINT8 *num_boxes, CvRect boxes[] );
    
   /*****************************************************************
    * updateLookup
    *
    * This updates the lookup table
    *
    * Parameters: 
    *  img - the image to update the lookup table with
    *
    * Modifies
    *  lookup table
    ****************************************************************
    */    
    void updateLookup(IplImage *img);
    
  private:
    //The HSV lookup table. '1' if the color matches, '0' otherwise.
    UINT8 lookup[HUE_RANGE+1][SATURATION_RANGE+1];   
    //Images created and used in this class
    IplImage *img_filt;
    IplImage *img_bw;
    IplImage *img_cont;
    IplImage *density_map;
    IplImage *img_connect;
    IplImage *img_hsv;
    
    //Pointer to the current frame.
    IplImage *img_rgb;
    
    CvSeq *contours;
    
    //The kernel used for segmentation
    IplConvKernel *filterElement;
    IplConvKernel *connectElement;
    
    //The storage used for segmentation
    CvMemStorage *storage;
};
#endif

/*
 * Author: Jared Holsopple
 *         Justin Hnatow
 *         Eric D Nelson
 * Version:
 *     $Id: TrackerColorSegmentation.h,v 1.7 2005/06/28 15:09:40 edn2065 Exp $
 *
 * Revisions:
 *     $Log: TrackerColorSegmentation.h,v $
 *     Revision 1.7  2005/06/28 15:09:40  edn2065
 *     Added multiple tracker color option to kernel
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
 *     Revision 1.2  2005/05/25 18:12:32  edn2065
 *     Restructured comments
 *
 *     Revision 1.1  2005/05/25 17:12:03  edn2065
 *     Adding JJJ versions to repository
 *
 *     Revision 1.8  2005/03/09 20:15:54  edn2065
 *     resized density map
 *
 *     Revision 1.7  2005/01/24 18:26:46  jmh1170
 *     tried adding density map but caused seg fault. checking in so that build will still work
 *
 *     Revision 1.6  2005/01/18 22:26:34  jmh1170
 *     removed 2d lookup table and made 1d
 *
 *     Revision 1.5  2005/01/18 22:05:52  jmh1170
 *     attempted 2d histogram vs 1d
 *     reverting to 1d because performance is worse without proper training
 *
 *     Revision 1.4  2005/01/18 20:39:07  jmh1170
 *     added tags for version and revision
 *
 */
