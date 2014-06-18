#include <stdio.h>
#include <fcntl.h>
#include <cv.h>
#include "highgui.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h> // needed for memset

#define PROXIMITY 40
struct coords
{
  int x,y;
}centers[20],centers_filt[20],centers_filt_temp[20];
int fruitCount;

int getimage();
int count;
int cutterX, cutterY;
int fruitX, fruitY;
int X;        
int troughNo = 0;

/* HSV ranges for colors
  PINK: H: 315-360, S: 128-255, V: 216-255
  BLUE: H: 220-240, S: 180-255, V: 100-200
  YELLOW: H: 30-70, S: 180-255, V: 150-255
*/
#define PINK_LOW cvScalar(300/2, 150, 140) 
#define PINK_HIGH cvScalar(360/2, 250, 255)
#define BLUE_LOW cvScalar(210/2, 180, 100) 
#define BLUE_HIGH cvScalar(240/2, 255, 200)
#define YELLOW_LOW cvScalar(30/2, 180, 150) 
#define YELLOW_HIGH cvScalar(70/2, 255, 255)
#define OBJ_SIZE 100
int cmd;
const char deviceName[] = "/dev/ttyUSB0";

 #define COMMAND_SUCCESS	 65 //A
 #define COMMAND_FAIL		 66 //B
 #define COMMAND_DATA		 67 //C
 #define COMMAND_LEFT		 68 // D
 #define COMMAND_RIGHT		 69 // E
 #define COMMAND_UP			 70 // F
 #define COMMAND_DOWN		 71 // G
 #define COMMAND_CUT		 72 // H
 #define COMMAND_FORWARD	 73 //I
 #define COMMAND_HORZ_RESET  74 // J
 #define COMMAND_VERT_RESET  75 // K
 #define COMMAND_BATTERY_LOW 76  //L
 #define COMMAND_HARVEST_DONE 77 // M
 #define COMMAND_GOTO        78  // N
 #define COMMAND_FREE_MOTOR 79 // O
 #define COMMAND_CUTTER      80 // P
 #define PACKET_SIZE          14 
 #define PACKET_SENTINEL "!"
 
#define STATUS int
#define STATUS_OK 0
int fd;
FILE *fp;

#if 0
STATUS sendCommand(int cmd, int arg0, int arsg1) {
    unsigned short sCmd, sArg0, sArg1;
    unsigned short rCmd, rArg0, rArg1;
    sCmd = (unsigned short)cmd;
    sArg0 = (unsigned short)arg0;
    sArg1 = (unsigned short)arg1;

    char data[20];
    int i, tmp;

    printf("fwrite(): %d\n", fwrite(&sCmd,  2, 1, fp));
    fwrite(&sArg0, 2, 1, fp);
    fwrite(&sArg1, 2, 1, fp);
    fflush(fp);
/*
    read(fd, &data, 12);
    printf("data: ");
    for(i = 0; i < 12; i++) {
        tmp = (int)data[i];
        printf("%x ", tmp);
    } */

    fread(&rCmd,  2, 1, fp);
    fread(&rArg0, 2, 1, fp);
    fread(&rArg1, 2, 1, fp); 
    printf("1st: rCmd: %d, rArg0: %d, rArg1: %d\n", rCmd, rArg0, rArg1);
/*
    read(fd, &rCmd,  2);
    read(fd, &rArg0, 2);
    read(fd, &rArg1, 2);
    printf("2nd: rCmd: %d, rArg0: %d, rArg1: %d\n", rCmd, rArg0, rArg1);
*/
    if(sCmd == rCmd && sArg0 == rArg0 && sArg1 == rArg1) {
        return STATUS_OK;
    }
    return !STATUS_OK;

}



void receiveCommand(int *data, int *arg0, int *arg1) {
    unsigned short rCmd, rArg0, rArg1;
    fread(&rCmd,  2, 1, fp);
    fread(&rArg0, 2, 1, fp);
    fread(&rArg1, 2, 1, fp);

    fwrite(&rCmd,  2, 1, fp);
    fwrite(&rArg0, 2, 1, fp);
    fwrite(&rArg1, 2, 1, fp);
    fflush(fp);
    
    if(data != NULL)
        *data = (int)rCmd;

    if(arg0 != NULL)
        *arg0 = (int)rArg0;

    if(arg1 != NULL)
        *arg1 = (int)rArg1;
}

int initCommunication() {
    int cmd;

    fp = fopen(deviceName, "rw");
    if(fp == NULL) {
        printf("Unable to open device %s.\n", deviceName);
        return -1;
    }

    return 0;
}

int closeCommunication() {
    return fclose(fp);
}

#endif

#if 1
STATUS sendCommand(int cmd, int arg0, int arg1) {
    unsigned char rCmd;
    int rArg0, rArg1;
    int i;
    char ch;
    char txPacket[20], rxPacket[20];

    snprintf(txPacket, sizeof(txPacket), "%c %05d %05d"PACKET_SENTINEL, (unsigned char)cmd,
        arg0, arg1);
    printf("txPacket : %s\n", txPacket);
/*    sCmd = (unsigned char)cmd;
    sArg0 = (unsigned char)arg0;
    sArg1 = (unsigned char)arg1;

    printf("sendCommand Tx: %x %x %x\n", (int)sCmd, (int)sArg0, (int)sArg1);
    write(fd, packet,  PACKET_SIZE);
    write(fd, &sArg0, 1);
    write(fd, &sArg1, 1); */

    write(fd, txPacket, PACKET_SIZE);
    sleep(1);
    /*
    i = 0;
    while(i < PACKET_SIZE) {
        read(fd, &ch, 1);
        rxPacket[i] = ch;
        i ++;
    }
    rxPacket[PACKET_SIZE] = 0;
    printf("rxPacket : %s\n", rxPacket);
    sscanf(rxPacket, "%c %d %d", &rCmd, &rArg0, &rArg1);*/
/*
    read(fd, &rCmd,  1);
    read(fd, &rArg0, 1);
    read(fd, &rArg1, 1); 

    printf("sendCommand Rx: ");
    printf("%x %x %x\n", (int)rCmd, (int)rArg0, (int)rArg1);    
    printf("\n");
*/
/*
    read(fd, &rCmd,  2);
    read(fd, &rArg0, 2);
    read(fd, &rArg1, 2);
    printf("2nd: rCmd: %d, rArg0: %d, rArg1: %d\n", rCmd, rArg0, rArg1);
*/
//    if(sCmd == rCmd && sArg0 == rArg0 && sArg1 == rArg1) {
        return STATUS_OK;
//    }
    return !STATUS_OK;
}

STATUS receiveCommand(int *data, int *arg0, int *arg1) {
    sleep(1);
    return STATUS_OK;
    char sCmd, rCmd;
    int sArg0, sArg1, rArg0, rArg1;
    int i;
    char txPacket[20], rxPacket[20];
    char ch;
    
    /* Receive data */
    i = 0;
    while(i < PACKET_SIZE) {
        read(fd, &ch, 1);
        rxPacket[i] = ch;
        i ++;
    }
    rxPacket[PACKET_SIZE] = 0;    
    printf("rxPacket : %s\n", rxPacket);
    sscanf(rxPacket, "%c %d %d", &rCmd, &rArg0, &rArg1);
    
   /* snprintf(txPacket, sizeof(txPacket), "%c %05d %05d"PACKET_SENTINEL, (unsigned char)rCmd,
        rArg0, rArg1);
    printf("txPacket : %s\n", txPacket);
    write(fd, txPacket, PACKET_SIZE); */
/*
    read(fd, &rCmd,  1);
    read(fd, &rArg0, 1);
    read(fd, &rArg1, 1); 

    printf("receiveCommand Rx: ");
    printf("%x %x %x\n", (int)rCmd, (int)rArg0, (int)rArg1);
    printf("\n");
    
    printf("receiveCommand Tx: %x %x %x\n", (int)rCmd, (int)rArg0, (int)rArg1);
    write(fd, &rCmd,  1);
    write(fd, &rArg0, 1);
    write(fd, &rArg1, 1);
*/
    if(data != NULL)
        *data = (int)rCmd;

    if(arg0 != NULL)
        *arg0 = (int)rArg0;
   
    if(arg1 != NULL)
        *arg1 = (int)rArg1;
        
    return STATUS_OK;
}

int initCommunication() {
    struct termios tio;
    int cmd;

    memset(&tio,0,sizeof(tio));
    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;
    
    fd = open(deviceName, O_RDWR | O_NOCTTY);// | O_NDELAY);
    if(fd == -1) {
        printf("Unable to open device %s.\n", deviceName);
        return -1;
    }

    cfsetospeed(&tio, B9600);            // 9600 baud
    cfsetispeed(&tio, B9600);            // 9600 baud
    tcsetattr(fd,TCSANOW,&tio);
    return 0;
}

int closeCommunication() {
    return close(fd);

}
#endif
IplImage* fruitDetect(IplImage *img) {
    // Convert the image into an HSV image
    IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
    IplImage* blur;
    
    // Create an image for the output
    IplImage* out = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
    IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage *temp = cvCreateImage(cvGetSize(img),8,1);

    // Perform a Gaussian blur
    cvSmooth( img, out, CV_GAUSSIAN,11, 11);
    cvCvtColor(out, imgHSV, CV_BGR2HSV);
    cvInRangeS(imgHSV, PINK_LOW, PINK_HIGH, imgThreshed);
    cvErode(imgThreshed,temp,NULL,2);
    cvDilate(temp,imgThreshed,NULL,1);

    cvReleaseImage(&imgHSV);
    cvReleaseImage(&temp);	
    cvReleaseImage( &out );	
    
    return imgThreshed;	
}

IplImage* cutterDetect(IplImage *img) {

    // Convert the image into an HSV image
    IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
    // Create an image for the output
    IplImage* out = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
    IplImage *temp = cvCreateImage(cvSize(img->width,img->height),8,1);
    IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);

    // Perform a Gaussian blur
    cvSmooth( img, out, CV_GAUSSIAN,15, 15);
    cvCvtColor(out, imgHSV, CV_BGR2HSV);
    
    cvInRangeS(imgHSV, BLUE_LOW, BLUE_HIGH, imgThreshed);
   
    cvErode(imgThreshed,temp,NULL,1);
    cvDilate(temp,imgThreshed,NULL,1);
    
    cvReleaseImage(&imgHSV);
    cvReleaseImage(&temp);	
    cvReleaseImage( &out );	
    return imgThreshed;
}

void getObjectCentre(IplImage* img, int* posX, int *posY)
{
 	// Calculate the moments to estimate the position of the ball
    CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
    cvMoments(img, moments, 1);

    // The actual moment values
    double moment10 = cvGetSpatialMoment(moments, 1, 0);
    double moment01 = cvGetSpatialMoment(moments, 0, 1);
    double area = cvGetCentralMoment(moments, 0, 0);

    *posX = moment10/area;
    *posY = moment01/area;
    delete moments;
}

IplImage* contour(IplImage* img)
{
    static int i;
    char fileName[20];
    CvMemStorage* store;
    IplImage* aux=NULL;
 
    if(aux == NULL)
    {
        aux = cvCreateImage(cvGetSize(img),8,1);
        store = cvCreateMemStorage(0);
    }  
    CvSeq * contours =0;
    cvFindContours(img,store,&contours);  //finding contours in an image
    cvZero(aux);
    //if(contours->total)
    {
      cvDrawContours(aux,contours,cvScalarAll(255),cvScalarAll(255),100);
    } 
     
    CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
    double M00, M01, M10;
    fruitCount=0;
    while(contours!=NULL)   //detects the moments means coords of individual contours
    {
        if( cvContourArea(contours,CV_WHOLE_SEQ) < 5 ) //detects only sizable objects
        {	
	        contours = contours->h_next;
	        continue;
	    }
        cvMoments(contours, moments);          
        M00 = cvGetSpatialMoment(moments,0,0); 
        M10 = cvGetSpatialMoment(moments,1,0); 
        M01 = cvGetSpatialMoment(moments,0,1); 
        centers[fruitCount].x = (int)(M10/M00);            //global variable, stores the centre coords of an object
        centers[fruitCount].y = (int)(M01/M00); 
        fruitCount++;                                          //important global variable, it represents the total no. of objects detected in the image if it is zero the no action :)
        contours = contours->h_next;
    }
    cvClearMemStorage(store);
    return aux;
}

// Updates cutterX and cutterY wrt given frame
STATUS cutterProcessing(IplImage* frame) {
    IplImage* imgRedThresh = cutterDetect(frame);
    getObjectCentre(imgRedThresh, &cutterX, &cutterY); 

    cvReleaseImage(&imgRedThresh);
    
    if(cutterX > 0 && cutterX <= 640 && cutterY > 0 && cutterY <= 480) 
    {
     printf("cutter: Detected");
     printf("Cutter's position: (%d,%d)\n",cutterX, cutterY);
     return STATUS_OK;
    }
    return !STATUS_OK;
}

// Detects fruits from frame and updates their positions in arrays
void fruitProcessing(IplImage* frame) {
    IplImage* imgYellowThresh = fruitDetect(frame);

    // Process image
    IplImage* con = contour(imgYellowThresh);   //running contour on yellow thresholded image

    // Debug start
    // fruitCount is no. of objects (fruits) in image
    printf("\nNo. of objects detected: %d", fruitCount);
    for(int idx = 0; idx < fruitCount ; idx ++) {
        printf("\nObj #%d: (%d, %d)", idx, centers[idx].x, centers[idx].y);
    }
    
    cvReleaseImage(&imgYellowThresh);
    cvReleaseImage(&con);
}

void findNearbyFruits() {
    count=0;
    for(int idx1 = 0; idx1 < fruitCount ; idx1 ++) {
    if(abs(centers[idx1].x - cutterX) <= PROXIMITY)
        centers_filt[count++] = centers[idx1];          //this array will come to be sorted as scan is done from bottom to top-no need to sort
    }
    fruitX=centers_filt[0].x;
    fruitY=centers_filt[0].y;          
}

STATUS fineTuneX(int argc)
{
     do
     {
           IplImage* frame = 0;
           // Load image from IP camera
           if(argc == 1)
           		getimage();
           frame = cvLoadImage("image.jpg");
           if(!frame)
           {
                printf("\n No input frame found !!!");
             	break;
           }

            // Step #1: Cutter detection and processing
            // Detect cutter
            if (cutterProcessing(frame) != STATUS_OK)
                break;

            // Step #2: Fruit detection and processing
            fruitProcessing(frame);
            
            // Step #3: Find nearby fruits
            findNearbyFruits();

            if(count == 0) {
                break;
            }
            
           printf("\nFine tunnig along X axis....");
           printf("\n fruitX=%d cutterX=%d",fruitX,cutterX);
           if(fruitX - cutterX > 10){
               sendCommand(COMMAND_RIGHT,1,0);
               receiveCommand(&cmd,NULL,NULL);
               if(cmd ==  COMMAND_FAIL)
                   printf("\Error right send command");
           }
           else if(cutterX - fruitX > 10){
               sendCommand(COMMAND_LEFT,1,0);
               receiveCommand(&cmd,NULL,NULL);
               if(cmd ==  COMMAND_FAIL)
                   printf("\Error right send command"); 
           }
     } while(abs(fruitX - cutterX) > 10) ;        
     
     if(abs(fruitX - cutterX) <= 10) 
        return STATUS_OK;
     return !STATUS_OK;
}


void goTo(int posX, int posY) {
    sendCommand(COMMAND_VERT_RESET, 0, 0);
    receiveCommand(&cmd,NULL,NULL);    
    sleep(4);
        
    sendCommand(COMMAND_UP, 80, 0);
    receiveCommand(&cmd,NULL,NULL);    
    sleep(10);    

    sendCommand(COMMAND_GOTO, posX, posY);
    receiveCommand(&cmd,NULL,NULL);
    sleep(10);
    sendCommand(COMMAND_VERT_RESET, 0, 0);
    receiveCommand(&cmd,NULL,NULL);
    sleep(5);

}

void goForward(int dist) {
    
    sendCommand(COMMAND_VERT_RESET, 0, 0);
    receiveCommand(&cmd,NULL,NULL);    
    sleep(4);
        
    sendCommand(COMMAND_UP, 80, 0);
    receiveCommand(&cmd,NULL,NULL);    
    sleep(10);    

    sendCommand(COMMAND_FORWARD, dist, 0);
    receiveCommand(&cmd,NULL,NULL);
    sleep(10);
    sendCommand(COMMAND_VERT_RESET, 0, 0);
    receiveCommand(&cmd,NULL,NULL);
    sleep(5);
}

void monitor() {
    // goto (0, 100)
    
    // take photo
}

int main(int argc, char *argv[])
{
    
    char fruitCurr = FALSE, fruitPrev = FALSE;
    int Flag;
    int data;

    if (initCommunication() != 0) {
        return 0;
    }
  
    printf("Enter trough number: ");
    scanf("%d", &troughNo);
    
    printf("Going to trough %d\n", troughNo);

    
    goTo(1000, 0);
    
    for (int j=0;j<8;j++)
    {
        for (int i=0;i<int(90/3);i++)
        {
                count=1;
                Flag=1;
  
                // Load image from IP camera
                IplImage* frame = 0;

                sleep(2);
                if(argc == 1)
                    getimage();
    	        frame = cvLoadImage("image.jpg");
                if(!frame){
	                printf("\n No input frame found !!!");
                    break;
	            }

                
                // Step #1: Cutter detection and processing
                // Detect cutter
                cutterProcessing(frame);

                // Step #2: Fruit detection and processing
                fruitProcessing(frame);
                
                // Step #3: Find nearby fruits
                findNearbyFruits();
                if (count == 0)
                {
                   printf("\nNo Objects detected in cutter's horizontal range YET");
                }

                //My modified code starts from here
                X=0;
                while(X < count)
                {
                     bool isCut = false;
                     
                    if(fineTuneX(argc) != STATUS_OK) {
                        isCut =false;
                        break;
                    }
                    else {
                        isCut = true;
                    }
                     
                     //printf("\n fruitX=%d 	 fruitY=%d",fruitX,fruitY);
                     while(cutterY >= fruitY + OBJ_SIZE)
                     {
                            printf ("\nup=> cutterY=%d fruitY=%d",cutterY,fruitY);
                            sendCommand( COMMAND_UP,5,0);
                            sleep(3);
                            receiveCommand(&cmd,NULL,NULL);
                            if(cmd ==  COMMAND_FAIL)
                                printf("\Error up send command");
                                
                            if(fineTuneX(argc) != STATUS_OK) {
                                isCut =false;
                                break;
                            }
                            else {
                                isCut = true;
                            }
                              
                     }

                    if(isCut == true) {                     
                         sendCommand(COMMAND_UP,7,0);  //going little up from fruit's center
                         receiveCommand(&cmd,NULL,NULL);
                         if(cmd ==  COMMAND_FAIL)
                              printf("\Error up send command");
                          
                         sendCommand(COMMAND_CUT,0,0);      //modify bot code to init the cutter in widest position           
                         receiveCommand(&cmd,NULL,NULL); 
                         if(cmd ==  COMMAND_FAIL)
                               printf("\Error cut send command");
                         sleep(2);
                         sendCommand(COMMAND_VERT_RESET,0,0);
                         receiveCommand(&cmd,NULL,NULL);
                         if(cmd ==  COMMAND_FAIL)
                           printf("\Error vert reset send command");
                         sleep(8);
                     }
                     X++;
               }
                if(troughNo == 0)
                {
                       printf ("\nmove right....");
                       sendCommand(COMMAND_RIGHT,3,0);
                       receiveCommand(&cmd,NULL,NULL);
                       if(cmd ==  COMMAND_FAIL)
                             printf("\Error right send command");
                       printf("\n");
                }
                else if(troughNo == 1)
                {
                       printf ("\nmove left Now....");
                       sendCommand(COMMAND_LEFT,3,0);
                       receiveCommand(&cmd,NULL,NULL);
                       if(cmd ==  COMMAND_FAIL)
                             printf("\Error left send command");
                       printf("\n");
                }
        } //inner for loop
        printf ("\nHorz resetting...");
        sendCommand(COMMAND_HORZ_RESET,0,0);
        receiveCommand(&cmd,NULL,NULL);
        if(cmd ==  COMMAND_FAIL)
               printf("\Error horz resetting send command");
        sleep(10);
        printf("\nVert resetting...");
        sendCommand(COMMAND_VERT_RESET,0,0);
        receiveCommand(&cmd,NULL,NULL);
        if(cmd ==  COMMAND_FAIL)
             printf("\Error vert resetting send command");          
        sleep(10); 

        printf ("\nforward, ");
        goForward(100);
        printf("\n");
    }
    sendCommand(COMMAND_HARVEST_DONE,0,0);
    receiveCommand(&cmd,NULL,NULL);
    if(cmd ==  COMMAND_FAIL)
          printf("\Error harvest send command");
    closeCommunication();
    return 0;

}


