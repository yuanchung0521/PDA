# PDA hw4 Channel Routing

This code is designed for optimizing channel routing in a circuit layout. The optimization involves constructing a grid-based representation of the layout, assigning segments to tracks, and minimizing the channel density. The program reads an input file specifying top and bottom boundaries, nets, and their connections, and produces an output file with optimized channel routing details.

## Files Overview

### `main.cpp`

- This file serves as the main driver for the channel routing optimization.
- It reads the input file, constructs the grid, performs constrained left-edge optimization, and outputs the final channel routing details.
- The optimization aims to minimize channel density and efficiently route nets through available tracks.

### `classtype.h`

- This header file defines the structures used in channel routing.
- It includes structures for boundaries (`Boundary`), nets (`Net`), segments (`Segment`), and tracks (`Track`).
- These structures are used to represent the layout and connections in the channel routing problem.

## How to Use

1. **Input Format**: The input file should follow a specific format, specifying top and bottom boundaries, nets, and their connections. Refer to the provided examples for correct formatting.

2. **Run the Program**:
   - Compile the program using the provided Makefile: `make`
   - Run the program: `./Lab4 input_file.txt output_file.txt`
     - Replace `input_file.txt` with the path to your input file.
     - Replace `output_file.txt` with the desired output file.

3. **Output Format**: The output file will contain the optimized channel routing details, including the channel density, net routing information, and any additional details specified in the program.

## Notes

- The code assumes a specific input format for correct processing. Ensure that the input follows the expected format to avoid unexpected behavior.
- Validate the correctness of the channel routing optimization for your specific use case, considering the provided constraints.
- Adjust the code as needed for different input formats or additional constraints in the channel routing problem.