#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <windows.h>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    double t1 = omp_get_wtime();

    int num_threads;
    num_threads = atoi(argv[1]);
    omp_set_num_threads(num_threads);

    int N;
    N = atoi(argv[2]);
                  
    int rank_mpi; //номер текущего процесса
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_mpi);
    int size_mpi; //Количество процессов в приложении
    MPI_Comm_size(MPI_COMM_WORLD, &size_mpi);
    
    int position = rank_mpi * (N / size_mpi);
    double sum = 0;
    #pragma omp parallel reduction(+:sum)
    {           
        int rank_omp, size_omp;
        rank_omp = omp_get_thread_num();
        size_omp = omp_get_num_threads();
        printf("I am %d thread from %d> process. Number of hybrid threads = %d. position = %d\n", rank_omp, 
            rank_mpi, size_omp * size_mpi, position);

        #pragma omp for schedule(static)
        for (int i = position; i < (position + (N / size_mpi)); i++) {
            double x = (i + 0.5) / N;
            sum += 4 / (1 + pow(x, 2));
        }
        int num = omp_get_thread_num();
        printf("%d thread sum = %.10f\n", num, sum);
    }
    sum /= N; //результат суммы в каждом потоке
    double buf = sum; //сообщение
    printf("Summ of process %d = %.10f\n", rank_mpi, sum);
    
    MPI_Status status; //атрибуты принятого сообщения
    
    if (rank_mpi == 0) { //схема мастер-рабочие
        for (int i = 1; i < size_mpi; i++) {
            MPI_Recv(&buf, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &status);
            printf("receive message <%.10f> from <%d>\n", buf, i);
            sum += buf;
        }
        double t2 = omp_get_wtime();
        printf("\nPi = %.10f, time = %.10f\n\n", sum, t2-t1);
    }
    else {
        MPI_Send(&buf, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}