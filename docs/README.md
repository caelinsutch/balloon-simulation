## Title: Semi-Realistic Balloon Inflation Simulation

### Summary
Our project aims to simulate the realistic behavior of a balloon as it inflates and interacts with other balloons. We will use a mass-spring simulation model and allow the user to inflate and deflate balloons created of arbitrary da. We will also implement a balloon-balloon collision detection system.

**Team Members:**
- Alicia Shanahan
- Brandon Guo
- Caelin Sutch
- Jason Telanoff

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
