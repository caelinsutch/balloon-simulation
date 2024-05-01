## Title: Semi-Realistic Balloon Inflation Simulation

See milestone and initial project description below 


**Team Members:**
- Alicia Shanahan
- Brandon Guo
- Caelin Sutch
- Jason Telanoff

# Abstract

Our project aimed to simulate the realistic behavior of a balloon as it inflates and interacts with other objects. We extended the project codebase and made several modifications to achieve our objectives, such as updating the PointMass class to store the normal, implementing two versions of the buildGrid function for generating points, and introducing an "inflation force" to simulate inflation and popping.



Throughout the development process, we encountered various challenges, including creating closed shapes that maintained integrity during inflation, aligning normals for visually correct results, properly laying out triangles to create a smooth surface, and debugging complex simulations. Despite these challenges, we successfully implemented a balloon simulation that exhibits realistic inflation and popping behavior in both a generated sphere and a rabbit read from a model file. While there is still room for improvement, our project demonstrates the potential for creating realistic balloon simulations using mass-spring systems and provides a foundation for future enhancements.

# Technical approach



We extended the ClothSim codebase used by the main project. To handle our new / changing shapes, we updated the code to store the normal in the PointMass class - which we updated in our simulation.



Originally, the cloth class was designed to render a flat “plane” of vertices, edges, and planes. While fine for a cloth, we ran into a few issues that are described below. Our final approach had two new implementations of buildGrid. 

The first version of buildGrid generates point masses and springs programmatically to create a cloth mesh. First, it clears all existing point masses and springs, generates the points from a specific distribution pattern based on pi and based on teh fibonacci points algorithm, then calculates the closest 4 point masses using the a helper function to create springs betweens its closest neighbor. 

The second version reads a obj file to generate a cloth mesh to create a triangular structure. For custom shapes, like bunnies, this worked great, but we ran out of time to have it properly generate spheres from obj files - leaving our first programmatic implementation for that task.

To track inflation and popping, we created a new “inflation” force applied to each point mass pointing from the “center” of the point masses (calculated by averaging out the masses) to the current point mass, effectively simulating the effect of “inflation” without having to simulate fluids or pressure differences. This inflation force increased at each time interval for realism. We calculated our new point mass positions using verlet integration, but depending on the shape chose to leave out self-collisions for simulation realism.

During each time step, we’d also check if the springs reached their “popping distance” - which was a randomized max-length that was between 2-3x the size of the rest length. This randomized length made the popping visual more accurate. If we found a point mass to have exceeded its allowed length, we’d mark it for removal. We experimented with several approaches, but found this the most realistic. 

  

# Problems



## Creating Closed Shapes

Before the checkpoint, we encountered a critical challenge in creating closed shapes that would hold integrity during inflation. The cloth class initially provided when rendered into a sphere shape would not be “closed” due to the lack of additional springs. We tried a few different shapes to see if we could fix this problem - including cubes and other geometries, but weren’t able to solve it by simply adapting the code due to core issues in the rendering algorithm. 

  

To fix this, we ended up having to rewrite portions of the point mass generation algorithm (buildGrid) - creating two versions - one that generated points programmatically and one that read from object files to create shapes. These algorithms - explained above in our technical approach, allowed us to create properly closed shapes.



## Aligning Normals

One of our largest ongoing challenges was creating and aligning normals that looked visually “correct” in our renderer and would behave properly when the shape was being inflated and popped. Since our algorithm still created a single “cloth”, the lack of normal alignment led to “swirly” normals that we attempted to fix by performing various sorts on the points before rendering, ensuring half edges aligned properly, and a few other methods. 

  

While we were able to solve enough of these issues that the simulation appears visually accurate, scaling our approach would require a significant rewrite of parts of our grid builder. 

  

Properly aligning and layout out triangles


![](https://lh7-us.googleusercontent.com/lmGfh3dqknqy5Gj2un--4EW_joWalOh2nSys5iSnvwpkU2idK5u1vT4yPP_By4ZTiR4N1uq1Eh6EquA64_9B03rqHhhHwsHlVgp7lHLzklH-__KfVkk2Bt3MuRwBlZsuX2-_pidfO0_fWw4XLAjTC_U)

One of the issues we needed to resolve for our implementation was the way the triangles were created when rendering our sphere. As seen in the above image, the sharp edges created by the triangles do not create the realistic smooth surface you would expect from a balloon. Because the starting code we chose to modify was originally built to render a rectangle, the logic behind gathering the neighboring points to create a triangle mesh needed to be much different to accommodate for a closed sphere in three dimensional space. 

  
  

Here, we can see the code we were writing to try and properly create these triangles. The goal was to use logic we had implemented earlier to get the neighboring points, and create triangles from those. When creating springs, a helper method was implemented to find the closest 4 point masses to a certain point mass. Since this logic was used for the springs, we decided to iterate through them to find nearby points. Once we find 4 points near each other on the surface of the sphere, we call a helper function to order them from point mass A to point mass D, and we create two triangles with these point masses. Unfortunately, this proved to be way more challenging than we thought and we weren’t able to get this working by the deadline.![](https://lh7-us.googleusercontent.com/pk5kJcIWiBH9IATZ0wWgMts-WMtjhMOXAwHS0bQz1CBaOF0B_gqGzAMDm8LkNG2gKk-4l8k6VnhjwWSZoqpF57dZ9kxQyQUAKos0chItKhYH32rcZH3FoswCUrboX3AMf9JJynGlfX8aVkEm0i8egdk)


![](https://lh7-us.googleusercontent.com/VB2B6fbQWSEW1e7sCRw57G18Le_ieBt6FYxSknnZFrKUwT3sP5_YYp5pRhAt0HfuLClzFNdN9CLXjpSyByEqog0CdjRrntoxTR8PhodhEPywuUf1YrSI0ffwOPYlVJ4Psfj-Ec6mh1nl0lS6dsfvtuQ)

From the terminal output above, we can see that the program segfaults after making the triangles. From printing out values from our helper function, we can see that it does not always classify every point in the way we need it to. Our initial thought was that this was the issue causing the segfault, but even after implementing various work-arounds the segfault persisted. The next thought was that because of the way we were iterating through the springs, we were creating multiple duplicate triangles. After adding checks to see if a spring had been used to make a triangle already, we still ended up with a segfault. Without clear ideas on what was causing this problem, we were unable to resolve it by the deadline. 

  

## Popping Approaches

We explored several different approaches to achieve a realistic popping effect. One approach was to use a threshold based on the maximum distance between point masses, so that when the distance exceeded a certain value, the balloon would be considered "popped" and the simulation would stop. Another approach was to calculate the total volume of the balloon based on the positions of the point masses, and use a volume threshold to trigger the popping behavior. We also experimented with using the force magnitudes acting on each spring to detect when the balloon had reached its limit. 

  

Ultimately, we found that a semi-random distance-based threshold providing a reliable trigger for the popping behavior, and the force magnitudes helping to create a more realistic visual effect of the balloon tearing apart at the seams. However, we also learned that tuning these thresholds and parameters to achieve the desired behavior required a significant amount of trial and error and iterative refinement.



# Lessons Learned

## Simulation Accuracy vs. Visual Accuracy

One important lesson we learned during this project is that simulation accuracy and visual accuracy are not always the same. In some cases, we had to make trade-offs between the two in order to achieve the desired results. For example, we found that using a more accurate physics simulation model (such as a higher-order integration method or a more complex material model) sometimes led to less visually appealing results, such as jittery or unstable motion. On the other hand, using a simpler simulation model with carefully tuned parameters often produced more visually pleasing results, even if it was less physically accurate.



## Importance of Proper Mesh Topology

Another lesson we learned is the importance of proper mesh topology for achieving realistic simulation results. We initially struggled with creating closed shapes that would hold their integrity during inflation, due to issues with the way the point masses and springs were generated. We found that having a well-structured mesh with evenly distributed point masses and properly oriented normals was crucial for achieving stable and realistic simulation results. This required us to invest significant time and effort into developing robust mesh generation algorithms that could handle a variety of shapes and topologies.



## Balancing Performance and Realism

A third lesson we learned is the importance of balancing performance and realism in real-time simulations. While it's tempting to use the most accurate and detailed simulation models possible, this can often lead to slow performance and long simulation times, especially for complex scenes with many objects and may even lead to non-realistic simulation results. 

  

## Debugging Complex Simulations

Finally, we learned that debugging complex simulations can be a significant challenge, especially when dealing with issues such as mesh topology, normal alignment, and collision detection. We often encountered segmentation faults and other hard-to-diagnose errors that required careful analysis and testing to resolve. We found that using visualization tools and debugging techniques such as printing out intermediate values was essential for identifying and fixing these issues. Additionally, we learned the importance of writing modular and well-documented code, as this made it much easier to isolate and fix problems when they arose.


## Results

**Video**:

We were able to create a successful implementation of realistic inflation and popping behaviors. The inflation process appeared smooth and natural, mimicking the behavior of a real balloon. When the springs exceeded their maximum allowed length, the popping effect was triggered, resulting in a convincing visual representation of the balloon bursting (i.e. spraying into larger pieces rather than small parts). Although there are still areas for improvement, such as further refining the triangle layout for even smoother surfaces and optimizing performance for more complex scenarios, the current implementation provides a solid foundation for simulating balloon dynamics.


## References

“Real Time Balloon Simulation” - https://www.cs.rpi.edu/~cutler/classes/advancedgraphics/S19/final_projects/michael_annie.pdf

“Semi-Realistic Balloon Simulation” - http://alumni.cse.ucsc.edu/~pault/262paper/262paper.pdf

“Fast GPU Computation of the Mass Properties of a General Shape and its Application to Buoyancy Simulation” - https://web.cs.ucla.edu/~dt/papers/pg06/pg06-preprint.pdf 

Stack Overflow Post given by TA - https://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere



## Contributions from each team member

**Jason**

Core implementation of the final reworked sphere generation problem, proper alignment of normals, debugging of bunny normal rendering and properly closing the shape.

**Caelin**

Initial implementation of sphere generation, simulated “popping” implementation and fine-tuning for spring system, some debugging of reworked sphere generation algorithm, and initial draft of final project report. 

**Alicia**

Implementation of balloon expansion off of models / other shapes, reading model objects, assistance in fixing face generation, initial draft of final project report.

**Brandon**

Brandon actively participated in the implementation of the balloon expansion feature for various models and shapes, assisted in resolving issues related to face generation, and contributed to the initial draft of the final project report.

---

# OLD README

### Summary
Our project aims to simulate the realistic behavior of a balloon as it inflates and interacts with other balloons. We will use a mass-spring simulation model and allow the user to inflate and deflate balloons created of arbitrary da. We will also implement a balloon-balloon collision detection system.

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

### Links
[Video](https://youtu.be/dOOmYj_uxRc)

[Slides](https://docs.google.com/presentation/d/1wMRkyjOQitPNDQzANH1oRUCLdz0gEiDiOveB_TAGoyQ/edit?usp=sharing)

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
