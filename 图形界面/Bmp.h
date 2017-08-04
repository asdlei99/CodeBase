/******************************************************************* 
*  Copyright(c) 2017 
*  All rights reserved. 
* 
*  �ļ�����: Bmp.h 
*  ��Ҫ����: bmpλͼ��ش��� 
* 
*  ��������: 2017-08-04 
*  ����:  gongluck 
*  ˵��: 
* 
*  �޸�����: 
*  ����:  
*  ˵��: 
******************************************************************/
#ifndef __BMP_H__
#define __BMP_H__

#include <Windows.h>
#include <stdio.h>

//�����ļ�ͷ��dib���ݱ���bmp�ļ�
bool SaveBmpWithFileInfo(const char* filename,const BITMAPFILEHEADER* fileheader,const BITMAPINFO* bmpinfo,const void* bits);
//����λ��ȡ����ؿ����ظߺ�dib���ݱ���bmp�ļ�
//���24���µĲ��ܴ���û�п����е�ɫ��������
bool SaveBmpWithBitscountAndWH(const char* filename,int nBits,int width,int height,const void* bits);
//��λͼ�����HBITMAP�������bmpͼƬ
//������CreateDIBitmap��CreateDIBSection�Ⱥ�����ȡHBITMAP
bool SavePicture(HBITMAP hbitmap,const char* filepath);
//��YV12תBGR24��û��Ч�ʿ��ԣ��Ƽ�ʹ��ffmpeg��
bool YV12ToBGR24_Native(unsigned char* pYUV,unsigned char* pBGR24,int width,int height);

#endif 