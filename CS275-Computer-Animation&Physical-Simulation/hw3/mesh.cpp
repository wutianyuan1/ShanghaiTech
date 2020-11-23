#include "mesh.hpp"

#ifdef SHOW_MESHGRID
static void lightDisable()
{
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}

static void lightEnable()
{
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
}
#endif

Mesh::Mesh() : m_vbuff(nullptr), m_meshsize(0)
{}

Mesh::Mesh(glm::vec3* vertices, size_t n) : m_vbuff(vertices), m_meshsize(n)
{}

void Mesh::cleanUp()
{
    delete[] m_vbuff;
}

glm::vec3* Mesh::getVertices()
{
    return m_vbuff;
}

glm::vec3* Mesh::NewGrid(size_t n, GLfloat edgeLen)
{
    glm::vec3* grid = new glm::vec3[n * n];
    GLfloat step = edgeLen / (n - 1);
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
        {
            grid[i*n + j].x = 0;
            grid[i*n + j].y = i * step;
            grid[i*n + j].z = j * step;
        }
    return grid; 
}

// Render the mesh
void Mesh::draw() 
{
    size_t n = m_meshsize, idx = 0;
    std::vector<Triangle> triangles((n - 1) * (n - 1) * 2);
    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            if (i < n - 1 && j < n - 1)
                triangles[idx++] = Triangle(m_vbuff[i * n + j], \
                                            m_vbuff[(i + 1) * n + j], \
                                            m_vbuff[i * n + j + 1]);
            if (i > 0 && j > 0) 
                triangles[idx++] = Triangle(m_vbuff[i * n + j], \
                                            m_vbuff[(i - 1) * n + j], \
                                            m_vbuff[i * n + j - 1]);
        }
    }
    for (size_t i = 0; i < triangles.size(); i++)
    {
        // std::cout << triangles[i] << std::endl;
        glBegin(GL_TRIANGLES);
            glNormal3fv( (const GLfloat *)(&(triangles[i].normal)) );
            glVertex3fv( (const GLfloat *)(&(triangles[i].p1_)) );
            glVertex3fv( (const GLfloat *)(&(triangles[i].p2_)) );
            glVertex3fv( (const GLfloat *)(&(triangles[i].p3_)) );
        glEnd();

#ifdef SHOW_MESHGRID
        lightDisable();
            glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
            glBegin(GL_LINES);
                glVertex3fv( (const GLfloat *)(&(triangles[i].p1_)) );
                glVertex3fv( (const GLfloat *)(&(triangles[i].p2_)) );
                glVertex3fv( (const GLfloat *)(&(triangles[i].p1_)) );
                glVertex3fv( (const GLfloat *)(&(triangles[i].p3_)) );
                glVertex3fv( (const GLfloat *)(&(triangles[i].p2_)) );
                glVertex3fv( (const GLfloat *)(&(triangles[i].p3_)) );
            glEnd();
        lightEnable();
#endif
    } 
    // exit(0);
}



