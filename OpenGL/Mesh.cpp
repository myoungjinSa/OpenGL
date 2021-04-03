#include "OpenGL.h"
#include "Mesh.h"

Vertex::Vertex()
	: position()
{

}

Vertex::~Vertex() {

}

ColorVertex::ColorVertex() 
	: color()
{

}

ColorVertex::~ColorVertex() {

}

Mesh::Mesh()
	: vertices(nullptr), indices(nullptr),vertexCount(0), vertexArrayId(0), vertexBufferId(0), indexCount(0), indexBufferId(0)
{

}

Mesh::~Mesh() {

}

bool Mesh::Initialize(const OpenGL& gl) {
	return InitializeBuffers(gl);
}

void Mesh::Shutdown(const OpenGL& gl) {
	ShutdownBuffers(gl);
}

void Mesh::Render(const OpenGL& gl) {
	RenderBuffers(gl);
}

bool Mesh::InitializeBuffers(const OpenGL& gl) {
	vertexCount = 3;
	indexCount = 3;
	vertices = new ColorVertex[vertexCount];
	if (!vertices)
		return false;

	indices = new unsigned int[indexCount];
	if (!indices)
		return false;

	//Bottom Left
	vertices[0].position = Vec3f(-1.0f, -1.0f, 0.0f);
	vertices[0].color = Vec3f(0.0f, 1.0f, 0.0f);
 
	//Top Middle
	vertices[1].position = Vec3f(0.0f, 1.0f, 0.0f);
	vertices[1].color = Vec3f(0.0f, 1.0f, 0.0f);

	//Bottom Right
	vertices[2].position = Vec3f(1.0f, -1.0f, 0.0f);
	vertices[2].color = Vec3f(0.0f, 1.0f, 0.0f);

	indices[0] = 0;	// bottom left
	indices[1] = 1; // top Middle
	indices[2] = 2; // bottom right

	//Allocate an OpenGL vertex array object.
	gl.glGenVertexArrays(1, &vertexArrayId);

	//Bind the vertex array object to store all the buffers and vertex attribute we create here.
	gl.glBindVertexArray(vertexArrayId);

	//Generate an ID for the vertex buffer.
	gl.glGenBuffers(1, &vertexBufferId);

	//Bind the vertex buffer and load the vertex(position and color) data into ther vertex buffer.
	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(ColorVertex), vertices, GL_STATIC_DRAW);

	//Enable the two vertex array attribute.
	gl.glEnableVertexAttribArray(0);		//Vertex Position
	gl.glEnableVertexAttribArray(1);		//Vertex color.

	//Specify the location and format of the position portion of the vertex buffer.
	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(ColorVertex), 0);

	//Specify the location and format of the color portion of the vertex buffer.
	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(ColorVertex), (unsigned char*)0 + (3 * sizeof(float)));

	//Generate an ID for the index buffer
	gl.glGenBuffers(1, &indexBufferId);

	//Bind the index buffer and load the index data into it.
	gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);


	//Now that the buffers have been loaded we can release the array data.
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;
	return true;
}

void Mesh::RenderBuffers(const OpenGL& gl) {
	gl.glBindVertexArray(vertexArrayId);

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Mesh::ShutdownBuffers(const OpenGL& gl) {
	//Disable the two vertex array attributes.
	gl.glDisableVertexAttribArray(0);
	gl.glDisableVertexAttribArray(1);
	
	//Release the vertex buffer.
	gl.glBindBuffer(GL_ARRAY_BUFFER, 0);
	gl.glDeleteBuffers(1, &vertexBufferId);

	//Release the index buffer.
	gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	gl.glDeleteBuffers(1, &indexBufferId);

	//Release the vertex array object.
	gl.glBindVertexArray(0);
	gl.glDeleteVertexArrays(1, &vertexArrayId);
}