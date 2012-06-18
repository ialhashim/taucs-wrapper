#pragma once
#include <string>
#include "TaucsMatrix.h"

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
};
