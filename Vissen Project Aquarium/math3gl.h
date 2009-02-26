#ifndef math3gl_h_
#define math3gl_h_
#include "math3.h"
#include "GL/gl.h"
/* (C) 2004 by Dmytry Lavrov.
contact: dmytry lavrov at fastmail dot fm
 You can use it almost freely but
 1: don't [re]move that comment block from sources.
 2: if you distibute this code,or derived works in binary form,put somewhere
 information that i wrote it.
 No warranty at all,use that at your own risk.
*/
// uses prefix glm for each thing.
inline void glmVertex(const math3::Vec3d &v){
	glVertex3d(v.x, v.y, v.z);
}
inline void glmVertex(const math3::Vec3f &v){
	glVertex3f(v.x, v.y, v.z);
}
inline void glmNormal(const math3::Vec3d &v){
	glNormal3d(v.x, v.y, v.z);
}
inline void glmNormal(const math3::Vec3f &v){
	glNormal3f(v.x, v.y, v.z);
}

template<class T> void glmSetTransformFrom(const math3::CoordSys<T> &c1){
        GLdouble m[16];
        math3::Matrix3x3<T> r;
        r=math3::QuaternionToMatrix(c1.orientation);
        m[0]=r.a[0][0];
        m[1]=r.a[1][0];
        m[2]=r.a[2][0];
        m[3]=0;

        m[4]=r.a[0][1];
        m[5]=r.a[1][1];
        m[6]=r.a[2][1];
        m[7]=0;

        m[8]=r.a[0][2];
        m[9]=r.a[1][2];
        m[10]=r.a[2][2];
        m[11]=0;

        m[12]=c1.position.x;
        m[13]=c1.position.y;
        m[14]=c1.position.z;
        m[15]=1;
        glLoadMatrixd(m);
};
template<class T> void glmSetTransformTo(const math3::CoordSys<T> &c1){
        glmSetTransformFrom(Inverse(c1));
};

template<class T> void glmMultTransformFrom(const math3::CoordSys<T> &c1){
        GLdouble m[16];
        math3::Matrix3x3<T> r;
        r=math3::QuaternionToMatrix(c1.orientation);
        m[0]=r.a[0][0];
        m[1]=r.a[1][0];
        m[2]=r.a[2][0];
        m[3]=0;

        m[4]=r.a[0][1];
        m[5]=r.a[1][1];
        m[6]=r.a[2][1];
        m[7]=0;

        m[8]=r.a[0][2];
        m[9]=r.a[1][2];
        m[10]=r.a[2][2];
        m[11]=0;

        m[12]=c1.position.x;
        m[13]=c1.position.y;
        m[14]=c1.position.z;
        m[15]=1;
        glMultMatrixd(m);
};
template<class T> void glmMultTransformTo(const math3::CoordSys<T> &c1){
        glmMulTransformFrom(Inverse(c1));
};

inline void glmLoadMatrix(const math3::Matrix4x4<float> &m){
	float tmp[16];
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			tmp[i+j*4]=m.a[i][j];
		};
	};
	glLoadMatrixf(tmp);
};
inline void glmLoadMatrix(const math3::Matrix4x4<double> &m){
	double tmp[16];
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			tmp[i+j*4]=m.a[i][j];
		};
	};
	glLoadMatrixd(tmp);
};


inline void glmMultMatrix(const math3::Matrix4x4<float> &m){
	float tmp[16];
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			tmp[i+j*4]=m.a[i][j];
		};
	};
	glMultMatrixf(tmp);
};
inline void glmMultMatrix(const math3::Matrix4x4<double> &m){
	double tmp[16];
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			tmp[i+j*4]=m.a[i][j];
		};
	};
	glMultMatrixd(tmp);
};


static const GLenum GLM_MODELVIEW_MATRIX=GL_MODELVIEW_MATRIX;
static const GLenum GLM_PROJECTION_MATRIX=GL_PROJECTION_MATRIX;
static const GLenum GLM_TEXTURE_MATRIX=GL_TEXTURE_MATRIX;

inline void glmGetMatrix(GLenum mname,math3::Matrix4x4<float> &m){
	float tmp[16];
	/*if(GLM_MODELVIEW_MATRIX==mname){
		glGetFloatv(GL_MODELVIEW_MATRIX,tmp);
	}else if(GLM_PROJECTION_MATRIX==mname){
		glGetFloatv(GL_PROJECTION_MATRIX,tmp);
	}else {
		return;
	};*/
	switch(mname){
case GLM_MODELVIEW_MATRIX:
	glGetFloatv(GL_MODELVIEW_MATRIX,tmp);
	break;
case GLM_PROJECTION_MATRIX:
	glGetFloatv(GL_PROJECTION_MATRIX,tmp);
	break;
case GLM_TEXTURE_MATRIX:
	glGetFloatv(GL_TEXTURE_MATRIX,tmp);
	break;
default:
	return ;
	};
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			m.a[i][j]=tmp[i+j*4];
		};
	};
};

inline void glmGetMatrix(GLenum mname,math3::Matrix4x4<double> &m){
	double tmp[16];
	/*if(GLM_MODELVIEW_MATRIX==mname){
	glGetFloatv(GL_MODELVIEW_MATRIX,tmp);
	}else if(GLM_PROJECTION_MATRIX==mname){
	glGetFloatv(GL_PROJECTION_MATRIX,tmp);
	}else {
	return;
};*/
	switch(mname){
	case GLM_MODELVIEW_MATRIX:
		glGetDoublev(GL_MODELVIEW_MATRIX,tmp);
		break;
	case GLM_PROJECTION_MATRIX:
		glGetDoublev(GL_PROJECTION_MATRIX,tmp);
		break;
	case GLM_TEXTURE_MATRIX:
		glGetDoublev(GL_TEXTURE_MATRIX,tmp);
		break;
	default:
		std::cout<<"bug"<<std::endl;
		return ;
	};
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			m.a[i][j]=tmp[i+j*4];
		};
	};
};
#endif
