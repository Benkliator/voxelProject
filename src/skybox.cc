#include "../lib/skybox.h"

Skybox::Skybox(std::vector<const char*> texturePath)
{
   float vertices[]
    {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f
    };

    unsigned int indices[]
    {
        0, 2, 1, // Front
        0, 3, 2,
        4, 5, 6, // Back
        4, 6, 7,
        0, 4, 7, // Left
        0, 7, 3,
        1, 6, 5, // Right
        1, 2, 6,
        3, 6, 2, // Top
        3, 7, 6,
        0, 1, 5, // Bottom
        0, 5, 4,
    };
    unsigned int vertexShader = loadShader(GL_VERTEX_SHADER, "./res/shaders/skybox.vert");
    unsigned int fragmentShader = loadShader(GL_FRAGMENT_SHADER, "./res/shaders/skybox.frag");    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    texture = loadTextureCube(texturePath, GL_RGB);

    // Generate buffers
    glGenVertexArrays(1, &vertexArrayObjID);
    glGenBuffers(1, &vertexBufferObjID);
    glGenBuffers(1, &elementBufferObjID);

    // Init drawing
    glBindVertexArray(vertexArrayObjID);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(unsigned int), (void*)(0));
    glEnableVertexAttribArray(0);
}

void Skybox::draw(glm::mat4 view)
{
    glUseProgram(shaderProgram);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glBindVertexArray(vertexArrayObjID);
     
    glDepthMask(false);
    glCullFace(GL_FRONT);
    glm::mat4 scaling = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 0.001f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.8f, 0.1f, 1000.0f);
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "scaling"), 1, GL_FALSE, glm::value_ptr(scaling));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjID);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    glCullFace(GL_BACK);
    glDepthMask(true);
}
