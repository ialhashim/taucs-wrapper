#pragma once
#include <string>
#include "TaucsMatrix.h"


/// Dirty little trick...
struct StdDoubleVectorTriplet{
    typedef unsigned int uint;
    std::vector<double> v1,v2,v3;
    uint size() const{ return v1.size(); }
    StdDoubleVectorTriplet(unsigned int size){
        v1.resize(size,0);
        v2.resize(size,0);
        v3.resize(size,0);
    }
};

class TaucsSolver{
public:
	static std::string title() { return "[TaucsSolver]: "; }

	// solve for "A*x=b"
	// A: the coefficient symmetry matrix, 
	// b: the right side column vector
	// x: the result
	static bool solve_symmetry(const TaucsMatrix& A, const std::vector<double>& b, std::vector<double>& x);
	
	
	// solve for "A*x=b"
	// A: the coefficient unymmetry matrix, 
	// b: the right side column vector
	// x: the result
	static bool solve_unsymmetry(const TaucsMatrix& A, const std::vector<double>& b, std::vector<double>& x);
  

	// solve for "A*x=b" in least square sence
	// A: the coefficient m * n matrix (m >= n)
	// b: the right side column vector
	// x: the result
    static bool solve_linear_least_square(const TaucsMatrix& A, const std::vector<double>& b, std::vector<double>& x);

    // solve for "A*x=b" in least square sense  
    static void solve_linear_least_square(const TaucsMatrix &A, const StdDoubleVectorTriplet &x, StdDoubleVectorTriplet &b);
};
