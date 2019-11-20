#pragma once
#include "stdafx.h"
#include "Headers.h"
#define m 38
#define n 107
#define D 106

//////////////////////////////////////////////////////////////////////////////////////////
//// ����
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
	getPSNR(Marked_Image);         // ���� �̹����� ���� �̹����� PSNR �� ����� ����
	////////////////////////////////
	Mat yuv_arr[3];
	cvtColor(Marked_Image, Marked_Image, COLOR_RGB2YCrCb);   // RGB to YCrCb
	split(Marked_Image, yuv_arr);                      // ä�� �и�
	Mat Marked_Y_channel = Mat(Marked_Image.cols, Marked_Image.rows, Marked_Image.type());
	int QRcodeSize;

	yuv_arr[0].copyTo(Marked_Y_channel);                // Y ä�� �и�

	// ������ QRcode ������� ������ ���
	Mat HH_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);
	Mat LH_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);
	Mat HL_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);

	Mat WT_result;
	yuv_arr[0].convertTo(Marked_Y_channel, CV_32F);      //uchar -> float
	WT(Marked_Y_channel, WT_result, 1);               // �и��� Y ä���� ������� 1�ܰ� DWT ����

	// �δ뿪�� ����� ������ ��� ����
//	Mat HH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
//	Mat HL_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
	Mat LH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());

//	HH_subband = WT_result(Rect(WT_result.cols / 2, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));
//	HL_subband = WT_result(Rect(WT_result.cols / 2, 0, WT_result.cols / 2, WT_result.rows / 2));
	LH_subband = WT_result(Rect(0, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));

	// DCT�� ������ 8x8 ũ���� ����
	Size blockSize(8, 8);
//	vector<Mat> HH_blocks;   // �� �δ뿪�� ����
//	vector<Mat> HL_blocks;
	vector<Mat> LH_blocks;
	int i = 0;

	// 256x256 ũ���� �δ뿪�� 1024���� 8x8 �� ������� ����
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
	// �� �δ뿪�� 1024���� ������ ������� ���Ե� ���͸�ũ ���� ����
	for (int i = 0; i < 1024; i++)
	{
		// DCT ����
	//	dct(HH_blocks[i], HH_blocks[i]);
	//	dct(HL_blocks[i], HL_blocks[i]);
		dct(LH_blocks[i], LH_blocks[i]);

		// DCT ���� �� ������ DC ���� �������� ���Ե� QR �����
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

	// QRcode�� ������ ��� ���� ����
	QRcodeSize = HL_recoverd_QRcode_Pixel.rows;
//	Mat QR_HH(QRcodeSize + 2, QRcodeSize + 2, HH_recoverd_QRcode_Pixel.type(), Scalar(255));
//	Mat QR_HL(QRcodeSize + 2, QRcodeSize + 2, HL_recoverd_QRcode_Pixel.type(), Scalar(255));
	Mat QR_LH(QRcodeSize + 2, QRcodeSize + 2, LH_recoverd_QRcode_Pixel.type(), Scalar(255));

	// ������ QRcode�� �ȼ� ���� ��ġ�� �°� ����
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
	// 32x32 ũ���� QR�� ũ�� Ȯ��
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
	
	getNCC();      // ���Ե� ���͸�ũ�� ����� ���͸�ũ �� NCC �� ���
}
*/

/* DCT-CRT ���� �ڵ� 
void Extract(Mat& Marked_Image)
{
	//imshow("Marked_Image", Marked_Image);
	////////////////////////////////
	getPSNR(Marked_Image);         // ���� �̹����� ���� �̹����� PSNR �� ����� ����
	////////////////////////////////
	Mat yuv_arr[3];
	cvtColor(Marked_Image, Marked_Image, COLOR_RGB2YCrCb);   // RGB to YCrCb
	split(Marked_Image, yuv_arr);                      // ä�� �и�
	Mat Marked_Y_channel = Mat(Marked_Image.cols, Marked_Image.rows, Marked_Image.type());
	int QRcodeSize;
	
	yuv_arr[0].convertTo(Marked_Y_channel, CV_32F);    //uchar -> float
	
	Mat ori_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);

	// DCT�� ������ 8x8 ũ���� ����
	Size blockSize(8, 8);
	vector<Mat> ori_blocks;
	int i = 0;

	// 256x256 ũ���� �δ뿪�� 1024���� 8x8 �� ������� ����
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x, y, blockSize.width, blockSize.height);
			ori_blocks.push_back(Mat(Marked_Y_channel, rect));
		}
	}

	int x = 0, y = 0;
	// 1024���� 8*8 ��Ͽ� dct ���� �� �� ���
	for (int i = 0; i < 1024; i++)
	{
		dct(ori_blocks[i], ori_blocks[i]);
	}
	// �� �δ뿪�� 1024���� ������ ������� ���Ե� ���͸�ũ ���� ����
	for (int i = 0; i < 1024; i++)
	{
		int Z_ori = ori_blocks[i].at<float>(0, 0);

		int p_ori = Z_ori % m;
		int q_ori = Z_ori % n;
		int d_ori = abs(p_ori - q_ori);
		int b_ori = p_ori + q_ori;

		// ���� ���� ������� QR�� �� �κа� ���� �κ� ����
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

	// QRcode�� ������ ��� ���� ����
	QRcodeSize = ori_recoverd_QRcode_Pixel.rows;
	Mat QR_ori(QRcodeSize + 2, QRcodeSize + 2, ori_recoverd_QRcode_Pixel.type(), Scalar(255));

	// ������ QRcode�� �ȼ� ���� ��ġ�� �°� ����
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			QR_ori.at<uchar>(i + 1, j + 1) = ori_recoverd_QRcode_Pixel.at<uchar>(i, j);
		}
	}

	Mat BIG_QR_ori(100, 100, ori_recoverd_QRcode_Pixel.type(), Scalar(255));

	int nn = 0;
	// 32x32 ũ���� QR�� ũ�� Ȯ��
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

	getNCC();      // ���Ե� ���͸�ũ�� ����� ���͸�ũ �� NCC �� ���
}
*/

/* DWT-DCT-CRT �ڵ� */
float Extract_CRT(Mat blocks)
{
	int Z = blocks.at<float>(0, 0);
	if (Z == 40 || Z == 44)
	{
		//if()// ���⿡ ��� �ȼ��� ���� ���� �߰�(�ܻ� �̹������� �ش��Ű�� ����) �ϸ� �� ������
		Z += 60; 
	}
	int p = Z % m;
	int q = Z % n;

	int d = abs(p - q);
	int b = p + q;	

	// ���� ���� ������� QR�� �� �κа� ���� �κ� ����
	if (b >= (D + 35) / 2) // watermark bit 1 p=2, q=40 b=42 (D+35)/2=70.5
	{
		return 255;
	}
	else // watermark bit 0
	{
		return 0;
	}
}

void Extract(Mat& Marked_Image)
{
	//imshow("Marked_Image", Marked_Image);
	getPSNR(Marked_Image); // ���� �̹����� ���� �̹����� PSNR �� ����� ����
	getSSIM(Marked_Image); // ���� �̹����� ���� �̹����� SSIM �� ����� ����

	Mat yuv_arr[3];
	cvtColor(Marked_Image, Marked_Image, COLOR_RGB2YCrCb);   // RGB to YCrCb
	split(Marked_Image, yuv_arr);                      // ä�� �и�
	Mat Marked_Y_channel = Mat(Marked_Image.cols, Marked_Image.rows, Marked_Image.type());
	int QRcodeSize;
	
	yuv_arr[0].copyTo(Marked_Y_channel);                // Y ä�� �и�
	yuv_arr[0].convertTo(Marked_Y_channel, CV_32F);    //uchar -> float

	// ������ QRcode ������� ������ ���
//	Mat HH_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);
	Mat LH_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);
//	Mat HL_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);

	Mat WT_result;
	yuv_arr[0].convertTo(Marked_Y_channel, CV_32F);      //uchar -> float
	WT(Marked_Y_channel, WT_result, 1);               // �и��� Y ä���� ������� 1�ܰ� DWT ����
	//imshow("Extracted_Image_WT", WT_result);

	// �δ뿪�� ����� ������ ��� ����
//	Mat HH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
//	Mat HL_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
	Mat LH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());

	//HH_subband = WT_result(Rect(WT_result.cols / 2, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2)); // HH
	//HL_subband = WT_result(Rect(WT_result.cols / 2, 0, WT_result.cols / 2, WT_result.rows / 2));
	LH_subband = WT_result(Rect(0, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));
	//LL_subband = WT_result(Rect(0, 0, WT_result.rows / 2, WT_result.cols / 2)); // LL

	// DCT�� ������ 8x8 ũ���� ����
	Size blockSize(8, 8);
	//	vector<Mat> HH_blocks;   // �� �δ뿪�� ����
	//	vector<Mat> HL_blocks;
	vector<Mat> LH_blocks;

	// 256x256 ũ���� �δ뿪�� 1024���� 8x8 �� ������� ����
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x, y, blockSize.width, blockSize.height);
			//			HH_blocks.push_back(Mat(HH_subband, rect));
			//			HL_blocks.push_back(Mat(HL_subband, rect));
			LH_blocks.push_back(Mat(LH_subband, rect));
		}
	}

	int x = 0, y = 0;
	//	cout << "EXTRACTED" << endl;
	int enter = 0;
//	cout << "Extracted DC Value" << endl;
	// 1024���� 8*8 ��Ͽ� dct ���� �� �� ���
	for (int i = 0; i < 1024; i++)
	{
		//		dct(HH_blocks[i], HH_blocks[i]);
		//		dct(HL_blocks[i], HL_blocks[i]);
		dct(LH_blocks[i], LH_blocks[i]);
		//if (LH_blocks[i].at<float>(0, 0) == 40) cout << "1 "; // QR ����� �������� Ȯ��
		//else cout << "0 ";
//		cout << LH_blocks[i].at<float>(0, 0) << " ";
		enter++;
		if (enter == 32)
		{
//			cout << endl;
			enter = 0;
		}
	}

	// �� �δ뿪�� 1024���� ������ ������� ���Ե� ���͸�ũ ���� ����
	for (int i = 0; i < 1024; i++)
	{
		//		HH_recoverd_QRcode_Pixel.at<uchar>((int)(y), (int)(x)) = Extract_CRT(HH_blocks[i]);
		//		HL_recoverd_QRcode_Pixel.at<uchar>((int)(y), (int)(x++)) = Extract_CRT(HL_blocks[i]);
		LH_recoverd_QRcode_Pixel.at<uchar>((int)(y), (int)(x++)) = Extract_CRT(LH_blocks[i]);

		if (x == 32)
		{
			y++;
			x = 0;
		}
	}

	// QRcode�� ������ ��� ���� ����
	QRcodeSize = LH_recoverd_QRcode_Pixel.rows;
	//	Mat QR_HH(QRcodeSize + 2, QRcodeSize + 2, HH_recoverd_QRcode_Pixel.type(), Scalar(255));
	//Mat QR_HL(QRcodeSize + 2, QRcodeSize + 2, HL_recoverd_QRcode_Pixel.type(), Scalar(255));
	Mat QR_LH(QRcodeSize + 2, QRcodeSize + 2, LH_recoverd_QRcode_Pixel.type(), Scalar(255));

	// ������ QRcode�� �ȼ� ���� ��ġ�� �°� ����
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			//			QR_HH.at<uchar>(i + 1, j + 1) = HH_recoverd_QRcode_Pixel.at<uchar>(i, j);
			//		QR_HL.at<uchar>(i + 1, j + 1) = HL_recoverd_QRcode_Pixel.at<uchar>(i, j);
			QR_LH.at<uchar>(i + 1, j + 1) = LH_recoverd_QRcode_Pixel.at<uchar>(i, j);
		}
	}

	// 32x32 ũ���� QR�� ũ�� 100x100���� Ȯ��
//	Mat BIG_QR_HH(100, 100, HH_recoverd_QRcode_Pixel.type(), Scalar(255));
//	Mat BIG_QR_HL(100, 100, HL_recoverd_QRcode_Pixel.type(), Scalar(255));
	Mat BIG_QR_LH(100, 100, LH_recoverd_QRcode_Pixel.type(), Scalar(255));
	int nn = 0;
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			//			BIG_QR_HH.at<uchar>(n, 3 * j) = QR_HH.at<uchar>(i, j);
			//			BIG_QR_HH.at<uchar>(n, 3 * j + 1) = QR_HH.at<uchar>(i, j);
			//			BIG_QR_HH.at<uchar>(n, 3 * j + 2) = QR_HH.at<uchar>(i, j);
			//			BIG_QR_HH.at<uchar>(n + 1, 3 * j) = QR_HH.at<uchar>(i, j);
			//			BIG_QR_HH.at<uchar>(n + 1, 3 * j + 1) = QR_HH.at<uchar>(i, j);
			//			BIG_QR_HH.at<uchar>(n + 1, 3 * j + 2) = QR_HH.at<uchar>(i, j);
			//			BIG_QR_HH.at<uchar>(n + 2, 3 * j) = QR_HH.at<uchar>(i, j);
			//			BIG_QR_HH.at<uchar>(n + 2, 3 * j + 1) = QR_HH.at<uchar>(i, j);
			//			BIG_QR_HH.at<uchar>(n + 2, 3 * j + 2) = QR_HH.at<uchar>(i, j);

			//			BIG_QR_HL.at<uchar>(nn, 3 * j) = QR_HL.at<uchar>(i, j);
			//			BIG_QR_HL.at<uchar>(nn, 3 * j + 1) = QR_HL.at<uchar>(i, j);
			//			BIG_QR_HL.at<uchar>(nn, 3 * j + 2) = QR_HL.at<uchar>(i, j);
			//			BIG_QR_HL.at<uchar>(nn + 1, 3 * j) = QR_HL.at<uchar>(i, j);
			//			BIG_QR_HL.at<uchar>(nn + 1, 3 * j + 1) = QR_HL.at<uchar>(i, j);
			//			BIG_QR_HL.at<uchar>(nn + 1, 3 * j + 2) = QR_HL.at<uchar>(i, j);
			//			BIG_QR_HL.at<uchar>(nn + 2, 3 * j) = QR_HL.at<uchar>(i, j);
			//			BIG_QR_HL.at<uchar>(nn + 2, 3 * j + 1) = QR_HL.at<uchar>(i, j);
			//			BIG_QR_HL.at<uchar>(nn + 2, 3 * j + 2) = QR_HL.at<uchar>(i, j);

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
	//imshow("DWT_DCT_CRT_HL_QRcode", BIG_QR_HL);
	imshow("DWT_DCT_CRT_LH_QRcode", BIG_QR_LH);

	//	imwrite("HH_QRcode.png", BIG_QR_HH);
	//	imwrite("HL_QRcode.png", BIG_QR_HL);
	imwrite("C:/Users/KJY/Desktop/result/m65n361/DWT_DCT_CRT_LH_QRcode.png", BIG_QR_LH);
	imwrite("C:/Users/KJY/Desktop/result/m65n361/DWT_DCT_CRT_LH_QRcode3232.png", LH_recoverd_QRcode_Pixel);

	getNCC();      // ���Ե� ���͸�ũ�� ����� ���͸�ũ �� NCC �� ���
}

/* DCT-CRT ���� 512512
void Extract(Mat& Marked_Image)
{
	//imshow("Marked_Image", Marked_Image);
	////////////////////////////////
	getPSNR(Marked_Image);         // ���� �̹����� ���� �̹����� PSNR �� ����� ����
	//getSSIM(Marked_Image);
	////////////////////////////////
	Mat yuv_arr[3];
	cvtColor(Marked_Image, Marked_Image, COLOR_RGB2YCrCb);   // RGB to YCrCb
	split(Marked_Image, yuv_arr);                      // ä�� �и�
	Mat Marked_Y_channel = Mat(Marked_Image.cols, Marked_Image.rows, Marked_Image.type());
	int QRcodeSize;

	yuv_arr[0].convertTo(Marked_Y_channel, CV_32F);    //uchar -> float

	Mat ori_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);

	// DCT�� ������ 8x8 ũ���� ����
	Size blockSize(8, 8);
	vector<Mat> ori_blocks;
	int i = 0;

	// 256x256 ũ���� �δ뿪�� 1024���� 8x8 �� ������� ����
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x + 128, y + 128, blockSize.width, blockSize.height);
			ori_blocks.push_back(Mat(Marked_Y_channel, rect));
		}
	}

	int x = 0, y = 0;
	// 1024���� 8*8 ��Ͽ� dct ���� �� �� ���
	for (int i = 0; i < 1024; i++)
	{
		dct(ori_blocks[i], ori_blocks[i]);
	}
	// �� �δ뿪�� 1024���� ������ ������� ���Ե� ���͸�ũ ���� ����
	for (int i = 0; i < 1024; i++)
	{
		int Z_ori = ori_blocks[i].at<float>(0, 0);
		if (Z_ori == 40 || Z_ori == 44)
		{
			//if()// ���⿡ ��� �ȼ��� ���� ���� �߰�(�ܻ� �̹������� �ش��Ű�� ����) �ϸ� �� ������
			Z_ori += 60;
		}
		int p_ori = Z_ori % m;
		int q_ori = Z_ori % n;
		int d_ori = abs(p_ori - q_ori);
		int b_ori = p_ori + q_ori;

		// ���� ���� ������� QR�� �� �κа� ���� �κ� ����
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

	// QRcode�� ������ ��� ���� ����
	QRcodeSize = ori_recoverd_QRcode_Pixel.rows;
	Mat QR_ori(QRcodeSize + 2, QRcodeSize + 2, ori_recoverd_QRcode_Pixel.type(), Scalar(255));

	// ������ QRcode�� �ȼ� ���� ��ġ�� �°� ����
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			QR_ori.at<uchar>(i + 1, j + 1) = ori_recoverd_QRcode_Pixel.at<uchar>(i, j);
		}
	}

	Mat BIG_QR_ori(100, 100, ori_recoverd_QRcode_Pixel.type(), Scalar(255));

	int nn = 0;
	// 32x32 ũ���� QR�� ũ�� Ȯ��
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

	getNCC();      // ���Ե� ���͸�ũ�� ����� ���͸�ũ �� NCC �� ���
}
*/