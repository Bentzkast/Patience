#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera
{
	glm::vec3 rigPos = glm::vec3(0.0f);
	glm::vec3 offset = glm::vec3(10.0f, 10.0f, 10.0f);
	const glm::vec3 UP = glm::vec3(0.0f, 1.0, 0.0f);

	glm::mat4 view;
	glm::mat4 projection;

	float scrollSpeed = 10.0f;
	float fov = 45.0f;
	float nearCull = 0.1f;
	float farCull = 100.0f;

	float rotation = 45.0f;

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

		glm::vec3 forwardVec = glm::normalize(-vectorToCamera);
		forwardVec.y = 0;
		// STUPID wa need a math way
		if(cameraDir.x < 1){
			forwardVec.x = -forwardVec.x;
		}
		else if(cameraDir.x == 0)
		{
			forwardVec.x = 0;
		}

		if(cameraDir.y < 1){
			forwardVec.z = -forwardVec.z;
		}
		else if(cameraDir.y == 0)
		{
			forwardVec.z = 0;
		}
		SDL_Log("%f, %f", forwardVec.x, forwardVec.z);

		// TODO camera need to use local vector 
		rigPos += forwardVec * dt * scrollSpeed;

		// Update Matrixes
		glm::vec3 cameraUp = glm::cross(vectorToCamera, cameraRight);

		this->view = glm::lookAt(GetCameraPos(), rigPos, cameraUp);
		this->projection = glm::perspective(glm::radians(fov), width / height, nearCull ,farCull);
	}
};