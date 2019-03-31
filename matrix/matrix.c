// gcc -pg -Wall main.c matrix.c -o main
// ./main 200 200
// gprof main gmon.out

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include "matrix.h"

matrix_t *matrix_create(int rows, int cols){
    matrix_t *m = NULL;
    unsigned int i;

    m = (matrix_t*) malloc( sizeof(matrix_t) );
    m->rows = rows;
    m->cols = cols;

    m->data = (double**) malloc(sizeof(double*) * rows);
    double* mat = (double*) malloc(sizeof(double) * rows * cols);

    for(i=0; i < rows; i++){
        m->data[i] = &mat[i*cols];
    }

    matrix_fill(m, 0.0);

    return m;
}

matrix_t *matrix_cpy(matrix_t *m){
    int i;
    matrix_t *C = matrix_create(m->rows, m->cols);

    memcpy(C->data[0], m->data[0], sizeof(double) * m->rows * m->cols);
    for(i=0; i < C->rows; i++){
        C->data[i] = &C->data[0][i*C->cols];
    }

    return C;
}

void matrix_destroy(matrix_t *m){
    free(m->data[0]);
    free(m->data);
    free(m);

    return;
}

void matrix_randfill(matrix_t *m){
    int i, j;
    for (i = 0; i < m->rows; i++) {
        for (j = 0; j < m->cols; j++) {
            m->data[i][j] = random();
        }
    }
}

void matrix_fill(matrix_t *m, double val){
   int i, j;
   for (i = 0; i < m->rows; i++) {
      for (j = 0; j < m->cols; j++) {
         m->data[i][j] = val;
      }
   }
}

void matrix_print(matrix_t *m){

   int i, j;
   for (i = 0; i < m->rows; i++) {
      for (j = 0; j < m->cols; j++) {
         printf("%.2f ", m->data[i][j]);
      }
      printf("\n");
   }
   fflush(stdout);
}

void array_print(double* vet, int begin, int end){
    int i;
    for(i = begin; i <= end; i++){
        printf("%.2f ", vet[i]);
    }
    printf("\n");
}

matrix_t *matrix_sum_serial(matrix_t *A, matrix_t *B){
    //Não podemos somar
    if( (A->rows != B->rows) && (A->cols != B->cols) ){
        printf("Impossível somar\n");
        return NULL;
    }

    int i,j;

    for(i = 0; i < A->rows; i++){
        for(j = 0; j < A->cols; j++){
            A->data[i][j] += B->data[i][j];
        }
    }

    return A;
}

matrix_t *matrix_sum_parallel(matrix_t *A, matrix_t *B, int nthreads){
    DadosThread *dt = NULL;
    pthread_t *threads = NULL;

    if (!(dt = (DadosThread *) malloc(sizeof(DadosThread) * nthreads))) {
       printf("Erro ao alocar dados da thread...\n");
       exit(EXIT_FAILURE);
    }

    if (!(threads = (pthread_t *) malloc(sizeof(pthread_t) * nthreads))) {
        printf("Erro ao alocar as threads...\n");
        exit(EXIT_FAILURE);
    }

    int nrows = A->rows;
    int ncols = A->cols;
    int i;

    long long int x = nrows * ncols / nthreads;
    for (i = 0; i < nthreads-1; i++) {
       dt[i].id = i;
       dt[i].l_i = x*i;
       dt[i].l_f = x*i + x - 1;

       dt[i].A = A;
       dt[i].B = B;
       pthread_create(&threads[i], NULL, sum_thread, (void *) (dt + i));
    }
    dt[i].id = i;
    dt[i].l_i = x*i;
    dt[i].l_f = (nrows * ncols) - 1;

    dt[i].A = A;
    dt[i].B = B;
    pthread_create(&threads[i], NULL, sum_thread, (void *) (dt + i));


    for (i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(dt);
    free(threads);

    return A;
}

void *sum_thread(void *arg){
    DadosThread *p = (DadosThread *) arg;
    int i;
    for(i = p->l_i; i <= p->l_f; i++){
        p->A->data[0][i] += p->B->data[0][i];
    }

  // printf("Thread %d terminou\n", p->id);

  return NULL;
}


matrix_t *matrix_multiply_serial(matrix_t *A, matrix_t *B, matrix_t *C){
    //Não podemos multiplicar
    if(A->cols != B->rows){
        printf("Impossível multiplicar\n");
        printf("m1: %d | m2: %d\n", A->cols, B->rows);
        return NULL;
    }

    int i,j,k;

    for(i = 0; i < A->rows; i++){
        for(j = 0; j < B->cols; j++){
            for(k = 0; k < A->cols; k++){
                C->data[i][j] += A->data[i][k] * B->data[k][j];
            }
        }
    }

    return C;
}

//Cada thread é responsável por um determinado número de linhas da matriz C
matrix_t *matrix_multiply_parallel(matrix_t *A, matrix_t *B, matrix_t *C, int nthreads){
   if(A->cols != B->rows){
        printf("Impossível multiplicar\n");
        exit(EXIT_FAILURE);
    }

    DadosThread *dt = NULL;
    pthread_t *threads = NULL;

    if (!(dt = (DadosThread *) malloc(sizeof(DadosThread) * nthreads))) {
       printf("Erro ao alocar dados da thread...\n");
       exit(EXIT_FAILURE);
    }

    if (!(threads = (pthread_t *) malloc(sizeof(pthread_t) * nthreads))) {
        printf("Erro ao alocar as threads...\n");
        exit(EXIT_FAILURE);
    }

    int nrows = A->rows;
    int ncols = B->cols;
    int i;
    long long int x;

    if(nthreads > nrows){ //Não é possível dividir entre todas a threads
        nthreads = nrows;
        for (i = 0; i < nthreads; i++) {
            dt[i].id = i;
            dt[i].l_i = i;
            dt[i].l_f = i;

            dt[i].A = A;
            dt[i].B = B;
            dt[i].C = C;
            pthread_create(&threads[i], NULL, multiply_thread, (void *) (dt + i));
        }
    }
    else{
        x = nrows / nthreads;
        for (i = 0; i < nthreads-1; i++) {
            dt[i].id = i;
            dt[i].l_i = x*i;
            dt[i].l_f = x*i + x - 1;

            dt[i].A = A;
            dt[i].B = B;
            dt[i].C = C;
            pthread_create(&threads[i], NULL, multiply_thread, (void *) (dt + i));
        }
        dt[i].id = i;
        dt[i].l_i = x*i;
        dt[i].l_f = nrows - 1;

        dt[i].A = A;
        dt[i].B = B;
        dt[i].C = C;
        pthread_create(&threads[i], NULL, multiply_thread, (void *) (dt + i));
    }

    for (i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(dt);
    free(threads);

    return C;
}

void *multiply_thread(void *arg){
    DadosThread *p = (DadosThread *) arg;

  // printf("Thread %d: %d %d\n", p->id, p->l_i, p->l_f);

    int i, j, k;
    for(i = p->l_i; i <= p->l_f; i++){ //Percorre as linhas de C relevantes
        for(j = 0; j < p->C->cols; j++){ //Percorre cada coluna de C
            for(k = 0; k < p->A->cols; k++){
                p->C->data[i][j] += p->A->data[i][k]*p->B->data[k][j];
            }
        }
    }


    return NULL;
}


/* ========================================================= */

matrix_t *mergesort_parallel(matrix_t* A, int nthreads){
    int i;
    DadosThread *dt = NULL;
    pthread_t *threads = NULL;

    if (!(dt = (DadosThread *) malloc(sizeof(DadosThread) * nthreads))) {
       printf("Erro ao alocar dados da thread...\n");
       exit(EXIT_FAILURE);
    }

    if (!(threads = (pthread_t *) malloc(sizeof(pthread_t) * nthreads))) {
        printf("Erro ao alocar as threads...\n");
        exit(EXIT_FAILURE);
    }

    // pthread_mutex_t* mutexes;

	// if(!(mutexes = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t) * nthreads))){
	// 	printf("Erro ao alocar os mutexes");
	// 	exit(EXIT_FAILURE);
	// }


	// for(i = 0; i < nthreads; i++){
	// 	if(pthread_mutex_init(&mutexes[i], NULL) != 0){
	// 		printf("Erro na inicialização de mvoid*) datautex\n");
	// 		exit(EXIT_FAILURE);
	// 	}
    // }

	//Guardar o inicio e fim de vetor de cada thread e que isso seja visível para as outras
    int** manip_atual = (int**) malloc(sizeof(int*)*nthreads);
    for(i = 0; i < nthreads; i++){
    	manip_atual[i] = (int*) malloc(sizeof(int)*2);
    }


    int nrows = A->rows;
    int ncols = A->cols;

    matrix_t *C = matrix_cpy(A);

    long long int x = nrows * ncols / nthreads;
    for (i = 0; i < nthreads; i++) {
       dt[i].id = i;
       dt[i].l_i = x*i;
       dt[i].l_f = x*i + x - 1;

       dt[i].A = A;
       dt[i].C = C;
       dt[i].vet = &C->data[0][x*i];
       dt[i].vet_size = x;

       dt[i].nthreads = nthreads;
       dt[i].threads = threads;
    //    dt[i].mutexes = mutexes;
       dt[i].manip_atual = manip_atual;

       //Talvez dê pra tirar l_i e l_f, mas talvez sejam uteis para as outras funções
       dt[i].manip_atual[i][0] = dt[i].l_i;
       dt[i].manip_atual[i][1] = dt[i].l_f;

       pthread_create(&threads[i], NULL, mergesort_thread, (void *) (dt + i));
    }

    dt[i-1].l_f = nrows*ncols - 1;
   	dt[i-1].manip_atual[i-1][1] = dt[i-1].l_f;
   	// printf("oi: %d\n", dt[i-1].l_f);

    dt[i-1].vet_size = (nrows * ncols) - dt[i-1].l_i + 1;

	    // for (i = 0; i < nthreads; i++) {
      //   if(pthread_join(threads[i], NULL) != 0){
      //   	printf("Erro de join da thread %d", i);
      //   	exit(EXIT_FAILURE);
      //   }
    pthread_join(threads[0], NULL);
        // printf("%d saiu com sucesso\n", i);


    // C = coisa_merge(dt, nthreads, nrows, ncols);


    // printf("\nC:\n");
    // matrix_print(C);

    // for(i = 0; i < nthreads; i++){
    // 	pthread_mutex_destroy(&mutexes[i]);
    // }

    for(i = 0; i < nthreads; i++){
    	free(manip_atual[i]);
    }
    free(manip_atual);

    free(dt);
    free(threads);

    return C;
}

matrix_t *coisa_merge(DadosThread *dt, int nthreads, int rows, int cols){
    matrix_t *A = matrix_create(rows, cols);
    int i, j, k;
    k = 0;
    for(i = 0; i < nthreads; i++){
        for(j = 0; j < dt[i].vet_size; j++){
            A->data[0][k] = dt[i].vet[j];
            k++;
        }
    }
    return A;
}

void *mergesort_thread(void *arg){

    DadosThread *p = (DadosThread *) arg;


    // pthread_mutex_lock(&p->mutexes[p->id]);


    int size, l_start;
    int n = p->vet_size;

    for(size = 1; size < n; size *= 2){
        for(l_start = 0; l_start < n - 1; l_start += 2*size){
            int mid = l_start + size - 1;
            int r_end = min(l_start + 2*size - 1, n-1);

            // merge(p->vet, l_start, mid, r_end);

        }
    }


    int active_id = p->id; //id relativo às threads ativas
    int combined_threads = 1;
    long long int x = p->C->rows * p->C->cols / p->nthreads;
    //Número de threads é ímpar
    if(p->nthreads % 2 != 0){
    	if(p->id == p->nthreads - 1){
			     printf("Sai:%d\n", p->id);

			// pthread_mutex_unlock(&p->mutexes[p->id]);
    		return NULL;
    	}

    	//Todas as threads (exceto a última, sem par) foram combinadas
    	while(combined_threads < p->nthreads-1){

    		// printf("%d  %d\n", p->id, active_id);
			// printf("%d\n", combined_threads);

	    	//Essa thread tem um id ativo par -> ela vai assumir o 'load' da seguinte
	    	if(active_id % 2 == 0){


	    		if(p->id + combined_threads < p->nthreads-1){
					// printf("%d Tentando entrar: %d\n", p->id, combined_threads);
          pthread_join(p->threads[p->id + combined_threads], NULL);
            // pthread_mutex_lock(&p->mutexes[p->id + combined_threads]);
					// printf("Consegui entrar:%d + %d\n", p->id, combined_threads);
					// printf("%d Inicio: %d   Meio: %d    Fim: %d\n", p->id,
	    				// p->manip_atual[p->id][0],
	    				// p->manip_atual[p->id][1],
	    				// p->manip_atual[p->id + combined_threads][1]);


	    			// merge(&p->C->data[0][0],
	    			// 	p->manip_atual[p->id][0],
	    			// 	p->manip_atual[p->id][1],
	    			// 	p->manip_atual[p->id + combined_threads][1]);

	    			p->manip_atual[p->id][1] = p->manip_atual[p->id + combined_threads][1];

	    			// pthread_mutex_unlock(&p->mutexes[p->id + combined_threads]);
	    			combined_threads *= 2;
	    		}
	    		active_id /= 2;

	    	}
	    	else{
				// printf("Sai:%d\n", p->id);

	    		// pthread_mutex_unlock(&p->mutexes[p->id]);
	    		return NULL;
	    	}
   	 	}

	    //Juntar a thread 0 com a thread sozinha
		// pthread_mutex_lock(&p->mutexes[p->id + p->nthreads - 1]);
    pthread_join(p->threads[p->id + p->nthreads - 1], NULL);

		// printf("Consegui entrar:%d + %d\n", p->id, p->nthreads-1);
			// printf("%d Inicio: %d   Meio: %d    Fim: %d\n", p->id,
			// p->manip_atual[p->id][0],
			// p->manip_atual[p->id][1],
			// p->manip_atual[p->id + p->nthreads-1][1]);


		// merge(&p->C->data[0][0],
		// 	p->manip_atual[p->id][0],
		// 	p->manip_atual[p->id][1],
		// 	p->manip_atual[p->id + p->nthreads-1][1]);

		// pthread_mutex_unlock(&p->mutexes[p->id + p->nthreads - 1]);
		// pthread_mutex_unlock(&p->mutexes[p->id]);
	}
  else{ //p->nthreads % 2 == 0
    	// int aux = last_two_multiple(p->nthreads);
    	//Todas as threads foram combinadas
    	while(combined_threads < p->nthreads){

    		// printf("%d  %d\n", p->id, active_id);
			// printf("%d\n", combined_threads);

	    	//Essa thread tem um id ativo par -> ela vai assumir o 'load' da seguinte
	    	if(active_id % 2 == 0){


	    		if(p->id + combined_threads < p->nthreads){
					printf("%d Tentando entrar: %d\n", p->id, combined_threads);
	    			// pthread_mutex_lock(&p->mutexes[p->id + combined_threads]);
                    pthread_join(p->threads[p->id + combined_threads], NULL);
					printf("Consegui entrar:%d + %d\n", p->id, combined_threads);
					// printf("%d Inicio: %d   Meio: %d    Fim: %d\n", p->id,
	    				// p->manip_atual[p->id][0],
	    				// p->manip_atual[p->id][1],
	    				// p->manip_atual[p->id + combined_threads][1]);


	    			// merge(&p->C->data[0][0],
	    			// 	p->manip_atual[p->id][0],
	    			// 	p->manip_atual[p->id][1],
	    			// 	p->manip_atual[p->id + combined_threads][1]);

	    			p->manip_atual[p->id][1] = p->manip_atual[p->id + combined_threads][1];

	    			// pthread_mutex_unlock(&p->mutexes[p->id + combined_threads]);
	    			combined_threads *= 2;

	    		}
	    // 		else{
					// printf("Tentando entrar:%d + %d\n", p->id, combined_threads);
	    // 			pthread_mutex_lock(&p->mutexes[p->id + combined_threads ]);
					// usleep(1000);
	    // 			pthread_mutex_unlock(&p->mutexes[p->id + combined_threads]);
	    // 			combined_threads = p->id + combined_threads ;
	    // 		}

	    		active_id /= 2;
	    	}
	    	else{
				printf("Sai:%d\n", p->id);

	    		// pthread_mutex_unlock(&p->mutexes[p->id]);
	    		pthread_exit(NULL);
	    	}
	    }
	    // if(p->id == 0)
	    // pthread_mutex_unlock(&p->mutexes[p->id]);

    }

  return NULL;
}

int last_two_multiple(int n){
	int temp = 1;
	while(temp < n){
		temp *= 2;
	}
	if(temp == n)
		return n;
	return temp/2;
}

matrix_t* matrix_sort_serial(matrix_t* m){
    int size = m->rows * m->cols;

    iterative_merge_sort(m->data[0], 0, size-1);
    return m;
}

matrix_t* matrix_sort_p(matrix_t* mat, int nthreads){
    thread_info* info = NULL;
    pthread_t *threads = NULL;

    if (!(info = (thread_info*) malloc(sizeof(thread_info) * nthreads))) {
        printf("Erro ao alocar dados da thread...\n");
        exit(EXIT_FAILURE);
    }

    if (!(threads = (pthread_t *) malloc(sizeof(pthread_t) * nthreads))) {
        printf("Erro ao alocar as threads...\n");
        exit(EXIT_FAILURE);
    }
    thread_sort_setup(info, mat, nthreads);
    int i;
    for(i = 0; i < nthreads; i++){
        pthread_create(&threads[i], NULL, sort_thread, (void*) (info + i));
    }

    for(i = 0; i < nthreads; i++){
        pthread_join(threads[i], NULL);
    }

    int size = mat->rows * mat->cols;
    double* temp1 = (double*) malloc(sizeof(double) * size);
    double* temp2 = (double*) malloc(sizeof(double) * size);
    
    for(i = 1; i < nthreads; i *= 2){
        int j;
        for(j = 0; j + i < nthreads; j += 2*i){
            merge(mat->data[0], info[j].left, info[j].right, info[j+i].right, temp1, temp2);
            info[j].right = info[j+i].right;
        }
    }

    // merge(mat->data[0], info[0].left, size/2-1, info[1].right, temp1, temp2);

}

matrix_t* matrix_sort_parallel(matrix_t* mat, int nthreads){

    DadosThread *dt = NULL;
    pthread_t *threads = NULL;

    if (!(dt = (DadosThread *) malloc(sizeof(DadosThread) * nthreads))) {
        printf("Erro ao alocar dados da thread...\n");
        exit(EXIT_FAILURE);
    }

    if (!(threads = (pthread_t *) malloc(sizeof(pthread_t) * nthreads))) {
        printf("Erro ao alocar as threads...\n");
        exit(EXIT_FAILURE);
    }

    int rows = mat->rows;
    int cols = mat->cols;
    int i;
    long long int x = nthreads / rows;

    for(i = 0; i < nthreads - 1; i++){
        dt[i].id = i;
        dt[i].l_i = x*i;
        dt[i].l_f = x*i + x - 1;
        dt[i].A = mat;
        pthread_create(&threads[i], NULL, sort_thread, (void*) (dt + i));
    }
    dt[i].id = i;
    dt[i].l_i = x*i;
    dt[i].l_f = (rows * cols) - 1;
    dt[i].A = mat;
    pthread_create(&threads[i], NULL, sort_thread, (void*) (dt + i));

    for (i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(dt);
    free(threads);

    return mat;
}

void* sort_thread(void* arg){
    thread_info *data = (thread_info *) arg;

    iterative_merge_sort(data->vet, data->left, data->right);
}


void thread_sort_setup(thread_info* t_info, matrix_t* m, int nthreads){
    long long int size = m->rows * m->cols;
    long long int part = size / nthreads;
    int i;

    // Arrumando os dados para as threads
    for (i = 0; i < nthreads - 1; i++) {
       t_info[i].left = part * i;
       t_info[i].right = part * (i + 1) - 1;
       t_info[i].vet = m->data[0];
    }
    // Caso não aconteca divisao perfeita entre o tamanho da matriz 
    // e o numero de threads, a ultima thread pega o restante.
    t_info[i].left = part * i;
    t_info[i].right = size - 1;
    t_info[i].vet = m->data[0];
}

void recursive_merge_sort(matrix_t* mat, int l, int r){
    if (l < r) {
        int m = l+(r-l)/2;

        recursive_merge_sort(mat, l, m);
        recursive_merge_sort(mat, m+1, r);

        // merge(mat->data[0], l, m, r);
    }
}

void iterative_merge_sort(double* vet, int begin, int end){
    int current; 
    int left, mid, right;

    int size = (end - begin + 1);
    double* temp1 = (double*) malloc(sizeof(double) * size);
    double* temp2 = (double*) malloc(sizeof(double) * size);

    for (current = 1; current < size; current = 2 * current){
        // array_print(vet, 0, size-1);
        for (left = begin; left < end; left += 2 * current) {
            right = min(left + 2*current - 1, end); 
            mid = left + current -1;
            
            if(mid > end) continue;

            // array_print(vet, left, right);
            // printf("%d -> %d | %d\n", left, right, mid);
            merge(vet, left, mid, right, temp1, temp2);
            // array_print(vet, left, right);
            // printf("\n");
        } 
    }
    // merge(vet, 0, mid, right);
}

void merge(double *vet, int l, int m, int r, double* L, double* R){   
    long int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    for (i = 0; i < n1; i++)
        L[i] = vet[l + i];

    for (j = 0; j < n2; j++)
        R[j] = vet[m + j + 1];

    // array_print(L, 0, n1-1);
    // array_print(R, 0, n2-1);
    

    i = 0; j = 0; k = l;
    while (i < n1 && j < n2){
        if (L[i] <= R[j]) vet[k] = L[i++];
        else vet[k] = R[j++];
        k++;
    }

    while (i < n1) vet[k++] = L[i++];

    while (j < n2) vet[k++] = R[j++];

    // free(L); free(R);
}




void iterative_quick_sort(double* vet, int left, int right){
    double* stack = (double*) malloc(sizeof(double) * (right - left + 1));
    int top = -1;

    stack[++top] = left;
    stack[++top] = right;

    while(top > -1){
        right = stack[top--];
        left = stack[top--];

        int pivot = quick_partition(vet, left, right);
        // printf("L: %d | P: %d | R: %d\n", left, pivot, right);

        if(pivot - 1 > left){
            stack[++top] = left;
            stack[++top] = pivot - 1;
        }

        if(pivot - 1 < right){
            stack[++top] = pivot + 1;
            stack[++top] = right;
        }
    }
}

int quick_partition(double* vet, int left, int right){
    double r = vet[right];
    int l = left - 1;
    int i;

    // printf("\n[%d, %d]\n", left, right);
    // array_print(vet, left, right);

    if(left > right) return l+1;

    for(i = left; i < right; i++){
        if(vet[i] <= r){
            l++;
            // printf("|%.2f -> %.2f|\n", vet[l], vet[i]);
            swap(&vet[l], &vet[i]);
        }
    }
    swap(&vet[l+1], &vet[right]);
    // printf("|%.2f -> %.2f|\n", vet[l+1], vet[right]);


    // array_print(vet, left, right);
    // printf("pivot: %.2f (%d)\n", vet[l+1], l+1);

    return l+1;
}

int min(int x, int y) {
    return (x < y) ? x : y;
}

void swap(double* a, double* b){
    double d = *a;
    *a = *b;
    *b = d;
}
