#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <numeric>
#include <set>
#include <stack>
#include <utility>
#include <vector>

typedef std::pair<int, int> grid_point;
typedef std::pair<grid_point, grid_point> edge;

using namespace std::chrono;

struct comp_t {
    bool operator()(const grid_point &lhs, const grid_point &rhs) const {
        if (lhs.second != rhs.second)
            return lhs.second < rhs.second;
        return lhs.first < rhs.first;
    }
};

struct Graph {
    std::set<grid_point> P_x_sorted;
    std::set<grid_point, comp_t> P_y_sorted;
    std::list<edge> E;
    bool grown = false;
    int L = 0;
};

std::vector<int> TerMapPermut(std::vector<grid_point> &terminals) {
    sort(terminals.begin(), terminals.end());

    std::map<grid_point, int> order;
    for (int i = 0; i < terminals.size(); ++i)
        order[terminals[i]] = i + 1;

    sort(terminals.begin(), terminals.end(),
        [](const auto &lhs, const auto &rhs) {
            if (lhs.second != rhs.second)
                return lhs.second < rhs.second;
            return lhs.first < rhs.first;
        }
    );

    std::vector<int> C(terminals.size());
    for (int i = 0; i < C.size(); ++i)
        C[i] = order[terminals[i]];

    return C;
}

Graph Permut(const std::vector<int> &C) {
    Graph P;
    for (int i = 0; i < C.size(); ++i) {
        P.P_x_sorted.insert(std::make_pair(C[i], i + 1));
        P.P_y_sorted.insert(std::make_pair(C[i], i + 1));
    }
    return P;
}

class Axes {
    private:
        std::vector<int> x_axis;
        std::vector<int> y_axis;

    public:
        template <class Iterable>
        Axes(const Iterable &itl, const Iterable &itr) {
            int size = std::distance(itl, itr);
            x_axis.resize(size);
            y_axis.resize(size);

            std::transform(itl, itr, x_axis.begin(),
                [](const auto &p) {
                    return p.first;
                }
            );
            std::transform(itl, itr, y_axis.begin(),
                [](const auto &p) {
                    return p.second;
                }
            );

            std::sort(x_axis.begin(), x_axis.end());
            std::sort(y_axis.begin(), y_axis.end());
        }

    grid_point converted(grid_point p) {
        p.first = x_axis[p.first - 1];
        p.second = y_axis[p.second - 1];
        return p;
    }
};

void apply_conversions(grid_point &p, std::stack<Axes> axes) {
    while (!axes.empty()) {
        auto ax = axes.top();
        p = ax.converted(p);
        axes.pop();
    }
}

int delta(grid_point p1, grid_point p2, const std::stack<Axes> &axes) {
    apply_conversions(p1, axes); apply_conversions(p2, axes);
    return abs(p1.first - p2.first) + abs(p1.second - p2.second);
}

bool extreme(Graph &G, const std::stack<Axes> &axes) {
    if (G.P_x_sorted.size() == 2) {
        auto p1 = *G.P_x_sorted.begin(), p2 = *G.P_x_sorted.rbegin();
        G.E.push_back({p1, p2});
        G.P_x_sorted.erase(G.P_x_sorted.begin());
        G.P_y_sorted.erase(p1);
        G.L += delta(p1, p2, axes);
        return false;
    }

    auto x_it1  = G.P_x_sorted.begin(),  x_it2  = G.P_x_sorted.begin();  ++x_it2;
    auto x_rit1 = G.P_x_sorted.rbegin(), x_rit2 = G.P_x_sorted.rbegin(); ++x_rit2;
    auto y_it1  = G.P_y_sorted.begin(),  y_it2  = G.P_y_sorted.begin();  ++y_it2;
    auto y_rit1 = G.P_y_sorted.rbegin(), y_rit2 = G.P_y_sorted.rbegin(); ++y_rit2;

    grid_point p, adj_p;
    if (y_it1->second != y_it2->second) {
        p = *y_it1;
        G.P_x_sorted.erase(p);
        G.P_y_sorted.erase(y_it1);
        adj_p = {p.first, p.second + 1};
    }
    else if (y_rit1->second != y_rit2->second) {
        p = *y_rit1;
        G.P_x_sorted.erase(p);
        G.P_y_sorted.erase(y_rit2.base());
        adj_p = {p.first, p.second - 1};
    }
    else if (x_rit1->first != x_rit2->first) {
        p = *x_rit1;
        G.P_x_sorted.erase(x_rit2.base());
        G.P_y_sorted.erase(p);
        adj_p = {p.first - 1, p.second};
    }
    else if (x_it1->first != x_it2->first) {
        p = *x_it1;
        G.P_x_sorted.erase(x_it1);
        G.P_y_sorted.erase(p);
        adj_p = {p.first + 1, p.second};
    }
    else return true;

    G.P_x_sorted.insert(adj_p);
    G.P_y_sorted.insert(adj_p);
    G.E.push_back({p, adj_p});
    G.L += delta(p, adj_p, axes);

    return extreme(G, axes);
}

bool equal_edge_lists(const std::list<edge> &lhs, const std::list<edge> &rhs) {
    if (lhs.size() != rhs.size())
        return false;

    for (auto itl = lhs.begin(), itr = rhs.begin(); itl != lhs.end(); ++itl, ++itr)
        if (*itl != *itr)
            return false;

    return true;
}

void fork(const Graph &G, std::list<Graph> &TreeList, const std::stack<Axes> &axes) {
    Graph G1, G2, G3;
    G1 = G2 = G3 = G;

    auto x_it1  = G.P_x_sorted.begin(),  x_it2  = G.P_x_sorted.begin();  ++x_it2;
    auto x_rit1 = G.P_x_sorted.rbegin(), x_rit2 = G.P_x_sorted.rbegin(); ++x_rit2;
    auto y_it1  = G.P_y_sorted.begin(),  y_it2  = G.P_y_sorted.begin();  ++y_it2;
    auto y_rit1 = G.P_y_sorted.rbegin(), y_rit2 = G.P_y_sorted.rbegin(); ++y_rit2;

    if (x_it1->first == x_it2->first) {
        auto eps_i_j = *x_it1, eps_i_k = *x_it2;
        auto eps_i_plus1_j = std::make_pair(x_it1->first + 1, x_it1->second);
        auto eps_i_plus1_k = std::make_pair(x_it2->first + 1, x_it2->second);

        G1.P_x_sorted.erase(eps_i_j); G1.P_x_sorted.erase(eps_i_k);
        G1.P_y_sorted.erase(eps_i_j); G1.P_y_sorted.erase(eps_i_k);
        G1.P_x_sorted.insert(eps_i_plus1_j); G1.P_x_sorted.insert(eps_i_plus1_k);
        G1.P_y_sorted.insert(eps_i_plus1_j); G1.P_y_sorted.insert(eps_i_plus1_k);
        G1.E.push_back({eps_i_j, eps_i_plus1_j});
        G1.E.push_back({eps_i_k, eps_i_plus1_k});
        G1.L += delta(eps_i_j, eps_i_plus1_j, axes)
              + delta(eps_i_k, eps_i_plus1_k, axes);
        TreeList.push_back(G1);

        G2.P_x_sorted.erase(eps_i_j); G2.P_x_sorted.erase(eps_i_k);
        G2.P_y_sorted.erase(eps_i_j); G2.P_y_sorted.erase(eps_i_k);
        G2.P_x_sorted.insert(eps_i_plus1_j);
        G2.P_y_sorted.insert(eps_i_plus1_j);
        G2.E.push_back(std::make_pair(eps_i_j, eps_i_plus1_j));
        G2.E.push_back(std::make_pair(eps_i_j, eps_i_k));
        G2.L += delta(eps_i_j, eps_i_plus1_j, axes)
              + delta(eps_i_j, eps_i_k, axes);
        TreeList.push_back(G2);

        G3.P_x_sorted.erase(eps_i_j); G3.P_x_sorted.erase(eps_i_k);
        G3.P_y_sorted.erase(eps_i_j); G3.P_y_sorted.erase(eps_i_k);
        G3.P_x_sorted.insert(eps_i_plus1_k);
        G3.P_y_sorted.insert(eps_i_plus1_k);
        G3.E.push_back(std::make_pair(eps_i_k, eps_i_plus1_k));
        G3.E.push_back(std::make_pair(eps_i_j, eps_i_k));
        G3.L += delta(eps_i_k, eps_i_plus1_k, axes)
              + delta(eps_i_j, eps_i_k, axes);
        TreeList.push_back(G3);

        for (auto it = TreeList.begin(); it != TreeList.end(); ++it)
            if (it->P_x_sorted == G.P_x_sorted && equal_edge_lists(it->E, G.E)) {
                TreeList.erase(it);
                break;
            }

        return;
    }

    if (x_rit1->first == x_it2->first) {
        auto eps_i_j = *x_rit2, eps_i_k = *x_it1;
        auto eps_i_minus1_j = std::make_pair(x_rit2->first - 1, x_it2->second);
        auto eps_i_minus1_k = std::make_pair(x_rit1->first - 1, x_it1->second);

        G1.P_x_sorted.erase(eps_i_j); G1.P_x_sorted.erase(eps_i_k);
        G1.P_y_sorted.erase(eps_i_j); G1.P_y_sorted.erase(eps_i_k);
        G1.P_x_sorted.insert(eps_i_minus1_j); G1.P_x_sorted.insert(eps_i_minus1_k);
        G1.P_y_sorted.insert(eps_i_minus1_j); G1.P_y_sorted.insert(eps_i_minus1_k);
        G1.E.push_back({eps_i_j, eps_i_minus1_j});
        G1.E.push_back({eps_i_k, eps_i_minus1_k});
        G1.L += delta(eps_i_j, eps_i_minus1_j, axes)
              + delta(eps_i_k, eps_i_minus1_k, axes);
        TreeList.push_back(G1);

        G2.P_x_sorted.erase(eps_i_j); G2.P_x_sorted.erase(eps_i_k);
        G2.P_y_sorted.erase(eps_i_j); G2.P_y_sorted.erase(eps_i_k);
        G2.P_x_sorted.insert(eps_i_minus1_j);
        G2.P_y_sorted.insert(eps_i_minus1_j);
        G2.E.push_back(std::make_pair(eps_i_j, eps_i_minus1_j));
        G2.E.push_back(std::make_pair(eps_i_j, eps_i_k));
        G2.L += delta(eps_i_j, eps_i_minus1_j, axes)
              + delta(eps_i_j, eps_i_k, axes);
        TreeList.push_back(G2);

        G3.P_x_sorted.erase(eps_i_j); G3.P_x_sorted.erase(eps_i_k);
        G3.P_y_sorted.erase(eps_i_j); G3.P_y_sorted.erase(eps_i_k);
        G3.P_x_sorted.insert(eps_i_minus1_k);
        G3.P_y_sorted.insert(eps_i_minus1_k);
        G3.E.push_back(std::make_pair(eps_i_k, eps_i_minus1_k));
        G3.E.push_back(std::make_pair(eps_i_j, eps_i_k));
        G3.L += delta(eps_i_k, eps_i_minus1_k, axes)
              + delta(eps_i_j, eps_i_k, axes);
        TreeList.push_back(G3);

        for (auto it = TreeList.begin(); it != TreeList.end(); ++it)
            if (it->P_x_sorted == G.P_x_sorted && equal_edge_lists(it->E, G.E)) {
                TreeList.erase(it);
                break;
            }

        return;
    }

    if (y_it1->second == y_it2->second) {
        auto eps_i_k = *y_it1, eps_j_k = *y_it2;
        auto eps_i_k_plus1 = std::make_pair(y_it1->first, y_it1->second + 1);
        auto eps_j_k_plus1 = std::make_pair(y_it2->first, y_it2->second + 1);

        G1.P_x_sorted.erase(eps_i_k); G1.P_x_sorted.erase(eps_j_k);
        G1.P_y_sorted.erase(eps_i_k); G1.P_y_sorted.erase(eps_j_k);
        G1.P_x_sorted.insert(eps_i_k_plus1); G1.P_x_sorted.insert(eps_j_k_plus1);
        G1.P_y_sorted.insert(eps_i_k_plus1); G1.P_y_sorted.insert(eps_j_k_plus1);
        G1.E.push_back({eps_i_k, eps_i_k_plus1});
        G1.E.push_back({eps_j_k, eps_j_k_plus1});
        G1.L += delta(eps_i_k, eps_i_k_plus1, axes)
              + delta(eps_j_k, eps_j_k_plus1, axes);
        TreeList.push_back(G1);

        G2.P_x_sorted.erase(eps_i_k); G2.P_x_sorted.erase(eps_j_k);
        G2.P_y_sorted.erase(eps_i_k); G2.P_y_sorted.erase(eps_j_k);
        G2.P_x_sorted.insert(eps_i_k_plus1);
        G2.P_y_sorted.insert(eps_i_k_plus1);
        G2.E.push_back(std::make_pair(eps_i_k, eps_i_k_plus1));
        G2.E.push_back(std::make_pair(eps_i_k, eps_j_k));
        G2.L += delta(eps_i_k, eps_i_k_plus1, axes)
              + delta(eps_i_k, eps_j_k, axes);
        TreeList.push_back(G2);

        G3.P_x_sorted.erase(eps_i_k); G3.P_x_sorted.erase(eps_j_k);
        G3.P_y_sorted.erase(eps_i_k); G3.P_y_sorted.erase(eps_j_k);
        G3.P_x_sorted.insert(eps_j_k_plus1);
        G3.P_y_sorted.insert(eps_j_k_plus1);
        G3.E.push_back(std::make_pair(eps_j_k, eps_j_k_plus1));
        G3.E.push_back(std::make_pair(eps_i_k, eps_j_k));
        G3.L += delta(eps_j_k, eps_j_k_plus1, axes)
              + delta(eps_i_k, eps_j_k, axes);
        TreeList.push_back(G3);

        for (auto it = TreeList.begin(); it != TreeList.end(); ++it)
            if (it->P_x_sorted == G.P_x_sorted && equal_edge_lists(it->E, G.E)) {
                TreeList.erase(it);
                break;
            }

        return;
    }

    if (y_rit1->second == y_rit2->second) {
        auto eps_i_k = *y_rit2, eps_j_k = *y_rit1;
        auto eps_i_k_minus1 = std::make_pair(y_rit2->first, y_rit2->second - 1);
        auto eps_j_k_minus1 = std::make_pair(y_rit1->first, y_rit1->second - 1);

        G1.P_x_sorted.erase(eps_i_k); G1.P_x_sorted.erase(eps_j_k);
        G1.P_y_sorted.erase(eps_i_k); G1.P_y_sorted.erase(eps_j_k);
        G1.P_x_sorted.insert(eps_i_k_minus1); G1.P_x_sorted.insert(eps_j_k_minus1);
        G1.P_y_sorted.insert(eps_i_k_minus1); G1.P_y_sorted.insert(eps_j_k_minus1);
        G1.E.push_back({eps_i_k, eps_i_k_minus1});
        G1.E.push_back({eps_j_k, eps_j_k_minus1});
        G1.L += delta(eps_i_k, eps_i_k_minus1, axes)
              + delta(eps_j_k, eps_j_k_minus1, axes);
        TreeList.push_back(G1);

        G2.P_x_sorted.erase(eps_i_k); G2.P_x_sorted.erase(eps_j_k);
        G2.P_y_sorted.erase(eps_i_k); G2.P_y_sorted.erase(eps_j_k);
        G2.P_x_sorted.insert(eps_i_k_minus1);
        G2.P_y_sorted.insert(eps_i_k_minus1);
        G2.E.push_back(std::make_pair(eps_i_k, eps_i_k_minus1));
        G2.E.push_back(std::make_pair(eps_i_k, eps_j_k));
        G2.L += delta(eps_i_k, eps_i_k_minus1, axes)
              + delta(eps_i_k, eps_j_k, axes);
        TreeList.push_back(G2);

        G3.P_x_sorted.erase(eps_i_k); G3.P_x_sorted.erase(eps_j_k);
        G3.P_y_sorted.erase(eps_i_k); G3.P_y_sorted.erase(eps_j_k);
        G3.P_x_sorted.insert(eps_j_k_minus1);
        G3.P_y_sorted.insert(eps_j_k_minus1);
        G3.E.push_back(std::make_pair(eps_j_k, eps_j_k_minus1));
        G3.E.push_back(std::make_pair(eps_i_k, eps_j_k));
        G3.L += delta(eps_j_k, eps_j_k_minus1, axes)
              + delta(eps_i_k, eps_j_k, axes);
        TreeList.push_back(G3);

        for (auto it = TreeList.begin(); it != TreeList.end(); ++it)
            if (it->P_x_sorted == G.P_x_sorted && equal_edge_lists(it->E, G.E)) {
                TreeList.erase(it);
                break;
            }

        return;
    }

    std::cout << "Impossible situation.\n";
    return;
}

Graph Const_optRST(Graph G, const std::stack<Axes> &axes) {
    std::list<Graph> TreeList;
    TreeList.emplace_back(G);

    auto it = TreeList.begin();
    while (it != TreeList.end()) {
        if (it->grown == false)
            if (extreme(*it, axes)) {
                fork(*it, TreeList, axes);
                it = TreeList.begin();
            }
            else {
                it->grown = true;
                ++it;
            }
        else
            ++it;
    }

    return *std::min_element(TreeList.begin(), TreeList.end(),
        [](const Graph &lhs, const Graph &rhs) {
            return lhs.L < rhs.L;
        }
    );
}

Graph RSTC(std::vector<grid_point> terminals, int district_size) {
    auto C = TerMapPermut(terminals);
    int max_size = std::max(std::min(district_size, 7), 2);
    std::stack<Axes> axes;
    axes.push(Axes(terminals.begin(), terminals.end()));

    Graph G_P = Permut(C);
    auto P = G_P.P_y_sorted;
    G_P.P_x_sorted.clear();
    G_P.P_y_sorted.clear();

    int n = P.size(), d_solved = 0;
    int size = std::min(n, max_size);

    auto itl = P.begin();
    auto itr = std::next(itl, size);
    auto last = std::next(P.end(), -1);

    std::vector<int> index(n);
    while (itl != last) {
        std::fill(index.begin(), index.end(), -1);
        auto it = itl;
        for (int i = 0; i < size; ++i, ++it)
            index[it->first - 1] = i;

        std::vector<int> district(size);
        for (int i = 0, count = 1; i < n; ++i)
            if (index[i] >= 0)
                district[index[i]] = count++;

        axes.push(Axes(itl, itr));
        auto G_d = Const_optRST(Permut(district), axes);

        for (auto p : G_d.P_x_sorted) {
            auto p_copy = p;
            apply_conversions(p_copy, axes);
            G_P.P_x_sorted.insert(p_copy);
        }
        for (auto p : G_d.P_y_sorted) {
            auto p_copy = p;
            apply_conversions(p_copy, axes);
            G_P.P_y_sorted.insert(p_copy);
        }

        auto E_d = G_d.E;
        for (auto &e : E_d) {
            apply_conversions(e.first, axes);
            apply_conversions(e.second, axes);
        }
        std::list<edge> E_filtered;
        std::copy_if(E_d.begin(), E_d.end(), std::back_inserter(E_filtered),
            [](const edge &e) {
                return e.first != e.second;
            }
        );
        G_P.E.merge(E_filtered,
            [](const auto &lhs, const auto &rhs) {
                return lhs.first == rhs.first  && lhs.second == rhs.second
                    || lhs.first == rhs.second && lhs.second == rhs.first;
            }
        );

        G_P.L += G_d.L;

        ++d_solved;
        axes.pop();
        size = std::min(n - (d_solved * (max_size - 1)), max_size);
        itl = std::next(itr, -1);
        itr = std::next(itl, size);
    }
    G_P.grown = true;

    return G_P;
}

/*
int main() {
    int n;
    std::ifstream f_in("sample.in");
    f_in >> n;
    std::vector<grid_point> terminals(n);
    for (int i = 0; i < n; ++i) {
        f_in >> terminals[i].first >> terminals[i].second;
    }
    int district_size = 7;

    Graph G = RSTC(terminals, district_size);

    for (auto it = G.P_x_sorted.begin(); it != G.P_x_sorted.end(); ++it)
        std::cout << "(" << it->first << ", " << it->second << ") ; ";
    std::cout << "\b\b \n";

    for (auto it = G.E.begin(); it != G.E.end(); ++it)
        std::cout << "(" << it->first.first  << ", " << it->first.second  << ") -> "
                  << "(" << it->second.first << ", " << it->second.second << ")\n";

    std::cout << G.L << std::endl;

    return 0;
}
*/

int main() {
    int n;
    std::cin >> n;
    std::vector<grid_point> terminals(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> terminals[i].first >> terminals[i].second;
    }
    int district_size = 7;

    auto start = high_resolution_clock::now();
    Graph G = RSTC(terminals, district_size);
    auto stop  = high_resolution_clock::now();

    auto duration_sec  = duration_cast<seconds>(stop - start);
    auto duration_msec = duration_cast<milliseconds>(stop - start);

    std::ofstream out1("Time");
    out1 << std::to_string(duration_sec.count()*1000 + duration_msec.count());
    out1.close();

    std::ofstream out2("Len");
    out2 << std::to_string(G.L);
    out2.close();

    FILE *fp = fopen("Tree.csv", "w+");
    fprintf(fp, "x1, y1, x2, y2\n");
    const char *x1, *x2, *y1, *y2;
    for (auto it = G.E.begin(); it != G.E.end(); ++it) {
        x1 = std::to_string(it->first.first).c_str();
        x2 = std::to_string(it->second.first).c_str();
        y1 = std::to_string(it->first.second).c_str();
        y2 = std::to_string(it->second.second).c_str();
        fprintf(fp, "%s, %s, %s, %s\n", x1, x2, y1, y2);
    }

    return 0;
}
