#include <mpi.h>
#include <omp.h>
#include <cmath>
#include <stdio.h>

int main(int argc, char* argv[])
{
    int x;
    x = atoi(argv[1]);
    
    MPI_Init(&argc, &argv);
               
    int rank_mpi; //номер текущего процесса
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_mpi);
    int size_mpi; //Количество процессов в приложении
    MPI_Comm_size(MPI_COMM_WORLD, &size_mpi);

    omp_set_num_threads(x);

    #pragma omp parallel
    {
        int rank_omp, size_omp;
        rank_omp = omp_get_thread_num();
        size_omp = omp_get_num_threads();
        printf("I am %d thread from %d> process. Number of hybrid threads = % d.\n", rank_omp, 
            rank_mpi, size_omp * size_mpi);

    }

    MPI_Finalize();
    return 0;
}