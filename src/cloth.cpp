#include <cstdlib>
#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "CGL/misc.h"
#include "CGL/vector2D.h"
#include "CGL/vector3D.h"
#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"
#include "pointMass.h"
#include "spring.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.

  // Vector2D x_range = Vector2D(INF_D, -INF_D);
  // Vector2D y_range = Vector2D(INF_D, -INF_D);

  // for (int i = 0; i < this->pinned.size(); i++) {
  //   if (this->pinned[i][0] < x_range.x) {
  //     x_range.x = this->pinned[i][0];
  //   } else if (this->pinned[i][0] > x_range.y) {
  //     x_range.y = this->pinned[i][0];
  //   }

  //   if (this->pinned[i][1] < y_range.x) {
  //     y_range.x = this->pinned[i][1];
  //   } else if (this->pinned[i][1] > y_range.y) {
  //     y_range.y = this->pinned[i][1];
  //   }
  // } 


	double radius = std::min(width, height) / 2.0;
	Vector3D center(width / 2.0, height / 2.0, 0.0);

	for (int y = 0; y < this->num_height_points; y++) {
		for (int x = 0; x < this->num_width_points; x++) {
			double u = (double)x / (num_width_points - 1);
			double v = (double)y / (num_height_points - 1);

			double theta = u * 2.0 * M_PI;
			double phi = v * M_PI;

			Vector3D pos;
			pos.x = radius * std::cos(theta) * std::sin(phi) + center.x;
			pos.y = radius * std::sin(theta) * std::sin(phi) + center.y;
			pos.z = radius * std::cos(phi);

			bool pin = false;
			for (int i = 0; i < this->pinned.size(); i++) {
				if (this->pinned[i][0] == x && this->pinned[i][1] == y) {
					pin = true;
				}
			}

			this->point_masses.emplace_back(PointMass(pos, pin));
		}
	}

	for (int j = 0; j < num_height_points; ++j) {
		for (int i = 0; i < num_width_points; ++i) {
			PointMass* pm = &point_masses[j * num_width_points + i];

			// Structural springs
			if (i < num_width_points - 1) {
				springs.emplace_back(pm, &point_masses[j * num_width_points + i + 1], STRUCTURAL);
			}
			if (j < num_height_points - 1) {
				springs.emplace_back(pm, &point_masses[(j + 1) * num_width_points + i], STRUCTURAL);
			}

			// Shearing springs
			if (i < num_width_points - 1 && j < num_height_points - 1) {
				springs.emplace_back(pm, &point_masses[(j + 1) * num_width_points + i + 1], SHEARING);
			}
			if (i > 0 && j < num_height_points - 1) {
				springs.emplace_back(pm, &point_masses[(j + 1) * num_width_points + i - 1], SHEARING);
			}

			// Bending springs
			if (i < num_width_points - 2) {
				springs.emplace_back(pm, &point_masses[j * num_width_points + i + 2], BENDING);
			}
			if (j < num_height_points - 2) {
				springs.emplace_back(pm, &point_masses[(j + 2) * num_width_points + i], BENDING);
			}
		}
	}
}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
	double mass = width * height * cp->density / num_width_points / num_height_points;
	double delta_t = 1.0f / frames_per_sec / simulation_steps;

	// TODO (Part 2): Compute total force acting on each point mass.
	Vector3D external_force = Vector3D(0.0, 0.0, 0.0);
	for (int i = 0; i < external_accelerations.size(); i++) {
		// F = ma
		external_force += external_accelerations[i] * mass;
	}

	// Apply outward force to each point mass
	double inflation_force = 1.0; // Adjust this value to control the inflation strength
	for (int i = 0; i < point_masses.size(); i++) {
		Vector3D normal = point_masses[i].position - Vector3D(width / 2.0, height / 2.0, 0.0);
		normal.normalize();
		point_masses[i].forces = external_force + normal * inflation_force;
	}

	// TODO (Part 2): Use Verlet integration to compute new point mass positions
	for (int i = 0; i < point_masses.size(); i++) {
		if (point_masses[i].pinned) continue;
		Vector3D a = point_masses[i].forces / mass;
		// Verlet integration
		Vector3D new_position = point_masses[i].position + (1 - cp->damping / 100.0) * (point_masses[i].position - point_masses[i].last_position) + a * delta_t * delta_t;
		// Update last position
		point_masses[i].last_position = point_masses[i].position;
		point_masses[i].position = new_position;
	}



  // TODO (Part 4): Handle self-collisions.
	build_spatial_map();
//	for (int i = 0; i < point_masses.size(); i ++) {
//		self_collide(point_masses[i], simulation_steps);
//	}


	// TODO (Part 3): Handle collisions with other primitives.
	for (int i = 0; i < point_masses.size(); i ++) {
		for (CollisionObject *object : *collision_objects) {
			object -> collide(point_masses[i]);
		}
	}

  // TODO (Part 2): Constrain the changes to be such that the spring does not change
  // in length more than 10% per timestep [Provot 1995].
	for (int i = 0; i < springs.size(); i ++) {
		Spring spring = springs[i];
		Vector3D direction = spring.pm_b -> position - spring.pm_a -> position;
		direction.normalize();
		double spring_length = (spring.pm_b -> position - spring.pm_a -> position).norm();
		// Check if over constraint
		if (spring_length > spring.rest_length * 1.1) {
			double new_length = spring_length - spring.rest_length * 1.1;
			// Check if a pinned and b isn't
			if (spring.pm_a -> pinned && !spring.pm_b -> pinned) {
				spring.pm_b -> position -= direction * new_length;
			}
			// a not pinned, b is
			if (!spring.pm_a -> pinned && spring.pm_b -> pinned) {
				spring.pm_a -> position += direction * new_length;
			}
			// both not pinned
			if (!spring.pm_a -> pinned && !spring.pm_b -> pinned) {
				spring.pm_a -> position += direction * (new_length / 2);
				spring.pm_b -> position -= direction * (new_length / 2);
			}
		}
	}
}

void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

	// Populate map
	for (int i = 0; i < point_masses.size(); i ++) {
		double hash = hash_position(point_masses[i].position);
		// Create new vector if not in map
		if (map.find(hash) == map.end()) {
			map[hash] = new vector<PointMass *>;
		}
		map[hash]->push_back(&point_masses[i]);
	}

}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
	// TODO (Part 4): Handle self-collision for a given point mass.
	int num_collisions = 0;
	Vector3D correction_vector = Vector3D(0, 0, 0);
	double hash = hash_position(pm.position);
	vector<PointMass *> *points = map[hash];
	for (PointMass *point : *points) {
		// Skip if same point
		if (point->position == pm.position) continue;
		double distance = (point -> position - pm.position).norm();

		// Check if collision
		if (distance < 2 * thickness) {
			Vector3D direction = pm.position - point->position;
			direction.normalize();
			// Compute correction vector
			correction_vector += direction * (2 * thickness - distance);
			num_collisions ++;
		}
	}
	if (num_collisions) {
		correction_vector = correction_vector / (double)num_collisions / simulation_steps;
		pm.position += correction_vector;
	}
}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.

	// Partition into boxes
	double w = 3.0 * width / num_width_points;
	double h = 3.0 * height / num_height_points;
	double t = max(w, h);

	// Truncate to box
	double x = (pos.x - fmod(pos.x, w)) / w;
	double y = (pos.y - fmod(pos.y, h)) / h;
	double z = (pos.z - fmod(pos.z, t)) / t;

	// Compute unique number
	return pow(x, 1) + pow(y, 2) + pow(z, 3);
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Get neighboring point masses:
      /*                      *
       * pm_A -------- pm_B   *
       *             /        *
       *  |         /   |     *
       *  |        /    |     *
       *  |       /     |     *
       *  |      /      |     *
       *  |     /       |     *
       *  |    /        |     *
       *      /               *
       * pm_C -------- pm_D   *
       *                      *
       */
      
      float u_min = x;
      u_min /= num_width_points - 1;
      float u_max = x + 1;
      u_max /= num_width_points - 1;
      float v_min = y;
      v_min /= num_height_points - 1;
      float v_max = y + 1;
      v_max /= num_height_points - 1;
      
      PointMass *pm_A = pm                       ;
      PointMass *pm_B = pm                    + 1;
      PointMass *pm_C = pm + num_width_points    ;
      PointMass *pm_D = pm + num_width_points + 1;
      
      Vector3D uv_A = Vector3D(u_min, v_min, 0);
      Vector3D uv_B = Vector3D(u_max, v_min, 0);
      Vector3D uv_C = Vector3D(u_min, v_max, 0);
      Vector3D uv_D = Vector3D(u_max, v_max, 0);
      
      
      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm_A, pm_C, pm_B, 
                                       uv_A, uv_C, uv_B));
      triangles.push_back(new Triangle(pm_B, pm_C, pm_D, 
                                       uv_B, uv_C, uv_D));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

    // Assign a halfedge pointer to the triangle
    t->halfedge = h1;

    // Assign halfedge pointers to point masses
    t->pm1->halfedge = h1;
    t->pm2->halfedge = h2;
    t->pm3->halfedge = h3;

    // Update all halfedge pointers
    h1->edge = e1;
    h1->next = h2;
    h1->pm = t->pm1;
    h1->triangle = t;

    h2->edge = e2;
    h2->next = h3;
    h2->pm = t->pm2;
    h2->triangle = t;

    h3->edge = e3;
    h3->next = h1;
    h3->pm = t->pm3;
    h3->triangle = t;
  }

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->clothMesh = clothMesh;
}
