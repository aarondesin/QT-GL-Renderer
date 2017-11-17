#include <DebugShapes\DebugShapeManager.h>
#include <Rendering\ShaderManager.h>
#include <glm\gtx\transform.hpp>
#include <Math\MathHelpers.h>
#include <Qt\qdebug.h>
#include <DebugTools\MyAssert.h>

DebugShapeManager DebugShapeManager::instance;

bool DebugShapeManager::initialize() 
{
	sphereGeometry = 0;
	lineGeometry = 0;
	coneGeometry = 0;
	stemGeometry = 0;
	arrowGeometry = 0;
	wirePlaneGeometry = 0;
	planeGeometry = 0;
	teapotGeometry = 0;
	hudGeometry = 0;
	triangleGeometry = 0;
	return true;
}

static Renderable* addTheRenderable(
	const GeometryInfo* geometry,
	const glm::mat4& where)
{
	return renderer.addRenderable(geometry, shaderManager.getPassThroughShaderPC(), where);
}

Renderable* DebugShapeManager::addSphere(const glm::vec3& location, float scale)
{
	return addTheRenderable(getSphereGeometry(), glm::translate(location) * glm::scale(scale, scale, scale));
}

Renderable* DebugShapeManager::addTriangle(const glm::vec3& location)
{
	return addTheRenderable(getTriangleGeometry(), glm::translate(location));
}

Renderable* DebugShapeManager::addArrow(const glm::vec3& location, const glm::vec3& direction)
{
	return addTheRenderable(getArrowGeometry(), glm::translate(location) * glm::mat4(MathHelpers::createBasisFromOneVector(direction)));
}

Renderable* DebugShapeManager::addCube(const glm::vec3& location, float scale)
{
	return addTheRenderable(getCubeGeometry(), glm::translate(location) * glm::scale(scale, scale, scale));
}

Renderable* DebugShapeManager::addWirePlane(const glm::vec3& location)
{
	return addTheRenderable(getWirePlaneGeometry(), glm::translate(location));
}

Renderable* DebugShapeManager::addPlane(const glm::vec3& location)
{
	return addTheRenderable(getPlaneGeometry(), glm::translate(location));
}

Renderable* DebugShapeManager::addLine(const glm::vec3& start, const glm::vec3& end)
{
	glm::vec3 theLine = end - start;

	glm::vec3 xBasis = glm::normalize(theLine);
	glm::mat4 theRotation(MathHelpers::createBasisFromOneVector(xBasis));

	glm::mat4 fullTransform =  // scale then rotate then translate 
		glm::translate(start) *
		theRotation * 
		glm::scale(glm::length(theLine), 1.0f, 1.0f);

	return addTheRenderable(getLineGeometry(), fullTransform);
}

static GeometryInfo* loadGeometry(ShapeData& data, GLuint drawMode = GL_TRIANGLES)
{
	GeometryInfo* ret =
		renderer.addGeometry(
		data.verts, data.vertexBufferSize(), 
		data.indices, data.numIndices,
		DT_UNSIGNED_SHORT, drawMode, VertexLayouts::getPcnutLayoutInfo());
	data.cleanUp();
	myAssert(ret != 0);
	return ret;
}

DebugShapeManager::VectorGraphic DebugShapeManager::addVector(
	const glm::vec3& from, const glm::vec3& to, float lengthDelta)
{
	if(coneGeometry == 0)
	{
		coneGeometry = loadGeometry(ShapeGenerator::makeCone());
		stemGeometry = loadGeometry(ShapeGenerator::makeCylinder());
	}
	VectorGraphic ret;
	ret.coneRenderable = renderer.addRenderable(
		coneGeometry, 
		shaderManager.getPassThroughShaderPC());
	ret.stemRenderable = renderer.addRenderable(
		stemGeometry, 
		shaderManager.getPassThroughShaderPC());
	ret.setEndPoints(from, to, lengthDelta);
	return ret;
}

GeometryInfo* DebugShapeManager::getTriangleGeometry()
{
	if(triangleGeometry == 0)
		triangleGeometry = loadGeometry(ShapeGenerator::make2DTriangle());
	return triangleGeometry;
}

GeometryInfo* DebugShapeManager::getPlaneGeometry()
{
	if(planeGeometry == 0)
		planeGeometry = loadGeometry(ShapeGenerator::makePlane(1));
	return planeGeometry;
}

GeometryInfo* DebugShapeManager::getWirePlaneGeometry()
{
	if(wirePlaneGeometry == 0)
		wirePlaneGeometry = loadGeometry(ShapeGenerator::makeWireframePlane(1));
	return wirePlaneGeometry;
}

GeometryInfo* DebugShapeManager::getCubeGeometry()
{
	if(cubeGeometry == 0)
		cubeGeometry = loadGeometry(ShapeGenerator::makeCube());
	return cubeGeometry;
}

GeometryInfo* DebugShapeManager::getSphereGeometry()
{
	if(sphereGeometry == 0)
		sphereGeometry = loadGeometry(ShapeGenerator::makeSphere(40));
	return sphereGeometry;
}

GeometryInfo* DebugShapeManager::getArrowGeometry()
{
	if(arrowGeometry == 0)
		arrowGeometry = loadGeometry(ShapeGenerator::makeArrow());
	return arrowGeometry;
}

GeometryInfo* DebugShapeManager::getTeapotGeometry()
{
	if(teapotGeometry == 0)
		teapotGeometry = loadGeometry(ShapeGenerator::makeTeapot());
	return teapotGeometry;
}

GeometryInfo* DebugShapeManager::getLineGeometry()
{
	if(lineGeometry == 0)
		lineGeometry = loadGeometry(ShapeGenerator::makeLine());
	return lineGeometry;
}

GeometryInfo* DebugShapeManager::getHudGeometry()
{
	if(hudGeometry == 0)
		hudGeometry = loadGeometry(ShapeGenerator::makeHudQuad());
	return hudGeometry;
}

void DebugShapeManager::VectorGraphic::setEndPoints(
	const glm::vec3& from, const glm::vec3& to, float lengthDelta)
{
	theLine = to - from;
	glm::vec3 xBasis = glm::normalize(theLine);
	glm::mat4 theRotation(MathHelpers::createBasisFromOneVector(xBasis));

	const float cylinderWeightLoss = 0.02f;
	glm::vec3 backOff = (lengthDelta / 2) * xBasis;
	const float CONE_SCALE = 0.3f;
	glm::vec3 coneHeight = ShapeGenerator::CONE_HEIGHT * CONE_SCALE * xBasis;
	stemRenderable->where =  // scale then rotate then translate 
		glm::translate(from + backOff) *
		theRotation * 
		glm::scale(glm::length(theLine) - lengthDelta - glm::length(coneHeight), 
		cylinderWeightLoss, cylinderWeightLoss);
	coneRenderable->where = 
		glm::translate(to - backOff - coneHeight) * 
		theRotation *
		glm::scale(CONE_SCALE, CONE_SCALE, CONE_SCALE);
}

void DebugShapeManager::VectorGraphic::setVisible(bool visible)
{
	coneRenderable->visible = visible;
	stemRenderable->visible = visible;
}

void DebugShapeManager::VectorGraphic::dispose()
{
	// TODO: do this
	//renderer.releaseRenderable(stemRenderable);
	//renderer.releaseRenderable(coneRenderable);
}