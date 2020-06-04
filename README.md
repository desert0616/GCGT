# GPU-based Compressed Graph Traversal (GCGT)

Source code for the paper:


<a href="https://dl.acm.org/doi/10.1145/3299869.3319871">
<img src="http://db-reproducibility.seas.harvard.edu/images/ACM_results_replicated.png" width="50">
GPU-based Graph Traversal on Compressed Graphs
</a>

## Datasets
Due to the capacity limitation, please obtain the datasets used in the paper from their original sources:
* ljournal-2008: http://law.di.unimi.it/webdata/ljournal-2008/
* uk-2002: http://law.di.unimi.it/webdata/uk-2002/
* brain: http://networkrepository.com/bn-human-Jung2015-M87113878.php
* twitter: http://an.kaist.ac.kr/traces/WWW2010.html
* uk-2007: http://law.di.unimi.it/webdata/uk-2007-05/

## Baselines
* Ligra and Ligra+: https://github.com/jshun/ligra
* Gunrock: https://github.com/gunrock/gunrock

## Reordering
* BFSOrder and LLP: http://law.di.unimi.it/software.php
* Gorder: https://github.com/datourat/Gorder

## Virtual Node Miner
Our third-party implementation of the **Virtual Node Miner** in [A Scalable Pattern Mining Approach to Web Graph Compression with Communities](https://dl.acm.org/doi/10.1145/1341531.1341547).
Please refer to [virtual_node_miner](https://github.com/desert0616/GCGT/tree/master/virtual_node_miner).

## Compressed Graph Representation (CGR) Compressor
The Compressed Graph Representation (CGR) Compressor.
Please refer to [cgr_compressor](https://github.com/desert0616/GCGT/tree/master/cgr_compressor).

## BFS-GCGT
The CUDA implementation of BFS based on the proposed GCGT.
Please refer to [bfs_gcgt](https://github.com/desert0616/GCGT/tree/master/bfs_gcgt).

## Reference
```
@inproceedings{DBLP:conf/sigmod/Sha19GCGT,
  author    = {Mo Sha and
               Yuchen Li and
               Kian{-}Lee Tan},
  title     = {GPU-based Graph Traversal on Compressed Graphs},
  booktitle = {{SIGMOD} Conference},
  pages     = {775--792},
  publisher = {{ACM}},
  year      = {2019}
}
```

## License
MIT