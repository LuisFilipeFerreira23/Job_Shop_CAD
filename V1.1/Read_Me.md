# Notes:

Both sequential.c and paralel.c are commented to ensure the lines are clearer
Duration of threads is being generated in the output of jss_solver_paralel.exe
Makespan and Matrixes are being printed in both outputs as per the requirements
Full time spent from start to finish printed at the end of both outputs as per the requirements

----

# Report:

A. Implementação Sequencial (5 vals)☑️
    1) Descreva o algoritmo escolhido para fazer a atribuição dos tempos de início das operações.⭕
    2) Apresente uma implementação sequencial da abordagem. 
        • Esta implementação deve receber dois parâmetros na linha de comandos: um ficheiro com os dados de entrada; e um ficheiro para gravar o resultado.☑️
        • A implementação deve cumprir as restrições do problema na solução gerada☑️

B. Implementação Paralela com Partilha de Memória (11 vals)☑️
    1) Seguindo a metodologia de Foster, apresente uma proposta de particionamento, uma divisão do problema em grupos de instruções e dados, tendo em conta a abordagem usada.
        • O algoritmo a usar deve ser identificado e ser preferencialmente o mesmo da implementação sequencial.☑️
        • Identifique o grupo de instruções que será executado por cada uma das threads.☑️
        • Descreva o padrão de comunicação usado entre as threads para a troca de informação.⭕
    2) Apresente uma implementação paralela usando partilha de memória da abordagem.⭕
        • Esta implementação deve receber dois parâmetros na linha de comandos: um ficheiro com os dados de entrada; e um ficheiro para gravar o resultado.
        • Adicionalmente, deve receber como parâmetro de entrada o número de threads.☑️
        • Compare e comente o resultado da simulação entre a versão paralela e a versão sequencial, nomeadamente os tempos de início das operações e o tempo de conclusão da última operação executada.⭕
        • O relatório deve identificar:⭕
            i. estrutura de dados usada no programa;
            ii. código inicial de arranque das threads do programa;
            iii. código executado pelas várias threads;
            iv. código final do programa.

    3) Identifique as seguintes características do programa paralelo:⭕
        • quais as variáveis globais partilhadas (só de leitura e de leitura/escrita), e as variáveis locais de cada thread;
        • as secções críticas (secções de código no programa paralelo), que podem criar situações de condição de corrida, e as variáveis envolvidas;
        • quais as técnicas de exclusão mútua utilizadas para garantir a correcção e determinismo do programa.

C. Análise do Desempenho (4 vals)
    1) De modo a medir o desempenho das implementações anteriores, acrescente no código um mecanismo de medição do tempo de execução.
        • Durante a medição do tempo de execução não deve haver qualquer output ou input para a consola. Os dados podem ser obtidos por um ficheiro de texto externo.☑️
        • O cálculo do tempo de execução na versão paralela deverá incluir a criação e término das threads.☑️
        • A recolha do tempo de execução de cada aplicação deve ser uma média ou soma de várias execuções, configurável (por exemplo: 10 repetições), em vez de uma única execução.⭕

    2) Registe numa tabela o tempo de execução do programa, para os vários nºs de threads (assuma como exemplo: SEQ, PC1, PC2, PC4, PC8, PC16, PC32).
        • O ficheiro de entrada deve ser escolhido de modo a gerar uma carga de computação com pelo menos 1 minuto de execução na versão sequencial.☑️
        • Use os mesmos dados de entrada para a versão concorrente. Compare as listas de resultados, nomeadamente, a ordem de execução das operações no tempo e o maior tempo de término das últimas operações.⭕
        • Apresente um gráfico em que usa para o eixo dos XX o nº de threads e para o eixo dos YY o tempo de execução.⭕

    3) Compare o desempenho da execução concorrente com a execução sequencial, calculando o valor de Speedup (S) (rácio entre tempo de execução sequencial T1, e o tempo de execução paralela Tp para p threads).⭕
        • Indique o nº de processadores presentes na máquina usada para os resultados.
        • Construa um gráfico com a evolução de S em função do valor de p (para os eixos YY e XX respectivamente).
        • Comente a variação do valor de S e apresente possíveis motivos.

----

# Steps:

1. Change gen.c number of jobs and machines
2. Compile it and run it to generate gg150.jss
3. Compile sequential and paralel into .exe
4. Run both .exes with an output result

----

# Compile generator:

gcc gen.c -o jss_gen

----
# Run generator:

.\jss_gen.exe

----

# Compile sequential:

gcc sequential.c -o jss_solver

----

# Run Sequential:

.\jss_solver.exe > sequential.output

----

# Compile Paralel:

gcc -fopenmp paralel.c -o jss_solver_paralel

----

# Run Paralel:

.\jss_solver_paralel.exe > paralel.output
