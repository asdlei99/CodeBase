/******************************************************************* 
*  Copyright(c) 2017 
*  All rights reserved. 
* 
*  �ļ�����: Jpeg.h 
*  ��Ҫ����: ʹ��Jpeglib
* 
*  ��������: 2017-08-14 
*  ����:  gongluck 
*  ˵��: 
* 
*  �޸�����: 
*  ����: 
*  ˵��: 
******************************************************************/
#ifndef __JPEG_H__
#define __JPEG_H__

#include <Windows.h>
#include <stdio.h>
#include "jpeglib.h"

//#pragma comment(lib,"libjpeg.lib")

/*
    1.outbufΪ��ʱ�����ؽ����ڴ���Ҫ�Ĵ�С������ͨ����������λ��ȡ�ͼ���ͼ��
    2.outbuf��Ϊ��ʱ���������ݵ�outbuf
 */
int DecodeJpeg2Bmp(const char* filename,void* outbuf,int bufsize,int* nBits,int* width,int* height);

#endif