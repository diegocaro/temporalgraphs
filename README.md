# Compact data structures for Temporal Graphs

This repository contains the code for evaluating a sequence-based and 
a multidimensional approach for representing temporal graphs.
More info at https://diegocaro.cl/thesis/

## Definition of Temporal Graphs 

Temporal graphs (also known as time-varying graphs) are graphs whose
vertex connectivity changes along time.


## Dependencies

- A compiler that supports C++11
- [Compact Data Structures Library][libcds] - low-level succinct data structures
- [Succinct Data Structure Library][sdsl-lite] - low-level succinct data structures 

## How to compile

First step: cloning and updating submodules
```
$ git clone https://github.com/diegocaro/temporalgraphs
$ cd temporalgraphs
$ git submodule update --init --recursive
```


Then compile each module using cmake:
```
$ cd edgelogbase/build
$ cmake ..
```

And do the same with cas, cet, adjlogbase, and so on... Thanks to Luiz Fernando Afra Brito for providing these instructions.

## Author
Original concept and prototype by [Diego Caro][diegocaro].

## Acknowledgements

This work was supported in part by a CONICYT doctoral fellowship, Fondecyt 1140428, and by Fondef D09I1185.

[libcds]: https://github.com/fclaude/libcds
[sdsl-lite]: https://github.com/simongog/sdsl-lite
[diegocaro]: https://github.com/diegocaro