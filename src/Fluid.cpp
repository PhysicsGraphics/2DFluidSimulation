//============================================================================
// Name        : Fluid.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// FluidSimulation.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#ifdef __unix_
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <GL/glxew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#elif defined(_WIN32)
#define GLM_FORCE_RADIANS
#include "..\thirdparties\glew-1.13.0\include\GL\glew.h"
//#include "..\thirdparties\glew-1.13.0\include\GL\glxew.h""
#include "..\thirdparties\freeglut\include\GL\freeglut.h"
#include "..\thirdparties\glm\glm\glm.hpp"
#include "..\thirdparties\glm\glm\gtc\type_ptr.hpp"
#include "..\thirdparties\glm\glm\gtc\matrix_transform.hpp"
#include "..\thirdparties\glm\glm\gtx\transform.hpp"
#endif


#include "ShaderProgram.h"
#include "TextureUpdate.h"
#include "FluidSolver.h"

using namespace std;

static int SCREEN_WIDTH = 600;
static int SCREEN_HEIGHT = 600;

static int RESOLUTION_WIDTH = 600;
static int RESOLUTION_HEIGHT = 600;

FluidSolver* pSolver = NULL;
static GLuint sPostionId = 0;

GLuint gLeftButtonState;
glm::ivec2 gLastMousePosition;
static glm::vec2 gForcePostion(0.0);
static glm::vec2 gForceDirection(0.0);

std::vector<glm::vec2> quadVertices;

GLuint gVaoQuad = 0;
GLuint gVboQuadVertices = 0;
GLint uSamplerQuad = -1;

TextureUpdate* pVelocity = NULL;
TextureUpdate* pPressure = NULL;
TextureUpdate* pInk = NULL;
RecordTexture* pTempPressure = NULL;
RecordTexture* pBoundary = NULL;

ShaderProgram* pFinalShader = NULL;

float gFps = 0;
int gPreviousTime = 0;
int gFrameCount = 0;

/////////////

void calculateFPS()
{
	gFrameCount++;
	int currentTime = glutGet(GLUT_ELAPSED_TIME);

	int timeInterval = currentTime - gPreviousTime;
	if (timeInterval > 1000)
	{
		gFps = gFrameCount / (timeInterval / 1000.0f);
		gPreviousTime = currentTime;
		gFrameCount = 0;
	}
}

void InitGLEW()
{
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "There was a problem initializing GLEW. Exiting..." << std::endl;
		exit(-1);
	}

	#ifdef __unix_
	glXSwapIntervalMESA(0);
	#elif defined(_WIN32)
	wglSwapIntervalEXT(0);
	#endif
}

void Init()
{
	quadVertices.push_back(glm::vec2(1.0f, -1.0f));
	quadVertices.push_back(glm::vec2(1.0f, 1.0f));
	quadVertices.push_back(glm::vec2(-1.0f, -1.0f));
	quadVertices.push_back(glm::vec2(-1.0f, 1.0f));

	glClearColor(0.0, 0.0, 0.0, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_ALPHA);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
}

void DisplayGL()
{
	glViewport(0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT);

	//======================== ADVECT =============================
	pSolver->Advect(pVelocity->mInTextue, pVelocity->mInTextue, *pBoundary, pVelocity->mOutTextue);
	pVelocity->SwapInOut();

	//======================== DIFFUSE ============================
	for (int i = 0; i < 0; ++i)
	{
		pSolver->Diffuse(pVelocity->mInTextue, pVelocity->mInTextue, *pBoundary, pVelocity->mOutTextue);
		pVelocity->SwapInOut();
	}

	//======================== FORCE ==============================
	pSolver->ApplyForces(pVelocity->mInTextue, pVelocity->mOutTextue, gForcePostion, gForceDirection);
	pVelocity->SwapInOut();
	//gForcePostion = glm::vec2(0.0);
	gForceDirection = glm::vec2(20.0, 0.0);

	pSolver->ApplyForces(pInk->mInTextue, pInk->mOutTextue, gForcePostion, gForceDirection);
	pInk->SwapInOut();
	gForcePostion = glm::vec2(-10.0);
	gForceDirection = glm::vec2(0.0);

	//======================== DIVERGENCE =========================
	pSolver->ComputeDivergence(pVelocity->mInTextue, *pBoundary, *pTempPressure);

	//======================== PROJECTION ==========================
	pPressure->mInTextue.ClearTexture();

	for (int i = 0; i < 30; ++i)
	{
		pSolver->ProjectPressure(pPressure->mInTextue, *pTempPressure, *pBoundary, pPressure->mOutTextue);
		pPressure->SwapInOut();
	}

	//======================== GRADIENT ============================
	pSolver->ComputeGradient(pPressure->mInTextue, pVelocity->mInTextue, *pBoundary, pVelocity->mOutTextue);
	pVelocity->SwapInOut();

	//======================== BOUNDARY ============================
//	pSolver->ApplyBoundaryCondition(pVelocity->mInTextue, *pBoundary, pVelocity->mOutTextue);
//	pVelocity->SwapInOut();

	//======================== INK =================================
	pSolver->Advect(pVelocity->mInTextue, pInk->mInTextue, *pBoundary, pInk->mOutTextue, 0.9995f);
	pInk->SwapInOut();

	//======================== FINAL RENDERING =====================
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_WIDTH);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pFinalShader->UseShader();

	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, pVelocity->mInTextue.mTextureId);
	glBindTexture(GL_TEXTURE_2D, pInk->mInTextue.mTextureId);
	glUniform1i(uSamplerQuad, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glUseProgram(0);

	glutSwapBuffers();
}

void IdleGL()
{
	calculateFPS();
	char str[200];
	sprintf(str, "FLUID SIMULATUION. FPS: %.2lf", gFps);
	glutSetWindowTitle(str);

	glutPostRedisplay();
}

void ReshapeGL(int w, int h)
{
	if (h == 0)
	{
		h = 1;
	}

	SCREEN_WIDTH = w;
	SCREEN_HEIGHT = h;
	glViewport(0, 0, w, h);


	glutPostRedisplay();
}

void MouseGL(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			gLeftButtonState = state;
			gLastMousePosition = glm::ivec2(x, y);
			gForcePostion = glm::vec2((float)x / SCREEN_WIDTH, (float)(SCREEN_HEIGHT - y) / SCREEN_HEIGHT);
		}
		else
		{
			gLeftButtonState = state;
		}
		break;
	}
}

void MotionGL(int x, int y)
{
	if (gLeftButtonState == GLUT_DOWN)
	{
		glm::ivec2 current = glm::ivec2(x, y);
		glm::ivec2 dir = current - gLastMousePosition;
		gForceDirection = 5.0f * glm::vec2((float)dir.x, (float)(-dir.y));
		gForcePostion = glm::vec2((float)x / SCREEN_WIDTH, (float)(SCREEN_HEIGHT - y) / SCREEN_HEIGHT);
		gLastMousePosition = current;
	}
}

void InitGL(int argc, char* argv[])
{
	std::cout << "Initialize OpenGL..." << std::endl;

	glutInit(&argc, argv);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitWindowPosition(200, 50);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("FLUID SIMULATION");

	Init();
	glutDisplayFunc(DisplayGL);
	glutIdleFunc(IdleGL);
	glutReshapeFunc(ReshapeGL);
	glutMouseFunc(MouseGL);
	glutMotionFunc(MotionGL);

	std::cout << "Initialize OpenGL Success!" << std::endl;
}

int main(int argc, char* argv[]) {

	std::string str;

	InitGL(argc, argv);
	InitGLEW();

	//======================= INPUT ==========================================

	std::vector<GLfloat> mInitVelocityFieldDistribution(2 * RESOLUTION_WIDTH * RESOLUTION_HEIGHT);
	std::vector<GLfloat> mInitPressureFieldDistribution(RESOLUTION_WIDTH * RESOLUTION_HEIGHT);
	std::vector<GLfloat> mInkFieldDistribution(RESOLUTION_WIDTH * RESOLUTION_HEIGHT);
	//std::vector<GLfloat> mBoundaryData(3 * RESOLUTION_WIDTH * RESOLUTION_HEIGHT);
	std::vector<GLfloat> mBoundaryData(RESOLUTION_WIDTH * RESOLUTION_HEIGHT);

	for (int i = 0; i < RESOLUTION_HEIGHT; i++)
	{
		for (int j = 0; j < RESOLUTION_WIDTH; j++)
		{
			mInitVelocityFieldDistribution[2 * i * RESOLUTION_WIDTH + 2 * j + 0] = 0.01 * sin(0.01f * j + 0.02f * i);//0.0f;
			mInitVelocityFieldDistribution[2 * i * RESOLUTION_WIDTH + 2 * j + 1] = 0.0f;

			mInitPressureFieldDistribution[i * RESOLUTION_WIDTH + j] = 0.0f;


			//if ((i < 3 * RESOLUTION_HEIGHT / 4) && (i > RESOLUTION_HEIGHT / 4) && (j < 3 * RESOLUTION_WIDTH / 4) && (j > RESOLUTION_WIDTH / 4))
			//{
			//	mInkFieldDistribution[i * RESOLUTION_WIDTH + j] = 1.0f;
			//}
			//else
			//{
			//	mInkFieldDistribution[i * RESOLUTION_WIDTH + j + 0] = 0.0f;
			//}


			if (i < RESOLUTION_HEIGHT / 4)
			{
				mInkFieldDistribution[i * RESOLUTION_WIDTH + j] = 0.0f;
			}
			else
			{
				mInkFieldDistribution[i * RESOLUTION_WIDTH + j] = 0.0f;
			}


//			float x = (float)(2 * i - RESOLUTION_HEIGHT) / RESOLUTION_HEIGHT;
//			float y = (float)(2 * j - RESOLUTION_WIDTH) / RESOLUTION_WIDTH;

			//if (x * x + y * y < 0.25)
			//{
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 0] = 1.0f;
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 1] = 0.0f;
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 2] = 0.0f;
			//}

			//if (i == 0)
			//{
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 0] = 1.0f;
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 1] = 0.0f;
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 2] = 0.0;// -1.0f;
			//}
			//if (i == RESOLUTION_HEIGHT - 1)
			//{
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 0] = 1.0f;
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 1] = 0.0f;
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 2] = 0.0;//1.0f;
			//}
			//if (j == 0)
			//{
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 0] = 1.0f;
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 1] = 0.0;//1.0f;
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 2] = 0.0f;
			//}
			//if (j == RESOLUTION_WIDTH - 1)
			//{
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 0] = 1.0f;
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 1] = 0.0;//-1.0f;
			//	mBoundaryData[3 * i * RESOLUTION_WIDTH + 3 * j + 2] = 0.0f;
			//}

			if (i == 0)
			{
				mBoundaryData[i * RESOLUTION_WIDTH + j] = 1.0f;
			}
			if (i == (RESOLUTION_HEIGHT - 1))
			{
				mBoundaryData[i * RESOLUTION_WIDTH + j] = 1.0f;
			}
			if (j == 0)
			{
				mBoundaryData[i * RESOLUTION_WIDTH + j] = 1.0f;
			}
			if (j == (RESOLUTION_WIDTH - 1))
			{
				mBoundaryData[i * RESOLUTION_WIDTH + j] = 1.0f;
			}

			//if (x * x + y * y > 0.85)
			//{
			//	mBoundaryData[i * RESOLUTION_WIDTH + j] = 1.0f;
			//}

//			if (10.0 * (x + 0.5) * (x + 0.5) + 0.04 * y * y < 0.001)
//			{
//				mBoundaryData[i * RESOLUTION_WIDTH + j] = 1.0f;
//			}

			//if ((x + 0.5) * (x + 0.5) + y * y < 0.02)
			//{
			//	mBoundaryData[i * RESOLUTION_WIDTH + j] = 1.0f;
			//}
		}
	}

	pVelocity = new TextureUpdate(RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 2, &mInitVelocityFieldDistribution[0]);
	pPressure = new TextureUpdate(RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 1, &mInitPressureFieldDistribution[0]);
	pInk = new TextureUpdate(RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 1, &mInkFieldDistribution[0]);

	pTempPressure = new RecordTexture(RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 1);
	//pBoundary = new RecordTexture(RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 3, &mBoundaryData[0]);
	pBoundary = new RecordTexture(RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 1, &mBoundaryData[0]);
	//======================= INPUT ==========================================

	//===== Generation and Binding of the VAO and VBOs for Quad ====

	pSolver = new FluidSolver();

	glGenVertexArrays(1, &gVaoQuad);
	glBindVertexArray(gVaoQuad);

	glGenBuffers(1, &gVboQuadVertices);
	glBindBuffer(GL_ARRAY_BUFFER, gVboQuadVertices);
	glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(glm::vec2), &quadVertices[0].x, GL_STATIC_DRAW);

	glEnableVertexAttribArray(sPostionId);
	glVertexAttribPointer(sPostionId, 2, GL_FLOAT, false, sizeof(glm::vec2), NULL);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//============================ FINAL ==================================================================================

	pFinalShader = new ShaderProgram("shader.vert", "final.frag");
	pFinalShader->UseShader();

	uSamplerQuad = glGetUniformLocation(pFinalShader->GetProgramId(), "uTexture");

	//========================================================================================================================

	glutMainLoop();

	delete pTempPressure;
	delete pBoundary;
	delete pVelocity;
	delete pInk;
	delete pSolver;
	delete pFinalShader;

	return 0;
}
