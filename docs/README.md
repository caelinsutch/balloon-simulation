## Title: Semi-Realistic Balloon Inflation Simulation

See milestone below 

### Summary
Our project aims to simulate the realistic behavior of a balloon as it inflates and interacts with other balloons. We will use a mass-spring simulation model and allow the user to inflate and deflate balloons created of arbitrary da. We will also implement a balloon-balloon collision detection system.

**Team Members:**
- Alicia Shanahan
- Brandon Guo
- Caelin Sutch

### Problem Description:
Simulating the realistic behavior of deformable objects, such as balloons, is a challenging problem in computer graphics. Accurately representing the stretching and tearing of the balloon material requires modeling its physical properties, such as elasticity and tensile strength. Additionally, the internal air pressure must be simulated to drive the balloon's expansion.

The internal air pressure will be simulated as a force acting on each point mass. The force due to pressure will be calculated using the relationship between pressure, surface area, and surface normal. This force will cause the balloon to expand and drive the dynamics of the simulation.

To simulate the balloon's dynamics, we will use Euler integration to update the positions and velocities of the point masses at each time step which allows us to approximate the motion of the point masses based on the forces acting on them and the elapsed time between each step.

By carefully tuning the spring constants, pressure values, and time step, we can achieve a realistic balloon simulation that accurately captures the material's elastic properties and the effects of internal air pressure. The spring constants will determine the stiffness of the balloon material, while the pressure values will control the rate and extent of the balloon's expansion. The time step will affect the stability and accuracy of the simulation.

### Goals and Deliverables:

We will measure the quality of our simulation by comparing the visual results to real-world balloon behavior. Success will be determined by the believability of the balloon's inflation and potential popping. Performance will be measured by the simulation's frame rate, aiming for real-time interaction.

### Schedule:

**Week 1:**
- Research and gather reference material on balloon physics and mass-spring systems
- Set up the development environment and basic project structure
- Implement the mass-spring system for the balloon surface

**Week 2:**
- Integrate internal air pressure simulation
- Add controls for inflation / deflation
- Tune spring constants and pressure values for accurate behavior

**Week 3:**
- Implement balloon popping simulation (if time allows)
- Add particle effects for escaping air (if time allows)
- Optimize simulation performance

**Week 4:**
- Refine and polish the simulation
- Prepare presentation materials
- Test and debug the final implementation

### Resources:
“Real Time Balloon Simulation” - https://www.cs.rpi.edu/~cutler/classes/advancedgraphics/S19/final_projects/michael_annie.pdf
“Semi-Realistic Balloon Simulation” - http://alumni.cse.ucsc.edu/~pault/262paper/262paper.pdf
“Fast GPU Computation of the Mass Properties of a General Shape and its Application to Buoyancy Simulation” - https://web.cs.ucla.edu/~dt/papers/pg06/pg06-preprint.pdf 

We will start the implementation from the existing poroject code and refer to relevant research papers and online resources for guidance on mass-spring systems and physically-based modeling techniques.

---

# Milestone

Create a short webpage for your milestone report. This should be about 1 page long if printed. You should briefly summarize what you have accomplished, preliminary results, reflect on progress relative to your plan, and update your work plan as appropriate. You must submit this milestone deliverable (and the final deliverable) on a persistent website that you can choose to keep up after class if you wish to present this work in your portfolio.

### Summary

The team has made progress in the first two weeks of development by implementing the mass-spring system for the balloon surface and integrating the internal air pressure simulation. However, they have encountered challenges in creating a closed shape for the balloon, which has led to difficulties in maintaining its stability. Experiments with various shapes have been unsuccessful, and the team suspects that the issue may lie within the rendering pipeline, potentially requiring a partial rewrite of the code.

### Preliminary Results

In the initial two weeks of development, significant strides have been made towards the realization of the balloon simulation project. The team has successfully implemented the fundamental mass-spring system, which serves as the foundation for simulating the elastic behavior of the balloon's surface. This achievement lays the groundwork for accurately modeling the balloon's deformation and response to external forces. Additionally, the team has integrated the internal air pressure simulation by adapting the provided starter code. This integration allows for the simulation of the balloon's internal dynamics, capturing the interaction between the air pressure and the balloon's surface.

Despite these advancements, the team has encountered a critical challenge in creating a closed shape for the balloon. The current implementation struggles to maintain the balloon's integrity, causing it to instantly pop due to the lack of additional springs that would effectively seal the shape. This instability hinders the realistic representation of the balloon's behavior and compromises the overall simulation quality. To address this issue, the team has experimented with various balloon shapes, including cubes and custom geometries. However, these attempts have not yielded the desired results, as the fundamental problem of achieving a closed shape persists.

Upon further investigation, the team suspects that the root cause of the issue may reside within the rendering pipeline. The current pipeline may not be adequately optimized to handle the specific requirements of the balloon simulation, particularly in terms of maintaining a closed shape. Consequently, a partial rewrite of the code may be necessary to address these underlying limitations. This rewrite would involve a thorough examination of the rendering process, identifying potential bottlenecks, and implementing targeted optimizations. By restructuring the relevant portions of the codebase, the team aims to enhance the rendering pipeline's ability to support the creation and maintenance of closed shapes, ultimately enabling a more stable and realistic balloon simulation.

**Current Implementation** - [Current Balloon Simulation](https://www.loom.com/share/d8ca0be555a54f0bb386dedfad2b7995)

In the current iteration, it's expanding and "pops" (really just tears from a lack of closed shape) almost immediately. While it looks alright, we could adapt this by adding springs to the edges of the balloon to keep it closed, then at a certain spring "length" have the spring randomly disconnect which should create more realistic popping (and is similar to what was done in "Semi-Realistic Balloon Simulation".

**Failing Expansion Logic**

```cpp
// Apply outward force to each point mass
double inflation_force = 1.0; // Adjust this value to control the inflation strength
for (int y = 0; y < num_height_points; y++) {
    for (int x = 0; x < num_width_points; x++) {
        int index = y * num_width_points + x;
        PointMass& pm = point_masses[index];

        Vector3D center = Vector3D(width / 2.0, height / 2.0, 0.0);
        Vector3D direction = pm.position - center;
        double distance = direction.norm();

        if (distance > 0.0) {
            direction.normalize();
            pm.forces += direction * inflation_force;
        }
    }
}
```

We calculate the center of the cloth as the midpoint of the width and height.
For each point mass, we compute the direction vector from the center of the cloth to the point mass position.

- [With Failing Expansion Logic](https://www.loom.com/share/2fd85fa057d04513b58a0bb862e3ed2b)

## Update Video
[https://youtu.be/dOOmYj_uxRc](https://youtu.be/dOOmYj_uxRc)

### Progress Relative to Plan

We're currently behind schedule due to the issues with the balloon shape. We will need to spend more time debugging and refactoring the code to achieve a closed shape. We will also need to adjust our work plan to account for the additional time required to complete this task.

Controls are not yet implemented, but we have a plan for how to implement them. We will need to integrate the controls and inflation/deflation features in the next week to stay on track.

### Updated Work Plan

**Week 3:**
- Refactor the code to create a closed balloon shape
- Implement controls for inflation / deflation
- Tune spring constants and pressure values for accurate behavior

- **Week 4:**
- Implement balloon popping simulation (if time allows)
- Add custom shape rendering
- Prepare presentation materials
