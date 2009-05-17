#include "camera.hpp"
#include <boost/bind.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image.hpp>
#include <opencv/highgui.h>
#include "textures.hpp"

using namespace boost::gil;

Camera::Camera(int const width, int const height, int const index) :
	_quit(false),
	_thread(boost::bind(&Camera::refreshThread, this, width, height, index))
{
}

Camera::~Camera()
{
	_quit = true;
	_thread.join();
}

void Camera::update_texture(Texture& texture)
{
	boost::shared_ptr<boost::gil::rgb8_image_t> frame;
	frame.swap(_lastFrame);

	if (!frame)
		return;

	if (texture._img.dimensions() != frame->dimensions())
		texture._img.recreate(frame->dimensions());
	texture._img.swap(*frame);
	texture.upload_texture();
}

void Camera::refreshThread(int const width, int const height, int const cameraIndex)
{
	CvCapture* capture = cvCreateCameraCapture(cameraIndex);

	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,  width);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, height);

	while (!_quit)
	{
		const IplImage* const frame = cvQueryFrame(capture);
		if (!frame)
			break;

		boost::shared_ptr<rgb8_image_t> img(new rgb8_image_t(frame->width, frame->height));
		copy_pixels(flipped_up_down_view(interleaved_view(frame->width, frame->height, (const bgr8_pixel_t*)frame->imageData, frame->widthStep)), view(*img));
		_lastFrame.swap(img);

		boost::thread::yield();
	}

	cvReleaseCapture(&capture);
}
