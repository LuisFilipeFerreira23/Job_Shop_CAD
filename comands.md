Compiling:
Sequential: (gcc <main.c> -o <output>)
gcc sequential.c -o sequential

Parallel with OpenMP library: (gcc -fopenmp <main.c> -o <output>)
gcc -fopenmp parallel.c -o parallel

Executing:
Sequential: (sequential.exe <input_file.jss> <output_file.txt>)
.\sequential.exe .\EntryFiles\gg3.jss .\Outputs\output_sequential.txt

Parallel: (parallel.exe <input_file.jss> <output_file.txt> <num_threads> <num_repetitions>)
.\parallel.exe .\EntryFiles\gg3.jss .\Outputs\output_parallel.txt 8 1

Validating (requires python): (py <input.jss> <output.txt>)
py validator.py .\EntryFiles\gg3.jss .\Outputs\output_sequential.txt