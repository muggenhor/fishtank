/*
 *  Vertex Array abstraction for OpenGL
 *  Copyright (C) 2007-2008  Giel van Schijndel
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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