#ifndef _FLUID_SOLVER_H_
#define _FLUID_SOLVER_H_

#include <iostream>

#ifdef __unix_
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <GL/glxew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#elif defined(_WIN32)
#define GLM_FORCE_RADIANS
#include "..\thirdparties\glew-1.13.0\include\GL\glew.h"
#include "..\thirdparties\glew-1.13.0\include\GL\wglew.h"
#include "..\thirdparties\freeglut\include\GL\freeglut.h"
#include "..\thirdparties\glm\glm\glm.hpp"
#include "..\thirdparties\glm\glm\gtc\type_ptr.hpp"
#endif

#include "ShaderProgram.h"
#include "RecordTexture.h"

class FluidSolver
{
public:
	FluidSolver();
	virtual ~FluidSolver();

	void Advect(const RecordTexture& refVelocity, const RecordTexture& refSource, const RecordTexture& refObstacle, RecordTexture& refTarget, float aDissipation = 1.0f) const;
	void Diffuse(const RecordTexture& refVelocity, const RecordTexture& refSource, const RecordTexture& refObstacle, RecordTexture& refTarget) const;
	void ApplyForces(const RecordTexture& refVelocity, RecordTexture& refTarget, const glm::vec2& aPos, const glm::vec2& aDir) const;
	void ComputeDivergence(const RecordTexture& refVelocity, const RecordTexture& refObstacle, RecordTexture& refTarget) const;
	void ProjectPressure(const RecordTexture& refPressure, const RecordTexture& refSource, const RecordTexture& refObstacle, RecordTexture& refTarget) const;
	void ComputeGradient(const RecordTexture& refPressure, const RecordTexture& refSource, const RecordTexture& refObstacle, RecordTexture& refTarget) const;
	void ApplyBoundaryCondition(const RecordTexture& refVelocity, const RecordTexture& refObstacle, RecordTexture& refTarget) const;

private:
	void ResetState() const;

private:
	ShaderProgram* pAdvectProgram;
	ShaderProgram* pDiffuseProgram;
	ShaderProgram* pForceProgram;
	ShaderProgram* pDivergenceProgram;
	ShaderProgram* pPressureProgram;
	ShaderProgram* pGradientProgram;
	ShaderProgram* pBoundaryProgram;

};

inline void
FluidSolver::Advect(const RecordTexture& refVelocity, const RecordTexture& refSource, const RecordTexture& refObstacle, RecordTexture& refTarget, float aDissipation) const
{
	pAdvectProgram->UseShader();
	GLint programId = pAdvectProgram->GetProgramId();

	GLint uSamplerQuad = glGetUniformLocation(programId, "uTexture");
	GLint uSamplerBeta = glGetUniformLocation(programId, "uTextureB");
	GLint uSamplerObstacle = glGetUniformLocation(programId, "uTextureObstacle");
	GLint uDissipation = glGetUniformLocation(programId, "uDissipation");

	glBindTexture(GL_TEXTURE_2D, refTarget.mTextureId);
	glBindFramebuffer(GL_FRAMEBUFFER, refTarget.mFboId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, refVelocity.mTextureId);
	glUniform1i(uSamplerQuad, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refSource.mTextureId);
	glUniform1i(uSamplerBeta, 1);
	glUniform1f(uDissipation, aDissipation);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, refObstacle.mTextureId);
	glUniform1i(uSamplerObstacle, 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	ResetState();

	return;
}

inline void
FluidSolver::Diffuse(const RecordTexture& refVelocity, const RecordTexture& refSource, const RecordTexture& refObstacle, RecordTexture& refTarget) const
{
	pDiffuseProgram->UseShader();
	GLint programId = pDiffuseProgram->GetProgramId();

	GLint uSamplerQuad = glGetUniformLocation(programId, "uTexture");
	GLint uSamplerBeta = glGetUniformLocation(programId, "uTextureB");
	GLint uSamplerObstacle = glGetUniformLocation(programId, "uTextureObstacle");

	glBindTexture(GL_TEXTURE_2D, refTarget.mTextureId);
	glBindFramebuffer(GL_FRAMEBUFFER, refTarget.mFboId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, refVelocity.mTextureId);
	glUniform1i(uSamplerQuad, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refSource.mTextureId);
	glUniform1i(uSamplerBeta, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, refObstacle.mTextureId);
	glUniform1i(uSamplerObstacle, 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	ResetState();
	return;
}

inline void
FluidSolver::ApplyForces(const RecordTexture& refVelocity, RecordTexture& refTarget, const glm::vec2& aPos, const glm::vec2& aDir) const
{
	pForceProgram->UseShader();
	GLint programId = pForceProgram->GetProgramId();

	GLint uSamplerQuad = glGetUniformLocation(programId, "uTexture");
	GLint uForcePosition = glGetUniformLocation(programId, "uForcePos");
	GLint uForceDirection = glGetUniformLocation(programId, "uForceDir");

	glBindTexture(GL_TEXTURE_2D, refTarget.mTextureId);
	glBindFramebuffer(GL_FRAMEBUFFER, refTarget.mFboId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, refVelocity.mTextureId);
	glUniform1i(uSamplerQuad, 0);

	glUniform2fv(uForcePosition, 1, glm::value_ptr(aPos));
	glUniform2fv(uForceDirection, 1, glm::value_ptr(aDir));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	ResetState();
	return;
}

inline void
FluidSolver::ComputeDivergence(const RecordTexture& refVelocity, const RecordTexture& refObstacle, RecordTexture& refTarget) const
{
	pDivergenceProgram->UseShader();
	GLint programId = pDivergenceProgram->GetProgramId();

	GLint uSamplerDivergence = glGetUniformLocation(programId, "uTexture");
	GLint uSamplerBeta = glGetUniformLocation(programId, "uTextureObstacle");

	glBindTexture(GL_TEXTURE_2D, refTarget.mTextureId);
	glBindFramebuffer(GL_FRAMEBUFFER, refTarget.mFboId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, refVelocity.mTextureId);
	glUniform1i(uSamplerDivergence, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refObstacle.mTextureId);
	glUniform1i(uSamplerBeta, 1);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	ResetState();
	return;
}

inline void
FluidSolver::ProjectPressure(const RecordTexture& refPressure, const RecordTexture& refSource, const RecordTexture& refObstacle, RecordTexture& refTarget) const
{
	pPressureProgram->UseShader();
	GLint programId = pPressureProgram->GetProgramId();

	GLint uSamplerQuad = glGetUniformLocation(programId, "uTexture");
	GLint uSamplerBeta = glGetUniformLocation(programId, "uTextureB");
	GLint uSamplerObstacle = glGetUniformLocation(programId, "uTextureObstacle");

	glBindTexture(GL_TEXTURE_2D, refTarget.mTextureId);
	glBindFramebuffer(GL_FRAMEBUFFER, refTarget.mFboId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, refPressure.mTextureId);
	glUniform1i(uSamplerQuad, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refSource.mTextureId);
	glUniform1i(uSamplerBeta, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, refObstacle.mTextureId);
	glUniform1i(uSamplerObstacle, 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	ResetState();
	return;
}

inline void
FluidSolver::ComputeGradient(const RecordTexture& refPressure, const RecordTexture& refSource, const RecordTexture& refObstacle, RecordTexture& refTarget) const
{
	pGradientProgram->UseShader();
	GLint programId = pGradientProgram->GetProgramId();

	GLint uSamplerQuad = glGetUniformLocation(programId, "uTexture");
	GLint uSamplerBeta = glGetUniformLocation(programId, "uTextureB");
	GLint uSamplerObstacle = glGetUniformLocation(programId, "uTextureObstacle");

	glBindTexture(GL_TEXTURE_2D, refTarget.mTextureId);
	glBindFramebuffer(GL_FRAMEBUFFER, refTarget.mFboId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, refPressure.mTextureId);
	glUniform1i(uSamplerQuad, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refSource.mTextureId);
	glUniform1i(uSamplerBeta, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, refObstacle.mTextureId);
	glUniform1i(uSamplerObstacle, 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	ResetState();
	return;
}

inline void
FluidSolver::ApplyBoundaryCondition(const RecordTexture& refVelocity, const RecordTexture& refObstacle, RecordTexture& refTarget) const
{
	pBoundaryProgram->UseShader();
	GLint programId = pBoundaryProgram->GetProgramId();

	GLint uSamplerQuad = glGetUniformLocation(programId, "uTexture");
	GLint uSamplerBound = glGetUniformLocation(programId, "uTextureObstacle");

	glBindTexture(GL_TEXTURE_2D, refTarget.mTextureId);
	glBindFramebuffer(GL_FRAMEBUFFER, refTarget.mFboId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, refVelocity.mTextureId);
	glUniform1i(uSamplerQuad, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refObstacle.mTextureId);
	glUniform1i(uSamplerBound, 1);

	glDrawArrays(GL_LINE_LOOP, 0, 4);

	ResetState();
	return;
}


#endif /* _FLUID_SOLVER_H_ */
