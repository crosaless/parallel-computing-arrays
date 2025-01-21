#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void rellenarMatriz(int** matriz, int filas, int columnas) {
	int i, j;
	for (i = 0; i < filas; i++) {
		for (j = 0; j < columnas; j++) {
			matriz[i][j] = rand() % 20;
		}
	}
}

int multiplicarMatrices(int* arregloAB, int t, int columnasA) {
	int R = 0;
	int i;
	for (i = 0; i < t; i++) {
		R += arregloAB[i] * arregloAB[i + columnasA];
	}
	return R;
}

int main(int argc, char* argv[]) {
	int filasA, columnasA, filasB, columnasB;
	int identificador, RangoProcesos;
	int cuenta = 0;
	double TiempoInicio, TiempoFin, TiempoTotal;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &identificador);
	MPI_Comm_size(MPI_COMM_WORLD, &RangoProcesos);
	
	if (identificador == 0) {
		printf("Ingrese el nro de filas de la matriz A: ");
		scanf("%d", &filasA);
		printf("Ingrese el nro de columnas de la matriz A: ");
		scanf("%d", &columnasA);
		printf("Ingrese el nro de filas de la matriz B: ");
		scanf("%d", &filasB);
		printf("Ingrese el nro de columnas de la matriz B: ");
		scanf("%d", &columnasB);
		
		if (columnasA != filasB) {
			printf("No se pueden multiplicar las matrices. El nro de columnas de A debe ser igual al nro de filas de B.\n");
			return 1;
		}
	}
	
	TiempoInicio = MPI_Wtime();
	
	int i, j;
	
	int** matrizA = (int**)malloc(filasA * sizeof(int*));
	for (i = 0; i < filasA; i++) {
		matrizA[i] = (int*)malloc(columnasA * sizeof(int));
	}
	int** matrizB = (int**)malloc(filasB * sizeof(int*));
	for (i = 0; i < filasB; i++) {
		matrizB[i] = (int*)malloc(columnasB * sizeof(int));
	}
	int** matrizResultado = (int**)malloc(filasA * sizeof(int*));
	for (i = 0; i < filasA; i++) {
		matrizResultado[i] = (int*)malloc(columnasB * sizeof(int));
	}
	
	if (identificador == 0) {
		srand(time(NULL));
		rellenarMatriz(matrizA, filasA, columnasA);
		rellenarMatriz(matrizB, filasB, columnasB);
	}
	
	int Totalelementos = 0;
	int t;
	int seguimiento[5];
	
	if (identificador == 0) {
		Totalelementos = columnasA + filasB;
		MPI_Bcast(&Totalelementos, 1, MPI_INT, 0, MPI_COMM_WORLD);
		
		for (i = 0; i < filasA; i++) {
			for (j = 0; j < columnasB; j++) {
				cuenta++;
				int* arregloAB = (int*)malloc(2 * Totalelementos * sizeof(int));
				
				for (t = 0; t < Totalelementos; t++) {
					arregloAB[t] = matrizA[i][t];
					arregloAB[Totalelementos + t] = matrizB[t][j];
				}
				
				seguimiento[0] = Totalelementos;
				seguimiento[1] = i;
				seguimiento[2] = j;
				if (i == (filasA - 1) && j == (columnasB - 1)) {
					seguimiento[3] = 5;
				}
				
				if (cuenta < RangoProcesos) {
					MPI_Send(arregloAB, 2 * Totalelementos, MPI_INT, cuenta, 01, MPI_COMM_WORLD);
					MPI_Send(seguimiento, 5, MPI_INT, cuenta, 00, MPI_COMM_WORLD);
				} else {
					int resultadoProvisorio[3];
					MPI_Recv(resultadoProvisorio, 3, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
					matrizResultado[resultadoProvisorio[1]][resultadoProvisorio[2]] = resultadoProvisorio[0];
					MPI_Send(arregloAB, 2 * Totalelementos, MPI_INT, status.MPI_SOURCE, 01, MPI_COMM_WORLD);
					MPI_Send(seguimiento, 5, MPI_INT, status.MPI_SOURCE, 00, MPI_COMM_WORLD);
				}
				
				free(arregloAB);
			}
		}
		
		for (i = 1; i < RangoProcesos; i++) {
			int resultadoProvisorio[3];
			MPI_Recv(resultadoProvisorio, 3, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			matrizResultado[resultadoProvisorio[1]][resultadoProvisorio[2]] = resultadoProvisorio[0];
			seguimiento[3] = 6;
			int* arregloAB = (int*)malloc(2 * Totalelementos * sizeof(int));
			MPI_Send(arregloAB, 2 * Totalelementos, MPI_INT, status.MPI_SOURCE, 01, MPI_COMM_WORLD);
			MPI_Send(seguimiento, 5, MPI_INT, status.MPI_SOURCE, 00, MPI_COMM_WORLD);
			free(arregloAB);
		}
	} else {
		int seguimiento[5];
		do {
			MPI_Recv(&Totalelementos, 1, MPI_INT, 0, 01, MPI_COMM_WORLD, &status);
			MPI_Recv(matrizResultado, 2 * Totalelementos, MPI_INT, 0, 01, MPI_COMM_WORLD, &status);
			MPI_Recv(seguimiento, 5, MPI_INT, 0, 00, MPI_COMM_WORLD, &status);
			if (seguimiento[3] != 6) {
				int R = multiplicarMatrices(matrizResultado[identificador - 1], Totalelementos, columnasA);
				int resultadoProvisorio[3];
				resultadoProvisorio[0] = R;
				resultadoProvisorio[1] = seguimiento[1];
				resultadoProvisorio[2] = seguimiento[2];
				MPI_Send(resultadoProvisorio, 3, MPI_INT, 0, 22, MPI_COMM_WORLD);
			}
		} while (seguimiento[3] != 5 && seguimiento[3] != 6);
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	TiempoFin = MPI_Wtime();
	TiempoTotal = TiempoFin - TiempoInicio;
	MPI_Finalize();
	
	if (identificador == 0) {
		printf("Tiempo total de ejecucion: %f segundos\n", TiempoTotal);
	}
	
	for (i = 0; i < filasA; i++) {
		free(matrizA[i]);
	}
	free(matrizA);
	
	for (i = 0; i < filasB; i++) {
		free(matrizB[i]);
	}
	free(matrizB);
	
	for (i = 0; i < filasA; i++) {
		free(matrizResultado[i]);
	}
	free(matrizResultado);
	
	return 0;
}
