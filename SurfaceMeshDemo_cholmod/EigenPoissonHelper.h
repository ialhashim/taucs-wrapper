#pragma once
#include <omp.h>
#include <iomanip>
#include <QElapsedTimer>

/// Eigen Components
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Eigen/CholmodSupport>
using namespace Eigen;

void solve_linear_least_square(SparseMatrix<double> & A, MatrixXd & B, MatrixXd & X){
    QElapsedTimer timer;
    double tfact, tsolve;
    
    /// Factorize the matrix
    timer.start();
        SparseMatrix<double> At  = A.transpose();
        SparseMatrix<double> AtA = At * A;
        typedef CholmodDecomposition< SparseMatrix<double> > Solver;
        //typedef SimplicialLDLT< SparseMatrix<double> > Solver;
        Solver solver;
        solver.compute(AtA);
    tfact = timer.elapsed()/1000.0;
    
    /// 3x Solves
    timer.restart();
        X.col(0) = solver.solve( At * B.col(0) );
        X.col(1) = solver.solve( At * B.col(1) );
        X.col(2) = solver.solve( At * B.col(2) );
    tsolve = timer.elapsed()/1000.0;
    
    /// Outputs times to stdoutput
    cout << "[CHOLMOD Linear Solver] Size: " 
         << AtA.cols() 
         << " Factorization(s): " << setprecision(3) << tfact 
         << " 3xSolve(s): " << setprecision(3) << tsolve << endl;    
}

/// Surface Mesh components
#include "SurfaceMeshHelper.h"

/// DEBUG LINES
#ifndef TIME_THIS_LIBRARY
    #define TIMER if(0)
#else
    #define TIMER
#endif

/// Linear problem structure:
/// | L | x = |  0  |
/// |---|     |-----|
/// | W |     | W*c |
///
/// In the code it will appear:
/// LHS   x = RHS
///
/// Make sure there is no garbage (pure indexes)
class EigenPoissonHelper : public SurfaceMeshHelper{
private:
    SparseMatrix<double> LHS;
    MatrixXd RHS;
    MatrixXd X;
    int nrows, ncols;

private:
    QElapsedTimer timer;

public:
    EigenPoissonHelper(SurfaceMeshModel* mesh) : SurfaceMeshHelper(mesh){

    }
    ~EigenPoissonHelper(){

    }

public:
    void solve(string eweights, string vconstraint, string vinitial, string vsolution){
        try{
            create_lhs(eweights, vconstraint);
            create_rhs(vconstraint, vinitial);
            solve_factorized(vsolution);
        } catch( std::exception& e ){
            cerr << e.what() << endl;
            exit(-1);
        }
    }

private:
    /// eproperty:     value of weights for L(i,j)
    /// vconstraint:   what's the strength of the initial guess?
    void create_lhs(string eproperty, string vconstraint){
        // qDebug() << "Create LHS #vertices: " << mesh->n_vertices() << "";

        /// Retrieve edge weights
        ScalarEdgeProperty eweight = mesh->get_edge_property<SurfaceMeshTypes::Scalar>(eproperty);
        if(!eweight) throw MissingPropertyException(eproperty);

        /// Instantiate/Allocate
        // TIMER timer.start();
        {
            nrows = 2*mesh->n_vertices();
            ncols = mesh->n_vertices();

            this->RHS = MatrixXd::Zero(nrows, 3);
            this->X = MatrixXd::Zero(ncols, 3);
        }
        // TIMER qDebug() << "Allocation: " << timer.elapsed() << "ms";

        typedef Triplet<double> TripletDouble;
        std::vector< TripletDouble > triplets;
        triplets.reserve(ncols * 6);

        /// - off diagonal
        // qDebug() << "Starting off diagonal..";
        // TIMER timer.start();
        {
            /// Set top half of matrix
            foreach(Edge e, mesh->edges()){
                Vertex v0 = mesh->vertex(e,0);
                Vertex v1 = mesh->vertex(e,1);

                triplets.push_back(TripletDouble(v0.idx(), v1.idx(), eweight[e]));
                triplets.push_back(TripletDouble(v1.idx(), v0.idx(), eweight[e]));
            }

            /// Top half diagonal
            foreach(Vertex v, mesh->vertices()){
                double sum = 0;
                foreach(Halfedge h, mesh->onering_hedges(v))
                    sum += eweight[mesh->edge(h)];

                triplets.push_back(TripletDouble(v.idx(),v.idx(), -sum));
            }

            /// Set bottom half of matrix (constraints)
            ScalarVertexProperty vweight = mesh->get_vertex_property<SurfaceMeshTypes::Scalar>(vconstraint);
            if(!vweight) throw MissingPropertyException(vconstraint);
            foreach(Vertex v, mesh->vertices())
            {
                triplets.push_back(TripletDouble(v.idx() + ncols, v.idx(), vweight[v]));
            }
        }
        
        // Construct from triplets
        SparseMatrix<double> m(nrows,ncols);
        m.setFromTriplets(triplets.begin(), triplets.end());
        LHS = m;
    }

public:
    void print_matrix(){
        cout << LHS;
    }

    /// vinitial:      where do I take the initial value?
    /// vsolution:     where should I store the solution?
    void create_rhs(string vconstraint, string vinitial=VPOINT){

        /// Retrieve & fill RHS (first half is zeros)
        Vector3VertexProperty _vinitial = getVector3VertexProperty(vinitial);
        ScalarVertexProperty _vconstraint = getScalarVertexProperty(vconstraint);

        /// Mesh => constraint vectors
        // TIMER timer.start();
        {
            foreach(Vertex v, mesh->vertices()){
                Vector3 u = _vinitial[v] * _vconstraint[v];
                RHS.row(ncols + v.idx()) = Vector3d(u.x(), u.y(), u.z());
            }
        }
        // TIMER qDebug() << "Build RHS vector: " << timer.elapsed() << "ms";
    }

    void solve_factorized(string vsolution=VPOINT){
        /// Factorize & Solve
        // TIMER timer.start();
        {
            solve_linear_least_square(LHS, RHS, X);
        }
        // TIMER qDebug() << "Factor & Solve: " << timer.elapsed() << "ms";
        
        /// Store solution in mesh property
        // TIMER timer.start();
        {
            Vector3VertexProperty _vsolution = getVector3VertexProperty(vsolution);
            foreach(Vertex v, mesh->vertices()){
                Vector3d p = X.row(v.idx());
                _vsolution[v] = Vector3(p[0], p[1], p[2]);
            }
        }
        // TIMER qDebug() << "Save back: " << timer.elapsed() << "ms";
    }
};
