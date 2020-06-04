#ifndef VIRTUAL_NODE_MINER_HPP
#define VIRTUAL_NODE_MINER_HPP

#include <cstdint>
#include <vector>
#include <cstdio>
#include <iostream>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <map>

#define MAX_HASH 1610612741
#define K 10
#define CLUSTER_THRESHOLD 30
#define VIRTUAL_THRESHOLD 10

using adjlist = std::vector<int>;
using cluster = std::pair<int, int>;

struct hash_row {
    int u;
    std::vector<int> hashes;
};

class hash_f {
    int a;
    int b;
    int c;

public:
    hash_f() {
        a = rand();
        b = rand();
        c = rand();
    };

    int hash(int x) {
        return unsigned(a * (x + c) + b) % MAX_HASH;
    }
};

class prefix_tree {
    struct pt_node {
        int v;
        int depth;
        pt_node *parent;
        std::vector<int> u_list;
        std::map<int, pt_node> children;

        int save() const {
            return (u_list.size() - 1) * (depth - 1) - 1;
        }

        void generate_pattern(std::unordered_set<int> &pattern) {
            pt_node *cur = this;
            while (cur->depth > 0) {
                pattern.insert(cur->v);
                cur = cur->parent;
            }
        }
    };

    std::unordered_map<int, int> _v_cnt;
    pt_node _root;

public:

    std::vector<pt_node *> potential_nodes;

    prefix_tree() {
        _root.v = -1;
        _root.depth = 0;
        _root.u_list.clear();
        _root.children.clear();
    }

    void count_v(int v) {
        if (_v_cnt.find(v) == _v_cnt.end()) {
            _v_cnt[v] = 1;
        } else {
            _v_cnt[v]++;
        }
    }

    void update_tree(pt_node &node, int u, adjlist &outlinks) {
        if (node.depth > 0) {
            node.u_list.emplace_back(u);
        }

        if (node.depth < outlinks.size()) {
            int child = outlinks[node.depth];
            if (node.children.find(child) == node.children.end()) {
                pt_node new_node;
                new_node.depth = node.depth + 1;
                new_node.v = child;
                new_node.parent = &node;
                node.children[child] = new_node;
            }
            update_tree(node.children[child], u, outlinks);
        }
    }

    void add_node(int u, adjlist &adj) {
        adjlist outlinks;
        for (auto v : adj) {
            if (_v_cnt[v] > 1) outlinks.emplace_back(v);
        }
        std::sort(outlinks.begin(), outlinks.end(),
                  [this](const int &a, const int &b) -> bool {
                      return _v_cnt[a] > _v_cnt[b];
                  });

        update_tree(_root, u, outlinks);
    }

    void find_potential_nodes(pt_node &node) {
        if (node.depth > 1 && node.children.size() != 1 && node.u_list.size() > 1) {
            potential_nodes.emplace_back(&node);
        }

        for (auto &child : node.children) {
            find_potential_nodes(child.second);
        }
    }

    void generate_vn() {
        find_potential_nodes(_root);
        std::sort(potential_nodes.begin(), potential_nodes.end(),
                  [](const pt_node *a, const pt_node *b) -> bool {
                      return a->save() > b->save();
                  });
    }
};

class virtual_node_miner {

    struct virtual_node {
        int u_id;
        std::vector<int> v_list;
    };

    std::vector<adjlist> _adjlists;
    size_t _num_node;
    size_t _raw_num_node;
    size_t _num_edge;
    size_t _raw_num_edge;

    std::vector<hash_f> _hashes;
    std::vector<hash_row> _hash_mat;
    std::vector<cluster> _clusters;
    std::vector<std::vector<virtual_node>> _cluster_virtual_nodes;

public:

    void reset_hash_function() {
        _hashes.clear();
        for (int i = 0; i < K; i++) {
            _hashes.emplace_back(hash_f());
        }
    }

    bool write_graph(const std::string &file_path) {
        FILE *fp = fopen(file_path.c_str(), "w");

        if (fp == nullptr) {
            std::cout << "graph file cannot create!" << std::endl;
            return false;
        }

        for (int i = 0; i < _num_node; i++) {
            for (int j = 0; j < _adjlists[i].size(); j++) {
                fprintf(fp, "%d\t%d\n", i, _adjlists[i][j]);
            }
        }
        fclose(fp);

        return true;
    }

    bool load_graph(const std::string &file_path) {
        auto load_start = time(nullptr);

        FILE *f = fopen(file_path.c_str(), "r");

        if (f == 0) {
            std::cout << "file cannot open!" << std::endl;
            return false;
        }

        std::vector<std::pair<int, int>> edges;

        int u = 0;
        int v = 0;
        _num_node = 0;
        while (fscanf(f, "%d %d", &u, &v) > 0) {
            assert(u >= 0);
            assert(v >= 0);
            _num_node = std::max(_num_node, size_t(u + 1));
            _num_node = std::max(_num_node, size_t(v + 1));
            edges.emplace_back(std::pair<int, int>(u, v));
        }
        fclose(f);

        std::sort(edges.begin(), edges.end());

        _raw_num_node = _num_node;

        _num_edge = edges.size();
        _raw_num_edge = _num_edge;

        _adjlists.clear();
        _adjlists.resize(_num_node);
        for (auto edge : edges) {
            _adjlists[edge.first].emplace_back(edge.second);
        }

        auto load_end = time(nullptr);

        return true;
    }

    void calc_min_hash(int u) {
        _hash_mat[u].u = u;
        _hash_mat[u].hashes.clear();

        for (int i = 0; i < K; i++) {
            int min_hash = MAX_HASH;
            for (auto v : _adjlists[u]) {
                min_hash = std::min(min_hash, _hashes[i].hash(v));
            }
            _hash_mat[u].hashes.emplace_back(min_hash);
        }
    }

    void generate_clusters(int col, int start, int end) {
        int idx = start;

        while (idx < end) {
            int idx2 = idx;
            while (idx2 < end && _hash_mat[idx].hashes[col] == _hash_mat[idx2].hashes[col]) {
                idx2++;
            }

            if (col == K - 1 || idx2 - idx <= CLUSTER_THRESHOLD) {

                int idx3 = idx2;
                while (idx2 < end) {
                    while (idx3 < end && _hash_mat[idx2].hashes[col] == _hash_mat[idx3].hashes[col]) {
                        idx3++;
                    }
                    if (idx3 - idx <= CLUSTER_THRESHOLD) {
                        idx2 = idx3;
                    } else {
                        break;
                    }
                }

                _clusters.emplace_back(cluster(idx, idx2));
            } else {
                generate_clusters(col + 1, idx, idx2);
            }
            idx = idx2;
        }
    }

    void modify_adjlist(int u, int virual_node_id, std::unordered_set<int> &pattern) {
        std::vector<int> new_adjlist;
        for (auto v : _adjlists[u]) {
            if (pattern.count(v) == 0) new_adjlist.emplace_back(v);
        }
        new_adjlist.emplace_back(virual_node_id);
        _adjlists[u] = new_adjlist;
    }

    void handle_cluster(int cluster_id, int start, int end) {
        prefix_tree pt;

        for (int i = start; i < end; i++) {
            for (auto v : _adjlists[_hash_mat[i].u]) pt.count_v(v);
        }

        for (int i = start; i < end; i++) {
            pt.add_node(_hash_mat[i].u, _adjlists[_hash_mat[i].u]);
        }

        pt.generate_vn();

        std::unordered_set<int> visited_nodes;
        for (auto potential_node : pt.potential_nodes) {
            int unvisited_node_cnt = 0;
            for (auto u : potential_node->u_list) {
                if (visited_nodes.count(u) == 0) unvisited_node_cnt++;
            }

            if ((unvisited_node_cnt - 1) * (potential_node->depth - 1) < VIRTUAL_THRESHOLD) continue;

            std::unordered_set<int> pattern;
            potential_node->generate_pattern(pattern);

            // create new virtual node
            virtual_node vn;
#pragma omp critical
            {
                vn.u_id = _num_node++;
            };

            for (auto v : pattern) vn.v_list.emplace_back(v);
            std::sort(vn.v_list.begin(), vn.v_list.end());

            for (auto u : potential_node->u_list) {
                if (visited_nodes.count(u) == 0) {
                    visited_nodes.insert(u);
                    modify_adjlist(u, vn.u_id, pattern);
                }
            }

            _cluster_virtual_nodes[cluster_id].emplace_back(vn);
        }
    }

    void one_pass() {
        reset_hash_function();

        // generate min-hash matrix
        _hash_mat.clear();
        _hash_mat.resize(_num_node);

#pragma omp parallel for
        for (int i = 0; i < _num_node; i++) {
            calc_min_hash(i);
        }
#pragma omp barrier

        std::sort(_hash_mat.begin(), _hash_mat.end(),
                  [](const hash_row &a, const hash_row &b) -> bool {
                      for (int i = 0; i < a.hashes.size(); i++) {
                          if (a.hashes[i] != b.hashes[i]) return a.hashes[i] < b.hashes[i];
                      }
                      return false;
                  });

        _clusters.clear();
        int end = 0;
        while (end < _num_node && _hash_mat[end].hashes[0] < MAX_HASH) end++;
        generate_clusters(0, 0, end);

        _cluster_virtual_nodes.clear();
        _cluster_virtual_nodes.resize(_clusters.size());

        // generate potential virtual node list
#pragma omp parallel for
        for (int i = 0; i < _clusters.size(); i++) {
            handle_cluster(i, _clusters[i].first, _clusters[i].second);
        }
#pragma omp barrier

        for (auto &vns : _cluster_virtual_nodes) {
            for (auto &vn : vns) {
                if (vn.u_id >= _adjlists.size()) _adjlists.resize(vn.u_id + 1);
                _adjlists[vn.u_id] = vn.v_list;
            }
        }
    }

    void refresh_num_edge() {
        size_t new_num_edge = 0;
        for (auto &adjlist : _adjlists) new_num_edge += adjlist.size();
        _num_edge = new_num_edge;
    }

    void compress(int pass_num = 3) {
        for (int pass = 0; pass < pass_num; pass++) {
            one_pass();

            refresh_num_edge();

            printf("pass %d: node: %lu/%lu (%.4lf)\tedge: %lu/%lu (%.4lf)\n", pass,
                   _num_node, _raw_num_node, float(_num_node) / _raw_num_node,
                   _num_edge, _raw_num_edge, float(_num_edge) / _raw_num_edge
            );
        }
    }
};


#endif /* VIRTUAL_NODE_MINER_HPP */