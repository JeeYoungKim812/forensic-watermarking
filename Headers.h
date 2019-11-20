
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <cstdlib>
#include <bitset>

using namespace std;
using namespace cv;

Mat		EmbedWatermark(Mat& original_img, Mat& QRcode);		// embed
void	ExtractWatermark(Mat& marked_img);						// extract

Mat		Embed(Mat& original_img, Mat& QrImg);
float	Embed_CRT(int dc, int value);

void	Extract(Mat& Marked_Image);
float	Extract_CRT(Mat blocks);

void	WT(Mat& img, Mat& dst, int NIter);
void	IWT(Mat& dst, Mat& idst, int NIter);

void getPSNR(Mat& Marked_Img);

long double l(double avgOriginal, double avgMarked);
long double c(float stdOriginal, float stdMarked);
long double s(Mat& Original, Mat& Marked, float avgOriginal, float avgMarked, float stdOriginal, float stdMarked);
void getSSIM(Mat& Marked_Img);
void getNCC();
float geStdDevValues(Mat& src);

// SVD¿ë ÇÔ¼ö
void calculate_SVD(Mat& src, Mat& U, Mat& S, Mat& V);

