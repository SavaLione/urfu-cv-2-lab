#include "optical_flow.h"

#include <exception>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>
#include <vector>

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

	//std::string window_name = "OpenCV colour object detection";

	//cv::namedWindow(window_name);

	// Create some random colors
	std::vector<cv::Scalar> colors;
	cv::RNG rng;
	for(int i = 0; i < 100; i++)
	{
		int r = rng.uniform(0, 256);
		int g = rng.uniform(0, 256);
		int b = rng.uniform(0, 256);
		colors.push_back(cv::Scalar(r, g, b));
	}

	cv::Mat old_frame, old_gray;
	std::vector<cv::Point2f> p0, p1;

	// Take first frame and find corners in it
	// video_capture_device.read(old_frame);
	video_capture_device >> old_frame;
	if(old_frame.empty())
	{
		while(!old_frame.empty())
		{
			try
			{
				video_capture_device >> old_frame;
			}
			catch(...)
			{
				spdlog::info("Processing");
			}
		}
	}
	cv::cvtColor(old_frame, old_gray, cv::COLOR_BGR2GRAY);
	goodFeaturesToTrack(old_gray, p0, 100, 0.3, 7, cv::Mat(), 7, false, 0.04);

	// Create a mask image for drawing purposes
	cv::Mat mask = cv::Mat::zeros(old_frame.size(), old_frame.type());

	while(!BOOL_EXIT)
	{
		cv::Mat frame;
		cv::Mat frame_gray;

		// if(!video_capture_device.read(frame))
		// {
		// 	spdlog::error("Can't read the frame from a camera.");
		// 	BOOL_EXIT = true;
		// }

		try
		{
			video_capture_device >> frame;
			if(frame.empty())
			{
				continue;
			}
		}
		catch(std::exception &e)
		{
			spdlog::error("Error: {}", e.what());
			continue;
		}

		cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

		// calculate optical flow
		std::vector<uchar> status;
		std::vector<float> err;
		cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.03);
		cv::calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, cv::Size(15, 15), 2, criteria);

		std::vector<cv::Point2f> good_new;
		for(uint i = 0; i < p0.size(); i++)
		{
			// Select good points
			if(status[i] == 1)
			{
				good_new.push_back(p1[i]);
				// draw the tracks
				line(mask, p1[i], p0[i], colors[i], 2);
				circle(frame, p1[i], 5, colors[i], -1);
			}
			else
				continue;
		}

		cv::Mat img;
		add(frame, mask, img);

		/* We are showing the result */
		cv::imshow("Window 1", frame);
		cv::imshow("Window 2", img);

		if(cv::waitKey(10) == 27)
		{
			spdlog::info("Esc key is pressed by user.");
			spdlog::info("Stoppig the application.");
			BOOL_EXIT = true;
		}

		// Now update the previous frame and previous points
		old_gray = frame_gray.clone();
		p0		 = good_new;
	}

	return 0;
}