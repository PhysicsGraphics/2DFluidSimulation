#include "TextureUpdate.h"


TextureUpdate::TextureUpdate()
	: mInTextue()
	, mOutTextue()
{
	// TODO Auto-generated constructor stub

}

TextureUpdate::TextureUpdate(GLsizei aWidth, GLsizei aHeight, unsigned int aComponentCount, const void* pData)
{
	mInTextue = RecordTexture(aWidth, aHeight, aComponentCount, pData);
	mOutTextue = RecordTexture(aWidth, aHeight, aComponentCount, NULL);

//	mInTextue = new RecordTexture(aWidth, aHeight, aComponentCount, pData);
//	mOutTextue = new RecordTexture(aWidth, aHeight, aComponentCount, NULL);
	return;
}

void
TextureUpdate::SwapInOut()
{
	RecordTexture tempTextureUpdate = mInTextue;
	mInTextue = mOutTextue;
	mOutTextue = tempTextureUpdate;
//	RecordTexture* pTempTextureUpdate = NULL;
//	pTempTextureUpdate = mInTextue;
//	mOutTextue = mInTextue;
//	mOutTextue = tempTextureUpdate;
}

TextureUpdate::~TextureUpdate()
{
//	delete mInTextue;
//	delete mOutTextue;
}
