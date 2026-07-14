# Lee-Moore Grid Router

This repository implements the classic Lee-Moore grid routing algorithm (also known as Lee's maze routing algorithm), a foundational algorithm used in electronic design automation (EDA) to route wires on a grid.

## Algorithm Principles

The Lee-Moore algorithm is a Breadth-First Search (BFS) based pathfinding technique. It guarantees finding the shortest path between a source pin and a sink pin on a grid, provided a path exists.

The routing process operates in two main phases:

### 1. Wave Expansion (Labeling Phase)
- **Initialization**: The routing grid is initialized. Obstructions and already routed nets are marked as blocked. The source node is labeled with a distance of `0`.
- **Expansion**: A wavefront propagates outwards from the source. In each iteration $L$, all unblocked orthogonal neighbors (up, down, left, right) of cells labeled $L-1$ are labeled with $L$.
- **Termination**: The expansion continues until either:
  1. The sink node is reached (labeled with the final distance).
  2. The queue becomes empty, indicating no path exists (blocked by obstructions).

```
   [0] -> [1] -> [2] -> [3] (Sink)
    |      |      |      |
   [1] -> [2] -> [3] -> [4]
```

### 2. Backtracing (Retrace Phase)
- Once the sink is reached with a final distance $L_{sink}$, the algorithm backtracks to reconstruct the path.
- Starting from the sink, it traces backward to any adjacent neighbor labeled $L_{sink} - 1$, then $L_{sink} - 2$, repeating until it reaches the source (labeled `0`).
- *Optimization*: To make backtracing efficient and deterministic, this implementation stores parent pointers during the expansion phase, allowing $O(\text{path\_length})$ backtracing.

---

## Sequential Multi-Net Routing & Ordering

When routing multiple nets on the same grid:
1. Nets are routed one after another.
2. When a net is successfully routed, its path is marked as a block/obstruction for all subsequent nets.
3. Because earlier routes can block later ones, the **order** in which nets are routed determines the overall success rate and wirelength.

### Optimization Strategies
To maximize the number of routed nets, this router evaluates the netlist under multiple ordering heuristics and selects the best result:
- **Default Order**: The original sequence provided in the netlist.
- **Failed-First**: Dynamically prioritizes nets that failed to route in the previous attempt, moving them to the front.
- **Reversed Order**: Routes nets in the reverse of the original sequence.
- **Longest Manhattan Distance First**: Routes nets with the longest straight-line span first, as they are more likely to get blocked by other paths.
- **Exhaustive Permutations**: For small problems (9 nets or fewer), the router evaluates all $N!$ order permutations to guarantee the mathematically optimal routing solution.
