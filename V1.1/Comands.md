# Compile sequential:

gcc sequential.c -o jss_solver

----

# Run Sequential:

.\jss_solver.exe

----

# Compile Paralel:

gcc -fopenmp paralel.c -o jss_solver_paralel

----

# Run Paralel:

.\jss_solver_paralel.exe

----

# Run Paralel with output:

.\jss_solver_paralel.exe > paralel.output