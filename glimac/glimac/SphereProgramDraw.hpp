
struct SphereProgram {
    glimac::Program m_Program;
    GLint           mvp,tex;
    GLint           mv;
    GLint           nm;
    std::unique_ptr<glimac::Image> luneImg;
    GLuint vao, vbo;
    GLuint texture_map;

    SphereProgram(const glimac::FilePath& applicationPath)
        : m_Program(loadProgram(applicationPath.dirPath() + "/Rollercoaster/shaders/lune.vs.glsl",
            applicationPath.dirPath() + "/Rollercoaster/shaders/lune.fs.glsl"))
    {
        luneImg = glimac::loadImage(applicationPath.dirPath() + "/Rollercoaster/textures/MoonMap.jpg");

        mvp = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        mv = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        nm = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        tex = glGetUniformLocation(m_Program.getGLId(), "texture_diffuse1");
    };

    void bindSphere(glimac::Sphere &sphere)
    {
        glGenTextures(1, &texture_map);
        glBindTexture(GL_TEXTURE_2D, texture_map);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, luneImg->getWidth(), luneImg->getHeight(), 0, GL_RGBA, GL_FLOAT, luneImg->getPixels());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);


        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sphere.getVertexCount() * sizeof(glimac::ShapeVertex), sphere.getDataPointer(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0,
            3, GL_FLOAT, GL_FALSE,
            sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, position)));
        glVertexAttribPointer(1,
            3, GL_FLOAT, GL_FALSE,
            sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, normal)));
        glVertexAttribPointer(2,
            2, GL_FLOAT, GL_FALSE,
            sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, texCoords)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };


};
