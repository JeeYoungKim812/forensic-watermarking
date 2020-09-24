#pragma once
#include "stdafx.h"
#include "Headers.h"
#define m 38
#define n 107
#define D 106

//////////////////////////////////////////////////////////////////////////////////////////
//// 추출
//////////////////////////////////////////////////////////////////////////////////////////

void ExtractWatermark(Mat& marked_img)
{
	Extract(marked_img);
}

/* DWT-DCT 
void Extract(Mat& Marked_Image)
{
	//imshow("Marked_Image", Marked_Image);
	////////////////////////////////
	getPSNR(Marked_Image);         // 원본 이미지와 삽입 이미지의 PSNR 값 계산을 위함
	////////////////////////////////
	Mat yuv_arr[3];
	cvtColor(Marked_Image, Marked_Image, COLOR_RGB2YCrCb);   // RGB to YCrCb
	split(Marked_Image, yuv_arr);                      // 채널 분리
	Mat Marked_Y_channel = Mat(Marked_Image.cols, Marked_Image.rows, Marked_Image.type());
	int QRcodeSize;

	yuv_arr[0].copyTo(Marked_Y_channel);                // Y 채널 분리

	// 추출한 QRcode 계수들을 저장할 행렬
	Mat HH_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);
	Mat LH_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);
	Mat HL_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);

	Mat WT_result;
	yuv_arr[0].convertTo(Marked_Y_channel, CV_32F);      //uchar -> float
	WT(Marked_Y_channel, WT_result, 1);               // 분리한 Y 채널을 대상으로 1단계 DWT 진행

	// 부대역의 계수를 저장할 행렬 변수
//	Mat HH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
//	Mat HL_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
	Mat LH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());

//	HH_subband = WT_result(Rect(WT_result.cols / 2, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));
//	HL_subband = WT_result(Rect(WT_result.cols / 2, 0, WT_result.cols / 2, WT_result.rows / 2));
	LH_subband = WT_result(Rect(0, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));

	// DCT를 진행할 8x8 크기의 블럭들
	Size blockSize(8, 8);
//	vector<Mat> HH_blocks;   // 각 부대역의 블럭들
//	vector<Mat> HL_blocks;
	vector<Mat> LH_blocks;
	int i = 0;

	// 256x256 크기의 부대역을 1024개의 8x8 블럭 사이즈로 분할
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x, y, blockSize.width, blockSize.height);
	//		HH_blocks.push_back(Mat(HH_subband, rect));
	//		HL_blocks.push_back(Mat(HL_subband, rect));
			LH_blocks.push_back(Mat(LH_subband, rect));
		}
	}

	int x = 0, y = 0;
	// 각 부대역의 1024개의 블럭들을 대상으로 삽입된 워터마크 추출 진행
	for (int i = 0; i < 1024; i++)
	{
		// DCT 진행
	//	dct(HH_blocks[i], HH_blocks[i]);
	//	dct(HL_blocks[i], HL_blocks[i]);
		dct(LH_blocks[i], LH_blocks[i]);

		// DCT 진행 후 값들의 DC 값을 기준으로 삽입된 QR 재생성
	//	float HH_sum = HH_blocks[i].at<float>(0, 0);
	//	float HL_sum = HL_blocks[i].at<float>(0, 0);
		float LH_sum = LH_blocks[i].at<float>(0, 0);

	//	HH_recoverd_QRcode_Pixel.at<uchar>((int)(y), (int)(x)) = ((HH_sum > 0) ? 255 : 0);
	//	HL_recoverd_QRcode_Pixel.at<uchar>((int)(y), (int)(x)) = ((HL_sum > 0) ? 255 : 0);
		LH_recoverd_QRcode_Pixel.at<uchar>((int)(y), (int)(x++)) = ((LH_sum > 0) ? 255 : 0);
			
		if (x == 32)
		{
			y++;
			x = 0;
		}
	}

	// QRcode를 생성할 행렬 변수 설정
	QRcodeSize = HL_recoverd_QRcode_Pixel.rows;
//	Mat QR_HH(QRcodeSize + 2, QRcodeSize + 2, HH_recoverd_QRcode_Pixel.type(), Scalar(255));
//	Mat QR_HL(QRcodeSize + 2, QRcodeSize + 2, HL_recoverd_QRcode_Pixel.type(), Scalar(255));
	Mat QR_LH(QRcodeSize + 2, QRcodeSize + 2, LH_recoverd_QRcode_Pixel.type(), Scalar(255));

	// 결정된 QRcode의 픽셀 값을 위치에 맞게 저장
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
	//		QR_HH.at<uchar>(i + 1, j + 1) = HH_recoverd_QRcode_Pixel.at<uchar>(i, j);
	//		QR_HL.at<uchar>(i + 1, j + 1) = HL_recoverd_QRcode_Pixel.at<uchar>(i, j);
			QR_LH.at<uchar>(i + 1, j + 1) = LH_recoverd_QRcode_Pixel.at<uchar>(i, j);
		}
	}

//	imshow("QR_HL", QR_HL);

//	Mat BIG_QR_HH(100, 100, HH_recoverd_QRcode_Pixel.type(), Scalar(255));
//	Mat BIG_QR_HL(100, 100, HL_recoverd_QRcode_Pixel.type(), Scalar(255));
	Mat BIG_QR_LH(100, 100, LH_recoverd_QRcode_Pixel.type(), Scalar(255));

	int nn = 0;
	// 32x32 크기의 QR의 크기 확장
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
		//	BIG_QR_HH.at<uchar>(n, 3 * j) = QR_HH.at<uchar>(i, j);
		//	BIG_QR_HH.at<uchar>(n, 3 * j + 1) = QR_HH.at<uchar>(i, j);
		//	BIG_QR_HH.at<uchar>(n, 3 * j + 2) = QR_HH.at<uchar>(i, j);
		//	BIG_QR_HH.at<uchar>(n + 1, 3 * j) = QR_HH.at<uchar>(i, j);
		//	BIG_QR_HH.at<uchar>(n + 1, 3 * j + 1) = QR_HH.at<uchar>(i, j);
		//	BIG_QR_HH.at<uchar>(n + 1, 3 * j + 2) = QR_HH.at<uchar>(i, j);
		//	BIG_QR_HH.at<uchar>(n + 2, 3 * j) = QR_HH.at<uchar>(i, j);
		//	BIG_QR_HH.at<uchar>(n + 2, 3 * j + 1) = QR_HH.at<uchar>(i, j);
		//	BIG_QR_HH.at<uchar>(n + 2, 3 * j + 2) = QR_HH.at<uchar>(i, j);

		//	BIG_QR_3.at<uchar>(n, 3 * j) = QR_HL.at<uchar>(i, j);
		//	BIG_QR_3.at<uchar>(n, 3 * j + 1) = QR_HL.at<uchar>(i, j);
		//	BIG_QR_3.at<uchar>(n, 3 * j + 2) = QR_HL.at<uchar>(i, j);
		//	BIG_QR_3.at<uchar>(n + 1, 3 * j) = QR_HL.at<uchar>(i, j);
		//	BIG_QR_3.at<uchar>(n + 1, 3 * j + 1) = QR_HL.at<uchar>(i, j);
		//	BIG_QR_3.at<uchar>(n + 1, 3 * j + 2) = QR_HL.at<uchar>(i, j);
		//	BIG_QR_3.at<uchar>(n + 2, 3 * j) = QR_HL.at<uchar>(i, j);
		//	BIG_QR_3.at<uchar>(n + 2, 3 * j + 1) = QR_HL.at<uchar>(i, j);
		//	BIG_QR_3.at<uchar>(n + 2, 3 * j + 2) = QR_HL.at<uchar>(i, j);

			BIG_QR_LH.at<uchar>(nn, 3 * j) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn, 3 * j + 1) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn, 3 * j + 2) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn + 1, 3 * j) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn + 1, 3 * j + 1) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn + 1, 3 * j + 2) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn + 2, 3 * j) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn + 2, 3 * j + 1) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn + 2, 3 * j + 2) = QR_LH.at<uchar>(i, j);
		}
		nn += 3;
	}

//	imshow("HH_QRcode", BIG_QR_HH);
//	imshow("HL_QRcode", BIG_QR_HL);
//	imshow("LH_QRcode", BIG_QR_LH);

//	imwrite("HH_QRcode.png", BIG_QR_HH);
//	imwrite("HL_QRcode.png", BIG_QR_HL);
	imwrite("C:/Users/KJY/Desktop/result/DWT_DCT_LH_QRcode.png", BIG_QR_LH);
	
	getNCC();      // 삽입된 워터마크와 추출된 워터마크 간 NCC 값 계산
}
*/

/* DCT-CRT 추출 코드 
void Extract(Mat& Marked_Image)
{
	//imshow("Marked_Image", Marked_Image);
	////////////////////////////////
	getPSNR(Marked_Image);         // 원본 이미지와 삽입 이미지의 PSNR 값 계산을 위함
	////////////////////////////////
	Mat yuv_arr[3];
	cvtColor(Marked_Image, Marked_Image, COLOR_RGB2YCrCb);   // RGB to YCrCb
	split(Marked_Image, yuv_arr);                      // 채널 분리
	Mat Marked_Y_channel = Mat(Marked_Image.cols, Marked_Image.rows, Marked_Image.type());
	int QRcodeSize;
	
	yuv_arr[0].convertTo(Marked_Y_channel, CV_32F);    //uchar -> float
	
	Mat ori_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);

	// DCT를 진행할 8x8 크기의 블럭들
	Size blockSize(8, 8);
	vector<Mat> ori_blocks;
	int i = 0;

	// 256x256 크기의 부대역을 1024개의 8x8 블럭 사이즈로 분할
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x, y, blockSize.width, blockSize.height);
			ori_blocks.push_back(Mat(Marked_Y_channel, rect));
		}
	}

	int x = 0, y = 0;
	// 1024개의 8*8 블록에 dct 적용 후 값 출력
	for (int i = 0; i < 1024; i++)
	{
		dct(ori_blocks[i], ori_blocks[i]);
	}
	// 각 부대역의 1024개의 블럭들을 대상으로 삽입된 워터마크 추출 진행
	for (int i = 0; i < 1024; i++)
	{
		int Z_ori = ori_blocks[i].at<float>(0, 0);

		int p_ori = Z_ori % m;
		int q_ori = Z_ori % n;
		int d_ori = abs(p_ori - q_ori);
		int b_ori = p_ori + q_ori;

		// 기준 값을 대상으로 QR의 흰 부분과 검은 부분 구성
		if (b_ori >= (D + 35) / 2) // watermark bit 1    d_ori >= (D) / 2  b_ori >= (D + 35) / 2
		{
			ori_recoverd_QRcode_Pixel.at<uchar>((int)(y), (int)(x++)) = 255;
		}
		else // watermark bit 0
		{
			ori_recoverd_QRcode_Pixel.at<uchar>((int)(y), (int)(x++)) = 0;
		}
		if (x == 32)
		{
			y++;
			x = 0;
		}
	}

	// QRcode를 생성할 행렬 변수 설정
	QRcodeSize = ori_recoverd_QRcode_Pixel.rows;
	Mat QR_ori(QRcodeSize + 2, QRcodeSize + 2, ori_recoverd_QRcode_Pixel.type(), Scalar(255));

	// 결정된 QRcode의 픽셀 값을 위치에 맞게 저장
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			QR_ori.at<uchar>(i + 1, j + 1) = ori_recoverd_QRcode_Pixel.at<uchar>(i, j);
		}
	}

	Mat BIG_QR_ori(100, 100, ori_recoverd_QRcode_Pixel.type(), Scalar(255));

	int nn = 0;
	// 32x32 크기의 QR의 크기 확장
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			BIG_QR_ori.at<uchar>(nn, 3 * j) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn, 3 * j + 1) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn, 3 * j + 2) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn + 1, 3 * j) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn + 1, 3 * j + 1) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn + 1, 3 * j + 2) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn + 2, 3 * j) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn + 2, 3 * j + 1) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn + 2, 3 * j + 2) = QR_ori.at<uchar>(i, j);
		}
		nn += 3;
	}

	//imshow("ori_QRcode", BIG_QR_ori);
	imwrite("C:/Users/KJY/Desktop/result/DCT_CRT_QRcode.png", BIG_QR_ori);

	getNCC();      // 삽입된 워터마크와 추출된 워터마크 간 NCC 값 계산
}
*/


/* DCT-CRT 추출 512512
void Extract(Mat& Marked_Image)
{
	//imshow("Marked_Image", Marked_Image);
	////////////////////////////////
	getPSNR(Marked_Image);         // 원본 이미지와 삽입 이미지의 PSNR 값 계산을 위함
	//getSSIM(Marked_Image);
	////////////////////////////////
	Mat yuv_arr[3];
	cvtColor(Marked_Image, Marked_Image, COLOR_RGB2YCrCb);   // RGB to YCrCb
	split(Marked_Image, yuv_arr);                      // 채널 분리
	Mat Marked_Y_channel = Mat(Marked_Image.cols, Marked_Image.rows, Marked_Image.type());
	int QRcodeSize;

	yuv_arr[0].convertTo(Marked_Y_channel, CV_32F);    //uchar -> float

	Mat ori_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);

	// DCT를 진행할 8x8 크기의 블럭들
	Size blockSize(8, 8);
	vector<Mat> ori_blocks;
	int i = 0;

	// 256x256 크기의 부대역을 1024개의 8x8 블럭 사이즈로 분할
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x + 128, y + 128, blockSize.width, blockSize.height);
			ori_blocks.push_back(Mat(Marked_Y_channel, rect));
		}
	}

	int x = 0, y = 0;
	// 1024개의 8*8 블록에 dct 적용 후 값 출력
	for (int i = 0; i < 1024; i++)
	{
		dct(ori_blocks[i], ori_blocks[i]);
	}
	// 각 부대역의 1024개의 블럭들을 대상으로 삽입된 워터마크 추출 진행
	for (int i = 0; i < 1024; i++)
	{
		int Z_ori = ori_blocks[i].at<float>(0, 0);
		if (Z_ori == 40 || Z_ori == 44)
		{
			//if()// 여기에 모든 픽셀이 같을 조건 추가(단색 이미지에만 해당시키기 위해) 하면 더 좋을듯
			Z_ori += 60;
		}
		int p_ori = Z_ori % m;
		int q_ori = Z_ori % n;
		int d_ori = abs(p_ori - q_ori);
		int b_ori = p_ori + q_ori;

		// 기준 값을 대상으로 QR의 흰 부분과 검은 부분 구성
		if (b_ori >= (D + 35) / 2) // watermark bit 1    d_ori >= (D) / 2  b_ori >= (D + 35) / 2
		{
			ori_recoverd_QRcode_Pixel.at<uchar>((int)(y), (int)(x++)) = 255;
		}
		else // watermark bit 0
		{
			ori_recoverd_QRcode_Pixel.at<uchar>((int)(y), (int)(x++)) = 0;
		}
		if (x == 32)
		{
			y++;
			x = 0;
		}
	}

	// QRcode를 생성할 행렬 변수 설정
	QRcodeSize = ori_recoverd_QRcode_Pixel.rows;
	Mat QR_ori(QRcodeSize + 2, QRcodeSize + 2, ori_recoverd_QRcode_Pixel.type(), Scalar(255));

	// 결정된 QRcode의 픽셀 값을 위치에 맞게 저장
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			QR_ori.at<uchar>(i + 1, j + 1) = ori_recoverd_QRcode_Pixel.at<uchar>(i, j);
		}
	}

	Mat BIG_QR_ori(100, 100, ori_recoverd_QRcode_Pixel.type(), Scalar(255));

	int nn = 0;
	// 32x32 크기의 QR의 크기 확장
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			BIG_QR_ori.at<uchar>(nn, 3 * j) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn, 3 * j + 1) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn, 3 * j + 2) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn + 1, 3 * j) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn + 1, 3 * j + 1) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn + 1, 3 * j + 2) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn + 2, 3 * j) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn + 2, 3 * j + 1) = QR_ori.at<uchar>(i, j);
			BIG_QR_ori.at<uchar>(nn + 2, 3 * j + 2) = QR_ori.at<uchar>(i, j);
		}
		nn += 3;
	}

	//imshow("ori_QRcode", BIG_QR_ori);
	imwrite("C:/Users/KJY/Desktop/result/[lena512]DCT_CRT_QRcode.png", BIG_QR_ori);
	//imwrite("C:/Users/KJY/Desktop/result/m65n361/DCT_CRT_QRcode.png", BIG_QR_ori);

	getNCC();      // 삽입된 워터마크와 추출된 워터마크 간 NCC 값 계산
}
*/
