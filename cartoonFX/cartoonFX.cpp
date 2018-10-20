// cartoonFX.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "highgui.h"
#include "cv.h"

using namespace std;
using namespace cv;

int g_edge_coreSize = 5;  //1 3 5 7 9̫���Ե���ࡢ��
int g_blur_coreSize = 7;  //1 3 5 7 9̫���Ե���٣�̫С������
int g_s = 5;              //���Ͷ�
int g_v = 10;             //����
int g_d = 15;             //�˴�С��Խ��Խ��ʱ
int g_sigma = 35;         //ֵ��Ϳ�����Ϊһ����Խ��Խ�⻬

//����change�����ı����
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

//��ָ�����ڴ�����������������ĺ�����Ϊ�ص�����
void createBar(String winName){
	createTrackbar("��Ե��ϸ",winName,&g_edge_coreSize,10 ,change_edge);
	createTrackbar("��Ե���",winName,&g_blur_coreSize,10 ,change_blur);
	createTrackbar("���Ͷ�"  ,winName,&g_s            ,255,change_s);
	createTrackbar("����"    ,winName,&g_v            ,255,change_v);
	createTrackbar("������",winName,&g_d            ,50 ,change_d);
	createTrackbar("�⻬�̶�",winName,&g_sigma        ,100,change_sigma);
}

//����ͼƬ��cartoonFy����
Mat cartoonFy(Mat img)
{
	//namedWindow("src");
	//Mat img = imread("..\\test.jpg");
	//imshow("src",img);

	//1.��ͨ�˲���ƽ��ͼ��
	Mat dst;
	bilateralFilter(img,dst,g_d,g_sigma,g_sigma);
	//namedWindow("dst");
	//imshow("dst",dst);
	//***��߱��Ͷȡ�����***
	Mat hsv;
	cvtColor(dst,hsv,CV_BGR2HSV);
	vector<Mat> channels;
	split(hsv, channels);
	channels[1] += g_s;
	channels[2] += g_v;
	merge(channels, hsv);
	cvtColor(hsv,hsv,CV_HSV2BGR);
	//imshow("hsv",hsv);

	//2.��ͨ�˲���ͻ����Ե
	Mat gray;
	cvtColor(dst,gray,CV_BGR2GRAY);
	g_blur_coreSize = g_blur_coreSize / 2 * 2 + 1;
	medianBlur(gray,gray,g_blur_coreSize);

	//***��̬ѧ���� �Ǳ�Ҫ***
	//Mat element = getStructuringElement(MORPH_RECT, Size(7,7));
	//morphologyEx(gray, gray, MORPH_CLOSE, element);
	//morphologyEx(gray, gray, MORPH_OPEN, element);
	
	Mat edge;
	g_edge_coreSize = g_edge_coreSize / 2 * 2 + 1;
	Laplacian(gray,edge,CV_8U,g_edge_coreSize);
	//***��ѡ��ͬ�ı�Ե����㷨***
	//double a = 70;
	//Canny(gray,edge,a,3*a);
	threshold(edge,edge,128,255,THRESH_BINARY_INV);
	//Ҳ����ͨ�����ص������ʵ�ֶ�ֵ��
	//edge = edge < 128;
	//imshow("edge",edge);

	//3.ͼ�����
	Mat outimg;
	hsv.copyTo(outimg,edge);
	//imshow("out",outimg);
	return outimg;
}

int _tmain(int argc, _TCHAR* argv[])
{
	/*��ѡ����ͼƬ������
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	compression_params.push_back(100);
	//*/
	
	char ch[64];
	//����·��
	string path = "..\\saveImg\\";
	//����system��������·���ļ���
	system("md ..\\saveImg\\");
	//����һ�����ڽ��в�������
	namedWindow("controller",0);
	createBar("controller");
	/*
	namedWindow("out");
	while(1){
		Mat img = imread("..\\1.jpg");
		img = cartoonFy(img);
		imshow("out",img);
		//�ո񱣴�ͼƬ��esc�˳�
		char c = cvWaitKey(1);
		if(32 == c) imwrite("1.jpg",img);//�����������ɵ���ͼƬ����
		if(27 == c) break;
	}
	//*/
	//*
	VideoCapture cap(0);
	Mat frame;

	namedWindow("cartoonFX(�ո񱣴�ͼƬ��esc�˳�)");
	while(cap.isOpened()){
		if(!cap.read(frame)) break;
		frame = cartoonFy(frame);
		imshow("cartoonFX(�ո񱣴�ͼƬ��esc�˳�)",frame);
		//�ո񱣴�ͼƬ��esc�˳�
		char c = cvWaitKey(1);
		if(32 == c) {
			//�ַ�����ch��¼����ʱ�䣬��ʱ������ȷ������Ψһ
			time_t t = time(0);
			strftime(ch, sizeof(ch), "%Y-%m-%d %H-%M-%S", localtime(&t)); //��-��-�� ʱ-��-��
			imwrite(path + ch + ".jpg",frame);//�����������ɵ���ͼƬ����
		}
		if(27 == c) break;
	}
	//*/
	return 0;
}