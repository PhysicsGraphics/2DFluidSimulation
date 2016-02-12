#include "RecordTexture.h"


RecordTexture::RecordTexture()
	: mFboId(0)
	, mColorRenderBuffer(0)
	, mTextureId(0)
	, mComponentCount(0)
{
	return;
}

RecordTexture::RecordTexture(GLsizei aWidth, GLsizei aHeight, unsigned int aComponentCount)
	: mComponentCount(aComponentCount)
{
	//GLuint textureId;

	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_2D, mTextureId);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	switch (mComponentCount)
	{
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, aWidth, aHeight, 0, GL_RED, GL_FLOAT, NULL);
		break;
	case 2:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, aWidth, aHeight, 0, GL_RG, GL_FLOAT, NULL);
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, aWidth, aHeight, 0, GL_RGB, GL_FLOAT, NULL);
		break;
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, aWidth, aHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		break;
	default:
		std::cerr << "Illegal Component Count of Record Texture!" << std::endl;
	}

	int i;
	i = glGetError();
	if (i != 0)
	{
		std::cout << "Error Occurs While Creating the Texture: " << i << std::endl;
	}

	//mTextureId = textureId;
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &mColorRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mColorRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, aWidth, aHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &mFboId);
	glBindFramebuffer(GL_FRAMEBUFFER, mFboId);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
							  GL_COLOR_ATTACHMENT0,
							  GL_RENDERBUFFER, mColorRenderBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER,
						   GL_COLOR_ATTACHMENT0,
						   GL_TEXTURE_2D, mTextureId, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RecordTexture&
RecordTexture::operator=(const RecordTexture& aRecordTexture)
{
	if (this == &aRecordTexture)
	{
		return *this;
	}

	mFboId = aRecordTexture.mFboId;
	mTextureId = aRecordTexture.mTextureId;
	mComponentCount = aRecordTexture.mComponentCount;

	return *this;
}

void
RecordTexture::ClearTexture()
{
	glBindBuffer(GL_FRAMEBUFFER, mFboId);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

//	glBindBuffer(GL_FRAMEBUFFER, 0);
}

RecordTexture::~RecordTexture()
{
	// TODO Auto-generated destructor stub
}



RecordTexture::RecordTexture(GLsizei aWidth, GLsizei aHeight, unsigned int aComponentCount, const void* pData)
	: mComponentCount(aComponentCount)
{
	//GLuint textureId;

	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_2D, mTextureId);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	switch (mComponentCount)
	{
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, aWidth, aHeight, 0, GL_RED, GL_FLOAT, pData);
		break;
	case 2:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, aWidth, aHeight, 0, GL_RG, GL_FLOAT, pData);
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, aWidth, aHeight, 0, GL_RGB, GL_FLOAT, pData);
		break;
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, aWidth, aHeight, 0, GL_RGBA, GL_FLOAT, pData);
		break;
	default:
		std::cerr << "Illegal Component Count of Record Texture!" << std::endl;
	}

	int i;
	i = glGetError();
	if (i != 0)
	{
		std::cout << "Error Occurs While Creating the Texture: " << i << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &mColorRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mColorRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, aWidth, aHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &mFboId);
	glBindFramebuffer(GL_FRAMEBUFFER, mFboId);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
							  GL_COLOR_ATTACHMENT0,
							  GL_RENDERBUFFER, mColorRenderBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER,
						   GL_COLOR_ATTACHMENT0,
						   GL_TEXTURE_2D, mTextureId, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
