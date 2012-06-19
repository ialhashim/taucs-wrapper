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
  
    bool isSymmetry = false;
    TaucsMatrix M(10, 5, isSymmetry);    
    M.set_coef(3, 0, 0.689);
    M.set_coef(4, 0, 0.084);
    M.set_coef(6, 0, 0.826);
    M.set_coef(7, 0, 0.443);
    M.set_coef(9, 0, 0.869);
    M.set_coef(0, 1, 0.162);
    M.set_coef(3, 1, 0.748);
    M.set_coef(5, 1, 0.152);
    M.set_coef(7, 1, 0.107);
    M.set_coef(8, 1, 0.775);
    M.set_coef(9, 1, 0.084);
    M.set_coef(0, 2, 0.794);
    M.set_coef(1, 2, 0.529);
    M.set_coef(2, 2, 0.263);
    M.set_coef(4, 2, 0.229);
    M.set_coef(6, 2, 0.538);
    M.set_coef(7, 2, 0.962);
    M.set_coef(9, 2, 0.400);
    M.set_coef(1, 3, 0.166);
    M.set_coef(3, 3, 0.451);
    M.set_coef(4, 3, 0.913);
    M.set_coef(6, 3, 0.996);
    M.set_coef(7, 3, 0.005);
    M.set_coef(9, 3, 0.260);
    M.set_coef(0, 4, 0.311);
    M.set_coef(1, 4, 0.602);
    M.set_coef(2, 4, 0.654);
    M.set_coef(6, 4, 0.078);
    M.set_coef(8, 4, 0.817);
    
	std::vector<double> b(10);
    b[0] = 0.875;
    b[1] = 0.670;
    b[2] = 0.412;
    b[3] = 0.956;
    b[4] = 0.442;
    b[5] = 0.066;
    b[6] = 1.353;
    b[7] = 1.277;
    b[8] = 0.550;
    b[9] = 1.143;
    
	std::vector<double> x(5);
	bool success = TaucsSolver::solve_linear_least_square(M,b,x);
    
    if (success) {
		cout << "output for non-symmetric test: " << std::endl;
		for (unsigned int i=0; i<5; ++i)
			cout << x[i] << "  " ;
		cout << std::endl;
	} else {
        printf("SOLVER FAIL!!!");
    }
	return success;
}
