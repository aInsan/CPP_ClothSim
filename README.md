# Cloth Simulation

This program simulates a cloth made up of a grid of points with interactions between them. The simulation is displayed using the graphics library Raylib.

# How to Use

Either Download/Compile the program or go [here](https://aInsan.github.io/)*THE SIM MAY RUN VERY SLOW ON WEB* to run the simulation
The simulation will start as soon as the program is run. You can interact with the cloth by clicking to delete points.

# Dependencies
This project was made possible thanks to the amazing [raylib](https://www.raylib.com/) library.

Raylib - Graphics library used to display the simulation.

Raymath - Math library for Raylib, used for vector arithmetic.

# Compiling and Running

Either download the latest release for windows or is you are on another platform, 
Clone the repository and navigate to the directory.
```
git clone https://github.com/aInsan/Simple_CPP_ClothSim.git
cd Simple_CPP_ClothSim
```
  Compile using your favorite C++ compiler
  
*Make sure you have the dependencies.*

Run the program.

# Details

The cloth is represented as a two-dimensional array of Point objects. Each Point object has a position vector, velocity vector. The cloth is initialized by placing each point at a certain coordinate with a velocity vector of zero. The top-left, top-middle, and top-right points are set to be anchor points.

The ApplyConstraints function makes sure that the points do not stretch apart. It applies constraints along the horizontal and vertical directions. If the distance between two points is longer than the maximum length, and both points are not anchor points, the points are moved so that their distance stays within bounds.
