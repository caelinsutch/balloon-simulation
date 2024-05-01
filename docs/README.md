# Abstract

Our project aimed to simulate the realistic behavior of a balloon as it inflates and interacts with other objects. We extended the project codebase and made several modifications to achieve our objectives, such as updating the PointMass class to store the normal, implementing two versions of the buildGrid function for generating points, and introducing an "inflation force" to simulate inflation and popping.

Throughout the development process, we encountered various challenges, including creating closed shapes that maintained integrity during inflation, aligning normals for visually correct results, properly laying out triangles to create a smooth surface, and debugging complex simulations. Despite these challenges, we successfully implemented a balloon simulation that exhibits realistic inflation and popping behavior in both a generated sphere and a rabbit read from a model file. While there is still room for improvement, our project demonstrates the potential for creating realistic balloon simulations using mass-spring systems and provides a foundation for future enhancements.

# Technical approach

We extended the ClothSim codebase used by the main project. To handle our new / changing shapes, we updated the code to store the normal in the PointMass class - which we updated in our simulation.

Originally, the cloth class was designed to render a flat “plane” of vertices, edges, and planes. While fine for a cloth, we ran into a few issues that are described below. Our final approach had two new implementations of buildGrid. 

The first version of buildGrid generates point masses and springs programmatically to create a cloth mesh. First, it clears all existing point masses and springs, generates the points from a specific distribution pattern based on pi and based on the fibonacci points algorithm, then calculates the closest 4 point masses using a helper function to create springs betweens its closest neighbor. 

The second version generates the mesh from an OBJ file. For custom shapes, like bunnies, this worked great, but we ran out of time to have it properly generate spheres from obj files - leaving our first programmatic implementation for that task.

To track inflation and popping, we created a new “inflation” force applied to each point mass pointing from the “center” of the point masses (calculated by averaging out the masses) to the current point mass, effectively simulating the effect of “inflation” without having to simulate fluids or pressure differences. This inflation force increased at each time interval for realism, and scaled down based on distance to the center to simulate an expanding volume. We calculated our new point mass positions using verlet integration, but depending on the shape chose to leave out self-collisions for simulation realism.

During each time step, we’d also check if the springs reached their “popping distance” - which was a randomized max-length that was between 2-3x the size of the rest length. This randomized length made the popping visual more accurate. If we found a point mass to have exceeded its allowed length, we’d mark it for removal. We experimented with several approaches, but found this the most realistic. 

# Problems

### Creating Closed Shapes

Before the checkpoint, we encountered a critical challenge in creating closed shapes that would hold integrity during inflation. The cloth class initially provided when rendered into a sphere shape would not be “closed” due to the lack of additional springs. We tried a few different shapes to see if we could fix this problem - including cubes and other geometries, but weren’t able to solve it by simply adapting the code due to core issues in the rendering algorithm. 

To fix this, we ended up having to rewrite portions of the point mass generation algorithm (buildGrid) - creating two versions - one that generated points programmatically and one that read from object files to create shapes. These algorithms - explained above in our technical approach, allowed us to create properly closed shapes.

### Aligning Normals

One of our largest ongoing challenges was creating and aligning normals that looked visually “correct” in our renderer and would behave properly when the shape was being inflated and popped. Since our algorithm still created a single “cloth”, the lack of normal alignment led to “swirly” normals that we attempted to fix by performing various sorts on the points before rendering, ensuring half edges aligned properly, and a few other methods. 

While we were able to solve enough of these issues that the simulation appears visually accurate, scaling our approach would require a significant rewrite of parts of our grid builder. 

### Properly aligning and layout out triangles


![](https://lh7-us.googleusercontent.com/lmGfh3dqknqy5Gj2un--4EW_joWalOh2nSys5iSnvwpkU2idK5u1vT4yPP_By4ZTiR4N1uq1Eh6EquA64_9B03rqHhhHwsHlVgp7lHLzklH-__KfVkk2Bt3MuRwBlZsuX2-_pidfO0_fWw4XLAjTC_U)

One of the issues we needed to resolve for our implementation was the way the triangles were created when rendering our sphere. As seen in the above image, the sharp edges created by the triangles do not create the realistic smooth surface you would expect from a balloon. Because the starting code we chose to modify was originally built to render a rectangle, the logic behind gathering the neighboring points to create a triangle mesh needed to be much different to accommodate for a closed sphere in three dimensional space. 

Here, we can see the code we were writing to try and properly create these triangles. The goal was to use logic we had implemented earlier to get the neighboring points, and create triangles from those. When creating springs, a helper method was implemented to find the closest 4 point masses to a certain point mass. Since this logic was used for the springs, we decided to iterate through them to find nearby points. Once we find 4 points near each other on the surface of the sphere, we call a helper function to order them from point mass A to point mass D, and we create two triangles with these point masses. Unfortunately, this proved to be way more challenging than we thought and we weren’t able to get this working by the deadline.![](https://lh7-us.googleusercontent.com/pk5kJcIWiBH9IATZ0wWgMts-WMtjhMOXAwHS0bQz1CBaOF0B_gqGzAMDm8LkNG2gKk-4l8k6VnhjwWSZoqpF57dZ9kxQyQUAKos0chItKhYH32rcZH3FoswCUrboX3AMf9JJynGlfX8aVkEm0i8egdk)

![](https://lh7-us.googleusercontent.com/VB2B6fbQWSEW1e7sCRw57G18Le_ieBt6FYxSknnZFrKUwT3sP5_YYp5pRhAt0HfuLClzFNdN9CLXjpSyByEqog0CdjRrntoxTR8PhodhEPywuUf1YrSI0ffwOPYlVJ4Psfj-Ec6mh1nl0lS6dsfvtuQ)

From the terminal output above, we can see that the program segfaults after making the triangles. From printing out values from our helper function, we can see that it does not always classify every point in the way we need it to. Our initial thought was that this was the issue causing the segfault, but even after implementing various work-arounds the segfault persisted. The next thought was that because of the way we were iterating through the springs, we were creating multiple duplicate triangles. After adding checks to see if a spring had been used to make a triangle already, we still ended up with a segfault. Without clear ideas on what was causing this problem, we were unable to resolve it by the deadline. 

However, we were able to utilize the OBJ file format to create a triangle mesh for our OBJ files.

### Popping Approaches

We explored several different approaches to achieve a realistic popping effect. One approach was to use a threshold based on the maximum distance between point masses, so that when the distance exceeded a certain value, the balloon would be considered "popped" and the simulation would stop. Another approach was to calculate the total volume of the balloon based on the positions of the point masses, and use a volume threshold to trigger the popping behavior. We also experimented with using the force magnitudes acting on each spring to detect when the balloon had reached its limit. 

Ultimately, we found that a semi-random distance-based threshold provided a reliable trigger for the popping behavior, and the force magnitudes helped to create a more realistic visual effect of the balloon tearing apart at the seams. However, we also learned that tuning these thresholds and parameters to achieve the desired behavior required a significant amount of trial and error and iterative refinement.

### The Cloth Sim Project

It became abundantly clear throughout making this project that the cloth sim homework was not built for this. Several values and algorithms were coded in such a way that only the original rectangular cloth would function properly. One such issue we ran into was the need to create a new mesh creation algorithm. We also ran into several other problems, and had to recode a couple portions of the starter code that were clearly marked “YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS.” This, on top of random bugs that would take us to files we’d never heard of before, made the development process a bit slower as we had to learn and understand the code given to us much more than through the homeworks.

One very annoying problem is that, for whatever reason, the code seems to run out of memory when too many springs are drawn as a wiremesh. The program would crash, and not say where it crashed or why it crashed, just that it reached some _breakpoint. The only way we found that solved this issue was only rendering 80% of the springs when using more complicated OBJ files. In the end, when to use the code given to you, but also when to bite the bullet and reimplement it yourself.

# Lessons Learned

### Simulation Accuracy vs. Visual Accuracy

One important lesson we learned during this project is that simulation accuracy and visual accuracy are not always the same. In some cases, we had to make trade-offs between the two in order to achieve the desired results. For example, we found that using a more accurate physics simulation model (such as a higher-order integration method or a more complex material model) sometimes led to less visually appealing results, such as jittery or unstable motion. On the other hand, using a simpler simulation model with carefully tuned parameters often produced more visually pleasing results, even if it was less physically accurate.

### Importance of Proper Mesh Topology

Another lesson we learned is the importance of proper mesh topology for achieving realistic simulation results. We initially struggled with creating closed shapes that would hold their integrity during inflation, due to issues with the way the point masses and springs were generated. We found that having a well-structured mesh with evenly distributed point masses and properly oriented normals was crucial for achieving stable and realistic simulation results. This required us to invest significant time and effort into developing robust mesh generation algorithms that could handle a variety of shapes and topologies.

### Balancing Performance and Realism

A third lesson we learned is the importance of balancing performance and realism in real-time simulations. While it's tempting to use the most accurate and detailed simulation models possible, this can often lead to slow performance and long simulation times, especially for complex scenes with many objects and may even lead to non-realistic simulation results. 

### Debugging Complex Simulations

Finally, we learned that debugging complex simulations can be a significant challenge, especially when dealing with issues such as mesh topology, normal alignment, and collision detection. We often encountered segmentation faults and other hard-to-diagnose errors that required careful analysis and testing to resolve. We found that using visualization tools and debugging techniques such as printing out intermediate values was essential for identifying and fixing these issues. Additionally, we learned the importance of writing modular and well-documented code, as this made it much easier to isolate and fix problems when they arose.

### Spheres are hard

We started by very simply morphing the cloth mesh from the homework into a sphere shape. We quickly learned this would not work, as the sphere isn’t actually closed, and thus, it would not act like a balloon. After receiving some fantastic advice from the wonderful CS 184 course staff (whom we are all extremely grateful for the work they put into making this course amazing), we implemented a different approach. Scrap the mesh generation completely, and implement one specifically for generating equidistant points on a sphere. This didn’t come easy, especially connecting the points with correct springs, but it all worked out in the end.

# Results

While there are quite a few improvements that can be made, we ended up with a program we’re proud of. The inflation and explosions are very satisfying (we probably spent upwards of an hour just watching them on repeat), and the code runs in mostly real time much to our surprise.

Here is a video showing off some of the final project: [https://youtu.be/5wX3CTwHaPw](https://youtu.be/5wX3CTwHaPw), though some of the best looking things were bugs that we’ve since fixed.

# If we had more time…

This project ended up taking a lot more time than expected. We came into it with dozens of ideas to make the greatest balloon simulator possible, and got close, but not quite there. If we had more time we would have liked to add the following:

### Sphere Mesh

An obvious one, as shown earlier, we got really close to creating an accurate mesh for our sphere, but couldn’t quite finish. We probably spent the most time on this one feature, and are mildly annoying we didn’t get it perfect.

### Different Air

We wanted to add buoyancy. Fill the balloon up with helium and watch it float, or fill it with sulfur hexafluoride and watch it fall quickly. Something along these lines would be really cool and fit in perfectly with a balloon simulation, but we sadly didn’t have enough time to get a working prototype.

### Fix Spring Rendering

Something that is still bothering us is why we can’t render all of the bunny’s springs. It doesn’t seem like anything in the code is stopping it, and Task Manager shows lower memory usage than other examples.

### Deflation

We always imagined a balloon flying around the room because someone opened the hole. We brainstormed ways to add this, with a slow deflation or a rapid one, but nothing worked how we wanted.

### Stretchy Rubber

As brought up by course staff in our feedback document, it would be really cool if the cloth would stretch out as it inflates, such that it wouldn’t deflate to its original size. Or, changing the spring constants as it expands to simulate the rubber stretching and losing its strength.

# References

“Real Time Balloon Simulation” - https://www.cs.rpi.edu/~cutler/classes/advancedgraphics/S19/final_projects/michael_annie.pdf

“Semi-Realistic Balloon Simulation” - http://alumni.cse.ucsc.edu/~pault/262paper/262paper.pdf

“Fast GPU Computation of the Mass Properties of a General Shape and its Application to Buoyancy Simulation” - https://web.cs.ucla.edu/~dt/papers/pg06/pg06-preprint.pdf 

https://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere


## Contributions from each team member

**Jason**

Core implementation of the final reworked sphere generation problem, proper alignment of normals, OBJ file support, debugging of bunny mesh and normal rendering. 

**Caelin** 

Initial implementation of sphere generation, simulated “popping” implementation and fine-tuning for spring system, some debugging of reworked sphere generation algorithm, and initial draft of final project report. 
  
**Alicia**

Implementation of balloon expansion off of models / other shapes, assistance in fixing face generation, initial draft of final project report.

**Brandon**

Brainstorming, project management, team communications, improving sphere creation and inflation, debugging as needed.

**