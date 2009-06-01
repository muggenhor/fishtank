#include "camera.hpp"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image.hpp>
#include <opencv/highgui.h>
#include "textures.hpp"

#define foreach BOOST_FOREACH

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
	/* Swap out the frame from the other thread into our local variable.
	 * Because swapping of shared_ptr's is an atomic operation we can do
	 * this without locking. After having swapped the pointer we can safely
	 * work on what it points to.
	 */
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

		// Construct a GIL view from the camera's image on which we can work
		dynamic_xy_step_transposed_type<bgr8c_view_t>::type camView(transposed_view(transposed_view(interleaved_view(frame->width, frame->height, (const bgr8_pixel_t*)frame->imageData, frame->widthStep))));

		// Apply all of our transformations to the view
		foreach (const Transformation& transform, _transformations)
			camView = transform(camView);

		// Copy the (transformed) image into memory controlled by the other thread.
		boost::shared_ptr<rgb8_image_t> img(new rgb8_image_t(camView.dimensions()));
		copy_and_convert_pixels(camView, view(*img));
		_lastFrame.swap(img);

		boost::thread::yield();
	}

	cvReleaseCapture(&capture);
}
