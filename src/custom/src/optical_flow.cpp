#include "optical_flow.h"

#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>

bool BOOL_EXIT = false;

int main()
{
	/* Default video capture device */
	cv::VideoCapture video_capture_device(0);

	if(video_capture_device.isOpened() == false)
	{
		spdlog::error("Cannot open the video camera.");
		BOOL_EXIT = true;
	}

	/* Print width and height of the capture device frame */
	spdlog::info(
		"Resolution of the capture device frame: {}x{}",
		video_capture_device.get(cv::CAP_PROP_FRAME_WIDTH),
		video_capture_device.get(cv::CAP_PROP_FRAME_HEIGHT));

	std::string window_name = "OpenCV colour object detection";

	cv::namedWindow(window_name);

	while(!BOOL_EXIT)
	{
		cv::Mat frame;

		if(!video_capture_device.read(frame))
		{
			spdlog::error("Can't read the frame from a camera.");
			BOOL_EXIT = true;
		}

		/* We are showing the result */
		cv::imshow(window_name, frame);

		if(cv::waitKey(10) == 27)
		{
			spdlog::info("Esc key is pressed by user.");
			spdlog::info("Stoppig the application.");
			BOOL_EXIT = true;
		}
	}

	return 0;
}