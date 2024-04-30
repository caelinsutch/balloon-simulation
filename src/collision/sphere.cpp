#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../misc/sphere_drawing.h"
#include "sphere.h"

using namespace nanogui;
using namespace CGL;

void Sphere::collide(PointMass& pm) {
	// TODO (Part 3): Handle collisions with spheres.
	Vector3D to_pm = pm.position - origin;
	double dist_to_pm = to_pm.norm();

	if (dist_to_pm < radius) {
		Vector3D tangent_point = origin + (radius / dist_to_pm) * to_pm;
		Vector3D correction = tangent_point - pm.last_position;
		correction *= (1 - friction);
		pm.position = pm.last_position + correction;
	}
}

void Sphere::render(GLShader& shader) {
	// We decrease the radius here so flat triangles don't behave strangely
	// and intersect with the sphere when rendered
	m_sphere_mesh.draw_sphere(shader, origin, radius * 0.92);
}
