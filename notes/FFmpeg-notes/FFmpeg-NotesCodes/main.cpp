#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

/*
#define __STDC_CONSTANT_MACROS
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif
*/

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")

#define INPUT "../bin/input.mkv"
#define OUTPUT "../bin/out.yuv"

int main()
{
	int res = 0;
	int videoStream = -1;//�����Ƶ���ı��
	char errBuf[BUFSIZ] = { 0 };
	FILE* fp_out = fopen(OUTPUT, "wb+");

	//��ʼ��FFMPEG  ��������������������ñ������ͽ�����
	av_register_all();
	printf("FFmpeg's version is: %d\n", avcodec_version());

	//FFMPEG���еĲ�����Ҫͨ�����AVFormatContext������
	AVFormatContext* pFormatCtx = NULL;

	//��������Ƶ�ļ�
	//Open an input stream and read the header. The codecs are not opened.
	if ((res = avformat_open_input(&pFormatCtx, INPUT, NULL, NULL)) < 0)
	{
		av_strerror(res, errBuf, sizeof(errBuf));
		printf("%s\n", errBuf);
		return -1;
	}
	av_dump_format(pFormatCtx, 0, NULL, 0); //�����Ƶ������Ϣ

	//������
	for (int i = 0; i < pFormatCtx->nb_streams; ++i)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			videoStream = i;
	}
	if (videoStream == -1)
	{
		printf("Didn't find a video stream.\n");
		return -1;
	}

	///���ҽ�����    
	AVCodecContext* pCodecCtx = pFormatCtx->streams[videoStream]->codec;
	AVCodec* pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}

	///�򿪽�����
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Could not open codec.");
		return -1;
	}

	AVFrame Frame = { 0 };//����ʼ����avcodec_decode_video2�ᱨ��
	AVPacket packet;
	int got_picture;
	while (1)
	{
		//��ȡ��Ƶ֡
		//return 0 if OK, < 0 on error or end of file
		if (av_read_frame(pFormatCtx, &packet) < 0)
		{
			break; //������Ϊ��Ƶ��ȡ����
		}
		if (packet.stream_index == videoStream)
		{
			//������Ƶ֡
			if (avcodec_decode_video2(pCodecCtx, &Frame, &got_picture, &packet) < 0)
			{
				printf("decode error.\n");
				return -1;
			}
			if (got_picture)
			{
				if (Frame.format == PIX_FMT_YUV420P)
				{
					//�����YUV��ʽ����Ƶ�������ݱ�����AVFrame��data[0]��data[1]��data[2]�С�
					//������Щ����ֵ�����������洢�ģ�ÿ����Ч����֮��洢��һЩ��Ч���ء�
					//������Y����Ϊ����data[0]��һ��������linesize[0] * height�����ݡ�
					//���ǳ����Ż��ȷ���Ŀ��ǣ�linesize[0]ʵ���ϲ������ڿ��width������һ���ȿ�ȴ�һЩ��ֵ��
					fwrite(Frame.data[0], Frame.linesize[0] * Frame.height, 1, fp_out);
					fwrite(Frame.data[1], Frame.linesize[1] * Frame.height / 2, 1, fp_out);
					fwrite(Frame.data[2], Frame.linesize[2] * Frame.height / 2, 1, fp_out);
				}
			}
		}
		av_free_packet(&packet);//���packet����ָ��Ļ�����
	}

	fclose(fp_out);
	avcodec_close(pCodecCtx);//�رս�����
	avformat_close_input(&pFormatCtx);//�ر�������Ƶ�ļ���avformat_free_context(pFormatCtx);�Ͳ���Ҫ��
	return 0;
}