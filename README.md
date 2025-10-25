# Algorithms for Constructing a Minimum Spanning Tree

This project is part of my **Master’s Thesis** at the **Faculty of Mathematics, University of Belgrade (2025)**.  
The goal of the research was to **compare several algorithms for constructing a Minimum Spanning Tree (MST)** and analyze their performance on sparse and dense graphs.

---

## 🧩 Overview

A **Minimum Spanning Tree (MST)** of an undirected, connected, weighted graph is a connected subgraph that includes all vertices and has the minimum possible total edge weight.  
For disconnected graphs, the problem extends to finding a *minimum spanning forest*.

## ⚙️ Implemented Algorithms

The following MST algorithms were implemented in **C++**:

1. **Prim’s algorithm**
2. **Kruskal’s algorithm**
3. **Borůvka’s algorithm**
4. **Reverse-Delete algorithm**
5. **Karger’s algorithm**
6. **Fredman–Tarjan algorithm**
7. **Chazelle’s algorithm**

Each algorithm was implemented independently and evaluated on randomly generated graphs.

---

## 📊 Evaluation and Analysis

- Input size was represented by the **number of vertices (n)**.  
- For each algorithm, the **execution time** was measured in milliseconds as a function of *n*.  
- All measurements were averaged over **100 randomly generated graphs** of the same size.  
- Experiments were performed separately for **sparse** and **dense** graphs.
