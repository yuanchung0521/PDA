# PDA hw3 Placement Assignment

This code is designed for optimizing the placement of NMOS and PMOS transistors in a circuit layout to minimize the Half Perimeter Wirelength (HPWL). The placement optimization is achieved by finding the Euler path through the circuit graph for both NMOS and PMOS transistors and aligning them appropriately.

## Files Overview

### `main.cpp`

- This file contains the main driver code for the placement optimization.
- It reads the input circuit description file, constructs the circuit graphs for NMOS and PMOS transistors, and performs the placement optimization.
- The optimization involves finding Euler paths through both graphs, considering possible alignments, and minimizing the HPWL.

### `classtype.h`

- This header file defines the structures used in the placement optimization.
- It includes structures for nodes (`NODE`), edges (`EDGE`), nets (`NET`), and the circuit graph (`GRAPH`).
- The `RECORD` structure is used to record and print the optimized placement.

### `euler.cpp`

- This file implements functions related to Euler path generation and HPWL calculation.
- The `performDFS` function finds Euler paths through the circuit graph.
- The `calHPWL` function calculates the HPWL based on the optimized placement.

## How to Use

1. **Input Format**: The input circuit description file should follow a spice format where each line represents a transistor in the circuit.

2. **Run the Program**:
   - Compile the program with the Makefile `make`
   - Run the program: `./Lab3 input_file.txt output_file.txt`
     - Replace `input_file.txt` with the path to your input file.
     - Replace `output_file.txt` with the desired output file.

3. **Output Format**: The output file will contain the optimized placement details, including the HPWL and the transistor placement for both NMOS and PMOS with Net information.


## Notes

- The code assumes that the input circuit is provided in a specific format, and any deviations may result in unexpected behavior.
- Check the correctness of the Euler path generation and HPWL calculation algorithms for the specific use case, this is just for connected graph.
- Adjust the code as needed for different input formats or additional constraints.
