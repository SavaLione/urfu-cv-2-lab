#include "optical_flow.h"

#include <algorithm>
#include <cstddef>
#include <exception>
#include <opencv2/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <random>
#include <spdlog/spdlog.h>
#include <vector>

bool BOOL_EXIT = false;

struct point
{
	int x;
	int y;
};

// bool is_in_circle(int cx, int cy, int r, int x, int y)
bool is_in_circle(point x1, point x2, int r)
{
	int dist = (x1.x - x2.x) * (x1.x - x2.x) + (x1.y - x2.y) * (x1.y - x2.y);
	if(dist <= r * r)
		return true;
	else
		return false;
}

// float my_max(float const& x, float const& y)
// {
// 	if(x + y)

// }

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

	// Objects
	// struct point
	// {
	// 	int x;
	// 	int y;
	// };

	point window_size = {640, 480};
	int circle_radius = 30;

	std::vector<point> objects;

	// Set seed
	std::mt19937::result_type const seed_value = 1234;

	std::mt19937 my_rng;
	my_rng.seed(seed_value);

	std::uniform_int_distribution<std::mt19937::result_type> dist_x(0, window_size.x);
	std::uniform_int_distribution<std::mt19937::result_type> dist_y(0, window_size.y);

	// objects.push_back({120, 120});
	// objects.push_back({20, 98});
	// objects.push_back({300, 300});

	int objects_size = 16;

	for(std::size_t i = 0; i < objects_size; i++)
	{
		point random_circle;
		random_circle.x = dist_x(my_rng);
		random_circle.y = dist_y(my_rng);
		objects.push_back(random_circle);
	}

	int time = 0;

	while(!BOOL_EXIT)
	{
		try
		{
			if(old_frame.empty())
			{
				while(!old_frame.empty())
				{
					video_capture_device >> old_frame;
				}

				cv::cvtColor(old_frame, old_gray, cv::COLOR_BGR2GRAY);
				goodFeaturesToTrack(old_gray, p0, 100, 0.3, 7, cv::Mat(), 7, false, 0.04);

				// Create a mask image for drawing purposes
				cv::Mat mask = cv::Mat::zeros(old_frame.size(), old_frame.type());
			}

			if(time > 60)
			// if(time > 0)
			{
				time = 0;
				mask = cv::Mat::zeros(old_frame.size(), old_frame.type());
			}

			// if(p0.size() >= 2)
			// {
			// 	p0.clear();
			// }

			// if(p1.size() >= 2)
			// {
			// 	p1.clear();
			// }

			// mask = cv::Mat::zeros(old_frame.size(), old_frame.type());

			cv::Mat frame;
			cv::Mat frame_gray;

			video_capture_device >> frame;
			if(frame.empty())
			{
				throw std::runtime_error("Frame is empty");
			}

			cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

			// calculate optical flow
			std::vector<uchar> status;
			std::vector<float> err;
			cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.03);
			// cv::calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, cv::Size(15, 15), 2, criteria);
			// cv::calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, cv::Size(15, 15), 2, criteria);
			cv::Mat flow;
			// cv::FarnebackOpticalFlow(old_gray, frame_gray, flow, status, err, cv::Size(15, 15), 2, criteria);
			cv::calcOpticalFlowFarneback(old_gray, frame_gray, flow, 0.5, 5, 5, 3, 7, 1.5, cv::OPTFLOW_FARNEBACK_GAUSSIAN);

			int step = 20;

			// for(std::size_t y = 0; y < flow.rows; y++)
			for(std::size_t y = 0; y < flow.rows; y += step)
			{
				// for(std::size_t x = 0; x < flow.cols; x++)
				for(std::size_t x = 0; x < flow.cols; x += step)
				{
					// cv::line(frame, {(int)x, (int)y}, cv::Vec2i(x, y), cv::Scalar(255, 255, 255));
					// cv::line(frame, {(int)x, (int)y}, cv::Vec2i(x + flow.depth(), y + flow.depth()), cv::Scalar(255, 255, 255));

					// cv::line(frame, {(int)y, (int)x}, cv::Vec2i(x + flow.at<cv::Vec2f>(y, x)[1], y + flow.at<cv::Vec2f>(y, x)[0]), cv::Scalar(255, 255, 255));

					// if(flow)
					// {

					// }
					// else
					// {

					// }

					// cv::line(frame, {(int)x, (int)y}, {(int)x, (int)y}, cv::Scalar(255, 255, 255));
					// cv::line(frame, {(int)x, (int)y}, cv::Vec2i(x + flow.at<cv::Vec2f>(x, y)[0], y + flow.at<cv::Vec2f>(x, y)[0]), cv::Scalar(255, 255, 255));
					// cv::line(frame, {(int)x, (int)y}, cv::Vec2i(x + 10, y + 10), cv::Scalar(255, 255, 255));

					// if(flow.at<cv::Vec2f>(x, y)[0] == 0 && flow.at<cv::Vec2f>(x, y)[1] == 0)
					// {

					// }
					// else
					// {
					// 	cv::line(frame, {(int)x, (int)y}, cv::Vec2i(x + 10, y + 10), cv::Scalar(255, 255, 255));
					// }

					int v_x = x + flow.at<cv::Vec2f>(y, x)[0];
					int v_y = y + flow.at<cv::Vec2f>(y, x)[1];

					//if(v_x > 640)
					//	v_x = 640;

					v_x = std::min(v_x, 639);
					v_y = std::min(v_y, 639);

					v_x = std::max(v_x, 0);
					v_y = std::max(v_y, 0);

					// cv::line(
					// 	frame,
					// 	{(int)x, (int)y},
					// 	cv::Vec2i(x + flow.at<cv::Vec2f>(x, y)[0], y + flow.at<cv::Vec2f>(x, y)[0]),
					// 	cv::Scalar(255, 255, 255));
					cv::line(
						frame,
						{(int)x, (int)y},
						cv::Vec2i(v_x, v_y),
						cv::Scalar(255, 255, 255));
				}
			}

			// std::vector<cv::Point2f> good_new;
			// // Do we have any good points?
			// bool fl = false;
			// for(uint i = 0; i < p0.size(); i++)
			// {
			// 	// Select good points
			// 	if(status[i] == 1)
			// 	{
			// 		good_new.push_back(p1[i]);
			// 		// draw the tracks
			// 		line(mask, p1[i], p0[i], colors[i], 2);
			// 		circle(frame, p1[i], 5, colors[i], -1);

			// 		fl = true;
			// 	}
			// }

			// if(!fl)
			// {
			// 	throw std::runtime_error("We do not have any good points");
			// }

			cv::Mat img;
			add(frame, mask, img);

			// // Objects
			// for(std::size_t i = 0; i < objects.size(); i++)
			// {
			// 	for(std::size_t z = 0; z < good_new.size(); z++)
			// 	{
			// 		point x1;
			// 		point x2 = {(int)good_new[z].x, (int)good_new[z].y};
			// 		if(is_in_circle(objects[i], x2, circle_radius))
			// 		{
			// 			spdlog::info("point x: {} y: {} - is in circle", objects[i].x, objects[i].y);
			// 			while(is_in_circle(objects[i], x2, circle_radius))
			// 			{
			// 				objects[i].x++;
			// 				objects[i].y++;
			// 			}
			// 		}
			// 	}
			// }

			// // Objects
			// for(std::size_t i = 0; i < objects.size(); i++)
			// {
			// 	//Color of the circle
			// 	cv::Scalar line_color(255, 255, 255);
			// 	cv::circle(frame, {objects[i].x, objects[i].y}, circle_radius, line_color, 4, 8);
			// }

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
			// p0		 = good_new;
		}
		catch(std::exception const &e)
		{
			spdlog::error(e.what());
		}
		catch(...)
		{
			spdlog::error("Unexpected error");
		}
	}

	return 0;
}