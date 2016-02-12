#ifndef RECORDTEXTURE_H_
#define RECORDTEXTURE_H_

#include <iostream>

#ifdef __unix_
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <GL/glxew.h>
#include <GL/freeglut.h>

#elif defined(_WIN32)
#define GLM_FORCE_RADIANS
#include "..\thirdparties\glew-1.13.0\include\GL\glew.h"
#include "..\thirdparties\glew-1.13.0\include\GL\wglew.h"
#include "..\thirdparties\freeglut\include\GL\freeglut.h"
#endif

class RecordTexture
{
public:
	RecordTexture();
	RecordTexture(GLsizei aWidth, GLsizei aHeight, unsigned int aComponentCount);
	RecordTexture(GLsizei aWidth, GLsizei aHeight, unsigned int aComponentCount, const void* pData);

	RecordTexture& operator=(const RecordTexture& aRecordTexture);
	void ClearTexture();

	virtual ~RecordTexture();

public:
	GLuint mFboId;
	GLuint mColorRenderBuffer;
	GLuint mTextureId;
	unsigned int mComponentCount;
};

#endif /* RECORDTEXTURE_H_ */
