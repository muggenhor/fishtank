#ifndef __INCLUDED_CAMERA_HPP__
#define __INCLUDED_CAMERA_HPP__

#include <cassert>
#include <boost/bind.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <vector>

// Forward declarations to allow pointers and references
class Texture;

class Transformation
{
	public:
		enum VIEW_TRANSFORMATIONS
		{
			FLIP_UP_DOWN,
			FLIP_LEFT_RIGHT,
			TRANSPOSE,
			ROTATE_90CW,
			ROTATE_90CCW,
			ROTATE_180,
			SUBIMAGE,
			SUBSAMPLE,
		};

	public:
		Transformation(VIEW_TRANSFORMATIONS trans, const std::ptrdiff_t x1 = -1, const std::ptrdiff_t y1 = -1, const std::ptrdiff_t x2 = -1, const std::ptrdiff_t y2 = -1) :
			trans(trans),
			x1(x1),
			y1(y1),
			x2(x2),
			y2(y2)
		{
		}

		template <typename View>
		View operator()(const View& view) const
		{
			switch (trans)
			{
				case FLIP_UP_DOWN:
					return boost::gil::flipped_up_down_view(view);
				case FLIP_LEFT_RIGHT:
					return boost::gil::flipped_left_right_view(view);
				case TRANSPOSE:
					return boost::gil::transposed_view(view);
				case ROTATE_90CW:
					return boost::gil::rotated90cw_view(view);
				case ROTATE_90CCW:
					return boost::gil::rotated90ccw_view(view);
				case ROTATE_180:
					return boost::gil::rotated180_view(view);
				case SUBIMAGE:
					return boost::gil::subimage_view(view, x1, y1, x2, y2);
				case SUBSAMPLE:
					return boost::gil::subsampled_view(view, x1, y1);
				default:
					assert(!"Bad enum!");
			}
		}

	private:
		VIEW_TRANSFORMATIONS trans;
		std::ptrdiff_t x1, y1, x2, y2;
};

class Camera
{
	public:
		Camera(int width, int height, int index = -1);

		template <typename ForwardIterator>
		Camera(int width, int height, ForwardIterator first, ForwardIterator last, int index = -1) :
			_quit(false),
			_transformations(first, last),
			_thread(boost::bind(&Camera::refreshThread, this, width, height, index))
		{
		}

		~Camera();

		void update_texture(Texture& texture);

	private:
		void refreshThread(int width, int height, int cameraIndex = -1);

	private:
		boost::shared_ptr<boost::gil::rgb8_image_t> _lastFrame;
		volatile bool                               _quit;
		std::vector<Transformation>                 _transformations;
		boost::thread                               _thread;
};

#endif // __INCLUDED_CAMERA_HPP__
