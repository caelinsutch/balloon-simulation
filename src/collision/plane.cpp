#include "iostream"
#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../clothSimulator.h"
#include "plane.h"

using namespace std;
using namespace CGL;

#define SURFACE_OFFSET 0.0001

void Plane::collide(PointMass& pm) {
	// TODO (Part 3): Handle collisions with planes.

	// Find tangent point
	Vector3D v = pm.position - point;
	float dist = dot(v, normal);
	float distLast = dot(pm.last_position - point, normal);

	if (dist * distLast <= 0) {
		// Intersected plane
		// Maybe change so that surface offset is opposite to gravity
		Vector3D tangentPoint = pm.position - (dist + (dist >= 0 ? SURFACE_OFFSET : -SURFACE_OFFSET)) * normal;

		Vector3D correctionVector = tangentPoint - pm.last_position;

		pm.position = pm.last_position + (1 - friction) * correctionVector;
	}
}

void Plane::render(GLShader& shader) {
	nanogui::Color color(0.7f, 0.7f, 0.7f, 1.0f);

	Vector3f sPoint(point.x, point.y, point.z);
	Vector3f sNormal(normal.x, normal.y, normal.z);
	Vector3f sParallel(normal.y - normal.z, normal.z - normal.x,
		normal.x - normal.y);
	sParallel.normalize();
	Vector3f sCross = sNormal.cross(sParallel);

	MatrixXf positions(3, 4);
	MatrixXf normals(3, 4);

	positions.col(0) << sPoint + 2 * (sCross + sParallel);
	positions.col(1) << sPoint + 2 * (sCross - sParallel);
	positions.col(2) << sPoint + 2 * (-sCross + sParallel);
	positions.col(3) << sPoint + 2 * (-sCross - sParallel);

	normals.col(0) << sNormal;
	normals.col(1) << sNormal;
	normals.col(2) << sNormal;
	normals.col(3) << sNormal;

	if (shader.uniform("u_color", false) != -1) {
		shader.setUniform("u_color", color);
	}
	shader.uploadAttrib("in_position", positions);
	if (shader.attrib("in_normal", false) != -1) {
		shader.uploadAttrib("in_normal", normals);
	}

	shader.drawArray(GL_TRIANGLE_STRIP, 0, 4);
}
