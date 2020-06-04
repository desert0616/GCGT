## Build
```make```

## Execution
```./cgr_compressor <input_graph> <output_graph>```

The input graph should be in the traditional edgelist format, i.e.,

```
V (node_size) E (edge_size)
<u1> <v1>
<u2> <v2>
...
<uE> <vE>
```

Please note that the edges are assumed to be **sorted** in advance.