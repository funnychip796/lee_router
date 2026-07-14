# Lee-Moore Grid Router

A simple grid routing tool based on the classic **Lee-Moore Algorithm** (also known as Lee's maze algorithm). It finds the shortest path between pins on a grid while avoiding obstacles.

## How the Algorithm Works

1. **Wave Expansion (Spread)**: Starts from the source pin and spreads outwards to adjacent cells step-by-step (like ripples in water) until it reaches the destination pin.
2. **Backtracing (Retrace)**: Once the destination is found, the algorithm traces back through the recorded steps to draw the shortest path.
3. **Re-routing (Retry)**: When routing multiple wires, the order in which they are routed matters. The program automatically tries different routing sequences to successfully route as many wires as possible.

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
