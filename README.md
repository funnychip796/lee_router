# Lee-Moore Grid Router

A simple grid routing tool based on the classic **Lee-Moore Algorithm** (also known as Lee's maze algorithm). It finds the shortest path between pins on a grid while avoiding obstacles.

## How the Algorithm Works

1. **Wave Expansion (Spread)**: Starts from the source pin and spreads outwards to adjacent cells step-by-step (like ripples in water) until it reaches the destination pin.
2. **Backtracing (Retrace)**: Once the destination is found, the algorithm traces back through the recorded steps to draw the shortest path.
3. **Re-routing (Retry)**: If a wire fails to route due to blockages, the grid is cleared, the failed wire is moved to the highest priority (front of the queue), and routing restarts.

## Compilation

Compile the program using `g++`:
```bash
g++ -O3 main.cpp -o lee_router
```

## Usage

Run the program by providing your grid netlist file:
```bash
./lee_router <netlist_file>
```

### Example
```bash
./lee_router grid.net
```
The program will print the routing waves, paths, and a summary directly to the screen (stdout).

## Running Test Cases

The repository includes a `testdata` folder containing sample netlists for testing:
- `testdata/sample.net` (Small grid test)
- `testdata/testcase1.net` (Standard grid test case)
- `testdata/testcase2.net` (Test case with more obstructions)

To run a test case, pass the path of the netlist file to the compiled executable. For example:
```bash
./lee_router testdata/testcase1.net
```
