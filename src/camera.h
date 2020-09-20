#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

struct Camera
{
	glm::vec3 rigPos = glm::vec3(0.0f);
	glm::vec3 offset = glm::vec3(10.0f, 10.0f, 10.0f);
	const glm::vec3 UP = glm::vec3(0.0f, 1.0, 0.0f);

	glm::mat4 view;
	glm::mat4 projection;

	float scrollSpeed = 5.0f;
	float fov = 45.0f;
	float nearCull = 0.1f;
	float farCull = 100.0f;

	float rotation = 45.0f;
	glm::vec3 cameraVel;

	glm::vec3 GetCameraPos()
	{
		return this->rigPos + this->offset;
	}

	glm::mat4 GetProjectionView()
	{
		return projection * view;
	}

	void LateUpdate(const float width, const float height, glm::ivec2 cameraDir, int rotDelta, float dt)
	{
		// Move normalize
		rotation += rotDelta * dt;

		offset.x = sin(rotation) * 10;
		offset.z = cos(rotation) * 10;
		glm::vec3 vectorToCamera = glm::normalize(offset);
		glm::vec3 cameraRight = glm::normalize(glm::cross(UP, vectorToCamera));

		glm::vec3 zoomVec = glm::normalize(vectorToCamera) * (float)cameraDir.y;
		glm::vec3 forwardVec = glm::vec3(zoomVec.x, 0, zoomVec.z);
		glm::vec3 rightVec = cameraRight *  (float)cameraDir.x;
		// STUPID wa need a math way
		// TODO camera need to use local vector 
		rigPos += forwardVec * scrollSpeed * dt;
		rigPos += rightVec * scrollSpeed * dt;

		// Update Matrixes
		glm::vec3 cameraUp = glm::cross(vectorToCamera, cameraRight);

		this->view = glm::lookAt(this->rigPos + this->offset, rigPos, cameraUp);
		this->projection = glm::perspective(glm::radians(fov), width / height, nearCull ,farCull);
	}
};