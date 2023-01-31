struct MoonProgram {
    glimac::Program m_Program;
    GLint           mvp;
    GLint           mv;
    GLint           nm;
    GLint           tex;

    MoonProgram(const glimac::FilePath& applicationPath)
        : m_Program(loadProgram(applicationPath.dirPath() + "TP5/shaders/3D.vs.glsl",
            applicationPath.dirPath() + "TP5/shaders/3D.fs.glsl"))
    {
        mvp = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        mv = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        nm = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        tex = glGetUniformLocation(m_Program.getGLId(), "uTexture");
    }
};