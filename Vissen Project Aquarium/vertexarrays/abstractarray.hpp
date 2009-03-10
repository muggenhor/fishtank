#ifndef __INCLUDED_ABSTRACTARRAY_HPP__
#define __INCLUDED_ABSTRACTARRAY_HPP__

#include <vector>
#include <eigen/vector.h>

template <typename CoordType, std::size_t CoordinateCount>
class AbstractArray
{
    public:
        typedef Eigen::Vector<CoordType, CoordinateCount> value_type;

        /** Virtual destructor to make sure that all subclasses have a virtual
         *  destructor as well.
         */
        virtual ~AbstractArray() {}

        /** Passes all of this AbstractArray's data to the OpenGL API.
         */
        void draw() const
        {
            glPassPointer(&_data[0]);
        }

        /** @return the amount of elements in this AbstractArray
         */
        std::size_t size() const
        {
            return _data.size();
        }

        /** @return the element at the given index
         */
        const value_type& operator[](std::size_t index) const
        {
            return _data[index];
        }

        /** Appends the given element to the end of this AbstractArray.
         *  @return the index given to this new element.
         */
        std::size_t push_back(const value_type& element)
        {
            _data.push_back(element);
            return _data.size() - 1;
        }

    protected:
        virtual void glPassPointer(value_type const * data) const = 0;

    private:
        /** Holds all data.
         */
        std::vector<value_type> _data;
};

#endif // __INCLUDED_ABSTRACTARRAY_HPP__
