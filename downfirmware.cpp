#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "qhyccd.h"

int main(int argc,char *argv[])
{
    int ret = QHYCCD_ERROR;
    char path[] = "/usr/local";
    
    ret = InitQHYCCDResource();
    if(ret == QHYCCD_SUCCESS){
        printf("Init SDK success!\n");
    }else{
        printf("Init SDK failed!\n");;
    }

    ret = OSXInitQHYCCDFirmware(path);
    if(ret == QHYCCD_SUCCESS){
        printf("Download firmware successed!\n");
    }else{
        printf("Download firmware failed!\n");
    }
}