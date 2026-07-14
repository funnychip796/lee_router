# Lee-Moore Grid Router

A dependency-minimized, C++98-compliant implementation of the classic Lee-Moore grid routing algorithm (BFS maze router).

## Features
- **Strict C++98 Compatibility**: Written in a "C with Classes" style, fully compatible with legacy environments.
- **Zero STL Algorithm/Container Dependencies (Except `std::vector` and basic I/O)**:
  - Custom stable `merge_sort` (replaces `std::stable_sort`).
  - Custom `next_permutation` (replaces `std::next_permutation`).
  - Custom `reverse` (replaces `std::reverse`).
  - Custom 2D boolean grid lookup (`PointSet`, replaces `std::set<Point>` providing $O(1)$ search complexity).
- **Robust Error Handling**: Exception-free code, using boolean returns and standard stream outputs.
- **Direct Redirection**: Outputs routing details (`grid`, `obstruction`, `order`, `wave`, `path`, `summary`) to `stdout`, and logs execution diagnostics to `stderr`.

## Compilation
You can compile the router with any C++ compiler. C++98 standard is strictly followed:
```bash
g++ -std=c++98 -Wall -Wextra -O3 main.cpp -o lee_router
```

## Usage
Run the binary by passing the netlist path as the single argument:
```bash
./lee_router <netlist_file>
```

To save the routing layout results to a text file while viewing runtime statistics in your terminal, use redirection:
```bash
./lee_router grid.net > routing_result.txt
```

## Netlist Format
The input file should follow this structure (lines starting with `//` are ignored as comments):
- **Grid size**: First non-comment line specifying grid size $N$ (grid is $0 \le x, y < N$).
- **Obstructions**: Formatted as `obstruction <x> <y>`.
- **Nets**: Formatted as `net <source_x> <source_y> <sink_x> <sink_y>`.

### Example
```text
15 // Grid size 15x15

// Obstruction nodes
obstruction 3 12
obstruction 3 11

// Nets to route
net 3 12 12 5
net 8 5 8 2
```
