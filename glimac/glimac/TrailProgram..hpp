struct EarthProgram {
    glimac::Program m_Program;

    GLint mvp;
    GLint mv;
    GLint nm;
    GLint tex;

    EarthProgram(const glimac::FilePath& applicationPath)
        : m_Program(loadProgram(applicationPath.dirPath() + "Rollercoaster/shaders/3D.vs.glsl",
            applicationPath.dirPath() + "Rollercoaster/shaders/multiTex3D.fs.glsl"))
    {
        mvp = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        mv = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        nm = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        tex = glGetUniformLocation(m_Program.getGLId(), "uTexture");
    }

    void renderSpline(GLFWwindow* window, glm::mat4 viewMatrix, glm::mat4 ProjMatrix, GLuint mvp, GLuint mv, GLuint nm,
        GLuint kd, GLuint ks, GLuint lint, GLuint ldir, GLuint shin, std::vector<glm::vec3> trail, std::vector<uint32_t>indexes)
    {



        GLuint vbo, ibo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, trail.size() * sizeof(glm::vec3), trail.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &ibo);

        //// => We bind on GL_ELEMENT_ARRAY_BUFFER, target reserved for IBOs
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(uint32_t), indexes.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0,
            3, GL_FLOAT, GL_FALSE,
            sizeof(GLfloat) * 3, (const GLvoid*)0);
        glVertexAttribPointer(1,
            3, GL_FLOAT, GL_FALSE,
            sizeof(GL_FLOAT), (const GLvoid*)(3 * sizeof(GLfloat)));



        glm::mat4 MVMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));
        //MVMatrix = glm::scale(MVMatrix, glm::vec3(2, 2, 2));
        MVMatrix = glm::rotate(MVMatrix, glm::radians(-90.f), glm::vec3(1, 0, 0));
        MVMatrix = glm::rotate(MVMatrix, glm::radians(-90.f), glm::vec3(0, 0, 1));
        MVMatrix = viewMatrix * MVMatrix;


        glm::mat4 MVPMatrix = ProjMatrix * MVMatrix;
        glm::vec3 newlumdir = glm::vec3(viewMatrix * glm::vec4(1));
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
        glUniformMatrix4fv(mv, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(nm, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glUniform3fv(kd, 1, glm::value_ptr(ukd));
        glUniform3fv(ks, 1, glm::value_ptr(uks));
        glUniform3fv(lint, 1, glm::value_ptr(li));
        glUniform3fv(ldir, 1, glm::value_ptr(glm::vec3(newlumdir)));
        glUniform1f(shin, sh);




        glDrawArrays(GL_QUADS, 0, (int)trail.size());

        //glDrawElements(GL_TRIANGLES,(GLsizei) indexes.size(), GL_UNSIGNED_INT, 0);

        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
};