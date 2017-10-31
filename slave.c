#include <stdio.h>
#include <math.h>
#include "pvm3.h"

int main(int argc, char **argv)
{
	double pw;	
	int mytid;
	int i, *a, *s;
	int num_data, master;
	double sum = 0; 

	/* enroll in PVM */
	mytid = pvm_mytid();

	/* receive portion of array */
	pvm_recv(-1,-1);
	pvm_upkint(&num_data, 1, 1);
	a = (int *) malloc(num_data*sizeof(int));
	s = (int *) malloc(num_data*sizeof(int));
	pvm_upkint(s, num_data, 1);
	pvm_upkint(a, num_data, 1);

	for(i = 0; i<num_data; i++) {
		sum += pow(a[i],s[i]);	
	}	
	
	/* send computed sum back to master */
        master = pvm_parent();
	pvm_initsend(PvmDataRaw);
	pvm_pkdouble(&sum, 1, 1);
	pvm_send(master, 7);

	pvm_exit();
}
