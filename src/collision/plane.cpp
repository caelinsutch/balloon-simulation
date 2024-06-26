#include "iostream"
#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../clothSimulator.h"
#include "plane.h"

using namespace std;
using namespace CGL;

#define SURFACE_OFFSET 0.0001


void Plane::collide(PointMass &pm) {
	Vector3D vector_new = pm.position - point;
	Vector3D vector_last = pm.last_position - point;
	// if the point is on the other side of the plane
	if (dot(vector_new, normal) * dot(vector_last, normal) <= 0) {
		// project the point onto the plane
		Vector3D unit = normal.unit();
		Vector3D tangent = pm.position - dot(unit, vector_new) * unit;
		Vector3D vector;
		// if the point is moving towards the plane, move it to the surface
		// otherwise, move it away from the surface
		if (dot(vector_last, normal) < 0) {
			vector = tangent - normal * SURFACE_OFFSET - pm.last_position;
		} else {
			vector = tangent + normal * SURFACE_OFFSET - pm.last_position;
		}
		// apply friction and update the position
		pm.position = pm.last_position + (1.0 - friction) * vector;
	}
}

void Plane::render(GLShader &shader) {
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
