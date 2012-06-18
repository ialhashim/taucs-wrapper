#include "TaucsMatrix.h"

// Taucs is a C library
extern "C" {
   #include "taucs.h"
}

const taucs_ccs_matrix* TaucsMatrix::get_TaucsMatrix() const{
    if (m_matrix != NULL) {
        taucs_ccs_free(m_matrix);
        m_matrix = NULL;
    }

    // Convert matrix's T type to the corresponding TAUCS constant
    int flags = TAUCS_DOUBLE;

    // We store only the lower triangle of symmetric matrices
    if (m_is_symmetric)
        flags |= TAUCS_TRIANGULAR | TAUCS_SYMMETRIC | TAUCS_LOWER;

    // Compute the number of non null elements in the matrix
    int nb_max_elements = 0;
    for (int col=0; col < m_column_dimension; col++)
        nb_max_elements += m_columns[col].dimension();

    // Create the TAUCS matrix wrapped by this object
    m_matrix = taucs_ccs_create(m_row_dimension, m_column_dimension, nb_max_elements, flags);

    // Fill m_matrix's colptr[], rowind[] and values[] arrays
    // Implementation note:
    // - rowind[] = array of non null elements of the matrix, ordered by columns
    // - values[] = array of row index of each element of rowind[]
    // - colptr[j] is the index of the first element of the column j (or where it
    //   should be if it doesn't exist) + the past-the-end index of the last column
    m_matrix->colptr[0] = 0;
    for (int col=0; col < m_column_dimension; col++)
    {
        // Number of non null elements of the column
        int nb_elements = m_columns[col].dimension();

        // Fast copy of column indices and values
        memcpy(&m_matrix->rowind[m_matrix->colptr[col]], &m_columns[col].m_indices[0], nb_elements*sizeof(int));
        Scalar* taucs_values = (Scalar*) m_matrix->values.v;
        memcpy(&taucs_values[m_matrix->colptr[col]], &m_columns[col].m_values[0],  nb_elements*sizeof(Scalar));

        // Start of next column will be:
        m_matrix->colptr[col+1] = m_matrix->colptr[col] + nb_elements;
    }

    return m_matrix;
}

TaucsMatrix::TaucsMatrix(int  dim, bool is_symmetric){
    assert(dim > 0);
    m_row_dimension     = dim;
    m_column_dimension  = dim;
    m_columns           = new Column[m_column_dimension];
    m_is_symmetric      = is_symmetric;
    m_matrix            = NULL;
}

TaucsMatrix::~TaucsMatrix()
{
    // Delete the columns array
    delete[] m_columns;
    m_columns = NULL;

    // Delete the wrapped TAUCS matrix
    if (m_matrix != NULL) {
        taucs_ccs_free(m_matrix);
        m_matrix = NULL;
    }
}

TaucsMatrix::TaucsMatrix(int  rows, int  columns, bool is_symmetric){
    assert(rows > 0);
    assert(columns > 0);
    if (is_symmetric) {
        assert(rows == columns);
    }

    m_row_dimension     = rows;
    m_column_dimension  = columns;
    m_columns           = new Column[m_column_dimension];
    m_is_symmetric      = is_symmetric;
    m_matrix            = NULL;
}
