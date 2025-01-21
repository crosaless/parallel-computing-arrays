#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void rellenarMatriz(int **matriz, int filas, int columnas) {
	int i, j;
	for (i = 0; i < filas; i++) {
		for (j = 0; j < columnas; j++) {
			matriz[i][j] = rand() % 20;  // Valores aleatorios entre 0 y 100
		}
	}
}

void multiplicarMatrices(int **matrizA, int **matrizB, int **matrizResultado, int filasA, int columnasA, int columnasB) {
	int i, j, k;
	for (i = 0; i < filasA; i++) {
		for (j = 0; j < columnasB; j++) {
			matrizResultado[i][j] = 0;
			for (k = 0; k < columnasA; k++) {
				matrizResultado[i][j] += matrizA[i][k] * matrizB[k][j];
			}
		}
	}
}

int main() {
	int filasA, columnasA, filasB, columnasB;
	clock_t inicio, fin;
	double tiempo;
	int i, j;
	
	
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
	
	int **matrizA, **matrizB, **matrizResultado; 
	
	matrizA = (int**) malloc(filasA*sizeof(int*));
		
	for (i=0; i<filasA; i++){
		matrizA[i]=(int*) malloc (columnasA*sizeof(int));
	}

	matrizB = (int**) malloc(filasB*sizeof(int*));
	
		for (i=0; i<filasB; i++){
			matrizB[i]=(int*) malloc (columnasB*sizeof(int));
	}
	
	matrizResultado = (int**) malloc(filasA*sizeof(int*));
	
	for (i=0; i<filasA; i++){
		matrizResultado[i]=(int*) malloc (columnasB*sizeof(int));
	}
	
	
	// Inicializar el generador de nros aleatorios con la hora actual
	srand(time(NULL));
	
	rellenarMatriz(matrizA, filasA, columnasA);
	/*printf("Matriz A con valores aleatorios:\n");
	for (int i = 0; i < filasA; i++) {
		for (int j = 0; j < columnasA; j++) {
			printf("%d ", matrizA[i][j]);
		}
		printf("\n");
	}*/
	
	rellenarMatriz(matrizB, filasB, columnasB);
	/*printf("Matriz B con valores aleatorios::\n");
	for (int i = 0; i < filasB; i++) {
		for (int j = 0; j < columnasB; j++) {
			printf("%d ", matrizB[i][j]);
		}
		printf("\n");
	}*/
	
	inicio = clock(); // Registra el tiempo de inicio
	multiplicarMatrices(matrizA, matrizB, matrizResultado, filasA, columnasA, columnasB);
	fin = clock(); // Registra el tiempo de finalizaci�n
	
	tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;
	
	/*printf("Resultado de la multiplicaci�n:\n");
	for (int i = 0; i < filasA; i++) {
		for (int j = 0; j < columnasB; j++) {
			printf("%d ", matrizResultado[i][j]);
		}
		printf("\n");
	}*/
	printf("Tiempo de ejecucion: %f segundos\n", tiempo);
	
	for(i=0; i<filasA; i++){
		free(matrizA[i]);
	}
	
	for(i=0; i<filasB; i++){
		free(matrizB[i]);
	}
	
	for(i=0; i<filasA; i++){
		free(matrizResultado[i]);
	}
	
	free(matrizA);
	free(matrizB);
	free(matrizResultado);
	return 0;
}
