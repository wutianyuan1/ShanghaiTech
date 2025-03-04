#include "solver.hpp"

float g = 9.8f;
float G = 0;

Solver::Solver(glm::vec3* mesh, size_t meshSize, float totalLen) 
: m_meshPtr(mesh), m_size(meshSize), m_nvert(meshSize*meshSize), m_totalLen(totalLen), fixed(meshSize*meshSize),
  p_prev((float*)m_meshPtr, 3 * m_nvert), p_curr(p_prev), springPos((5 * m_size - 2) * (m_size - 1))
{
    this->build();
    this->buildMatrices();
}

void Solver::buildMatrices()
{
    L.resize(3 * m_nvert, 3 * m_nvert);
    M.resize(3 * m_nvert, 3 * m_nvert);
    J.resize(3 * m_nvert, m_nspring);
    for (auto& spr : m_spring)
        for (int j = 0; j < 3; j++)
        {
            L.insert(3 * spr.first + j, 3 * spr.first + j)   = 1;
            L.insert(3 * spr.second + j, 3 * spr.second + j) = 1;
            L.insert(3 * spr.first + j, 3 * spr.second + j)  = -1;
            L.insert(3 * spr.second + j, 3 * spr.first + j)  = -1;
        }

    
    for (size_t i = 0; i < m_spring.size(); i++)
    {
        Spring spr = m_spring[i];
        for (int j = 0; j < 3; j++)
        {
            J.insert(3 * spr.first + j, 3 * i + j) = 1;
            J.insert(3 * spr.first + j, 3 * i + j) = -1;
        }
    }

    for (size_t i = 0; i < m_nvert; i++)
        for (int j = 0; j < 3; j++)
            M.insert(3 * i + j, 3 * i + j) = m_mass[i];

    SparseMat tmp = M + m_step * m_step * L;
    solverMat.compute(tmp);
}


void Solver::setCubeConstraint()
{
    for (size_t i = m_size/3 - 1; i < 2*m_size/3; ++i)
        for (size_t j = m_size/3 - 1; j < 2*m_size/3; ++j)
        {
            m_fext[3 * (i * m_size + j) + 0] = 0;
            fixed[i * m_size + j] = 1;   
        }      
}

void Solver::setCircleConstraint()
{
    float r = (float) m_size / 4, center = (float) m_size / 2;
    for (size_t i = 0; i < m_size; ++i)
        for (size_t j = 0; j < m_size; ++j)
            if ((i -center) * (i - center) + (j - center) * (j - center) < r * r)
                m_fext[3 * (i * m_size + j) + 0] = -G;     
}


void Solver::animateCircleForce(int t, int nsteps)
{
    if (t == 30)
        this->setCircleConstraint();
    if (t > 50)
        this->reset();
    else
        this->run(nsteps);
}

void Solver::animateCubeCollision(int t, int nsteps)
{
    if (t == 10)
        this->setCubeConstraint();
    if (t > 37)
        this->reset();
    else
        this->run(nsteps);
}

void Solver::reset()
{
    for (size_t i = m_size/3; i < 2*m_size/3; ++i)
        for (size_t j = m_size/3; j < 2*m_size/3; ++j)
        {
            m_fext[3 * (i * m_size + j) + 0] = G;
            fixed[i * m_size + j] = 0;
        }   
}

void Solver::constraintStep()
{
    const int step_c = 30;
    const float tau = 0.15;
    for (int i = 0; i < step_c; i++)
    {
        for (size_t sprIdx = 0; sprIdx < m_nspring; sprIdx++)
        {
            Spring spr = m_spring[sprIdx];
            glm::vec3 posdiff = m_meshPtr[spr.first] - m_meshPtr[spr.second];
            Vector3f pos(reinterpret_cast<float*>(&posdiff));
            
            float curr = pos.norm(), origin = m_restLen[sprIdx];
            float diff = (curr - (1 + tau) * origin) / curr, ratio = fabs(curr - origin) / origin;
            if (ratio < tau) continue;
            float c1 = 0.5, c2 = 0.5;
            if (fixed[spr.first] == 1) { c1 = 0; c2 = 1; }
            if (fixed[spr.second] == 1)
            {
                if (c1 != 0) c1 = 1;
                else c1 = 0;
                c2 = 0.0;
            }
            m_meshPtr[spr.first].x -= diff * pos[0] * c1;
            m_meshPtr[spr.first].y -= diff * pos[1] * c1;   
            m_meshPtr[spr.first].z -= diff * pos[2] * c1;             
            m_meshPtr[spr.second].x += diff * pos[0] * c2;
            m_meshPtr[spr.second].y += diff * pos[1] * c2;   
            m_meshPtr[spr.second].z += diff * pos[2] * c2;                     
        }        
    }
}

void Solver::build()
{
    auto n = m_size;
    float cellLen = m_totalLen / (n - 1);
    int curr = 0;

    m_nspring = (n - 1) * (5 * n - 2);
    m_mass = VectorXf::Ones(m_size * m_size) / (m_size * m_size);
    G = m_mass[0] * g;
    m_stiff = VectorXf::Ones(m_nspring); // k = 1.0 for all springs
    m_restLen = VectorXf(m_nspring);
    m_fext = Vector3f(G, 0, 0).replicate(m_nvert, 1); // gravity
    m_step = 0.01;
    m_spring.reserve(m_nspring);

    for (size_t i = 0; i < m_nvert; i++)
        fixed[i] = 0;
    
    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            if (i != n - 1 && j != n - 1)
            {
                // struct springs
                m_spring[curr] = Spring(i * n + j, (i + 1) * n + j);
                m_restLen[curr++] = cellLen;
                m_spring[curr] = Spring(i * n + j, i * n + j + 1);
                m_restLen[curr++] = cellLen;
                // shear springs
                m_spring[curr] = Spring(i * n + j, (i + 1) * n + j + 1);
                m_restLen[curr++] = 1.4142136 * cellLen;
                m_spring[curr] = Spring((i + 1) * n + j, i * n + j + 1);
                m_restLen[curr++] = 1.4142136 * cellLen;           
                // bend springs
                if (i % 2 == 0 && i < n - 2)
                {
                    m_spring[curr] =  Spring(i * n + j, (i + 2) * n + j);
                    m_restLen[curr++] = 2 * cellLen;
                }
                if (j % 2 == 0 && j < n - 2)
                {
                    m_spring[curr] = Spring(i * n + j, i * n + j + 2);
                    m_restLen[curr++] = 2 * cellLen;
                }
                continue;
            }
            else if (i == n - 1 && j != n - 1) // last row
            {
                m_spring[curr] = Spring(i * n + j, i * n + j + 1);
                m_restLen[curr++] = cellLen;
                if (j % 2 == 0 && j < n - 2)
                {
                    m_spring[curr] = Spring(i * n + j, i * n + j + 2);
                    m_restLen[curr++] = 2 * cellLen;
                }
                continue;
            }
            else if (j == n - 1 && i != n - 1) // last col
            {
                m_spring[curr] = Spring(i * n + j, (i + 1) * n + j);
                m_restLen[curr++] = cellLen;
                if (i % 2 == 0)
                {
                    m_spring[curr] =  Spring(i * n + j, (i + 2) * n + j);
                    m_restLen[curr++] = 2 * cellLen;
                }
                continue;
            }
        }
    }
}


void Solver::step()
{
    for (size_t sprCnt = 0; sprCnt < m_spring.size(); sprCnt++)
    {
        Spring spr = m_spring[sprCnt];
        Vector3f p(p_curr[3*spr.first] - p_curr[3*spr.second], p_curr[3*spr.first + 1] - p_curr[3*spr.second + 1], 
                   p_curr[3*spr.first + 2] - p_curr[3*spr.second + 2]);
        p.normalize();
        for (int i = 0; i < 3; i++)
            springPos[3 * sprCnt + i] = m_restLen[sprCnt] * p[i];
    }
    VectorXf tmp = inertialTerm + (m_step * m_step) * J * springPos + m_step * m_step * m_fext;
    VectorXf tmpCurr = solverMat.solve(tmp);
    for (size_t i = 0; i < m_nvert; i++)
        if (fixed[i] != 1)
        {
            p_curr[3*i + 0] = tmpCurr[3*i + 0];
            p_curr[3*i + 1] = tmpCurr[3*i + 1];
            p_curr[3*i + 2] = tmpCurr[3*i + 2];
        }
}

void Solver::run(int nsteps)
{
   float a = 0.996;
   inertialTerm = M * ((a + 1) * (p_curr) - a * p_prev);
   p_prev = p_curr;
   for (int i = 0; i < nsteps; i++)
   {
        step();
        constraintStep();       
   }
}