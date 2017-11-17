#ifndef DEBUG_SHAPE_MANAGER_H
#define DEBUG_SHAPE_MANAGER_H
#include <ExportImport.h>
#include <Rendering\MeRenderer.h>
#include <vector>
#include <DebugShapes\ShapeGenerator.h>

class ME_DEBUG_DLL_SHARED DebugShapeManager
{
public:
	class ME_DEBUG_DLL_SHARED VectorGraphic
	{
		friend class DebugShapeManager;
		Renderable* coneRenderable;
		Renderable* stemRenderable;
		glm::vec3 theLine;
	public:
		void setEndPoints(const glm::vec3& from, const glm::vec3& to, float lengthDelta = 0);
		void setVisible(bool visible);
		const glm::vec3& getVector() const { return theLine; }
		void dispose();
	};
private:
	GeometryInfo* hudGeometry;
	GeometryInfo* sphereGeometry;
	GeometryInfo* triangleGeometry;
	GeometryInfo* wirePlaneGeometry;
	GeometryInfo* planeGeometry;
	GeometryInfo* arrowGeometry;
	GeometryInfo* lineGeometry;
	GeometryInfo* coneGeometry;
	GeometryInfo* stemGeometry;
	GeometryInfo* cubeGeometry;
	GeometryInfo* teapotGeometry;

	// Singleton
	static DebugShapeManager instance;
	DebugShapeManager() {}
	DebugShapeManager(DebugShapeManager&);
	DebugShapeManager& operator=(DebugShapeManager&);
public:
	static DebugShapeManager& getInstance() { return instance; }
	bool initialize();
	bool shutdown() { return true; }

	Renderable* addTriangle(const glm::vec3& location = glm::vec3());
	Renderable* addPlane(const glm::vec3& location = glm::vec3());
	Renderable* addWirePlane(const glm::vec3& location = glm::vec3());
	Renderable* addCube(const glm::vec3& location = glm::vec3(), float scale = 1.0f);
	Renderable* addSphere(const glm::vec3& location = glm::vec3(), float scale = 1.0f);
	Renderable* addArrow(const glm::vec3& location = glm::vec3(), const glm::vec3& direction = glm::vec3(0.0f, 0.0f, -1.0f));
	Renderable* addTeapot(const glm::vec3& location = glm::vec3());
	Renderable* addLine(const glm::vec3& start = glm::vec3(), const glm::vec3& end = glm::vec3(0.0f, 0.0f, -1.0f));
	VectorGraphic addVector(const glm::vec3& from = glm::vec3(), const glm::vec3& to = glm::vec3(0.0f, 1.0f, 0.0f), float lengthDelta = 0);	

	GeometryInfo* getTriangleGeometry();
	GeometryInfo* getPlaneGeometry();
	GeometryInfo* getWirePlaneGeometry();
	GeometryInfo* getCubeGeometry();
	GeometryInfo* getSphereGeometry();
	GeometryInfo* getArrowGeometry();
	GeometryInfo* getTeapotGeometry();
	GeometryInfo* getLineGeometry();
	GeometryInfo* getHudGeometry();
};

#define debugShapeManager DebugShapeManager::getInstance()

#endif