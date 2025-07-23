#pragma once
#include <vector>
#include <opencv2/core.hpp>
namespace lzsd {
	typedef struct
	{
		double all_area;		//�������
		double jixing_area;		//�������
		double huaguan_area;	//�������
		double bingbian_area;	//�������
		double lieguo_area;		//�ѹ����
		double ganba_area;		//�ɰ����
		double zhuanse_area;	//תɫ���

		int width;				//����
		int height;				//�̾�

	} dashiziInfo;

	extern "C" __declspec(dllexport)  int init_dashiziDefect(std::string weights_path);
	extern "C" __declspec(dllexport)  void release_dashiziDefect();
	extern "C" __declspec(dllexport)  void push_frames_dashizi(cv::Mat& bgr_image, lzsd::dashiziInfo &infos, cv::Mat & result_img, bool is_draw);
}