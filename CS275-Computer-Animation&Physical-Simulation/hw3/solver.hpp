#pragma once

#include <eigen3/Eigen/Eigen>
#include "base.hpp"

using Vector3f      = Eigen::Vector3f;
using VectorXf      = Eigen::VectorXf;
using Spring        = std::pair<unsigned int, unsigned int>;
using CholeskyMat   = Eigen::SimplicialLLT<Eigen::SparseMatrix<float> >;
using SparseMat     = Eigen::SparseMatrix<float>;

class Solver
{
public:
    Solver() = delete;
    Solver(glm::vec3* meshPtr, size_t meshSize, float m_totalLen);

    void run(int nsteps);
    void animateCircleForce(int t, int nsteps);
    void animateCubeCollision(int t, int nsteps);

private:
    void build();
    void buildMatrices();

    void constraintStep();
    void step();
    void reset();

    void setCubeConstraint();
    void setCircleConstraint();

private:
    glm::vec3*  m_meshPtr;
    size_t      m_size;
    size_t      m_nvert;
    size_t      m_nspring;
    float       m_step;
    float       m_totalLen;
    VectorXf    m_stiff;
    VectorXf    m_mass;
    VectorXf    m_restLen;
    VectorXf    m_fext;
    std::vector<Spring> m_spring;
    std::vector<int>    fixed;
    
private:
    Eigen::Map<Eigen::VectorXf>     p_prev;
    VectorXf                        p_curr;
    VectorXf                        inertialTerm;
    VectorXf                        springPos;
    SparseMat                       M;
    SparseMat                       L;
    SparseMat                       J; 
    CholeskyMat                     solverMat;
};

