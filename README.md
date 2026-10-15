# Numerical Simulation Laboratory

This repository contains the exercises for the Numerical Simulation Laboratory course at Università degli Studi di Milano, A.Y. 2025/2026, taught by Professor D. E. Galli.

Student ID: 438192A

## Repository structure

Each numbered folder contains one exercise session. Inside each folder you will find the C++ code (either directly in the folder or in `SOURCE/`), input files where needed (`INPUT/`), the generated data (`OUTPUT*/`) and a Jupyter notebook with the analysis and discussion of the results.

The contents of the exercises are the following:

| Folder | Topic |
|--------|-------|
| `01` | Pseudo-random number generator, Buffon's needle experiment |
| `02` | Monte Carlo integration and random walks |
| `03` | European option pricing (Black-Scholes) |
| `04` | Molecular dynamics |
| `06` | 1D Ising model |
| `07` | NVT Monte Carlo simulations |
| `08` | Variational Monte Carlo and simulated annealing |
| `09` | Genetic algorithm for the TSP |
| `10` | Parallel tempering (with MPI) for the TSP |
| `11` | Neural networks: function regression |
| `12` | Deep and convolutional neural networks: classification |
| `LIBS` | Shared libraries (random number generator, data blocking, genetic algorithm, simulated annealing, parallel tempering) |

## Requirements

- C++ compiler `g++` and `make`
- [Armadillo](https://arma.sourceforge.net/) 15.2.4 (not included in the repository)
- MPI for parallelization
- Python 3 with Jupyter, NumPy and Matplotlib
- TensorFlow / Keras for neural networks

## How to compile and run

In the exercise folder, or in its `SOURCE/` subfolder where present, compile with:

```bash
make
```

Then run the executable:

- **Exercises 04, 06, 07:** `./<executable_name>.exe`
- **All other exercises:** `./<executable_name>`

Some programs ask for input at runtime:

- **Exercise 08.1:** enter the trial wave function parameters `mu` and `sigma`.
- **Exercise 09:** choose the TSP configuration to simulate by typing `circle`, `square` or `italy`.

Executables are not included in the repository and must be compiled locally.

