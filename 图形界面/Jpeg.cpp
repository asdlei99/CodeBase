/******************************************************************* 
*  Copyright(c) 2017 
*  All rights reserved. 
* 
*  �ļ�����: Jpeg.cpp
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
#include "Jpeg.h"

int DecodeJpeg2Bmp(const char* filename,void* outbuf,int bufsize,int* nBits,int* width,int* height)
{
    if(filename == NULL)
        return -1;

    //��������ʼ����ѹ������ͬʱ�ƶ�������Ϣ������
    jpeg_decompress_struct cinfo = {0};
    jpeg_error_mgr error = {0};
    cinfo.err = jpeg_std_error(&error);
    jpeg_create_decompress(&cinfo);

    //��jpgͼ���ļ�����ָ��Ϊ��ѹ�������Դ�ļ�
    FILE* finput = fopen(filename,"rb");
    if(finput == NULL)
        return -2;

    jpeg_stdio_src(&cinfo,finput);

    //��ȡͼ����Ϣ
    jpeg_read_header(&cinfo,TRUE);

    //����ͼ����Ϣ����һ��ͼ�񻺳���
    if(outbuf == NULL)
    {
        *nBits = cinfo.num_components * 8;
        *width = cinfo.image_width;
        *height = cinfo.image_height;
        jpeg_destroy_decompress(&cinfo);
        fclose(finput);
        return sizeof(BYTE)*cinfo.image_width*(cinfo.image_height+1)*cinfo.num_components;
    }
        
    JSAMPROW data = (JSAMPROW)outbuf;

    //��ʼ��ѹ��
    jpeg_start_decompress(&cinfo);
    JSAMPROW row_pointer[1];
    while (cinfo.output_scanline < cinfo.output_height)
    {
        row_pointer[0] = &data[(cinfo.output_scanline)*cinfo.image_width*cinfo.num_components];
        jpeg_read_scanlines(&cinfo,row_pointer ,1);
    }
    jpeg_finish_decompress(&cinfo);

    //�ͷ���Դ
    jpeg_destroy_decompress(&cinfo);
    fclose(finput);
    return 0;
}