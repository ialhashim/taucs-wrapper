#pragma once
#include <iomanip>
#include <QElapsedTimer>

/// Taucs Components
#include <Taucs/TaucsMatrix.h>
#include <Taucs/TaucsSolver.h>

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
class TaucsPoissonHelper : public SurfaceMeshHelper{
private: 
    TaucsMatrix*             LHS;
    StdDoubleVectorTriplet*  RHS;
    StdDoubleVectorTriplet*    X;
    unsigned int           nrows;
    unsigned int           ncols;
    
private:
    QElapsedTimer timer;
    
public:
    TaucsPoissonHelper(SurfaceMeshModel* mesh) : SurfaceMeshHelper(mesh){
        this->LHS = NULL;
        this->RHS = NULL;
        this->X = NULL;
        nrows = ncols = 0;
    }
    ~TaucsPoissonHelper(){ 
        if(LHS) delete LHS; 
        if(RHS) delete RHS;
        if(X) delete X;
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
        qDebug() << "#vertices: " << mesh->n_vertices() << "";
        
        /// Retrieve edge weights
        ScalarEdgeProperty eweight = mesh->get_edge_property<Scalar>(eproperty);
        if(!eweight) throw MissingPropertyException(eproperty);
        
        /// Instantiate/Allocate
        TIMER timer.start();
        {
            nrows = 2*mesh->n_vertices();
            ncols = mesh->n_vertices();
            this->LHS = new TaucsMatrix(nrows, ncols,false);
            this->RHS = new StdDoubleVectorTriplet(nrows);
            this->X   = new StdDoubleVectorTriplet(ncols);
        }
        TIMER qDebug() << "Allocation: " << timer.elapsed() << "ms";
        
        /// - off diagonal
        TIMER timer.start();
        {
            /// Set top half of matrix        
            foreach(Edge e, mesh->edges()){
                Vertex v0 = mesh->vertex(e,0);
                Vertex v1 = mesh->vertex(e,1);
                LHS->set_coef(v0.idx(),v1.idx(),eweight[e]);
                LHS->set_coef(v1.idx(),v0.idx(),eweight[e]);
                // cout << "Setting: " << v0.idx() << " " << v1.idx() << endl;
            }
            /// Top half diagonal
            foreach(Vertex v, mesh->vertices()){
                double sum = 0;
                foreach(Halfedge h, mesh->onering_hedges(v))
                    sum += eweight[mesh->edge(h)]; 
                LHS->set_coef(v.idx(),v.idx(),-sum);
            }
            /// Set bottom half of matrix (constraints)
            ScalarVertexProperty vweight = mesh->get_vertex_property<Scalar>(vconstraint);
            if(!vweight) throw MissingPropertyException(vconstraint);
            foreach(Vertex v, mesh->vertices())
                LHS->set_coef(v.idx()+ncols, v.idx(), vweight[v]);        
        }
        TIMER qDebug() << "LHS Build time: " << timer.elapsed() << "ms";
    }

public:
    void print_matrix(){
        cout << "M: [" << LHS->column_dimension() << "x" << LHS->row_dimension() << "]" << endl;
        for(int irow=0; irow<LHS->row_dimension(); irow++){
            /// Print row of M
            cout << "| ";
            for(int icol=0; icol<LHS->column_dimension(); icol++)
                cout << setw(6) << setprecision(2) << LHS->get_coef(irow,icol) << " ";
            cout << "|";
            
            /// X or space
            cout << ((irow==0) ? " x=  ":"     ");

            /// Print row of RHS
            cout << "|";
            cout << setw(6) << setprecision(2) << RHS->v1[irow];
            cout << setw(6) << setprecision(2) << RHS->v2[irow];
            cout << setw(6) << setprecision(2) << RHS->v3[irow];
            cout << "|";
            cout << endl;
        }        
    }
    
    /// vinitial:      where do I take the initial value?
    /// vsolution:     where should I store the solution?
    void create_rhs(string vconstraint, string vinitial=VPOINT){     
        Q_ASSERT(RHS!=NULL && LHS!=NULL && X!=NULL);
        
        /// Retrieve & fill RHS (first half is zeros)
        Vector3VertexProperty  _vinitial = getVector3VertexProperty(vinitial);
        ScalarVertexProperty _vconstraint = getScalarVertexProperty(vconstraint);
        
        /// Mesh => constraint vectors
        TIMER timer.start();
        {
            foreach(Vertex v, mesh->vertices()){
                RHS->v1[ncols + v.idx()] = _vinitial[v].x()*_vconstraint[v];
                RHS->v2[ncols + v.idx()] = _vinitial[v].y()*_vconstraint[v];
                RHS->v3[ncols + v.idx()] = _vinitial[v].z()*_vconstraint[v];
            }
        }
        TIMER qDebug() << "Build RHS vector: " << timer.elapsed() << "ms";
    }   

    void solve_factorized(string vsolution=VPOINT){
        /// Factorize & Solve
        TIMER timer.start();
        {
            TaucsSolver::solve_linear_least_square(*LHS,*RHS,*X);
        }
        TIMER qDebug() << "Factor & Solve: " << timer.elapsed() << "ms";
        
        
        /// Store solution in mesh property
        TIMER timer.start();
        {
            Vector3VertexProperty _vsolution = getVector3VertexProperty(vsolution);
            foreach(Vertex v, mesh->vertices()){
                _vsolution[v].x() = X->v1[v.idx()];
                _vsolution[v].y() = X->v2[v.idx()];
                _vsolution[v].z() = X->v3[v.idx()];
            }
        }
        TIMER qDebug() << "Save back: " << timer.elapsed() << "ms";        
    }
};
