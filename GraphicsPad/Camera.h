#pragma once
#include <glm\glm.hpp>
#include <iostream>
using namespace std;

class Camera
{
	glm::vec3 position;
	glm::vec3 viewDirection;
	const glm::vec3 UP;
	glm::vec2 oldMousePosition;
	static const float MOVEMENT_SPEED;
	glm::vec3 strafeDirection;
public:
	/*Camera() : position(glm::vec3(0.0f, 0.0f, 0.0f)), 
		viewDirection (glm::vec3(0.0f, 0.0f, 1.0f)) 
	{
		cout << viewDirection.x << "," << viewDirection.y << "," << viewDirection.z << endl;
	}*/
	Camera();
	void mouseUpdate(const glm::vec2& newMousePosition);
	glm::mat4 getWorldToViewMatrix() const;

	void moveForward();
	void moveBackward();
	void strafeLeft();
	void strafeRight();
	void moveUp();
	void moveDown();
	glm::vec3 getPosition() const { return position; }
	void setPosition(glm::vec3 pos) { position = pos; }
	float fov = 60.0f;
	float clipNear = 0.1f;
	float clipFar = 100.0f;
};
