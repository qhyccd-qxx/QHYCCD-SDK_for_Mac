#include <stdio.h>
#include <time.h>
#include "qhyccd.h"
#include "highgui.h"

#define BIN1X1MODE

int main(int argc,char *argv[])
{
    int s = 0,num = 0,found = 0;
    int ret = QHYCCD_ERROR,ret_live = QHYCCD_ERROR;
    char id[32];
    unsigned int w,h,bpp,channels;
    unsigned char *ImgData;
    double chipw,chiph,pixelw,pixelh;
    IplImage *image,*dst;
    CvSize dst_cvsize;
    dst_cvsize.width  = 800;
    dst_cvsize.height = 600;
    char path[] = "/usr/local";
    qhyccd_handle *camhandle;
    
    ret = InitQHYCCDResource();
    if(ret == QHYCCD_SUCCESS)
    {
        printf("System Information::Init SDK success!\n");
    }
    else
    {
        goto failure;
    }
    /*ret = OSXInitQHYCCDFirmware(path);
    if(ret == QHYCCD_SUCCESS){
        printf("Download firmware successed!\n");
    }else{
        printf("Download firmware failed!\n");
    }*/
    num = ScanQHYCCD();
    if(num > 0)
    {
        printf("System Information::Found %d QHYCCD device.\n",num);
    }
    else
    {
        printf("System Information::Not Found QHYCCD device,please check the usblink or the power\n");
        goto failure;
    }

    for(int i = 0;i < num;i++)
    {
        ret = GetQHYCCDId(i,id);
        if(ret == QHYCCD_SUCCESS)
        {
            printf("Connected to the QHYCCD device.(id:%s)\n",id);
            found = 1;
            break;
        }
    }

    if(found == 1)
    {
        camhandle = OpenQHYCCD(id);
        if(camhandle != NULL)
        {
            printf("System Information::Open QHYCCD device success!\n");
        }
        else
        {
            printf("System Information::Open QHYCCD device failed!(%d)\n",ret);
            goto failure;
        }
        ret = SetQHYCCDStreamMode(camhandle,1);
        if(ret == QHYCCD_SUCCESS)
        {
            printf("System Information::Set QHYCCD device stream mode success!\n");
        }
        else
        {
            printf("System Information::Set QHYCCD device stream mode failed!(%d)\n",ret);
            goto failure;
        }

        ret = InitQHYCCD(camhandle);
        if(ret == QHYCCD_SUCCESS)
        {
            printf("System Information::Init QHYCCD device success!\n");
        }
        else
        {
            printf("System Information::Init QHYCCD device failed!(%d)\n",ret);
            goto failure;
        }
        
        ret = GetQHYCCDChipInfo(camhandle,&chipw,&chiph,&w,&h,&pixelw,&pixelh,&bpp);
        if(ret == QHYCCD_SUCCESS)
        {
            printf("System Information::Get QHYCCD ChipInfo success!\n");
            printf("                    CCD/CMOS chip information  :\n");
            printf("                    CCD/CMOS chip width        :%3f mm\n",chipw);
            printf("                    CCD/CMOS chip height       :%3f mm\n",chiph);
            printf("                    CCD/CMOS chip pixel width  :%3f um\n",pixelw);
            printf("                    CCD/CMOS chip pixel height :%3f um\n",pixelh);
            printf("                    CCD/CMOS chip width        :%d\n",w);
            printf("                    CCD/CMOS chip height       :%d\n",h);
            printf("                    CCD/CMOS chip depth        :%d\n",bpp);
        }
        else
        {
            printf("System Information::Get QHYCCD ChipInfo failed!(%d)\n",ret);
            goto failure;
        }
        
        int bits,speed,traffic;
        bool onoff;
        bits    = 8;
        speed   = 1;
        traffic = 1;
        onoff   = true;
        ret = IsQHYCCDControlAvailable(camhandle,CAM_COLOR);
        if(ret == BAYER_GB || ret == BAYER_GR || ret == BAYER_BG || ret == BAYER_RG)
        {
            printf("System Information::This QHYCCD device is a color camera!(%d)\n",onoff);
            SetQHYCCDDebayerOnOff(camhandle,onoff);
            SetQHYCCDParam(camhandle,CONTROL_WBR,64);//set camera param by definition
            SetQHYCCDParam(camhandle,CONTROL_WBG,64);
            SetQHYCCDParam(camhandle,CONTROL_WBB,64);
        }else{
            printf("System Information::This QHYCCD device is not a color camera!\n");
        }
        
        ret = IsQHYCCDControlAvailable(camhandle,CONTROL_DDR);
        if(ret == QHYCCD_SUCCESS){
        	printf("System Information::This QHYCCD device has DDR!\n");
        	ret = SetQHYCCDParam(camhandle,CONTROL_DDR,true);
        	if(ret == QHYCCD_SUCCESS){
        		printf("System Information::Open QHYCCD device DDR success!\n");
        	}else{
        		printf("System Information::Open QHYCCD device DDR failed!(%d)",ret);
        	}
        }else{
        	printf("System Information::This QHYCCD device doesn't have DDR!\n");
        }
        
        ret = IsQHYCCDControlAvailable(camhandle,CONTROL_TRANSFERBIT);
        if(ret == QHYCCD_SUCCESS)
        {
        	printf("System Information::Can set this QHYCCD device transfer bits!\n");
            ret = SetQHYCCDBitsMode(camhandle,bits);
            if(ret == QHYCCD_SUCCESS)
            {
                printf("System Information::Set QHYCCD device transfer bits success!(%d)\n",bits);
            }else{
            	printf("System Information::Set QHYCCD device transfer bits failed!(%d)\n",ret);
            }
        }else{
        	printf("System Information::Can't set this QHYCCD device transfer bits!\n");
        }
        
        ret = IsQHYCCDControlAvailable(camhandle,CONTROL_OFFSET);
        if(ret == QHYCCD_SUCCESS)
        {
        	printf("System Information::Can set this QHYCCD device offset.\n");
            ret = SetQHYCCDParam(camhandle,CONTROL_OFFSET,50);
            if(ret == QHYCCD_SUCCESS)
            {
                printf("System Information::Set QHYCCD device offset success!\n");
            }else{
            	printf("System Information::Set QHYCCD device offset failed!(%d)\n",ret);
            }
        }else{
        	printf("System Information::Can't set this QHYCCD device offset!\n");
        }
        
        ret = IsQHYCCDControlAvailable(camhandle,CONTROL_GAIN);
        if(ret == QHYCCD_SUCCESS)
        {
        	printf("System Information::Can set this QHYCCD device gain.");
            ret = SetQHYCCDParam(camhandle,CONTROL_GAIN,50);
            if(ret == QHYCCD_SUCCESS){
            	printf("System Information::Set QHYCCD device gain success!\n");
            }else{
                printf("System Information::Set QHYCCD device gain failed!(%d)\n",ret);
            }
        }else{
        	printf("System Information::Can't set this QHYCCD device gain.");
        }

		ret = IsQHYCCDControlAvailable(camhandle,CONTROL_USBTRAFFIC);
        if(ret == QHYCCD_SUCCESS)
        {
        	printf("System Information::Can set this QHYCCD device USBTraffic!\n");
            ret = SetQHYCCDParam(camhandle,CONTROL_USBTRAFFIC,traffic);
            if(ret == QHYCCD_SUCCESS)
            {
                printf("System Information::Set QHYCCD device USBTraffic success!(%d)\n",traffic);
            }else{
            	printf("System Information::Set QHYCCD device USBTraffic failed!(%d)\n",ret);
            	goto failure;
            }
        }else{
        	printf("System Information::Can't set this QHYCCD device USBTraffic!\n");
        }
        
        int exp_time = 0;
        ret = IsQHYCCDControlAvailable(camhandle,CONTROL_EXPOSURE);
        if(ret == QHYCCD_SUCCESS){
        	printf("System Information::Can set this QHYCCD device exposure time.\n");
		    exp_time = GetQHYCCDParam(camhandle,CONTROL_EXPOSURE);
			if(exp_time > 0)
				printf("System Information::QHYCCD device exposure time is %3d ms.\n",exp_time/1000);
			else
		    	printf("System Information::Get QHYCCD device exposure time failed!(%d)\n",ret);
	    	
	    	ret = SetQHYCCDParam(camhandle,CONTROL_EXPOSURE,20*1000);
			if(ret == QHYCCD_SUCCESS){
				printf("System Information::Set QHYCCD device exposure time success!\n");
				exp_time = GetQHYCCDParam(camhandle,CONTROL_EXPOSURE);
				if(exp_time > 0)
					printf("System Information::QHYCCD device exposure time is %3d ms.\n",exp_time/1000);
				else
					printf("System Information::Get QHYCCD device exposure time failed!(%d)",ret);
			}else{
				printf("System Information::Set QHYCCD device exposure time failed!(%d)\n",ret);
			}
        }
        
        ret = IsQHYCCDControlAvailable(camhandle,CONTROL_SPEED);
        if(ret == QHYCCD_SUCCESS){
        	printf("System Information::Can set this QHYCCD device speed!\n");
		    ret = SetQHYCCDParam(camhandle,CONTROL_SPEED,speed);
		    if(ret == QHYCCD_SUCCESS)
		    {
		    	printf("System Information::Set QHYCCD device speed succeed!(%d)\n",speed);
		    }else{
		    	printf("System Information::Set QHYCCD device speed failed!(%d)\n",ret);
		    	goto failure;
		    }
		}else{
			printf("System Information::Can't set this QHYCCD device speed!\n");
		}
		
		#ifdef BIN1X1MODE
		ret = IsQHYCCDControlAvailable(camhandle,CAM_BIN1X1MODE);
		if(ret == QHYCCD_SUCCESS){
			printf("Can set this camera 1X1 bin mode.\n");
			ret = SetQHYCCDBinMode(camhandle,1,1);
			if(ret == QHYCCD_SUCCESS){
				printf("Set camera 1X1 bin mode success!\n");
				ret = SetQHYCCDResolution(camhandle,0,0,w,h);
				if(ret == QHYCCD_SUCCESS)
				{
				    printf("Set camera resolution success!\n");
				}
				else
				{
				    printf("Set camera resolution failed!(%d)\n",ret);
				    goto failure;
				}
			}else{
				printf("Set camera 1X1 bin mode failed!(%d)",ret);
			}
		}
		#endif
		#ifdef BIN2X2MODE
		ret = IsQHYCCDControlAvailable(camhandle,CAM_BIN2X2MODE);
		if(ret == QHYCCD_SUCCESS){
			printf("Can set this camera 2X2 bin mode.\n");
			ret = SetQHYCCDBinMode(camhandle,2,2);
			if(ret == QHYCCD_SUCCESS){
				printf("Set camera 2X2 bin mode success!\n");
				ret = SetQHYCCDResolution(camhandle,0,0,w/2,h/2);
				if(ret == QHYCCD_SUCCESS)
				{
				    printf("Set camera resolution success!\n");
				}
				else
				{
				    printf("Set camera resolution failed!(%d)\n",ret);
				    goto failure;
				}
			}else{
				printf("Set camera 2X2 bin mode failed!(%d)",ret);
			}
		}
		#endif
		#ifdef BIN3X3MODE
		ret = IsQHYCCDControlAvailable(camhandle,CAM_BIN3X3MODE);
		if(ret == QHYCCD_SUCCESS){
			printf("Can set this camera 3X3 bin mode.\n");
			ret = SetQHYCCDBinMode(camhandle,3,3);
			if(ret == QHYCCD_SUCCESS){
				printf("Set camera 3X3 bin mode success!\n");
				ret = SetQHYCCDResolution(camhandle,0,0,w/3,h/3);
				if(ret == QHYCCD_SUCCESS)
				{
				    printf("Set camera resolution success!\n");
				}
				else
				{
				    printf("Set camera resolution failed!(%d)\n",ret);
				    goto failure;
				}
			}else{
				printf("Set camera 3X3 bin mode failed!(%d)",ret);
			}
		}
		#endif
		#ifdef BIN4X4MODE
		ret = IsQHYCCDControlAvailable(camhandle,CAM_BIN4X4MODE);
		if(ret == QHYCCD_SUCCESS){
			printf("Can set this camera 4X4 bin mode.\n");
			ret = SetQHYCCDBinMode(camhandle,4,4);
			if(ret == QHYCCD_SUCCESS){
				printf("Set camera 4X4 bin mode success!\n");
				ret = SetQHYCCDResolution(camhandle,0,0,w/4,h/4);
				if(ret == QHYCCD_SUCCESS)
				{
				    printf("Set camera resolution success!\n");
				}
				else
				{
				    printf("Set camera resolution failed!(%d)\n",ret);
				    goto failure;
				}
			}else{
				printf("Set camera 1X1 bin mode failed!(%d)",ret);
				goto failure;
			}
		}
		#endif
		
		int length = GetQHYCCDMemLength(camhandle);
        if(length > 0)
        {
        	printf("Get camrea memory length success!\n");
            ImgData = (unsigned char *)malloc(length*2);
            memset(ImgData,0,length);
        }
        else
        {
            printf("Get camera memory length failed!(%d)\n",ret);
            goto failure;
        }

		int t_start,t_end;
        t_start = time(NULL);
        int fps = 0,t_num = 0;
        
        ret = BeginQHYCCDLive(camhandle);
        if(ret == QHYCCD_SUCCESS)
        {
            printf("BeginQHYCCDLive success!\n");
            //cvNamedWindow("qhyccd",1);

		    while(ret == QHYCCD_SUCCESS)
		    {
		    	while(ret_live == QHYCCD_ERROR){
		            ret_live = GetQHYCCDLiveFrame(camhandle,&w,&h,&bpp,&channels,ImgData);
		        }
		        //image = cvCreateImage(cvSize(w,h),bpp,channels);
		        //image->imageData = (char *)ImgData;
		        
		        //cvShowImage("qhyccd",image);
		        //cvWaitKey(1);
		        ret_live = QHYCCD_ERROR;
		        fps++;
		        
                t_end = time(NULL);
                if(t_end - t_start >= 1){
                	t_num ++;
                	if(t_num % 5 == 0){
                		printf("Time pass:%3d | Frame rate:%5.1f\n",t_num,(float)fps/5);
                		fps = 0;
                	}else
	                	printf("Time pass:%3d | bpp = %d channels = %d\n",t_num,bpp,channels);
                	t_start = time(NULL);
                }
                
		        if(t_num >= 120)
		        {
		        	//break;
		        	ret = QHYCCD_ERROR;
		        }
		    }
            
            //cvDestroyWindow("qhyccd");
            //cvReleaseImage(&image);
        }
        else
        {
            printf("BeginQHYCCDLive failed\n");
            goto failure;
        }
    
		if(ImgData != NULL){
	        delete(ImgData);  
		}
    }
    else
    {
        printf("The camera is not QHYCCD or other error \n");
        goto failure;
    }
    
    if(camhandle)
    {
        ret = StopQHYCCDLive(camhandle);
        if(ret == QHYCCD_SUCCESS){
        	printf("Stop QHYCCD live success!\n");
        }

        ret = CloseQHYCCD(camhandle);
        if(ret == QHYCCD_SUCCESS)
        {
            printf("Close QHYCCD success!\n");
        }
        else
        {
            goto failure;
        }
    }

    ret = ReleaseQHYCCDResource();
    if(ret == QHYCCD_SUCCESS)
    {
        printf("Rlease SDK Resource  success!\n");
    }
    else
    {
        goto failure;
    }
    return 0;

failure:
    printf("some fatal error happened\n");
    return 1;
}
