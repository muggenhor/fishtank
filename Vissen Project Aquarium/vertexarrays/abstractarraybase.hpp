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
#include <eigen/vector.h>
#include <eigen/matrix.h>

template <typename CoordType, std::size_t CoordinateCount, typename Derived>
class AbstractArrayBase
{
    public:
        typedef Eigen::Vector<CoordType, CoordinateCount>       value_type;
        typedef Eigen::Vector<CoordType, CoordinateCount + 1>   trans_value_type;
        typedef Eigen::Matrix<CoordType, CoordinateCount>       matrix_type;
        typedef Eigen::Matrix<CoordType, CoordinateCount + 1>   trans_matrix_type;

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

        void leftmultiply(const matrix_type& m)
        {
            value_type r;

            // Loop over all vertices and left-multiply them by the matrix we're given
            for (typename std::vector<value_type>::iterator
                 i  = _data.begin();
                 i != _data.end();
                 ++i)
            {
                // Compute the product vector of (vector *i) * (matrix m) and store it in (vector r).
                m.leftmultiply(*i, &r);

                // Store the result back in our original (vector *i)
                *i = r;
            }

            // Mark the data as changed
            static_cast<Derived*>(this)->DataChanged();
        }

        void leftmultiply(const trans_matrix_type& m)
        {
            trans_value_type r;

            // Loop over all vertices and left-multiply them by the matrix we're given
            for (typename std::vector<value_type>::iterator
                 i  = _data.begin();
                 i != _data.end();
                 ++i)
            {
                trans_value_type vTmp;

                for (unsigned int j = 0; j < CoordinateCount; ++j)
                {
                    vTmp[j] = (*i)[j];
                }

                vTmp[CoordinateCount] = static_cast<CoordType>(1);

                // Compute the product vector of (vector *i) * (matrix m) and store it in (vector r).
                m.leftmultiply(vTmp, &r);

                // Store the result back in our original (vector *i)
                for (unsigned int j = 0; j < CoordinateCount; ++j)
                {
                    (*i)[j] = r[j];
                }
            }

            // Mark the data as changed
            static_cast<Derived*>(this)->DataChanged();
        }

        void multiply(const matrix_type& m)
        {
            value_type r;

            // Loop over all vertices and multiply them by the matrix we're given
            for (typename std::vector<value_type>::iterator
                 i  = _data.begin();
                 i != _data.end();
                 ++i)
            {
                // Compute the product vector of (matrix m) * (vector *i) and store it in (vector r).
                m.multiply(*i, &r);

                // Store the result back in our original (vector *i)
                *i = r;
            }

            // Mark the data as changed
            static_cast<Derived*>(this)->DataChanged();
        }

        void multiply(const trans_matrix_type& m)
        {
            trans_value_type r;

            // Loop over all vertices and multiply them by the matrix we're given
            for (typename std::vector<value_type>::iterator
                 i  = _data.begin();
                 i != _data.end();
                 ++i)
            {
                trans_value_type vTmp;

                for (unsigned int j = 0; j < CoordinateCount; ++j)
                {
                    vTmp[j] = (*i)[j];
                }

                vTmp[CoordinateCount] = static_cast<CoordType>(1);

                // Compute the product vector of (matrix m) * (vector *i) and store it in (vector r).
                m.multiply(vTmp, &r);

                // Store the result back in our original (vector *i)
                for (unsigned int j = 0; j < CoordinateCount; ++j)
                {
                    (*i)[j] = r[j];
                }
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