#include "FluidSolver.h"

FluidSolver::FluidSolver()
	: pAdvectProgram(NULL)
	, pDiffuseProgram(NULL)
	, pForceProgram(NULL)
	, pDivergenceProgram(NULL)
	, pPressureProgram(NULL)
	, pGradientProgram(NULL)
	, pBoundaryProgram(NULL)
{
	pAdvectProgram = new ShaderProgram("shader.vert", "advect.frag");
	pDiffuseProgram = new ShaderProgram("shader.vert", "diffuseShader.frag");
	pForceProgram = new ShaderProgram("shader.vert", "forceShader.frag");
	pDivergenceProgram = new ShaderProgram("shader.vert", "divergence.frag");
	pPressureProgram = new ShaderProgram("shader.vert", "pressure.frag");
	pGradientProgram = new ShaderProgram("shader.vert", "gradient.frag");
	pBoundaryProgram = new ShaderProgram("shader.vert", "boundaryShader.frag");
}

FluidSolver::~FluidSolver()
{
	delete pAdvectProgram;
	delete pDiffuseProgram;
	delete pForceProgram;
	delete pDivergenceProgram;
	delete pPressureProgram;
	delete pGradientProgram;
	delete pBoundaryProgram;
}

void
FluidSolver::ResetState() const
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
