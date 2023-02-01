
struct MoonProgram {
    glimac::Program m_Program;
    GLint           mvp,tex,kd,ks,shin,ldir,lint;
    GLint           mv;
    GLint           nm;
    std::unique_ptr<glimac::Image> luneImg;
    GLuint vao, vbo;
    GLuint texture_map;
    glm::mat4 MVPMoon, MVMoon, NormalMoon;
    glm::vec3 li, ukd, uks;
    float sh;

    MoonProgram(const glimac::FilePath& applicationPath, glm::vec3 pli, glm::vec3 pukd, glm::vec3 puks, float psh)
        : m_Program(loadProgram(applicationPath.dirPath() + "/Rollercoaster/shaders/lune.vs.glsl",
            applicationPath.dirPath() + "/Rollercoaster/shaders/lune.fs.glsl"))
    {
        luneImg = glimac::loadImage(applicationPath.dirPath() + "/Rollercoaster/textures/MoonMap.jpg");

        mvp = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        mv = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        nm = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        tex = glGetUniformLocation(m_Program.getGLId(), "texture_diffuse1");
        kd = glGetUniformLocation(m_Program.getGLId(), "uKd");
        ks = glGetUniformLocation(m_Program.getGLId(), "uKs");
        shin = glGetUniformLocation(m_Program.getGLId(), "uShininess");
        ldir = glGetUniformLocation(m_Program.getGLId(), "uLightDir_vs");
        lint = glGetUniformLocation(m_Program.getGLId(), "uLightIntensity");
        li = pli;
        ukd = pukd;
        uks = puks;
        sh = psh;
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


    void drawSphere(glm::mat4 viewMatrix,glm::mat4 ProjMatrix,glm::vec3 newlumdir,glimac::Sphere &sphere,float time)
    {
        m_Program.use();
        glm::mat4 MVMoon = glm::translate(glm::mat4(1.0f), glm::vec3(0.5, 0.5, -0.5));
        //MVMoon = glm::rotate(MVMoon, glm::radians(time * 4), glm::vec3(0, 1, 0)); // Translation * Rotation
        MVMoon = glm::scale(MVMoon, glm::vec3(0.1, 0.1, 0.1));
        MVMoon = glm::rotate(MVMoon, glm::radians(time * 8), glm::vec3(0, 1, 0));
        MVMoon = viewMatrix * MVMoon;
        MVPMoon = ProjMatrix * MVMoon;
        NormalMoon = glm::transpose(glm::inverse(MVMoon));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_map);
        glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(MVPMoon));
        glUniformMatrix4fv(mv, 1, GL_FALSE, glm::value_ptr(MVMoon));
        glUniformMatrix4fv(nm, 1, GL_FALSE, glm::value_ptr(NormalMoon));
        glUniform1i(tex, 0);
        glUniform3fv(kd, 1, glm::value_ptr(ukd));
        glUniform3fv(ks, 1, glm::value_ptr(uks));
        glUniform3fv(lint, 1, glm::value_ptr(li));
        glUniform3fv(ldir, 1, glm::value_ptr(newlumdir));
        glUniform1f(shin, sh);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }


};
