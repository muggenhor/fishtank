#ifndef __INCLUDED_CAMERA_HPP__
#define __INCLUDED_CAMERA_HPP__

#include <boost/gil/typedefs.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

// Forward declarations to allow pointers and references
class Texture;

class Camera
{
	public:
		Camera(int width, int height, int index = 0);
		~Camera();

		void update_texture(Texture& texture);

	private:
		void refreshThread(int width, int height, int cameraIndex = -1);

	private:
		boost::shared_ptr<boost::gil::rgb8_image_t> _lastFrame;
		volatile bool                               _quit;
		boost::thread                               _thread;
};

#endif // __INCLUDED_CAMERA_HPP__
