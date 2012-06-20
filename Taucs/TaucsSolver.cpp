#include <iostream>
#include "TaucsSolver.h"
#include "TaucsException.h"
#include "taucs_addon.h"
#include <ctime>
typedef unsigned int uint;

// Taucs is a C library
#ifndef TAUCS_H
#define TAUCS_H
extern "C" { 
    #include "taucs.h"
}
#endif

using namespace std;
bool TaucsSolver::solve_symmetry(const TaucsMatrix& matrix, const std::vector<double>& rhs, std::vector<double>& result)
{
	unsigned int num_row = matrix.row_dimension();
	unsigned int num_col = matrix.column_dimension();

	if (num_row != num_col) {
		std::cerr << "num_row != num_col" << std::endl;
		return false;
	}
	
	if (num_row != rhs.size()) {
		std::cerr << "num_row != rhs.size()" << std::endl;
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
		std::cerr << title() << "solve failed!" << std::endl;

	rc = taucs_linsolve(NULL, &F, 0, NULL, NULL, factor, opt_arg);
	if (rc != TAUCS_SUCCESS)
		std::cerr << title() << "free failed!" << std::endl;

	// clean
	//taucs_ccs_free(A); // A will be free by TaucsMatrix

	return (rc == TAUCS_SUCCESS);
}


bool TaucsSolver::solve_unsymmetry(const TaucsMatrix& matrix, const std::vector<double>& rhs, std::vector<double>& result)
{
	unsigned int num_row = matrix.row_dimension();
	unsigned int num_col = matrix.column_dimension();

	if (num_row != num_col) {
		std::cerr << title() << "num_row != num_col" << std::endl;
		return false;
	}
	
	if (num_row != rhs.size()) {
		std::cerr << title() << "num_row != rhs.size()" << std::endl;
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
		std::cerr << title() << "ordering failed" << std::endl;
		return false;
	}

	taucs_io_handle* LU = taucs_io_create_multifile("taucs.L");
	if (LU == NULL) {
		std::cerr << title() << "can not create multifile" << std::endl;
		return false;
	}

	// factorization
	int memory_mb = int(taucs_available_memory_size() / 1048576.0);
	int rc = taucs_ooc_factor_lu(A, perm, LU, memory_mb * 1048576.0);
	if (rc != TAUCS_SUCCESS) {
		std::cerr << title() << "factorization failed" << std::endl;
		return false;
	}

	// solve
	rc = taucs_ooc_solve_lu(LU, &(result[0]), (void*)&(rhs[0]));
	if (rc != TAUCS_SUCCESS) {
		std::cerr << title() << "solving failed" << std::endl;
		return false;
	}

	// clean
	//taucs_ccs_free(A); // A will be free by TaucsMatrix

	return (rc == TAUCS_SUCCESS);
}

/// Solves 3 rhs at the same time
void TaucsSolver::solve_linear_least_square(const TaucsMatrix& matrix, const StdDoubleVectorTriplet& rhs, StdDoubleVectorTriplet& results){
    clock_t start,end;
    double CPS = CLOCKS_PER_SEC; 
    uint num_row = matrix.row_dimension();
	uint num_col = matrix.column_dimension();
	
    /// Checks
	if (num_row < num_col)      throw TaucsException("num_row < num_col");
    if (num_row != rhs.size())  throw TaucsException("num_row != rhs.size()");
        
	/// Compute least squares matrixes
	taucs_ccs_matrix* A = (taucs_ccs_matrix*)matrix.get_TaucsMatrix();
	taucs_ccs_matrix* At = TaucsAddOn::MatrixTranspose(A);
	taucs_ccs_matrix* AtA = TaucsAddOn::Mul2NonSymmMatSymmResult(At, A);
    
    /// Options
    char* factor[] = {"taucs.factor.LLT=true", NULL};
    char* solve [] = {"taucs.factor=false", NULL};
    
    /// Factorize the matrix using LL^t just once
    start = clock();
    void* F = NULL;
    if(1){
        int fact_retval = taucs_linsolve(AtA, &F, 0, NULL, NULL, factor, NULL);
        if(fact_retval != TAUCS_SUCCESS) throw TaucsException("Factorization Failed");
    }
    end = clock();
    double tfact = (end-start)/CPS;
    
    /// Reuse this memory for the three solutions
    start = clock();
        std::vector<double> AtB1(num_col);
        std::vector<double> AtB2(num_col);
        std::vector<double> AtB3(num_col);
        TaucsAddOn::MulNonSymmMatrixVector(At, &(rhs.v1[0]), &(AtB1[0]));
        TaucsAddOn::MulNonSymmMatrixVector(At, &(rhs.v2[0]), &(AtB2[0]));
        TaucsAddOn::MulNonSymmMatrixVector(At, &(rhs.v3[0]), &(AtB3[0]));
    end = clock();    
    // cerr << "Premultiply RHS: " << (end-start)/CPS << "s" << endl;
    
    /// Solve three RHS (space for improvement, see 3rd arg of _linsolve)
    start = clock();
    {
        int solve_retval=0;
        solve_retval |= taucs_linsolve(AtA, &F, 1, &(results.v1[0]), &(AtB1[0]), solve, NULL);
        solve_retval |= taucs_linsolve(AtA, &F, 1, &(results.v2[0]), &(AtB2[0]), solve, NULL);
        solve_retval |= taucs_linsolve(AtA, &F, 1, &(results.v3[0]), &(AtB3[0]), solve, NULL);       
        if (solve_retval != TAUCS_SUCCESS) throw TaucsException("ERROR!!!! LL^t Solve Failed");
    }
    end = clock();
    double tsolve = (end-start)/CPS;

    /// Outputs times
    cout << num_col << ", " << tfact << ", " << tsolve << "\n";

    
    /// I am done.. free up
    start = clock();
    {
        void* F = NULL;
        int free_retval = taucs_linsolve(NULL, &F, 0, NULL, NULL, NULL, NULL);
        if( free_retval != TAUCS_SUCCESS ) throw TaucsException("Cannot free memory");
        taucs_ccs_free(At);
        taucs_ccs_free(AtA);
    }
    end = clock();
    // cerr << "Free: " << (end-start)/CPS << "s" << endl;
}

bool TaucsSolver::solve_linear_least_square(const TaucsMatrix& matrix, const std::vector<double>& rhs, std::vector<double>& result)
{
	unsigned int num_row = matrix.row_dimension();
	unsigned int num_col = matrix.column_dimension();
	
	if (num_row < num_col) {
		std::cerr << title() << "num_row < num_col" << std::endl;
		return false;
	}

	if (num_row != rhs.size()) {
		std::cerr << title() << "num_row != rhs.size()" << std::endl;
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
		std::cerr << title() << "solve failed" << std::endl;

	rc = taucs_linsolve(NULL, &F, 0, NULL, NULL, factor, opt_arg);
	if (rc != TAUCS_SUCCESS) 
		std::cerr << title() << "free failed" << std::endl;

	// clean
	//taucs_ccs_free(A); // A will be free by TaucsMatrix
	taucs_ccs_free(At);
	taucs_ccs_free(AtA);

	return (rc == TAUCS_SUCCESS);
}
