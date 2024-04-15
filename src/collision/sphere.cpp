#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../misc/sphere_drawing.h"
#include "sphere.h"

using namespace nanogui;
using namespace CGL;

void Sphere::collide(PointMass &pm) {
	Vector3D direction = (pm.position - this->origin);
	if(direction.norm() <= this->radius)
	{
		Vector3D collision = this->origin + direction.unit() * this->radius;
		Vector3D corrected_point = collision - pm.last_position;
		pm.position = pm.last_position + (1-this->friction) * corrected_point;
	}
}

void Sphere::render(GLShader &shader) {
  // We decrease the radius here so flat triangles don't behave strangely
  // and intersect with the sphere when rendered
  m_sphere_mesh.draw_sphere(shader, origin, radius * 0.92);
}
