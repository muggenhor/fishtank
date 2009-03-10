#ifndef __INCLUDED_TRIANGLEARRAY_HPP__
#define __INCLUDED_TRIANGLEARRAY_HPP__

#include <vector>
#include <boost/static_assert.hpp>
#include <eigen/vector.h>
#include <windows.h>
#include <winnt.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>
#include "gl_type_constants.hpp"
#include "vertexarray.hpp"
#include "texcoordarray.hpp"

extern bool extglDrawRangeElementsChecked;
extern PFNGLDRAWRANGEELEMENTSEXTPROC glDrawRangeElementsEXT;

template <typename IndexIntegerType, typename VertexCoordType, typename TexCoordType, std::size_t VertexCoordinateCount = 3, std::size_t TexCoordCount = 2>
class TriangleArray
{
    public:
        typedef typename VertexArray<VertexCoordType, VertexCoordinateCount>::value_type vertex_type;
        typedef typename TexCoordArray<TexCoordType, TexCoordCount>::value_type texcoord_type;

        void draw() const
        {
            // Bail out if there's nothing to draw
            if (_indices.empty())
                return;

            // Pass all of our triangle data
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            assert(_VertexArray.size() == _TexCoordArray.size());

            // Pass the VertexArray
            _VertexArray.draw();
            _TexCoordArray.draw();

#ifndef NDEBUG
            for (typename std::vector<IndexIntegerType>::const_iterator i = _indices.begin(); i != _indices.end(); ++i)
                assert(*i < _VertexArray.size());
#endif

            // Check for the glDrawRangeElements extension (speeds up fetching of Vertex Array data)
            if (!extglDrawRangeElementsChecked)
            {
                extglDrawRangeElementsChecked = true;
                if (strstr((const char*)glGetString(GL_EXTENSIONS), "GL_EXT_draw_range_elements") != NULL)
                {
                    glDrawRangeElementsEXT = (PFNGLDRAWRANGEELEMENTSEXTPROC)wglGetProcAddress("glDrawRangeElementsEXT");
                }
            }

            // Draw all contained triangles based on the array of indices
            if (glDrawRangeElementsEXT)
                glDrawRangeElementsEXT(GL_TRIANGLES, 0, _VertexArray.size() - 1, _indices.size(), OpenGLTypeConstant<IndexIntegerType>::constant, &_indices[0]);
            else
                glDrawElements(GL_TRIANGLES, _indices.size(), OpenGLTypeConstant<IndexIntegerType>::constant, &_indices[0]);

            // Disable the GL_VERTEX_ARRAY client state to prevent strange behaviour
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        std::size_t uniqueVertices() const
        {
            return _VertexArray.size();
        }

        void AddTriangle(const vertex_type* v, const texcoord_type* t)
        {
            for (unsigned int i = 0; i < 3; ++i)
                AddPoint(v[i], t[i]);
        }

        void AddTriangle(const vertex_type* v)
        {
            static texcoord_type t;
            for (unsigned int i = 0; i < static_cast<unsigned int>(t.size()); ++i)
                t[i] = 0;

            for (unsigned int i = 0; i < 3; ++i)
                AddPoint(v[i], t);
        }

    private:
        void AddPoint(const vertex_type& vertex, const texcoord_type& texcoord)
        {
            unsigned int index = 0;

            // Try to find an instance of the given vertex
            for (index = 0; index < _VertexArray.size(); ++index)
            {
                if (_VertexArray[index]  == vertex
                 && _TexCoordArray[index] == texcoord)
                {
                    // If we found a vertex that's the same then add the index
                    // to the index table and bail out
                    _indices.push_back(index);

                    return;
                }
            }

            // If we found no instance of this vertex then we'll obviously need to add it
            _VertexArray.push_back(vertex);
            _TexCoordArray.push_back(texcoord);
            _indices.push_back(index);
        }

    private:
        VertexArray<VertexCoordType, VertexCoordinateCount> _VertexArray;
        TexCoordArray<TexCoordType, TexCoordCount> _TexCoordArray;
        std::vector<IndexIntegerType> _indices;
};

#endif // __INCLUDED_IMD_HPP__
