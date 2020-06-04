## Environment and Dependency
This code is developed and tested on:
* Ubuntu 18.04
* TITAN V with Nvidia Drive 440.33.01
* CUDA 10.2
* [CUB](https://nvlabs.github.io/cub/) v1.8.0

## Build
To build this, use ```make```.
You may need to modify the ```Makefile``` with a proper setting, e.g., nvcc path, include path, GPU architecture, GCGT-related parameters.

## Execution
```./bfs_gcgt <cgr_path>```

For example, if the **sample.cgr.graph** and **sample.cgr.offset** are located in DIR_PATH, then execute

```./bfs_gcgt DIR_PATH/sample.cgr```.

It will conduct BFS 100 times from random source nodes.

