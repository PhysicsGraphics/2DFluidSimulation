#ifndef TEXTUREUPDATE_H_
#define TEXTUREUPDATE_H_

#include "RecordTexture.h"

class TextureUpdate
{
private:

public:
	TextureUpdate();
	TextureUpdate(GLsizei aWidth, GLsizei aHeight, unsigned int aComponentCount, const void* pData);

	void SwapInOut();

	virtual ~TextureUpdate();

public:
	RecordTexture mInTextue;
	RecordTexture mOutTextue;
};


#endif /* TEXTUREUPDATE_H_ */
