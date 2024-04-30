#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

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

double pointMassDist(PointMass* pm0, PointMass* pm1) {
	// Calculate differences in x, y, and z coordinates
	double dx = pm1->position.x - pm0->position.x;
	double dy = pm1->position.y - pm0->position.y;
	double dz = pm1->position.z - pm0->position.z;

	// Calculate the Euclidean distance using the formula sqrt(dx^2 + dy^2 + dz^2)
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

// Define a struct to hold the PointMass pointer and its distance
struct PointMassDistance {
	PointMass* pointMass;
	double distance;
};

// Comparator function to sort PointMassDistance objects based on distance
bool comparePointMassDistance(const PointMassDistance& a, const PointMassDistance& b) {
	return a.distance < b.distance;
}

std::vector<PointMass*> closestAfter(PointMass* pm, int i, int samples, std::vector<PointMass>& point_masses) {
	std::vector<PointMassDistance> distances;

	for (++i; i < samples; ++i) {
		double dist = pointMassDist(pm, &point_masses[i]);
		distances.push_back({ &point_masses[i], dist });
	}

	// Sort the distances vector based on distance
	std::sort(distances.begin(), distances.end(), comparePointMassDistance);

	// Create a vector to hold the closest 4 PointMass pointers
	std::vector<PointMass*> closestPoints;

	// Add the 4 closest PointMass pointers to the closestPoints vector
	for (int j = 0; j < std::min(4, static_cast<int>(distances.size())); ++j) {
		closestPoints.push_back(distances[j].pointMass);
	}

	return closestPoints;
}


void Cloth::buildGrid() {
	// Clear any existing point masses and springs
	point_masses.clear();
	springs.clear();

	//float radius = 0.5;
	double phi = PI * (sqrt(5.0) - 1.0);
	int total = num_height_points * num_width_points;
	int samples = total / 4;

	printf("Generating points\n");

	// Generate point masses on a sphere
	for (int i = 0; i < total; ++i) {
		if (i >= samples) {
			point_masses.emplace_back(Vector3D(0, 0, 0), false);
			continue;
		}
		float y = 1 - (i / float(samples - 1)) * 2;
		float radius = sqrt(1 - y * y);

		double theta = phi * i;

		float x = cos(theta) * radius;
		float z = sin(theta) * radius;

		// Create point mass and add to the list
		point_masses.emplace_back(Vector3D(x, y + 1.0, z), false);
	}

	point_masses[0].pinned = true;

	printf("Generating springs\n");

	// Generate springs
	for (int i = 0; i < samples; ++i) {
		PointMass* pm = &point_masses[i];
		vector<PointMass*> closest4 = closestAfter(pm, i, samples, point_masses);
		for (int j = 0; j < closest4.size(); ++j) {
			springs.emplace_back(pm, closest4[j], SHEARING);
		}
	}
}

/*void Cloth::buildGridHelper(double x_offset, double y_offset, double z_offset, bool horizontal) {
	// TODO (Part 1): Build a grid of masses and springs.

	  // Clear any existing point masses and springs

	// Calculate the spacing between point masses
	double dx = width / (num_width_points - 1);
	double dy = height / (num_height_points - 1);

	// Generate point masses
	for (int j = 0; j < num_height_points; ++j) {
		for (int i = 0; i < num_width_points; ++i) {
			// Calculate position based on orientation
			double x = i * dx;
			double y = horizontal ? 1.0 + (rand() % 2000 - 1000) / 1000000.0 : j * dy;
			double z = horizontal ? j * dy : 1.0 + (rand() % 2000 - 1000) / 1000000.0;

			// Create point mass and add to the list
			point_masses.emplace_back(Vector3D(x + x_offset, y + x_offset, z + x_offset), false);
		}
	}

	// Pin pinned
	for (std::vector<int> l : pinned) {
		int x = l[0];
		int y = l[1];
		point_masses[y * num_width_points + x].pinned = true;
	}



	// Generate springs
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
*/
void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters* cp,
	vector<Vector3D> external_accelerations,
	vector<CollisionObject*>* collision_objects) {
	double mass = width * height * cp->density / num_width_points / num_height_points;
	double delta_t = 1.0f / frames_per_sec / simulation_steps;

	// TODO (Part 2): Compute total force acting on each point mass.
	for (PointMass& pm : point_masses) {
		pm.forces = Vector3D();
		// External forces
		for (const Vector3D& a : external_accelerations) {
			pm.forces += mass * a;
		}
	}

	for (Spring& s : springs) {
		if ((s.spring_type == STRUCTURAL && cp->enable_structural_constraints) ||
			(s.spring_type == BENDING && cp->enable_bending_constraints) ||
			(s.spring_type == SHEARING && cp->enable_shearing_constraints)) {
			Vector3D force_dir = s.pm_a->position - s.pm_b->position;
			double current_length = force_dir.norm();
			double displacement = current_length - s.rest_length;

			// Calculate the force magnitude based on Hooke's Law
			double force_magnitude = (s.spring_type == BENDING ? 0.2 : 1.0) * cp->ks * displacement;

			// Apply equal and opposite forces to the point masses
			Vector3D force_a = force_dir.unit() * force_magnitude;
			Vector3D force_b = -force_a;

			s.pm_a->forces -= force_a;
			s.pm_b->forces -= force_b;
		}
	}

	inflation_force += 0.1;

	for (Spring s : springs) {
		PointMass* pa = s.pm_a;
		PointMass* pb = s.pm_b;

		pa->norm += pa->position - pb->position;
		pb->norm += pb->position - pa->position;
	}

	/*for (PointMass& pm : point_masses) {
		pm.forces += pm.norm.unit() * inflation_force;
	}*/

	Vector3D avg_pos(0, 0, 0);
	for (const PointMass& pm : point_masses) {
		avg_pos += pm.position;
	}
	avg_pos /= point_masses.size();

	for (PointMass& pm : point_masses) {
		float dist = (pm.position - avg_pos).norm();
		pm.forces += (avg_pos - pm.position).unit() * inflation_force / (dist * dist);
	}

	// TODO (Part 2): Use Verlet integration to compute new point mass positions
	for (PointMass& pm : point_masses) {
		if (!pm.pinned) {
			// Compute new position using Verlet integration
			Vector3D new_position = pm.position + (1 - cp->damping / 100.0) * (pm.position - pm.last_position) + pm.forces / mass * delta_t * delta_t;

			// Update position and last_position
			pm.last_position = pm.position;
			pm.position = new_position;

			// Reset forces for the next time step
			pm.forces = Vector3D();
		}
	}

	// TODO (Part 4): Handle self-collisions.
	/*build_spatial_map();
	for (PointMass& p : point_masses)
		self_collide(p, simulation_steps);*/

		// TODO (Part 3): Handle collisions with other primitives.
	for (const auto& c : *collision_objects) {
		for (PointMass& pm : point_masses)
			c->collide(pm);
	}


	// TODO (Part 2): Constrain the changes to be such that the spring does not change
	// in length more than 10% per timestep [Provot 1995].
	// Apply deformation constraints to springs
	/*for (Spring& s : springs) {
		if (!s.pm_a->pinned || !s.pm_b->pinned) {
			// Calculate the current length of the spring
			double current_length = (s.pm_a->position - s.pm_b->position).norm();

			// Calculate the maximum allowed length (10% greater than rest length)
			double max_length = 1.1 * s.rest_length;

			// Check if the current length exceeds the maximum allowed length
			if (current_length > max_length) {
				// Compute the correction factor
				double correction_factor = (max_length - current_length) / current_length;

				// Compute the correction vector along the direction of the spring
				Vector3D correction = correction_factor * (s.pm_b->position - s.pm_a->position);

				// Apply half of the correction to each point mass, unless one is pinned
				if (!s.pm_a->pinned && !s.pm_b->pinned) {
					s.pm_a->position -= 0.5 * correction;
					s.pm_b->position += 0.5 * correction;
				}
				else if (!s.pm_a->pinned) {
					s.pm_a->position -= correction;
				}
				else if (!s.pm_b->pinned) {
					s.pm_b->position += correction;
				}
			}
		}
	}*/

}

void Cloth::build_spatial_map() {
	map.clear();

	// TODO (Part 4): Build a spatial map out of all of the point masses.
	for (PointMass& pm : point_masses) {
		float hash = hash_position(pm.position);
		if (map.find(hash) == map.end()) {
			map[hash] = new vector<PointMass*>();
		}
		map[hash]->push_back(&pm);
	}
}

void Cloth::self_collide(PointMass& pm, double simulation_steps) {
	// Calculate hash for the given point mass position
	float hash = hash_position(pm.position);

	// Look up candidates for collision from the hash map
	auto it = map.find(hash);
	double threshold = 2.0 * thickness;

	if (it != map.end()) {
		std::vector<PointMass*>& candidates = *(it->second);

		// Iterate through each candidate for collision
		for (PointMass* candidate_pm : candidates) {
			if (candidate_pm == &pm) continue; // Avoid self-collision

			// Calculate distance between point masses
			double distance = (pm.position - candidate_pm->position).norm();

			// Check if collision occurs
			if (distance < threshold) {
				// Compute correction vector towards separation
				Vector3D correction = (threshold - distance) * (pm.position - candidate_pm->position).unit();

				// Apply correction to positions (scaled by simulation_steps)
				pm.position += 0.5 * correction / simulation_steps;
				candidate_pm->position -= 0.5 * correction / simulation_steps;
			}
		}
	}
}


float Cloth::hash_position(Vector3D pos) {
	// TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.
	double w = 3.0 * width / num_width_points;
	double h = 3.0 * height / num_height_points;
	double t = max(w, h);

	float x = pos.x / w;
	float y = pos.y / h;
	float z = pos.z / t;

	float hash_key = (int)x + (int)y * num_width_points + (int)z * num_width_points * num_height_points;

	return hash_key;
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
	PointMass* pm = &point_masses[0];
	for (int i = 0; i < point_masses.size(); i++) {
		pm->position = pm->start_position;
		pm->last_position = pm->start_position;
		pm++;
	}
}

void Cloth::buildClothMesh() {
	if (point_masses.size() == 0) return;

	ClothMesh* clothMesh = new ClothMesh();
	vector<Triangle*> triangles;

	for (int i = 0; i < springs.size(); i++) {
		PointMass* p1a = springs[i].pm_a;
		PointMass* p1b = springs[i].pm_b;

		for (int j = i + 1; j < i + 5; j++) {
			PointMass* p2a = springs[j].pm_a;
			PointMass* p2b = springs[j].pm_a;

			if (p1a == p2a) {
				for (int k = j + 1; k < i + 10; k++) {
					if (p1a == springs[k].pm_a && p2b == springs[k].pm_b) {
						Vector3D zero_vec;

						triangles.push_back(new Triangle(p1a, p1b, p2b,
							zero_vec, zero_vec, zero_vec));
					}
				}
			}
		}
	}

	// Create vector of triangles
	for (int y = 0; y < num_height_points - 1; y++) {
		for (int x = 0; x < num_width_points - 1; x++) {
			PointMass* pm = &point_masses[y * num_width_points + x];
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

			PointMass* pm_A = pm;
			PointMass* pm_B = pm + 1;
			PointMass* pm_C = pm + num_width_points;
			PointMass* pm_D = pm + num_width_points + 1;

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
		Triangle* t = triangles[i];

		// Allocate new halfedges on heap
		Halfedge* h1 = new Halfedge();
		Halfedge* h2 = new Halfedge();
		Halfedge* h3 = new Halfedge();

		// Allocate new edges on heap
		Edge* e1 = new Edge();
		Edge* e2 = new Edge();
		Edge* e3 = new Edge();

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
		Triangle* t = triangles[i];

		if (topLeft) {
			// Get left triangle, if it exists
			if (i % num_width_tris != 0) { // Not a left-most triangle
				Triangle* temp = triangles[i - 1];
				t->pm1->halfedge->twin = temp->pm3->halfedge;
			}
			else {
				t->pm1->halfedge->twin = nullptr;
			}

			// Get triangle above, if it exists
			if (i >= num_width_tris) { // Not a top-most triangle
				Triangle* temp = triangles[i - num_width_tris + 1];
				t->pm3->halfedge->twin = temp->pm2->halfedge;
			}
			else {
				t->pm3->halfedge->twin = nullptr;
			}

			// Get triangle to bottom right; guaranteed to exist
			Triangle* temp = triangles[i + 1];
			t->pm2->halfedge->twin = temp->pm1->halfedge;
		}
		else {
			// Get right triangle, if it exists
			if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
				Triangle* temp = triangles[i + 1];
				t->pm3->halfedge->twin = temp->pm1->halfedge;
			}
			else {
				t->pm3->halfedge->twin = nullptr;
			}

			// Get triangle below, if it exists
			if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
				Triangle* temp = triangles[i + num_width_tris - 1];
				t->pm2->halfedge->twin = temp->pm3->halfedge;
			}
			else {
				t->pm2->halfedge->twin = nullptr;
			}

			// Get triangle to top left; guaranteed to exist
			Triangle* temp = triangles[i - 1];
			t->pm1->halfedge->twin = temp->pm2->halfedge;
		}

		topLeft = !topLeft;
	}

	clothMesh->triangles = triangles;
	this->clothMesh = clothMesh;
}
