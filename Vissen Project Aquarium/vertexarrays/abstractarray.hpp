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

#include "abstractarraybase.hpp"
#include <boost/serialization/base_object.hpp>
#include "vertexbuffer.hpp"

template <typename CoordType, std::size_t CoordinateCount, bool supportVBO, typename Derived>
class AbstractArray;

template <typename CoordType, std::size_t CoordinateCount, typename Derived>
class AbstractArray<CoordType, CoordinateCount, false, Derived> : public AbstractArrayBase<CoordType, CoordinateCount, AbstractArray<CoordType, CoordinateCount, false, Derived> >
{
    public:
        typedef AbstractArrayBase<CoordType, CoordinateCount,
                AbstractArray<CoordType, CoordinateCount, false, Derived> > base_type;
        typedef typename base_type::value_type                              value_type;
        typedef typename base_type::transform_type                          transform_type;

    private:
        friend class AbstractArrayBase<CoordType, CoordinateCount,
               AbstractArray<CoordType, CoordinateCount, false, Derived> >;

        /** Passes all of this AbstractArray's data to the OpenGL API.
         */
        void _draw() const
        {
            // This is required to make sure that OpenGL can actually work with our data
            BOOST_STATIC_ASSERT(sizeof(value_type) == sizeof(CoordType[CoordinateCount]));

            return static_cast<const Derived *>(this)->glPassPointer(&(*this)[0]);
        }

        void DataChanged() {}

        bool _HasVBO() const
        {
            return false;
        }

        friend class boost::serialization::access;

        template <class Archive>
        void serialize(Archive & ar, const unsigned int /* version */)
        {
            ar & boost::serialization::base_object< AbstractArrayBase<CoordType, CoordinateCount, AbstractArray<CoordType, CoordinateCount, false, Derived> > >(*this);
        }
};

template <typename CoordType, std::size_t CoordinateCount, typename Derived>
class AbstractArray<CoordType, CoordinateCount, true, Derived> : public AbstractArrayBase<CoordType, CoordinateCount, AbstractArray<CoordType, CoordinateCount, true, Derived> >
{
    public:
        typedef AbstractArrayBase<CoordType, CoordinateCount,
                AbstractArray<CoordType, CoordinateCount, true, Derived> >  base_type;
        typedef typename base_type::value_type                              value_type;
        typedef typename base_type::transform_type                          transform_type;

        AbstractArray() :
            _vbo(VertexBufferObject::is_supported() ? new VertexBufferObject : 0),
            _vbo_updated(false)
        {}

        AbstractArray(bool use_vbo) :
            _vbo(use_vbo ? new VertexBufferObject : 0),
            _vbo_updated(false)
        {}

        void UseVBO()
        {
            if (!_vbo)
            {
                _vbo_updated = false;
                _vbo = new VertexBufferObject;
            }
        }

        void UseVA()
        {
            delete _vbo;
            _vbo = 0;
        }

    private:
        friend class AbstractArrayBase<CoordType, CoordinateCount,
               AbstractArray<CoordType, CoordinateCount, true, Derived> >;

        /** Passes all of this AbstractArray's data to the OpenGL API.
         */
        void _draw() const
        {
            // This is required to make sure that OpenGL can actually work with our data
            BOOST_STATIC_ASSERT(sizeof(value_type) == sizeof(CoordType[CoordinateCount]));

            if (_vbo)
            {
                if (!_vbo_updated)
                {
                    _vbo->bufferData(this->size() * sizeof(value_type), &(*this)[0]);
                }

                _vbo->bind();
                return static_cast<const Derived *>(this)->glPassPointer(0);
                _vbo->unbind();
            }
            else
            {
                return static_cast<const Derived *>(this)->glPassPointer(&(*this)[0]);
            }
        }

        void DataChanged()
        {
            if (base_type::empty())
            {
                if (_vbo)
                    _vbo->clear();
                _vbo_updated = true;
            }
            else
            {
                _vbo_updated = false;
            }
        }

        bool _HasVBO() const
        {
            return _vbo != 0;
        }

        friend class boost::serialization::access;

        template <class Archive>
        void serialize(Archive & ar, const unsigned int /* version */)
        {
            ar & boost::serialization::base_object< AbstractArrayBase<CoordType, CoordinateCount, AbstractArray<CoordType, CoordinateCount, true, Derived> > >(*this);
        }

        VertexBufferObject*     _vbo;
        mutable bool            _vbo_updated;
};

#endif // __INCLUDED_ABSTRACTARRAY_HPP__
