#include <iostream>
#include "virtual_node_miner.hpp"
#include <string>
#include <vector>
#include <cstdio>
#include <sys/resource.h>


int main(int argc,char *argv[]) {

    if (argc != 3) {
        printf("incorrect arguments.\n");
        printf("<input_graph> <output_graph>\n");
        abort();
    }
    std::string input_path(argv[1]);
    std::string output_path(argv[2]);

    const rlim_t kStackSize = 1 * 1024 * 1024 * 1024;
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < kStackSize)
        {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }

    virtual_node_miner vnminer;
    vnminer.load_graph(input_path);
    vnminer.compress(5);
    vnminer.write_graph(output_path);

    return 0;
}