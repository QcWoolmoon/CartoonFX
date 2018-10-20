// cartoonFX.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "highgui.h"
#include "cv.h"

using namespace std;
using namespace cv;

int g_edge_coreSize = 5;  //1 3 5 7 9太大边缘过多、粗
int g_blur_coreSize = 7;  //1 3 5 7 9太大边缘减少，太小噪点过多
int g_s = 5;              //饱和度
int g_v = 10;             //亮度
int g_d = 15;             //核大小，越大越费时
int g_sigma = 35;         //值域和空域设为一样，越大越光滑

//六个change函数改变参数
void change_edge(int pos,void*){
	g_edge_coreSize = pos;
}

void change_blur(int pos,void*){
	g_blur_coreSize = pos;
}

void change_s(int pos,void*){
	g_s = pos;
}

void change_v(int pos,void*){
	g_v = pos;
}

void change_d(int pos,void*){
	g_d = pos;
}

void change_sigma(int pos,void*){
	g_sigma = pos;
}

//在指定窗口创建滚动条并把上面的函数作为回调函数
void createBar(String winName){
	createTrackbar("边缘粗细",winName,&g_edge_coreSize,10 ,change_edge);
	createTrackbar("边缘噪点",winName,&g_blur_coreSize,10 ,change_blur);
	createTrackbar("饱和度"  ,winName,&g_s            ,255,change_s);
	createTrackbar("亮度"    ,winName,&g_v            ,255,change_v);
	createTrackbar("处理精度",winName,&g_d            ,50 ,change_d);
	createTrackbar("光滑程度",winName,&g_sigma        ,100,change_sigma);
}

//单个图片的cartoonFy处理
Mat cartoonFy(Mat img)
{
	//namedWindow("src");
	//Mat img = imread("..\\test.jpg");
	//imshow("src",img);

	//1.低通滤波，平滑图像
	Mat dst;
	bilateralFilter(img,dst,g_d,g_sigma,g_sigma);
	//namedWindow("dst");
	//imshow("dst",dst);
	//***提高饱和度、亮度***
	Mat hsv;
	cvtColor(dst,hsv,CV_BGR2HSV);
	vector<Mat> channels;
	split(hsv, channels);
	channels[1] += g_s;
	channels[2] += g_v;
	merge(channels, hsv);
	cvtColor(hsv,hsv,CV_HSV2BGR);
	//imshow("hsv",hsv);

	//2.高通滤波，突出边缘
	Mat gray;
	cvtColor(dst,gray,CV_BGR2GRAY);
	g_blur_coreSize = g_blur_coreSize / 2 * 2 + 1;
	medianBlur(gray,gray,g_blur_coreSize);

	//***形态学处理 非必要***
	//Mat element = getStructuringElement(MORPH_RECT, Size(7,7));
	//morphologyEx(gray, gray, MORPH_CLOSE, element);
	//morphologyEx(gray, gray, MORPH_OPEN, element);
	
	Mat edge;
	g_edge_coreSize = g_edge_coreSize / 2 * 2 + 1;
	Laplacian(gray,edge,CV_8U,g_edge_coreSize);
	//***可选不同的边缘检测算法***
	//double a = 70;
	//Canny(gray,edge,a,3*a);
	threshold(edge,edge,128,255,THRESH_BINARY_INV);
	//也可以通过重载的运算符实现二值化
	//edge = edge < 128;
	//imshow("edge",edge);

	//3.图像组合
	Mat outimg;
	hsv.copyTo(outimg,edge);
	//imshow("out",outimg);
	return outimg;
}

int _tmain(int argc, _TCHAR* argv[])
{
	/*可选保存图片的质量
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	compression_params.push_back(100);
	//*/
	
	char ch[64];
	//保存路径
	string path = "..\\saveImg\\";
	//调用system方法创建路径文件夹
	system("md ..\\saveImg\\");
	//单独一个窗口进行参数调节
	namedWindow("controller",0);
	createBar("controller");
	/*
	namedWindow("out");
	while(1){
		Mat img = imread("..\\1.jpg");
		img = cartoonFy(img);
		imshow("out",img);
		//空格保存图片，esc退出
		char c = cvWaitKey(1);
		if(32 == c) imwrite("1.jpg",img);//第三个参数可调节图片质量
		if(27 == c) break;
	}
	//*/
	//*
	VideoCapture cap(0);
	Mat frame;

	namedWindow("cartoonFX(空格保存图片，esc退出)");
	while(cap.isOpened()){
		if(!cap.read(frame)) break;
		frame = cartoonFy(frame);
		imshow("cartoonFX(空格保存图片，esc退出)",frame);
		//空格保存图片，esc退出
		char c = cvWaitKey(1);
		if(32 == c) {
			//字符数组ch记录保存时间，用时间命名确保名称唯一
			time_t t = time(0);
			strftime(ch, sizeof(ch), "%Y-%m-%d %H-%M-%S", localtime(&t)); //年-月-日 时-分-秒
			imwrite(path + ch + ".jpg",frame);//第三个参数可调节图片质量
		}
		if(27 == c) break;
	}
	//*/
	return 0;
}