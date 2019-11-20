#pragma once
#include "stdafx.h"
#include "Headers.h"
#define EMBED_VALUE 45
#define DC_BOUNDARY 4080
#define m 38
#define n 107
#define D 106

//////////////////////////////////////////////////////////////////////////////////////////
//// 삽입
//////////////////////////////////////////////////////////////////////////////////////////
Mat EmbedWatermark(Mat& HostImg, Mat& QrImg)
{
	return Embed(HostImg, QrImg);
}

/* DWT-DCT
Mat Embed(Mat& HostImg, Mat& QrImg)
{
	Mat yuv;
	Mat yuv_arr[3];
	Mat WT_result;
	Mat IWT_result;
	Mat Qr_Pixel = Mat(QrImg.rows, QrImg.cols, QrImg.type()); // 32x32 QRcode 각 픽셀 값을 255, 0으로 저장할 행렬 변수 생성

	// QR의 데이터를 0과 255로 설정
	for (int y = 0; y < QrImg.rows; y++)
	{
		for (int x = 0; x < QrImg.cols; x++)
		{
			Qr_Pixel.at<uchar>(y, x) = ((int)QrImg.at<uchar>(y, x) > 125) ? 255 : 0;
		}
	}

	cvtColor(HostImg, yuv, COLOR_RGB2YCrCb);    // RGV to YCrCb
	split(yuv, yuv_arr);						// 채널 분리
	Mat ori;
	yuv_arr[0].convertTo(ori, CV_32F);      //uchar -> float

	WT(yuv_arr[0], WT_result, 1);               // Y채널을 대상으로 1단계 DWT 진행

	//imshow("Y",yuv_arr[0]);
	//imshow("삽입 전 WT result", WT_result);

	// 부대역의 계수를 저장할 행렬 변수    
	//Mat HH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
	//Mat HL_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
	Mat LH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());

	//HH_subband = WT_result(Rect(WT_result.cols / 2, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));
	//HL_subband = WT_result(Rect(WT_result.cols / 2, 0, WT_result.cols / 2, WT_result.rows / 2));
	LH_subband = WT_result(Rect(0, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));

	// DCT를 진행할 8x8 크기의 블럭들
	Size blockSize(8, 8);
	vector<Mat> HH_blocks;	// 각 부대역의 블럭들
	vector<Mat> HL_blocks;
	vector<Mat> LH_blocks;
	int value[1024];		// QR의 삽입 값을 저장할 배열
	int i = 0;

	// 256x256 크기의 부대역을 1024개의 8x8 블럭 사이즈로 분할
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x, y, blockSize.width, blockSize.height);
		//	HH_blocks.push_back(Mat(HH_subband, rect));
		//	HL_blocks.push_back(Mat(HL_subband, rect));
			LH_blocks.push_back(Mat(LH_subband, rect));
			// QR의 삽입 값을 지정
			value[i++] = ((int)Qr_Pixel.at<uchar>((int)(y / 8), (int)(x / 8)) > 125 ? 3 : -3);
		}
	}

	int x = 0, y = 0;
	
	
	// 각 부대역의 1024개 블럭들을 대상으로 워터마크 데이터 삽입 진행
	for (int i = 0; i < 1024; i++)
	{
	//	dct(HH_blocks[i], HH_blocks[i]);
	//	dct(HL_blocks[i], HL_blocks[i]);
		dct(LH_blocks[i], LH_blocks[i]);

		// 각 부대역의 DC 값 구함 = Z_대역
	//	int Z_HH = HH_blocks[i].at<float>(0, 0);
	//	int Z_HL = HL_blocks[i].at<float>(0, 0);
		int Z_LH = LH_blocks[i].at<float>(0, 0);

	//	Z_HH = Z_HH + (23 * value[i]);
//		HH_blocks[i].at<float>(0, 0) = Z_HH;
//		Z_HL = Z_HL + (23 * value[i]);
//		HL_blocks[i].at<float>(0, 0) = Z_HL;
		Z_LH = Z_LH + (23 * value[i]);
		LH_blocks[i].at<float>(0, 0) = Z_LH;

	//	idct(HH_blocks[i], HH_blocks[i]);
	//	idct(HL_blocks[i], HL_blocks[i]);
		idct(LH_blocks[i], LH_blocks[i]);

		x += 8;
		if (x == 256)
		{
			x = 0;
			y += 8;
		}
	}

	// 최신화된 부대역들을 기존의 DWT 결과에 갱신
	IWT(WT_result, IWT_result, 1);

	// 호스트 이미지의 Cb 채널을 워터마크 삽입된 Cb채널로 변경
	for (int i = 0; i < HostImg.rows; i++) 
	{
		for (int j = 0; j < HostImg.cols; j++)
		{
			yuv.at<Vec3b>(i, j)[0] = (int)IWT_result.at<float>(i, j);
		}
	}

	cvtColor(yuv, yuv, COLOR_YCrCb2RGB); // RGB to YCrCb

	//imshow("* DWT-DCT Marked Image", yuv);
	// 압축률 지정을 위한 부분
	vector<int> param75 = vector<int>(2);
	param75[0] = 1;// CV_IMWRITE_JPEG_QUALITY;
	param75[1] = 75;//default(95) 0-100

	vector<int> param95 = vector<int>(2);
	param95[0] = 1;// CV_IMWRITE_JPEG_QUALITY;
	param95[1] = 95;//default(95) 0-100

	//imshow("* DWT-DCT-CRT Marked Image", yuv);
	imwrite("C:/Users/KJY/Desktop/result/[splash]DWT_DCT_LH.png", yuv);
	imwrite("C:/Users/KJY/Desktop/result/[splash]DWT_DCT_LH_75.jpg", yuv, param75);
	imwrite("C:/Users/KJY/Desktop/result/[splash]DWT_DCT_LH_95.jpg", yuv, param95);

	return yuv;
}
*/

/* DCT-CRT 코드 
Mat Embed(Mat& HostImg, Mat& QrImg)
{
	Mat yuv;
	vector<Mat> yuv_arr(3);
	Mat Qr_Pixel = Mat(QrImg.rows, QrImg.cols, QrImg.type()); // 32x32 QRcode 각 픽셀 값을 255, 0으로 저장할 행렬 변수 생성

	// QR의 데이터를 0과 255로 설정
	for (int y = 0; y < QrImg.rows; y++)
		for (int x = 0; x < QrImg.cols; x++)
			Qr_Pixel.at<uchar>(y, x) = ((int)QrImg.at<uchar>(y, x) > 125) ? 255 : 0;

	cvtColor(HostImg, yuv, COLOR_RGB2YCrCb);    // RGV to YCrCb
	split(yuv, yuv_arr);                  // 채널 분리
	Mat ori;
	yuv_arr[0].convertTo(ori, CV_32F);      // type 0 -> type 5

	// DCT를 진행할 8x8 크기의 블럭들
	Size blockSize(8, 8);
	vector<Mat> ori_blocks;
	int value[1024];      // QR의 삽입 값을 저장할 배열
	int i = 0;


	// 256x256 크기의 부대역을 1024개의 8x8 블럭 사이즈로 분할
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x, y, blockSize.width, blockSize.height);
			ori_blocks.push_back(Mat(ori, rect));
			// QR의 삽입 값을 지정
			value[i++] = ((int)Qr_Pixel.at<uchar>((int)(y / 8), (int)(x / 8)) > 125 ? 3 : -3);
		}
	}

	int x = 0, y = 0;	
	// 1024개의 8*8 블록에 dct 적용 후 값 출력
	for (int i = 0; i < 1024; i++)
	{
		dct(ori_blocks[i], ori_blocks[i]);
	}

	vector<int> tmp;
	// 각 부대역의 1024개의 8*8 블럭들을 대상으로 워터마크 데이터 삽입 진행
	//1024개의 각 8*8블럭 DC계수(Z)에 CRT 적용하여 워터마크 삽입
	for (int i = 0; i < 1024; i++)
	{
		int Z = (int)ori_blocks[i].at<float>(0, 0); // dc값 가져옴
		int p = Z % m;
		int q = Z % n;

		int d = abs(p - q);
		int b = p + q;

		//Watermark bit = '0'
		if (value[i] == -3) // black
		{
			if (b < (D + 35) / 2) // d < D/2 b < (D+35) / 2
			{
				ori_blocks[i].at<float>(0, 0) = (float)Z;
			}
			else
			{
				for (int j = 8; j < 256; j = j + 8)
				{
					if (Z + j < 4080)  // Z' = Z + 8
					{
						p = (Z + j) % m;               // Z'에 대한 p,q,d 구하기
						q = (Z + j) % n;
						d = abs(p - q);
						b = p + q;
						if (b < (D + 35) / 2)
						{
							Z = Z + j;
							ori_blocks[i].at<float>(0, 0) = (float)Z;
							break;
						}
					}
					//Sub : - 8
					if (Z - j > 0) //Z' = Z -8
					{
						p = (Z - j) % m;             
						q = (Z - j) % n;
						d = abs(p - q);
						b = p + q;
						if (b < (D + 35) /2)
						{              
							Z = Z - j;                  
							ori_blocks[i].at<float>(0, 0) = (float)Z;
							break;
						}
					}
				}
			}
		}
		//Watermark bit '1'
		else
		{
			if (b >= (D + 35) / 2) // b >= (D+35) / 2    d >= D / 2
			{
				ori_blocks[i].at<float>(0, 0) = (float)Z;
			}
			else
			{
				for (int j = 8; j < 256; j = j + 8)
				{
					if (Z + j < 4080)   // Z' = Z + 8
					{
						p = (Z + j) % m;
						q = (Z + j) % n;
						d = abs(p - q);
						b = p + q;
						if (b >= (D + 35) / 2)  //b >= (D + 35)/2    d >= D / 2
						{
							Z = Z + j;                
							ori_blocks[i].at<float>(0, 0) = (float)Z;
							break;
						}
					}
					if (Z - j > 0)
					{
						p = (Z - j) % m;
						q = (Z - j) % n;
						d = abs(p - q);
						b = p + q;
						if (b >= (D + 35) / 2)
						{
							Z = Z - j;               
							ori_blocks[i].at<float>(0, 0) = (float)Z;
							break;
						}
					}
				}
			}
		}
		dct(ori_blocks[i], ori_blocks[i], DCT_INVERSE);               //IDCT
	} 

	ori.convertTo(yuv_arr[0], CV_8U);               //float -> uchar
	merge(yuv_arr, yuv);

	cvtColor(yuv, yuv, COLOR_YCrCb2RGB); // YCrCb to RGB

	//imshow("* DCT-CRT Marked Image", yuv);

	// 압축률 지정을 위한 부분
	vector<int> param75 = vector<int>(2);
	param75[0] = 1;// CV_IMWRITE_JPEG_QUALITY;
	param75[1] = 75;//default(95) 0-100

	vector<int> param95 = vector<int>(2);
	param95[0] = 1;// CV_IMWRITE_JPEG_QUALITY;
	param95[1] = 95;//default(95) 0-100

	//imshow("* DWT-DCT-CRT Marked Image", yuv);
	imwrite("C:/Users/KJY/Desktop/result/[black]DCT_CRT.png", yuv);
	imwrite("C:/Users/KJY/Desktop/result/[black]DCT_CRT_75.jpg", yuv, param75);
	imwrite("C:/Users/KJY/Desktop/result/[black]DCT_CRT_95.jpg", yuv, param95);

	waitKey(0);
	return yuv;
} 
*/

/* DWT-DCT-CRT */
float Embed_CRT(int dc, int value) 
{
	int Z = dc; // dc값 가져옴  // 단색일때 Z값 모두 0
	int p = Z % m;				// p=0
	int q = Z % n;				// q=0

	int d = abs(p - q);			// 
	int b = p + q;				// b=0

	//Watermark bit = '0'
	if (value == -3) // black
	{
		if (b < (D + 35) / 2) // d < D/2 b < (D+35) / 2	// 단색일때 0 < 70.5 이므로 Z=0 그대로 출력
		{
			return (float)Z;
		}
		else
		{
			for (int j = 8; j < 256; j = j + 8)
			{
				if (Z + j < DC_BOUNDARY)
				{
					p = (Z + j) % m; 
					q = (Z + j) % n;
					d = abs(p - q);
					b = p + q;
					if (b < (D + 35) / 2)
					{
						Z = Z + j;
						return (float)Z;
						break;
					}
				}
				if (Z - j > 0)
				{
					p = (Z - j) % m; 
					q = (Z - j) % n;
					d = abs(p - q);
					b = p + q;
					if (b < (D + 35) / 2)
					{            
						Z = Z - j; 
						return (float)Z;
						break;
					}
				}
			}
		}
	}
	else
	{
		if (b >= (D + 35) / 2) // d < D/2 b < (D+35) / 2	// 단색일때 0 < 70.5 이므로 else로 넘어감
		{
			return (float)Z + EMBED_VALUE;
		}
		else
		{
			for (int j = 8; j < 256; j = j + 8)
			{
				if (Z + j < DC_BOUNDARY)
				{
					p = (Z + j) % m;
					q = (Z + j) % n;
					d = abs(p - q);
					b = p + q;
					if (b >= (D + 35) / 2)
					{
						Z = Z + j;  
						return (float)Z + EMBED_VALUE;
						break;
					}
				}
				if (Z - j > 0)
				{
					p = (Z - j) % m;
					q = (Z - j) % n;
					d = abs(p - q);
					b = p + q;
					if (b >= (D + 35) / 2)
					{
						Z = Z - j;
						return (float)Z + EMBED_VALUE;
						break;
					}
				}
			}
		}
	}

}

Mat Embed(Mat& HostImg, Mat& QrImg)
{
	Mat yuv;
	vector<Mat> yuv_arr(3);
	Mat WT_result;
	Mat IWT_result;
	Mat Qr_Pixel = Mat(QrImg.rows, QrImg.cols, QrImg.type()); // 32x32 QRcode 각 픽셀 값을 255, 0으로 저장할 행렬 변수 생성

	// QR의 데이터를 0과 255로 설정
	for (int y = 0; y < QrImg.rows; y++)
	{
		for (int x = 0; x < QrImg.cols; x++)
		{
			Qr_Pixel.at<uchar>(y, x) = ((int)QrImg.at<uchar>(y, x) > 125) ? 255 : 0;
		}
	}

	cvtColor(HostImg, yuv, COLOR_RGB2YCrCb);    // RGV to YCrCb
	split(yuv, yuv_arr); // 채널 분리

	WT(yuv_arr[0], WT_result, 1); // Y채-널을 대상으로 1단계 DWT 진행

	// 부대역의 계수를 저장할 행렬 변수    
//	Mat HH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
//	Mat HL_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
	Mat LH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());

//	HH_subband = WT_result(Rect(WT_result.cols / 2, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));
//	HL_subband = WT_result(Rect(WT_result.cols / 2, 0, WT_result.cols / 2, WT_result.rows / 2));
	LH_subband = WT_result(Rect(0, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2)); // real L
//	LL_subband = WT_result(Rect(0, 0, WT_result.rows / 2, WT_result.cols / 2)); // LL

	// DCT를 진행할 8x8 크기의 블럭들
	Size blockSize(8, 8);
//	vector<Mat> HH_blocks; // 각 부대역의 블럭들
//	vector<Mat> HL_blocks;
	vector<Mat> LH_blocks;
	int value[1024]; // QR의 삽입 값을 저장할 배열
	int i = 0;
	// 256x256 크기의 부대역을 1024개의 8x8 블럭 사이즈로 분할
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x, y, blockSize.width, blockSize.height);
//			HH_blocks.push_back(Mat(HH_subband, rect));
//			HL_blocks.push_back(Mat(HL_subband, rect));
			LH_blocks.push_back(Mat(LH_subband, rect));
			// QR의 삽입 값을 지정
			value[i++] = ((int)Qr_Pixel.at<uchar>((int)(y / 8), (int)(x / 8)) > 125 ? 3 : -3);
		}
	}

	// 1024개의 8*8 블록에 dct 적용
	int enter = 0;
//	cout << "Original DC value" << endl;
	for (int i = 0; i < 1024; i++)
	{
//		dct(HL_blocks[i], HL_blocks[i]);
//		dct(HH_blocks[i], HH_blocks[i]);
		dct(LH_blocks[i], LH_blocks[i]);
	//	cout << (int)LH_blocks[i].at<float>(0, 0) << " ";
		enter++;
		if (enter == 32)
		{
	//		cout << endl;
			enter = 0;
		}
	}

	// 각 부대역의 1024개의 8*8 블럭들을 대상으로 워터마크 데이터 삽입 진행
	//1024개의 각 8*8블럭 DC계수(Z)에 CRT 적용하여 워터마크 삽입
	enter = 0;
//	cout << endl << "After Watermark Embedding DC Value" << endl;
	for (int i = 0; i < 1024; i++)
	{
//		HH_blocks[i].at<float>(0, 0) = Embed_CRT((int)HH_blocks[i].at<float>(0, 0), value[i]);
//		HL_blocks[i].at<float>(0, 0) = Embed_CRT((int)HL_blocks[i].at<float>(0, 0), value[i]);
		LH_blocks[i].at<float>(0, 0) = Embed_CRT((int)LH_blocks[i].at<float>(0, 0), value[i]);
//		cout << (int)LH_blocks[i].at<float>(0, 0) << " ";
		enter++;
		if (enter == 32)
		{
//			cout << endl;
			enter = 0;
		}

//		dct(HH_blocks[i], HH_blocks[i], DCT_INVERSE);
//		dct(HL_blocks[i], HL_blocks[i], DCT_INVERSE);
		dct(LH_blocks[i], LH_blocks[i], DCT_INVERSE);
	}

	// IWT 수행
	IWT(WT_result, IWT_result, 1);
	IWT_result.convertTo(yuv_arr[0], CV_8U);
	merge(yuv_arr, yuv);

	cvtColor(yuv, yuv, COLOR_YCrCb2RGB); // YCrCb to RGB


   // 압축률 지정을 위한 부분
	vector<int> param75 = vector<int>(2);
	param75[0] = 1;// CV_IMWRITE_JPEG_QUALITY;
	param75[1] = 75;//default(95) 0-100

	vector<int> param95 = vector<int>(2);
	param95[0] = 1;// CV_IMWRITE_JPEG_QUALITY;
	param95[1] = 95;//default(95) 0-100

	//imshow("* DWT-DCT-CRT Marked Image", yuv);
	imwrite("C:/Users/KJY/Desktop/insert.png", yuv);
	imwrite("C:/Users/KJY/Desktop/insert_75.jpg", yuv, param75);
	imwrite("C:/Users/KJY/Desktop/insert_95.jpg", yuv, param95);

	return yuv;
}

/* DCT-CRT 코드 512512
Mat Embed(Mat& HostImg, Mat& QrImg)
{
	Mat yuv;
	vector<Mat> yuv_arr(3);
	Mat Qr_Pixel = Mat(QrImg.rows, QrImg.cols, QrImg.type()); // 32x32 QRcode 각 픽셀 값을 255, 0으로 저장할 행렬 변수 생성

	// QR의 데이터를 0과 255로 설정
	for (int y = 0; y < QrImg.rows; y++)
		for (int x = 0; x < QrImg.cols; x++)
			Qr_Pixel.at<uchar>(y, x) = ((int)QrImg.at<uchar>(y, x) > 125) ? 255 : 0;

	cvtColor(HostImg, yuv, COLOR_RGB2YCrCb);    // RGV to YCrCb
	split(yuv, yuv_arr);                  // 채널 분리
	Mat ori;
	yuv_arr[0].convertTo(ori, CV_32F);      // type 0 -> type 5

	// DCT를 진행할 8x8 크기의 블럭들
	Size blockSize(8, 8);
	vector<Mat> ori_blocks;
	int value[1024];      // QR의 삽입 값을 저장할 배열
	int i = 0;


	// 256x256 크기의 부대역을 1024개의 8x8 블럭 사이즈로 분할
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x + 128, y + 128, blockSize.width, blockSize.height);
			ori_blocks.push_back(Mat(ori, rect));
			// QR의 삽입 값을 지정
			value[i++] = ((int)Qr_Pixel.at<uchar>((int)(y / 8), (int)(x / 8)) > 125 ? 3 : -3);
		}
	}

	int x = 0, y = 0;
	// 1024개의 8*8 블록에 dct 적용 후 값 출력
	for (int i = 0; i < 1024; i++)
	{
		dct(ori_blocks[i], ori_blocks[i]);
	}

	vector<int> tmp;
	// 각 부대역의 1024개의 8*8 블럭들을 대상으로 워터마크 데이터 삽입 진행
	//1024개의 각 8*8블럭 DC계수(Z)에 CRT 적용하여 워터마크 삽입
	for (int i = 0; i < 1024; i++)
	{
		int Z = (int)ori_blocks[i].at<float>(0, 0); // dc값 가져옴
		int p = Z % m;
		int q = Z % n;

		int d = abs(p - q);
		int b = p + q;
		
		//Watermark bit = '0'
		if (value[i] == -3) // black
		{
			if (b < (D + 35) / 2) // d < D/2 b < (D+35) / 2
			{
				ori_blocks[i].at<float>(0, 0) = (float)Z;
			}
			else
			{
				for (int j = 8; j < 256; j = j + 8)
				{
					if (Z + j < 4080)  // Z' = Z + 8
					{
						p = (Z + j) % m;               // Z'에 대한 p,q,d 구하기
						q = (Z + j) % n;
						d = abs(p - q);
						b = p + q;
						if (b < (D + 35) / 2)
						{
							Z = Z + j;
							ori_blocks[i].at<float>(0, 0) = (float)Z;
							break;
						}
					}
					//Sub : - 8
					if (Z - j > 0) //Z' = Z -8
					{
						p = (Z - j) % m;
						q = (Z - j) % n;
						d = abs(p - q);
						b = p + q;
						if (b < (D + 35) /2)
						{
							Z = Z - j;
							ori_blocks[i].at<float>(0, 0) = (float)Z;
							break;
						}
					}
				}
			}
		}
		//Watermark bit '1'
		else
		{
			if (b >= (D + 35) / 2) // b >= (D+35) / 2    d >= D / 2
			{
				ori_blocks[i].at<float>(0, 0) = (float)Z;
			}
			else
			{
				for (int j = 8; j < 256; j = j + 8)
				{
					if (Z + j < 4080)   // Z' = Z + 8
					{
						p = (Z + j) % m;
						q = (Z + j) % n;
						d = abs(p - q);
						b = p + q;
						if (b >= (D + 35) / 2)  //b >= (D + 35)/2    d >= D / 2
						{
							Z = Z + j;
							ori_blocks[i].at<float>(0, 0) = (float)Z;
							break;
						}
					}
					if (Z - j > 0)
					{
						p = (Z - j) % m;
						q = (Z - j) % n;
						d = abs(p - q);
						b = p + q;
						if (b >= (D + 35) / 2)
						{
							Z = Z - j;
							ori_blocks[i].at<float>(0, 0) = (float)Z;
							break;
						}
					}
				}
			}
		}
		dct(ori_blocks[i], ori_blocks[i], DCT_INVERSE);               //IDCT
	}

	ori.convertTo(yuv_arr[0], CV_8U);               //float -> uchar
	merge(yuv_arr, yuv);

	cvtColor(yuv, yuv, COLOR_YCrCb2RGB); // YCrCb to RGB

	//imshow("* DCT-CRT Marked Image", yuv);

	// 압축률 지정을 위한 부분
	vector<int> param75 = vector<int>(2);
	param75[0] = 1;// CV_IMWRITE_JPEG_QUALITY;
	param75[1] = 75;//default(95) 0-100

	vector<int> param95 = vector<int>(2);
	param95[0] = 1;// CV_IMWRITE_JPEG_QUALITY;
	param95[1] = 95;//default(95) 0-100

	//imshow("* DWT-DCT-CRT Marked Image", yuv);
	imwrite("C:/Users/KJY/Desktop/result/[lena512]DCT_CRT.png", yuv);
	imwrite("C:/Users/KJY/Desktop/result/[lena512]DCT_CRT_75.jpg", yuv, param75);
	imwrite("C:/Users/KJY/Desktop/result/[lena512]DCT_CRT_95.jpg", yuv, param95);

	waitKey(0);
	return yuv;
}
*/