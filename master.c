#include <stdio.h>
#include <math.h>
#include "pvm3.h"
#define SLAVE "/homes/e1250018/slave"

void usage(int *p,int *n) 
{
	
	printf("choose N please [3,10,50,100] ");
	scanf("%d",n);
	printf("choose P please [2,4,8,16] ");
	scanf("%d",p);
}

int main(int argc, char** argv)
{
	int P=2,N=3;	
	int mytid, *task_ids;
	int *supercript,*a,*am,*index;
	double sum = 0.0, *results;
	int i,k,j,m, msgtype, num_data ;
	int n;
	int *arg;
	int km[] = {1,2,4,8,16,32,64};

	usage(&P,&N);

	num_data = ceil((double)N/P);

	/* allocate space for arrays */
	task_ids	= (int *)malloc(sizeof(int)*P);
	results 	= (double *)malloc(sizeof(double)*P);
	index 		= (int *)malloc(sizeof(int)*P);
	a 		= (int *)malloc(sizeof(int)*N);
	supercript	= (int *)malloc(sizeof(int)*N);
	am 		= (int *)malloc(sizeof(int)*N);
	arg		= (int *)malloc(sizeof(char *)*P);
	
	/* Get values for the coefficients */
	for (i = 0; i < N; i++) {
		
		printf("\nenter a%d ",i+1);
		scanf("%d",&a[i]);
		/*
		a[i] = i+1;
		*/
	}
	
	/* Arrange the arguments of the slaves */
	k = N % P;
	if ( k == 0 ) k = P;
	for( i = 0; i < k ; i++) {
		arg[i] = num_data;
	}		
	for( ; i < P ; i++) {
		arg[i] = num_data - 1;
	}		

	/* Form index array according to the arg numbers*/
	index[0] = 0;
	for ( i = 1; i < P; i++) {
		index[i] = index[i-1] + arg[i-1];	
	}
	
	/* Arrange array a for load balance */
	for ( k = 0,i = 0; i < P; i++) {
		for ( j = 0; j < arg[i]; j++,k++) {
			if (k == N) break;
			supercript[k] = j*P+i +1;
			am[k] = a[supercript[k] -1 ];
		}
	}

	/* enroll in PVM */
	mytid = pvm_mytid();

	/* spawn worker tasks */
	pvm_spawn(SLAVE, (char **)0, PvmTaskDefault, "", P, task_ids);
	
	/* print supercripts and coeffiecients for task_ids*/ 
	printf(" \nsupercripts and coefficients: ");
	for ( m =0, k = 0,i = 0; i < P; i++) {
		printf("\nprocessor:%d\t",i+1);
		printf(" supercripts:");
		for ( j = 0; j < arg[i]; j++,m++) {
			if (m == N) break;
			printf("%d ",supercript[m]);
		}
		printf("\tcoefficients:");
		for ( j = 0; j < arg[i]; j++,k++) {
			if (k == N) break;
			printf("%d ",am[k]);
		}
	}
	printf("\n");
	
	/* send data to worker tasks */
	for (i = 0; i < P; i++) {
		if ( task_ids[i] == 0 ) break;
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&arg[i], 1, 1);		/*arg number*/
		pvm_pkint(&supercript[index[i]], arg[i], 1); 	/*supercripts*/
		pvm_pkint(&am[index[i]], arg[i], 1); 	/*coefs*/
		pvm_send(task_ids[i], 4);
	}

	/* wait and gather results */
	msgtype = 7;
	for (i = 0; i < P; i++) {
		pvm_recv(task_ids[i], msgtype);
		pvm_upkdouble(&results[i], 1, 1);
		sum += results[i];
	}
	
	/* processor results */
	printf(" \nprocessor results: ");
	for ( i = 0; i < P; i++) {
		printf("\nprocessor:%d\t",i+1);
		printf("%e",results[i]);	
	}
	printf("\nThe sum is %e \n",sum);
	/*printf("The sum is %.0f \n",sum);*/

	pvm_exit();
}
