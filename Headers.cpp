#pragma once
#include "stdafx.h"
#include "Headers.h"


void calculate_SVD(Mat& src, Mat& U, Mat& S, Mat& V)
{
	Mat S_zero = Mat(src.rows, src.cols, src.type());
	S_zero = Scalar(0);
	V = V.t();
	SVD::compute(src, S, U, V, 4);  // full SVD
	for (int i = 0; i < src.rows; i++)
		S_zero.at<float>(i, i) = S.at<float>(i, 0);

	S = S_zero;
}

// PSNR 값 계산
void getPSNR(Mat& Marked_Img)
{
	Mat Original;
	Mat Marked;
	Mat s1;

	Original = imread("C:/Users/KJY/Desktop/result/lena.png");
	//Original = imread("C:/Users/KJY/Desktop/result/black256.png"); // dct-crt 용

	Marked_Img.copyTo(Marked);
	
	absdiff(Original, Marked, s1);	// | Original - Marked |
	s1.convertTo(s1, CV_32F);		// cannot make a square on 8 bits
	s1 = s1.mul(s1);				// | Original - Marked |^2
	
	Scalar s = sum(s1);				// element sum per channel
	
	double sse = s.val[0] + s.val[1] + s.val[2];	// sum channels
	
	double mse = sse / (double)(Original.channels() * Original.total());
	double psnr = 10.0 * log10((255 * 255) / mse);
	cout << "*************** PSNR : " << psnr << endl;
}

// NCC 값 계산
void getNCC()
{
	Mat Original_QRcode;
	Mat Extracted_QRcode;
	int x, y;
	int cnt = 0;
	//while (cnt < 3)
	//{
		Original_QRcode = imread("Original_QRCODE.png");
		//Original_QRcode = imread("Original_QRCODE3232.png");
		/*if(cnt == 0)
			Extracted_QRcode = imread("HH_QRcode.png");
		else if (cnt == 1)
			Extracted_QRcode = imread("HL_QRcode.png");
		else if (cnt == 2)*/
		//Extracted_QRcode = imread("C:/Users/KJY/Desktop/result/DWT_DCT_LH_QRcode.png"); // dwt-dct
		
		Extracted_QRcode = imread("C:/Users/KJY/Desktop/result/DWT_DCT_CRT_LH_QRcode.png"); // dwt-dct-crt
//		Extracted_QRcode = imread("C:/Users/KJY/Desktop/result/m65n361/DWT_DCT_CRT_LH_QRcode.png"); // dwt-dct-crt m65 n361
		//Extracted_QRcode = imread("C:/Users/KJY/Desktop/result/DWT_DCT_CRT_LH_QRcode3232.png"); // dwt-dct-crt 3232size QR
	//	Extracted_QRcode = imread("C:/Users/KJY/Desktop/result/DCT_CRT_QRcode.png");  // dct-crt
	//	Extracted_QRcode = imread("C:/Users/KJY/Desktop/result/m65n361/DCT_CRT_QRcode.png");  // dct-crt m65 n361

		float Value = 0.0f;
		float avgOriginal = 0.0f;
		float stdOriginal = 0.0f;
		float avgExtracted = 0.0f;
		float stdExtracted = 0.0f;

//		cout << "Original QRCode Pixel Value" << endl;
		int one = 0;
		for (y = 0; y < 100; y++)
		{
			for (x = 0; x < 100; x++)
			{
		//		cout << (int)Original_QRcode.at<char>(y, x) << " ";
				if ((int)Extracted_QRcode.at<char>(y, x) == 0) // black
					one++;
			}
		//	cout << endl;
		}
//		cout << one << endl;

//		cout << endl << "Extracted QRCode Pixel Value" << endl;
		one = 0;
		for (y = 0; y < 100; y++)
		{
			for (x = 0; x < 100; x++)
			{
			//	cout << (int)Extracted_QRcode.at<char>(y, x) << " ";
				if ((int)Extracted_QRcode.at<char>(y, x) == 0) // black
					one++;
			}
		//	cout << endl;
		}
//		cout << one << endl;

		for (y = 0; y < 100; y++)
		{
			for (x = 0; x < 100; x++)
			{
				avgOriginal += Original_QRcode.at<char>(y, x); // 원 워터마크의 모든 픽셀 값의 합
				avgExtracted += Extracted_QRcode.at<char>(y, x); // 추출된 워터마크의 모든 픽셀 값의 합
			}
		}
		avgOriginal /= 10000;
		avgExtracted /= 10000;
//		cout << endl << "NCC >> avgOriginal: " << avgOriginal << endl;
//		cout << "NCC >> avgExtracted: " << avgExtracted << endl;

		stdOriginal = geStdDevValues(Original_QRcode); // 원 영상 밝기의 표준편차
		stdExtracted = geStdDevValues(Extracted_QRcode); // 추출된 영상 밝기의 표준편차
//		cout << endl << "NCC >> stdOriginal: " << stdOriginal << endl;
//		cout << "NCC >> stdExtracted: " << stdExtracted << endl;

		for (y = 0; y < 100; y++)
		{
			for (x = 0; x < 100; x++)
			{
				Value += ((Original_QRcode.at<char>(y, x) - avgOriginal) * (Extracted_QRcode.at<char>(y, x) - avgExtracted)) / (stdOriginal * stdExtracted);
			}
		}
		Value /= 10000;
		/*if (cnt == 0)
			cout << "HH NCC : " << Value << endl;
		else if (cnt == 1)
			cout << "HL NCC : " << Value << endl;
		else if (cnt == 2)*/
			cout << "*************DWT-DCT-CRT LH NCC : " << Value << endl;
		//cout << "*************DCT-CRT NCC : " << Value << endl;
		//cnt++;
	//}
}

float geStdDevValues(Mat& src)
{
	int imageWidth = src.cols;
	int imageHeight = src.rows;

	int x, y;
	float meanValues = 0.0f;
	float tempMeanValues = 0.0f;
	float stdDevValues = 0.0f;
	float tempstdDevValues = 0.0f;

	for (y = 0; y < imageHeight; y++)
	{
		for (x = 0; x < imageWidth; x++)
		{
			float data1 = src.at<char>(y, x);
			tempMeanValues += data1;
		}
	}
	meanValues = tempMeanValues / (float)(imageHeight * imageWidth);

	for (y = 0; y < imageHeight; y++)
	{
		for (x = 0; x < imageWidth; x++)
		{
			float data2 = meanValues - src.at<char>(y, x);
			tempstdDevValues += data2 * data2;
		}
	}
	tempstdDevValues /= (float)(imageHeight * imageWidth);
	stdDevValues = (float)sqrt(tempstdDevValues);

	return stdDevValues;
}

long double l(double avgOriginal, double avgMarked)
{
	double c1 = (0.01 * 255) * (0.01 * 255);
	return ((2 * avgOriginal*avgMarked) + c1) / ((avgOriginal * avgOriginal) + (avgMarked * avgMarked) + c1);
}

long double c(float stdOriginal, float stdMarked)
{
	double c2 = (0.03 * 255) * (0.03 * 255);
	return ((2 * stdOriginal*stdMarked )+ c2) / ((stdOriginal * stdOriginal) + (stdMarked * stdMarked) + c2);
}

long double s(Mat& Original, Mat& Marked, float avgOriginal, float avgMarked, float stdOriginal, float stdMarked)
{
	double c3 = ((0.03 * 255) * (0.03 * 255)) / 2;
	double convariance = 0;

	int x = 0, y = 0;
	for (y = 0; y < 512; y++)
	{
		for (x = 0; x < 512; x++)
		{
			convariance += (Original.at<char>(y, x) - avgOriginal)*(Marked.at<char>(y, x) - avgMarked);
		}
	}
	convariance = convariance / 262144;

	return (convariance + c3) / ((stdOriginal * stdMarked) + c3);
}

// SSIM값 계산
void getSSIM(Mat& Marked_Img)
{
	Mat Original;
	Mat Marked;
	Mat s1;

	double avgOriginal = 0.0f;
	double stdOriginal = 0.0f;
	double avgMarked = 0.0f;
	double stdMarked = 0.0f;

	Original = imread("C:/Users/KJY/Desktop/result/lena.png");
	//Original = imread("C:/Users/KJY/Desktop/result/black256.png"); // dct-crt 용
	Marked_Img.copyTo(Marked);

	int x = 0, y = 0;
	for (y = 0; y < 512; y++)
	{
		for (x = 0; x < 512; x++)
		{
			avgOriginal += Original.at<char>(y, x); // 원 워터마크의 모든 픽셀 값의 합
			avgMarked += Marked.at<char>(y, x); // 추출된 워터마크의 모든 픽셀 값의 합
		}
	}
	avgOriginal /= 262144; // 512*512
	avgMarked /= 262144; // 512*512

	stdOriginal = geStdDevValues(Original); // 원 영상 밝기의 표준편차
	stdMarked = geStdDevValues(Marked); 

	long double SSIM = 0;
	SSIM += l(avgOriginal, avgMarked)*c(stdOriginal, stdMarked)*s(Original, Marked, avgOriginal, avgMarked, stdOriginal, stdMarked);

	cout << "*************** SSIM : " << SSIM << endl;

	double c3 = ((0.03 * 255) * (0.03 * 255)) / 2;
	double convariance = 0;

	x = 0, y = 0;
	for (y = 0; y < 512; y++)
	{
		for (x = 0; x < 512; x++)
		{
			convariance += (Original.at<char>(y, x) - avgOriginal)*(Marked.at<char>(y, x) - avgMarked);
		}
	}
	convariance = convariance / 262144;

	cout << "*************** SSIM : " << (2 * avgOriginal*avgMarked + (0.01 * 255) * (0.01 * 255))*(2 * convariance + (0.03 * 255) * (0.03 * 255))
		/ ((avgOriginal*avgOriginal + avgMarked * avgMarked + (0.01 * 255) * (0.01 * 255))*(stdOriginal*stdOriginal + stdMarked * stdMarked + (0.03 * 255) * (0.03 * 255)));
	cout << endl;
}