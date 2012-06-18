#pragma once
// This codes is copied and modified a little from CGAL/TaucsMatrix.h

/// taucs.h was defining a max preprocessor that collided with std::max 
#ifdef max
    #undef max
#endif
#include <vector>
#include <cassert>

// Forward declaration
template<class T> struct Taucs_number;
extern "C" { 
    struct taucs_ccs_matrix;
}

/// The class TaucsMatrix is a C++ wrapper around TAUCS' matrix type taucs_ccs_matrix.
/// This kind of matrix can be either symmetric or not. 
/// @note Symmetric matrices store only the lower triangle.
typedef double Scalar;
class TaucsMatrix{
    
	// Public types
public:
	typedef Scalar NT;

	// Private types
private:
	/*
	* A column of a TaucsMatrix. The column is compressed, and stored in the form of
	* (a vector of values) + (a vector of indices).
	*/
	class Column
	{
	public:

		// (Vector of values) + (vector of indices) (linked)
		std::vector<Scalar>   m_values;
		std::vector<int>      m_indices;

	public:

		// Return the number of elements in the column
		int dimension() const    { return static_cast<int>(m_values.size()); }

		// column{index} <- column{index} + val
		void add_coef(int index, Scalar val)
		{
			// Search for element in vectors
			std::vector<int>::iterator      index_it;
			std::vector<Scalar>::iterator   value_it;
			for (index_it = m_indices.begin(), value_it = m_values.begin();
				index_it != m_indices.end();
				index_it++, value_it++)
			{
				if(*index_it == index) {
					*value_it += val;       // +=
					return;
				}
			}

			// Element doesn't exist yet if we reach this point
			m_indices.push_back(index);
			m_values.push_back(val);
		}

		// column{index} <- val
		void set_coef(int index, Scalar val)
		{
			// Search for element in vectors
			std::vector<int>::iterator          index_it;
			std::vector<Scalar>::iterator   value_it;
			for (index_it = m_indices.begin(), value_it = m_values.begin();
				index_it != m_indices.end();
				index_it++, value_it++)
			{
				if(*index_it == index) {
					*value_it = val;        // =
					return;
				}
			}

			// Element doesn't exist yet if we reach this point
			m_indices.push_back(index);
			m_values.push_back(val);
		}

		// return column{index} (0 by default)
		Scalar get_coef(int index) const
		{
			// Search for element in vectors
			std::vector<int>::const_iterator        index_it;
			std::vector<Scalar>::const_iterator value_it;
			for (index_it = m_indices.begin(), value_it = m_values.begin();
				index_it != m_indices.end();
				index_it++, value_it++)
			{
				if(*index_it == index)
					return *value_it;       // return value
			}

			// Element doesn't exist yet if we reach this point
			return 0;
		}
	}; // class Column


public:

	/// Create a square matrix initialized with zeros.
    /// @param Matrix dimension (square)
    /// @param Symmetric/hermitian
	TaucsMatrix(int  dim, bool is_symmetric = false);

	/// Create a rectangular matrix initialized with zeros.
    /// @param Matrix #rows
    /// @param Matrix #cols
    /// @param Symmetric/hermitian
	TaucsMatrix(int  rows, int  columns, bool is_symmetric = false);

	/// Delete this object and the wrapped TAUCS matrix.
	~TaucsMatrix();

	/// Return the matrix number of rows
	int row_dimension() const    { return m_row_dimension; }
	/// Return the matrix number of columns
	int column_dimension() const { return m_column_dimension; }

	/// Read access to a matrix coefficient.
	///
	/// Preconditions:
	/// - 0 <= i < row_dimension().
	/// - 0 <= j < column_dimension().
	Scalar get_coef(int i, int j) const
	{
		// For symmetric matrices, we store only the lower triangle
		// => swap i and j if (i, j) belongs to the upper triangle
		if (m_is_symmetric && (j > i))
			std::swap(i, j);

		assert(i < m_row_dimension);
		assert(j < m_column_dimension);
		return m_columns[j].get_coef(i);
	}

	/// Write access to a matrix coefficient: a_ij <- val.
	///
	/// Optimization:
	/// For symmetric matrices, TaucsMatrix stores only the lower triangle
	/// set_coef() does nothing if (i, j) belongs to the upper triangle.
	///
	/// Preconditions:
	/// - 0 <= i < row_dimension().
	/// - 0 <= j < column_dimension().
	void set_coef(int i, int j, Scalar val)
	{
		if (m_is_symmetric && (j > i))
			return;

		assert(i < m_row_dimension);
		assert(j < m_column_dimension);
		m_columns[j].set_coef(i, val);
	}

	/// Write access to a matrix coefficient: a_ij <- a_ij + val.
	///
	/// Optimization:
	/// For symmetric matrices, TaucsMatrix stores only the lower triangle
	/// add_coef() does nothing if (i, j) belongs to the upper triangle.
	///
	/// Preconditions:
	/// - 0 <= i < row_dimension().
	/// - 0 <= j < column_dimension().
	void add_coef(int i, int j, Scalar val)
	{
		if (m_is_symmetric && (j > i))
			return;

		assert(i < m_row_dimension);
		assert(j < m_column_dimension);
		m_columns[j].add_coef(i, val);
	}

	/// Construct and return the TAUCS matrix wrapped by this object.
	/// Note: the TAUCS matrix returned by this method is valid
	///       only until the next call to set_coef(), add_coef() or get_TaucsMatrix().
	const taucs_ccs_matrix* get_TaucsMatrix() const;

private:

	/// TaucsMatrix cannot be copied (yet)
	TaucsMatrix(const TaucsMatrix& rhs);
	TaucsMatrix& operator=(const TaucsMatrix& rhs);

	// Fields
private:

	// Matrix dimensions
	int     m_row_dimension, m_column_dimension;

	// Columns array
	Column* m_columns;

	// Symmetric/hermitian?
	bool    m_is_symmetric;

	/// The actual TAUCS matrix wrapped by this object.
	// This is in fact a COPY of the columns array
	mutable taucs_ccs_matrix* m_matrix;

}; // TaucsMatrix


/// The class Taucs_symmetric_matrix is a C++ wrapper
/// around a TAUCS *symmetric* matrix (type taucs_ccs_matrix).
/// Symmetric matrices store only the lower triangle.
/// Concept: Model of the SparseLinearAlgebraTraits_d::Matrix concept.
struct Taucs_symmetric_matrix : public TaucsMatrix{
	// Public types
public:
	typedef Scalar NT;

	// Public operations
public:

	/// Create a square SYMMETRIC matrix initialized with zeros.
	/// The max number of non 0 elements in the matrix is automatically computed.
	Taucs_symmetric_matrix(int  dim) : TaucsMatrix(dim, true){}

	/// Create a square SYMMETRIC matrix initialized with zeros.
	Taucs_symmetric_matrix(int  rows, int  columns)
		///< matrix (automatically computed if 0).
		: TaucsMatrix(rows, columns, true){}
};

#if 0
// Utility class to TaucsMatrix
// Convert matrix's T type to the corresponding TAUCS constant (called TAUCS_FLAG)
template<class T> struct Taucs_number {};
template<> struct Taucs_number<double> { enum { TAUCS_FLAG = TAUCS_DOUBLE }; };
template<> struct Taucs_number<float>  { enum { TAUCS_FLAG = TAUCS_SINGLE }; };
#endif
