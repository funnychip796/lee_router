#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/*
 * Swaps the values of two variables.
 */
template <typename T>
void my_swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

/*
 * In-place reverses the elements of a vector within the range [first, last).
 */
template <typename T>
void my_reverse(vector<T>& v, int first, int last) {
    --last;
    while (first < last) {
        my_swap(v[first], v[last]);
        ++first;
        --last;
    }
}

/*
 * In-place reverses the entire vector.
 */
template <typename T>
void my_reverse_all(vector<T>& v) {
    my_reverse(v, 0, static_cast<int>(v.size()));
}

/*
 * Merges two sorted sub-vectors while preserving the stability of the sort.
 */
template <typename T, typename Cmp>
void merge_helper(vector<T>& v, int lo, int mid, int hi, Cmp cmp) {
    vector<T> tmp;
    tmp.reserve(static_cast<size_t>(hi - lo));
    int i = lo, j = mid;
    while (i < mid && j < hi) {
        if (!cmp(v[j], v[i]))
            tmp.push_back(v[i++]);
        else
            tmp.push_back(v[j++]);
    }
    while (i < mid) tmp.push_back(v[i++]);
    while (j < hi)  tmp.push_back(v[j++]);
    for (int k = lo; k < hi; ++k) v[k] = tmp[k - lo];
}

/*
 * Recursive core logic for merge sort.
 */
template <typename T, typename Cmp>
void my_merge_sort(vector<T>& v, int lo, int hi, Cmp cmp) {
    if (hi - lo <= 1) return;
    int mid = lo + (hi - lo) / 2;
    my_merge_sort(v, lo, mid, cmp);
    my_merge_sort(v, mid, hi, cmp);
    merge_helper(v, lo, mid, hi, cmp);
}

/*
 * Provides a stable sorting interface for the entire vector.
 */
template <typename T, typename Cmp>
void my_stable_sort(vector<T>& v, Cmp cmp) {
    my_merge_sort(v, 0, static_cast<int>(v.size()), cmp);
}

/*
 * Generates the next lexicographical permutation of the sequence. If the sequence is already the largest permutation, reverses it to the smallest permutation and returns false.
 */
template <typename T>
bool my_next_permutation(vector<T>& v) {
    int n = static_cast<int>(v.size());
    if (n <= 1) return false;

    int i = n - 2;
    while (i >= 0 && !(v[i] < v[i + 1])) --i;

    if (i < 0) {
        my_reverse(v, 0, n);
        return false;
    }

    int j = n - 1;
    while (!(v[i] < v[j])) --j;

    my_swap(v[i], v[j]);
    my_reverse(v, i + 1, n);
    return true;
}

/*
 * Calculates and returns the absolute value of an integer.
 */
int my_abs(int x) { return x < 0 ? -x : x; }

/*
 * Manages a set of points based on a 2D boolean array to check grid coordinate occupancy in O(1) time.
 */
struct PointSet {
    int n;
    vector<vector<bool> > used;

    explicit PointSet(int grid_n)
        : n(grid_n), used(grid_n, vector<bool>(grid_n, false)) {}

    bool insert(int x, int y) {
        if (used[y][x]) return false;
        used[y][x] = true;
        return true;
    }

    bool count(int x, int y) const { return used[y][x]; }
};

struct Point {
    int x;
    int y;
    Point() : x(0), y(0) {}
    Point(int x_, int y_) : x(x_), y(y_) {}
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
    bool operator<(const Point& o)  const {
        return (y < o.y) || (y == o.y && x < o.x);
    }
};

struct Net {
    int   id;
    Point source;
    Point sink;
    Net() : id(0) {}
};

struct Problem {
    int                n;
    vector<Point> obstructions;
    vector<Net>   nets;
    Problem() : n(0) {}
};

struct WaveRecord {
    int                level;
    vector<Point> cells;
    WaveRecord() : level(0) {}
};

struct NetRoute {
    bool                    success;
    vector<Point>      path;
    vector<WaveRecord> waves;
    NetRoute() : success(false) {}
};

struct RouteAttempt {
    vector<NetRoute> net_routes;
    int                   routed_count;
    int                   total_wirelength;
    vector<int>      order;
    RouteAttempt() : routed_count(0), total_wirelength(0) {}
};

const int FREE        = -1;
const int OBSTRUCTION = -2;
const int TERMINAL    = -3;

/*
 * Trims leading and trailing whitespace characters (including spaces, tabs, and newlines) from a string.
 */
string trim(const string& s) {
    const size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    const size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

/*
 * Checks if the given coordinate (x, y) is within the boundaries of the n x n grid.
 */
bool in_bounds_n(int x, int y, int n) {
    return x >= 0 && y >= 0 && x < n && y < n;
}

/*
 * Parses the specified netlist file, extracts the grid size, obstruction positions, and net terminal coordinates, and performs format and boundary validations.
 */
bool read_netlist(const string& filename, Problem& prob) {
    ifstream input(filename.c_str());
    if (!input) {
        cerr << "Error: Cannot open netlist: " << filename << '\n';
        return false;
    }

    string line;
    bool size_read = false;
    int  net_id    = 0;
    int  line_no   = 0;

    while (getline(input, line)) {
        ++line_no;
        size_t comment = line.find("//");
        if (comment != string::npos) line = line.substr(0, comment);
        line = trim(line);
        if (line.empty()) continue;

        istringstream iss(line);

        if (!size_read) {
            if (!(iss >> prob.n) || prob.n <= 0) {
                cerr << "Error: Invalid grid size on line " << line_no << '\n';
                return false;
            }
            string extra;
            if (iss >> extra) {
                cerr << "Error: Unexpected token '" << extra << "' in: " << line << '\n';
                return false;
            }
            size_read = true;
            continue;
        }

        string kw;
        iss >> kw;

        if (kw == "obstruction") {
            Point q;
            if (!(iss >> q.x >> q.y)) {
                cerr << "Error: Malformed obstruction: " << line << '\n';
                return false;
            }
            string extra;
            if (iss >> extra) {
                cerr << "Error: Unexpected token '" << extra << "' in: " << line << '\n';
                return false;
            }
            prob.obstructions.push_back(q);

        } else if (kw == "net") {
            Net net;
            net.id = net_id++;
            if (!(iss >> net.source.x >> net.source.y >> net.sink.x >> net.sink.y)) {
                cerr << "Error: Malformed net: " << line << '\n';
                return false;
            }
            string extra;
            if (iss >> extra) {
                cerr << "Error: Unexpected token '" << extra << "' in: " << line << '\n';
                return false;
            }
            prob.nets.push_back(net);

        } else {
            cerr << "Error: Unknown keyword '" << kw << "' on line " << line_no << '\n';
            return false;
        }
    }

    if (!size_read) {
        cerr << "Error: Missing grid size\n";
        return false;
    }
    if (prob.nets.empty()) {
        cerr << "Error: Netlist contains no nets\n";
        return false;
    }

    PointSet obs_set(prob.n);
    for (size_t i = 0; i < prob.obstructions.size(); ++i) {
        const Point& q = prob.obstructions[i];
        if (!in_bounds_n(q.x, q.y, prob.n)) {
            cerr << "Error: Obstruction outside grid\n";
            return false;
        }
        if (!obs_set.insert(q.x, q.y)) {
            cerr << "Error: Duplicate obstruction at (" << q.x << "," << q.y << ")\n";
            return false;
        }
    }

    PointSet term_set(prob.n);
    for (size_t i = 0; i < prob.nets.size(); ++i) {
        const Net& net = prob.nets[i];
        if (!in_bounds_n(net.source.x, net.source.y, prob.n) ||
            !in_bounds_n(net.sink.x,   net.sink.y,   prob.n)) {
            cerr << "Error: Net terminal outside grid\n";
            return false;
        }
        if (net.source == net.sink) {
            cerr << "Error: Net " << net.id << " has identical source and sink\n";
            return false;
        }

        const Point terminals[2] = { net.source, net.sink };
        for (int t = 0; t < 2; ++t) {
            if (!term_set.insert(terminals[t].x, terminals[t].y)) {
                cerr << "Error: Two nets share the same terminal cell\n";
                return false;
            }
        }
    }

    return true;
}

/*
 * Uses the Lee-Moore algorithm to perform a breadth-first search for the shortest connection path from source to sink on a single net.
 */
NetRoute lee_route(const Problem& p,
                          const Net& net,
                          const vector<vector<int> >& occupied) {
    static const int dx[4] = {1, -1, 0, 0};
    static const int dy[4] = {0, 0, 1, -1};

    vector<vector<int> >   labels(p.n, vector<int>(p.n, -1));
    vector<vector<Point> > parent(p.n, vector<Point>(p.n, Point(-1, -1)));

    vector<Point> current_wave;
    current_wave.push_back(net.source);
    labels[net.source.y][net.source.x] = 0;

    NetRoute result;
    WaveRecord wr0;
    wr0.level = 0;
    wr0.cells = current_wave;
    result.waves.push_back(wr0);

    bool path_exists = false;
    int  level       = 1;

    while (!current_wave.empty() && !path_exists) {
        vector<Point> next_wave;

        for (size_t ci = 0; ci < current_wave.size() && !path_exists; ++ci) {
            const Point& cur = current_wave[ci];

            for (int d = 0; d < 4; ++d) {
                Point nb(cur.x + dx[d], cur.y + dy[d]);

                if (!in_bounds_n(nb.x, nb.y, p.n)) continue;
                if (labels[nb.y][nb.x] != -1)       continue;

                const bool own_pin = (nb == net.source || nb == net.sink);
                if (occupied[nb.y][nb.x] != FREE && !own_pin) continue;

                labels[nb.y][nb.x] = level;
                parent[nb.y][nb.x] = cur;
                next_wave.push_back(nb);

                if (nb == net.sink) {
                    path_exists = true;
                    break;
                }
            }
        }

        if (!next_wave.empty()) {
            WaveRecord wr;
            wr.level = level;
            wr.cells = next_wave;
            result.waves.push_back(wr);
        }

        current_wave = next_wave;
        ++level;
    }

    if (!path_exists) return result;

    Point cur = net.sink;
    while (!(cur == net.source)) {
        result.path.push_back(cur);
        cur = parent[cur.y][cur.x];
    }
    result.path.push_back(net.source);

    my_reverse_all(result.path);

    result.success = true;
    return result;
}

/*
 * Generates and returns the initial occupancy matrix of the grid based on the original obstructions and all net terminals.
 */
vector<vector<int> > make_initial_occupancy(const Problem& p) {
    vector<vector<int> > occ(p.n, vector<int>(p.n, FREE));
    for (size_t i = 0; i < p.obstructions.size(); ++i)
        occ[p.obstructions[i].y][p.obstructions[i].x] = OBSTRUCTION;
    for (size_t i = 0; i < p.nets.size(); ++i) {
        occ[p.nets[i].source.y][p.nets[i].source.x] = TERMINAL;
        occ[p.nets[i].sink.y  ][p.nets[i].sink.x  ] = TERMINAL;
    }
    return occ;
}

/*
 * Routes nets in the specified order, updating the occupancy matrix as paths are successfully routed, and returns the result of this routing attempt.
 */
RouteAttempt route_in_order(const Problem& p, const vector<int>& order) {
    vector<vector<int> > occ = make_initial_occupancy(p);

    RouteAttempt result;
    result.net_routes.resize(p.nets.size());
    result.order = order;

    for (size_t oi = 0; oi < order.size(); ++oi) {
        int        idx = order[oi];
        const Net& net = p.nets[static_cast<size_t>(idx)];
        NetRoute   route = lee_route(p, net, occ);

        if (route.success) {
            ++result.routed_count;
            result.total_wirelength += static_cast<int>(route.path.size()) - 1;
            for (size_t pi = 0; pi < route.path.size(); ++pi) {
                const Point& q = route.path[pi];
                if (!(q == net.source) && !(q == net.sink))
                    occ[q.y][q.x] = idx;
            }
        }
        result.net_routes[static_cast<size_t>(idx)] = route;
    }
    return result;
}

/*
 * Compares two routing attempts: prioritizes the number of successfully routed nets, and resolves ties by choosing the shorter total wirelength.
 */
bool better(const RouteAttempt& a, const RouteAttempt& b) {
    if (a.routed_count != b.routed_count) return a.routed_count > b.routed_count;
    return a.total_wirelength < b.total_wirelength;
}

/*
 * Attempts routing in the specified order and updates the best solution if the candidate is better than the current best.
 */
void consider_order(const Problem& p,
                            const vector<int>& order,
                            RouteAttempt& best) {
    RouteAttempt cand = route_in_order(p, order);
    if (better(cand, best)) best = cand;
}

/*
 * Functor: Compares the Manhattan distance of two nets, used to prioritize routing the longest nets first.
 */
struct LongestFirst {
    const vector<Net>& nets;
    explicit LongestFirst(const vector<Net>& n) : nets(n) {}
    bool operator()(int a, int b) const {
        const Net& na = nets[static_cast<size_t>(a)];
        const Net& nb = nets[static_cast<size_t>(b)];
        int da = my_abs(na.source.x - na.sink.x) + my_abs(na.source.y - na.sink.y);
        int db = my_abs(nb.source.x - nb.sink.x) + my_abs(nb.source.y - nb.sink.y);
        return da > db;
    }
};

/*
 * Attempts multiple routing retries using various heuristic orders (failed-first, reversed, longest-distance-first, and exhaustive permutations for small sizes) to find the optimal result.
 */
RouteAttempt route_with_retries(const Problem& p) {
    const int N = static_cast<int>(p.nets.size());

    vector<int> original(static_cast<size_t>(N));
    for (int i = 0; i < N; ++i)
        original[static_cast<size_t>(i)] = i;

    RouteAttempt best = route_in_order(p, original);

    vector<int> failed_first;
    for (int i = 0; i < N; ++i)
        if (!best.net_routes[static_cast<size_t>(i)].success)
            failed_first.push_back(i);
    for (int i = 0; i < N; ++i)
        if (best.net_routes[static_cast<size_t>(i)].success)
            failed_first.push_back(i);
    if (failed_first != original) consider_order(p, failed_first, best);

    vector<int> rev_order = original;
    my_reverse_all(rev_order);
    consider_order(p, rev_order, best);

    vector<int> longest_first = original;
    my_stable_sort(longest_first, LongestFirst(p.nets));
    consider_order(p, longest_first, best);

    if (N <= 9) {
        vector<int> perm = original;
        do {
            consider_order(p, perm, best);
        } while (my_next_permutation(perm));
    }

    return best;
}

/*
 * Formats and writes the detailed routing results to the standard output.
 */
void write_result(const Problem& p, const RouteAttempt& result) {
    cout << "grid " << p.n << '\n';
    for (size_t i = 0; i < p.obstructions.size(); ++i)
        cout << "obstruction " << p.obstructions[i].x << ' ' << p.obstructions[i].y << '\n';

    cout << "order";
    for (size_t i = 0; i < result.order.size(); ++i) cout << ' ' << result.order[i];
    cout << '\n';

    for (size_t i = 0; i < p.nets.size(); ++i) {
        const Net&      net   = p.nets[i];
        const NetRoute& route = result.net_routes[i];

        cout << "net " << net.id
            << ' ' << net.source.x << ' ' << net.source.y
            << ' ' << net.sink.x   << ' ' << net.sink.y
            << ' ' << (route.success ? "routed" : "failed") << '\n';

        for (size_t wi = 0; wi < route.waves.size(); ++wi) {
            const WaveRecord& wave = route.waves[wi];
            cout << "wave " << net.id << ' ' << wave.level;
            for (size_t ci = 0; ci < wave.cells.size(); ++ci)
                cout << ' ' << wave.cells[ci].x << ' ' << wave.cells[ci].y;
            cout << '\n';
        }

        if (route.success) {
            cout << "path " << net.id;
            for (size_t pi = 0; pi < route.path.size(); ++pi)
                cout << ' ' << route.path[pi].x << ' ' << route.path[pi].y;
            cout << '\n';
        }
    }

    cout << "summary " << result.routed_count << ' ' << p.nets.size()
        << ' ' << result.total_wirelength << '\n';
}

/*
 * Entry point: parses command-line arguments, reads the netlist, executes routing retries, and writes results to stdout and run info to stderr.
 */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <netlist>\n";
        return 1;
    }
    const string in_file = argv[1];

    Problem prob;
    
    if (!read_netlist(in_file, prob)) {
        return 1;
    }

    const RouteAttempt result = route_with_retries(prob);

    write_result(prob, result);

    cerr << "\n--- Run Info ---\n";
    cerr << "Routed " << result.routed_count << " of " << prob.nets.size() << " nets.\n";
    cerr << "Total wirelength: " << result.total_wirelength << "\n";
    cerr << "Selected order:";
    for (size_t i = 0; i < result.order.size(); ++i)
        cerr << ' ' << result.order[i];
    cerr << "\n";

    return 0;
}
