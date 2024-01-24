# PDA hw2 Floorplan Assignment

## Introduction
This repository contains a C++ implementation for solving floorplan optimization problems. The program utilizes Simulated Annealing (SA) and a B* Tree (BST) to efficiently arrange blocks in a given ratio, aiming to minimize the total area while adhering to user-defined aspect ratio constraints.

## Usage
To run the program, provide two command-line arguments: the input file containing block dimensions and the desired output file for the optimized floorplan.

```bash
./Floorplan input.txt output.txt
```

## Input File Format
The input file should adhere to the following format:
- The first line contains lower and upper bounds for the aspect ratio (Rl, Ru).
- Subsequent lines represent each block with its name, width, and height.

Example:
```
1.5 2.5
Block1 5 2
Block2 3 4
Block3 2 1
```

## Code Structure
- **main.cpp**: The main program that reads input, constructs the BST, and applies the SA algorithm to optimize the floorplan.
- **classtype.h**: Header file containing class and struct declarations for BLOCK, NODE, CONTOUR, BST, and ANSWER.
- **contour.cpp**: Implementation of functions related to the CONTOUR struct.
- **bst.cpp**: Implementation of functions related to the BST class.
- **sa.cpp**: Implementation of functions related to the SA algorithm.

## Algorithm Overview
The floorplan optimization is achieved through a combination of Binary Space Partitioning and Simulated Annealing:
1. The input blocks are initially arranged in a BST.
2. The SA algorithm perturbs the arrangement to explore new configurations.
3. The cost function considers the total area and aspect ratio, penalizing configurations violating user-defined constraints.
4. SA accepts or rejects perturbations based on the Metropolis criterion.
5. The process continues until convergence or a specified number of iterations.

## Running SA Algorithm
The SA algorithm is invoked by calling the `SA` function in `main.cpp`. The algorithm aims to find an optimized floorplan with reduced area and improved aspect ratio.

## Class Definitions
- **BLOCK**: Represents a block with a name, width, height, rotation status, position, and contour information.
- **NODE**: Represents a node in the BST, containing information about the associated block and its position in the tree.
- **CONTOUR**: Represents the contour of the floorplan, including X and Y coordinates and links to adjacent contours.
- **BST**: Manages the Binary Space Partitioning Tree, including construction, rotation, swapping, and moving operations.
- **ANSWER**: Represents the solution with the best arrangement, providing details about area, aspect ratio, and block positions.
