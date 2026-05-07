#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    // definir aqui o número de jobs e máquinas, e o nome do arquivo a ser criado
    int jobs = 100;
    int machines = 100;
    FILE *f = fopen("gg100.jss", "w");
    if (!f)
        return 1;

    fprintf(f, "%d %d\n", jobs, machines);
    srand(time(NULL));

    for (int i = 0; i < jobs; i++)
    {
        // alterar aqui o número de máquinas a serem alocadas para cada job
        int m_ids[100];
        for (int m = 0; m < machines; m++)
            m_ids[m] = m;

        for (int m = 0; m < machines; m++)
        {
            int target = rand() % machines;
            int temp = m_ids[m];
            m_ids[m] = m_ids[target];
            m_ids[target] = temp;
        }
        for (int j = 0; j < machines; j++)
        {
            fprintf(f, "%d %d ", m_ids[j], (rand() % 90) + 10);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    // alterar o printf para refletir o número de jobs e máquinas criados
    printf("Created a heavy 100x100 matrix.\n");
    return 0;
}