# Compact Data Structures and Query Processing for Temporal Graphs
PhD Thesis 
by Diego Caro A. \<diegocaro at udec cl\>

Advisor: [Dr. M. Andrea Rodríguez](http://www.inf.udec.cl/~andrea/)
Co-Advisor: [Dr. Nieves R. Brisaboa](http://lbd.udc.es/ShowResearcherInformation.do?lang=en_US&id=12)
[Department of Computer Science](http://www.inf.udec.cl)
University of Concepción, Chile

The [thesis](thesis.pdf) + [slides](slides.pdf) are freely available.

## Abstract

Temporal graphs represent vertices and binary relations that change along time. A temporal graph could be represented as several static graphs (or snapshots), one per each time point in the lifetime of the graph. The main problem of this representation is the space usage when edges remain unchanged for long periods of time, as consecutive snapshots tend to be very similar between them. Consequently, most of the space correspond to a replication of previous snapshots.

In this thesis, we explore alternative representations of temporal graphs that are efficient in space and in time to answer adjacency operations. On one hand, a representation based on a *log of changes* stores the time instant when edges appear or disappear. A *multidimensional binary matrix*, on the other hand, stores data of edges and their valid time intervals as cells in a 4D matrix, which reduces space while maintaining a good retrieval time.

<!-- 
As a baseline of comparison, we present two first strategies: the Time-interval Log per Edge ($\Edglog$) and the Adjacency Log of Events ($\Adjlog$), both using compression techniques over an inverted index that represent the logs. We use a Compact Suffix Array to represent temporal graphs as a sequence of 4-tuples, where adjacency operations are answered as a pattern matching problem.

We introduce two new strategies to represent temporal graphs using compact data structures. Compact Adjacency Sequence ($\CAS$) represents changes on adjacent vertices as a sequence stored in a $\WT$, and the Compact Events ordered by Time ($\CET$) represents the edges that change in each time instant using an $\IWT$, a new compact data structure specifically designed in this work that is able to represent a sequence of multidimensional symbols (that is, tuples of symbols encoded together).

We finally propose to represent temporal graphs as cells in a 4D binary matrix: two dimensions to represent extreme vertices of an edge and two dimensions to represent the temporal interval when the edge exists. This strategy generalizes the idea of the adjacency matrix for storing static graphs. The proposed structure called Compressed $\Kd$ ($\cKd$) is capable to deal with unclustered data with a good use of space. The $\cKd$ uses asymptotically the same space than the information-theoretical lower bound for storing cells in a 4D binary matrix, without considering any regularity. Techniques that group leaves into buckets and compress nodes with few children show to improve the performance in time of the $\cKd$.

We experimentally evaluate all the structures and compare them with previous alternatives in the state-of-the-art based on snapshots and logs, showing that our proposals can represent large temporal graphs making efficient use of space, while keeping good time performance for a wide range of useful queries. We conclude that the use of compact data structures open the possibility for the design of interesting representations of temporal graphs that fit the needs of different application domains.
--> 
## Publications

### Journal

 - D. Caro, A. Rodríguez, N. R. Brisaboa, and A. Fariña, “Compressed $k^d$-tree for temporal graphs,” Knowledge and Information Systems, pp. 1–43, Dec. 2015. DOI: <http://dx.doi.org/10.1007/s10115-015-0908-6>
 - D. Caro, M. Andrea Rodríguez, and N. R. Brisaboa, “Data structures for temporal graphs based on compact sequence representations,” Information Systems, vol. 51, pp. 1–26, Jul. 2015. DOI: <http://dx.doi.org/10.1016/j.is.2015.02.002>

### International Conferences

 - N. R. Brisaboa, D. Caro, A. Fariña, and M. A. Rodríguez, "A Compressed Suffix-Array Strategy for Temporal-Graph Indexing," presented at the Proceedings of the 21st International Symposium on String Processing and Information Retrieval - Volume 8799, Ouro Preto, Brazil, 2014, vol. 8799, pp. 77–88. DOI: <http://dx.doi.org/10.1007/978-3-319-11918-2_8>
 - G. D. Bernardo, N. R. Brisaboa, D. Caro, and M. A. Rodriguez, “Compact Data Structures for Temporal Graphs,” presented at the Data Compression Conference (DCC), 2013, 2013, p. 477. DOI: <http://dx.doi.org/10.1109/DCC.2013.59> 

### International Workshops

 - D. Caro, "A compressed hexatree for temporal-graph indexing... or how to compress the $k^4$-tree," presented at the SPIRE 2014 Workshop on Compression, Text, and Algorithms (WCTA 2014), Ouro Preto, Brazil.

### Manuscripts 
 - N. R. Brisaboa, D. Caro, A. Fariña, and M. A. Rodríguez, "Using Compressed Suffix-Arrays for a Compact Representation of Temporal-Graphs". (under review)


## Code and datasets
Visit <http://github.com/diegocaro/temporalgraphs>.


# Acknowledgements

Through the journey of pursuing the PhD I received support from people all over the world. With these words I would like to say thank you to all of them. In particular, I would like to say thanks to Diego Seco for his help in the preliminary discussions of the structures, to Antonio Fariña for his infinite support in the development and implementation of many data structures, to Guillermo de Bernardo and Susana Ladra for introduce me to the $k^2$ world, to Gonzalo Navarro and Simon Gog for their suggestions on the improvement of the data structures and the experimental evaluation, and to Claudio Sanhueza from Yahoo! Labs Chile who helped me to obtain datasets for the experimental evaluation.

And of course, I would like to say thank to my co-phd-workers José Fuentes, Erick Elejalde, Natalia Jaña, Sandra Álvarez, Guillermo de Bernardo and Leticia González. 

This work was partially supported by a CONICYT doctoral fellowship, by Fondecyt 1140428, and by Fondef D09I1185.

The idea of creating this web site was borrowed from my friend and colleage [José Fuentes](http://thesis.josefuentes.cl).
