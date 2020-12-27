#include "cube.hpp"

const float vs[36][3] = {
    -5.0f, -5.0f, -5.0f,     // front 
    5.0f, -5.0f, -5.0f,
    5.0f,  5.0f, -5.0f,
    5.0f,  5.0f, -5.0f,
    -5.0f,  5.0f, -5.0f,
    -5.0f, -5.0f, -5.0f,

    -5.0f, -5.0f,  5.0f,    // back
    5.0f, -5.0f,  5.0f,
    5.0f,  5.0f,  5.0f,
    5.0f,  5.0f,  5.0f,
    -5.0f,  5.0f,  5.0f,
    -5.0f, -5.0f,  5.0f,

    -5.0f,  5.0f,  5.0f,   // left
    -5.0f,  5.0f, -5.0f,
    -5.0f, -5.0f, -5.0f,
    -5.0f, -5.0f, -5.0f,
    -5.0f, -5.0f,  5.0f,
    -5.0f,  5.0f,  5.0f,

    5.0f,  5.0f,  5.0f,  // right
    5.0f,  5.0f, -5.0f,
    5.0f, -5.0f, -5.0f,
    5.0f, -5.0f, -5.0f,
    5.0f, -5.0f,  5.0f,
    5.0f,  5.0f,  5.0f,

    -5.0f, -5.0f, -5.0f,  // bottom
    5.0f, -5.0f, -5.0f,
    5.0f, -5.0f,  5.0f,
    5.0f, -5.0f,  5.0f,
    -5.0f, -5.0f,  5.0f,
    -5.0f, -5.0f, -5.0f,

    -5.0f,  5.0f, -5.0f,  // top
    5.0f,  5.0f, -5.0f,
    5.0f,  5.0f,  5.0f,
    5.0f,  5.0f,  5.0f,
    -5.0f,  5.0f,  5.0f,
    -5.0f,  5.0f, -5.0f,
};

Cube::Cube(int mode)
: vertices(36), _mode(mode)
{
    std::vector<vec3f> tmpVerts;
    if (_mode == MODE_LINE)
    {

    }
    else
    {
        for (int i = 0; i < 36; i++)
        {
            vertices[i].pos.x = vs[i][0] + 25;
            vertices[i].pos.y = vs[i][1] + 25;
            vertices[i].pos.z = vs[i][2] + 25;
            vertices[i].color.x = 1.0f;
        }
        for (int i = 0; i < 6; i++)
            vertices[i].normal = vec3f(0, 0, -1);
        for (int i = 6; i < 12; i++)
            vertices[i].normal = vec3f(0, 0, 1);
        for (int i = 12; i < 18; i++)
            vertices[i].normal = vec3f(-1, 0, 0);
        for (int i = 18; i < 24; i++)
            vertices[i].normal = vec3f(1, 0, 0);
        for (int i = 24; i < 30; i++)
            vertices[i].normal = vec3f(0, -1, 0);
        for (int i = 30; i < 36; i++)
            vertices[i].normal = vec3f(0, 1, 0);
    }
        
    
	unsigned long long vertexSize = sizeof(CubeVertex);
	unsigned long long vec3Size = sizeof(glm::vec3);

	glGenVertexArrays(1, &vaoId);
	glGenBuffers(1, &vboId);

	glBindVertexArray(vaoId);

	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * vertexSize, vertices.data(), GL_STATIC_DRAW);

	for (int i = 0; i < 3; i++)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)(i * vec3Size));
	}
}

Cube::~Cube()
{
	glDeleteVertexArrays(1, &vaoId);
	glDeleteBuffers(1, &vboId);
}

void Cube::render()
{
	glBindVertexArray(vaoId);
    if (_mode == MODE_LINE)
	    glDrawArrays(GL_LINES, 0, GLsizei(vertices.size()));
    else
        glDrawArrays(GL_TRIANGLES, 0, 36);
}