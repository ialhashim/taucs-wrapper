#include <iostream>
#include "TaucsSolver.h"
#include "taucs_addon.h"

// Taucs is a C library
#ifndef TAUCS_H
#define TAUCS_H
extern "C" { 
    #include "taucs.h"
}
#endif

bool TaucsSolver::solve_symmetry(const TaucsMatrix& matrix, const std::vector<double>& rhs, std::vector<double>& result)
{
	unsigned int num_row = matrix.row_dimension();
	unsigned int num_col = matrix.column_dimension();

	if (num_row != num_col) {
		std::cout << title() << "num_row != num_col" << std::endl;
		return false;
	}
	
	if (num_row != rhs.size()) {
		std::cout << title() << "num_row != rhs.size()" << std::endl;
		return false;
	}

	// A
	taucs_ccs_matrix* A = (taucs_ccs_matrix*)matrix.get_TaucsMatrix();

	// X
	result.resize(num_col);

	// solve
	void* F = NULL;
	char* factor[] = {"taucs.factor.LLT=true", NULL};
	// char* solve[]  = {"taucs.factor=false", NULL};
	void* opt_arg[] = { NULL };

	/* this should work, factor, solve, free */
	int rc = taucs_linsolve(A, &F, 1, &(result[0]), (void*)&(rhs[0]), factor, opt_arg);
	if (rc != TAUCS_SUCCESS)
		std::cout << title() << "solve failed!" << std::endl;

	rc = taucs_linsolve(NULL, &F, 0, NULL, NULL, factor, opt_arg);
	if (rc != TAUCS_SUCCESS)
		std::cout << title() << "free failed!" << std::endl;

	// clean
	//taucs_ccs_free(A); // A will be free by TaucsMatrix

	return (rc == TAUCS_SUCCESS);
}


bool TaucsSolver::solve_unsymmetry(const TaucsMatrix& matrix, const std::vector<double>& rhs, std::vector<double>& result)
{
	unsigned int num_row = matrix.row_dimension();
	unsigned int num_col = matrix.column_dimension();

	if (num_row != num_col) {
		std::cout << title() << "num_row != num_col" << std::endl;
		return false;
	}
	
	if (num_row != rhs.size()) {
		std::cout << title() << "num_row != rhs.size()" << std::endl;
		return false;
	}

	// A
	taucs_ccs_matrix* A = (taucs_ccs_matrix*)matrix.get_TaucsMatrix();

	// X
	result.resize(num_col);

	int*    perm = NULL;
	int*    invperm = NULL;

	// ordering
	taucs_ccs_order(A, &perm, &invperm,	"colamd");
	if ( perm == NULL || invperm == NULL) {
		std::cout << title() << "ordering failed" << std::endl;
		return false;
	}

	taucs_io_handle* LU = taucs_io_create_multifile("taucs.L");
	if (LU == NULL) {
		std::cout << title() << "can not create multifile" << std::endl;
		return false;
	}

	// factorization
	int memory_mb = int(taucs_available_memory_size() / 1048576.0);
	int rc = taucs_ooc_factor_lu(A, perm, LU, memory_mb * 1048576.0);
	if (rc != TAUCS_SUCCESS) {
		std::cout << title() << "factorization failed" << std::endl;
		return false;
	}

	// solve
	rc = taucs_ooc_solve_lu(LU, &(result[0]), (void*)&(rhs[0]));
	if (rc != TAUCS_SUCCESS) {
		std::cout << title() << "solving failed" << std::endl;
		return false;
	}

	// clean
	//taucs_ccs_free(A); // A will be free by TaucsMatrix

	return (rc == TAUCS_SUCCESS);
}


bool TaucsSolver::solve_linear_least_square(const TaucsMatrix& matrix, const std::vector<double>& rhs, std::vector<double>& result)
{
	unsigned int num_row = matrix.row_dimension();
	unsigned int num_col = matrix.column_dimension();
	
	if (num_row < num_col) {
		std::cout << title() << "num_row < num_col" << std::endl;
		return false;
	}

	if (num_row != rhs.size()) {
		std::cout << title() << "num_row != rhs.size()" << std::endl;
		return false;
	}

	// A
	taucs_ccs_matrix* A = (taucs_ccs_matrix*)matrix.get_TaucsMatrix();
	taucs_ccs_matrix* At = TaucsAddOn::MatrixTranspose(A);
	taucs_ccs_matrix* AtA = TaucsAddOn::Mul2NonSymmMatSymmResult(At, A);

	std::vector<double> AtB(num_col);
	TaucsAddOn::MulNonSymmMatrixVector(At, &(rhs[0]), &(AtB[0]));

	// X
	result.resize(num_col);

	// solve
	void* F = NULL;
	char* factor[] = {"taucs.factor.LLT=true", NULL};
	// char* solve[]  = {"taucs.factor=false", NULL};
	void* opt_arg[] = { NULL };

	int rc = taucs_linsolve(AtA, &F, 1, &(result[0]), &(AtB[0]), factor, opt_arg);
	if (rc != TAUCS_SUCCESS)
		std::cout << title() << "solve failed" << std::endl;

	rc = taucs_linsolve(NULL, &F, 0, NULL, NULL, factor, opt_arg);
	if (rc != TAUCS_SUCCESS) 
		std::cout << title() << "free failed" << std::endl;

	// clean
	//taucs_ccs_free(A); // A will be free by TaucsMatrix
	taucs_ccs_free(At);
	taucs_ccs_free(AtA);

	return (rc == TAUCS_SUCCESS);
}
