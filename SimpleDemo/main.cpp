#include <Taucs/TaucsMatrix.h>
#include <Taucs/TaucsSolver.h>

#include <iostream>
using namespace std;

int main(int, char**){
    //		10     0    0   -2   -1   -1
    //		 0    10    0    0   -2    0
    //		 0     0   15    0    0    0
    //		-2     0    0   10   -1    0
    //		-1    -2    0   -1    1    0
    //		-1     0    0    0    0    6
	//	b = (-9, 10, 45, 33, −4, 35)T
  
    bool isSymmetry = true;
    
	const unsigned int n = 6;
	TaucsMatrix M(n, n, isSymmetry);
    if(isSymmetry){
        M.set_coef(0, 0, 10.0);
        M.set_coef(3, 0, -2.0);
        M.set_coef(4, 0, -1.0);
        M.set_coef(5, 0, -1.0);
        M.set_coef(1, 1, 10.0);
        M.set_coef(4, 1, -2.0);
        M.set_coef(2, 2, 15.0);
        M.set_coef(3, 3, 10.0);
        M.set_coef(4, 3, -1.0);
        M.set_coef(4, 4, 1.0);
        M.set_coef(5, 5,  6.0);
    } else {
        
    }
    
	std::vector<double> b(n);
	b[0] = -9;
	b[1] = 10;
	b[2] = 45; 
	b[3] = 33;  
	b[4] = -4;  
	b[5] = 35;
    
	std::vector<double> x(n);
	bool success = (isSymmetry) ?
        TaucsSolver::solve_symmetry(M, b, x): 
        TaucsSolver::solve_unsymmetry(M,b,x);
    
    if (success) {
		cout << "output for symmetry test: " << std::endl;
		for (unsigned int i=0; i<n; ++i)
			cout << x[i] << "  " ;
		cout << std::endl;
	} else {
        printf("SOLVER FAIL!!!");
    }
	return success;
}
