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

#ifndef __INCLUDED_ABSTRACTARRAYBASE_HPP__
#define __INCLUDED_ABSTRACTARRAYBASE_HPP__

#include <vector>
#include <Eigen/Core>
#include <Eigen/Geometry>

template <typename CoordType, std::size_t CoordinateCount, typename Derived>
class AbstractArrayBase
{
    public:
        typedef Eigen::Matrix<CoordType, CoordinateCount, 1>    value_type;
        typedef Eigen::Transform<CoordType, CoordinateCount>    transform_type;

        /** Virtual destructor to make sure that all subclasses have a virtual
         *  destructor as well.
         */
        virtual ~AbstractArrayBase() {}

        bool HasVBO() const
        {
            return static_cast<const Derived *>(this)->_HasVBO();
        }

        /** Passes all of this AbstractArrayBase's data to the OpenGL API.
         */
        void draw() const
        {
            return static_cast<const Derived *>(this)->_draw();
        }

        /** @return the amount of elements in this AbstractArrayBase
         */
        std::size_t size() const
        {
            return _data.size();
        }

        /** @return true if this AbstractArrayBase contains no vertices, false
         *          otherwise.
         */
        bool empty() const
        {
            return _data.empty();
        }

        /** @return a reference to the element at the given index
         */
        value_type& operator[](std::size_t index)
        {
            // Mark the data as changed
            static_cast<Derived*>(this)->DataChanged();

            return _data[index];
        }

        /** @return the element at the given index
         */
        const value_type& operator[](std::size_t index) const
        {
            return _data[index];
        }

        /** Appends the given element to the end of this AbstractArrayBase.
         *  @return the index given to this new element.
         */
        std::size_t push_back(const value_type& element)
        {
            _data.push_back(element);

            // Mark the data as changed
            static_cast<Derived*>(this)->DataChanged();

            return _data.size() - 1;
        }

        /** Removes all elements from this AbstractArrayBase
         */
        void clear()
        {
            _data.clear();

            // Mark the data as changed
            static_cast<Derived*>(this)->DataChanged();
        }

        void leftmultiply(const transform_type& m)
        {
            // Loop over all vertices and left-multiply them by the matrix we're given
            for (typename std::vector<value_type>::iterator
                 i  = _data.begin();
                 i != _data.end();
                 ++i)
            {
                *i = *i * m;
            }

            // Mark the data as changed
            static_cast<Derived*>(this)->DataChanged();
        }

        void multiply(const transform_type& m)
        {
            // Loop over all vertices and multiply them by the matrix we're given
            for (typename std::vector<value_type>::iterator
                 i  = _data.begin();
                 i != _data.end();
                 ++i)
            {
                *i = m * *i;
            }

            // Mark the data as changed
            static_cast<Derived*>(this)->DataChanged();
        }

    protected:
        /* Protected constructors to make sure only classes derived from
         * AbstractArrayBase can be instantiated.
         */
        AbstractArrayBase() {}
        AbstractArrayBase(const AbstractArrayBase& rhs) :
            _data(rhs._data)
        {}

    private:
        /** Holds all data.
         */
        std::vector<value_type> _data;
};

#endif // __INCLUDED_ABSTRACTARRAYBASE_HPP__
