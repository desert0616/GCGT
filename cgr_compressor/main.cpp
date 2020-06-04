#include <iostream>
#include <omp.h>
#include "cgr_compressor.hpp"
#include <string>

int main(int argc,char *argv[]) {

    if (argc != 3) {
        printf("incorrect arguments.\n");
        printf("<input_path> <output_path>\n");
        abort();
    }
    std::string input_path(argv[1]);
    std::string output_path(argv[2]);

    auto compressor = cgr_compressor(3, 4, 8 * 32, 8 * 32);
    compressor.load_graph(input_path);

    printf("%s graph loaded.\n", input_path.c_str());

    compressor.compress();
    compressor.write_cgr(output_path);
    printf("CGR generation completed.\n");

    return 0;
}