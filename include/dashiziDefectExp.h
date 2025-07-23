#pragma once
#include <vector>
#include <opencv2/core.hpp>
namespace lzsd {
	typedef struct
	{
		double all_area;		//整体面积
		double jixing_area;		//畸形面积
		double huaguan_area;	//花冠面积
		double bingbian_area;	//病变面积
		double lieguo_area;		//裂果面积
		double ganba_area;		//干巴面积
		double zhuanse_area;	//转色面积

		int width;				//长径
		int height;				//短径

	} dashiziInfo;

	extern "C" __declspec(dllexport)  int init_dashiziDefect(std::string weights_path);
	extern "C" __declspec(dllexport)  void release_dashiziDefect();
	extern "C" __declspec(dllexport)  void push_frames_dashizi(cv::Mat& bgr_image, lzsd::dashiziInfo &infos, cv::Mat & result_img, bool is_draw);
}