#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/FilePath.hpp>
#include <glimac/FreeflyCamera.hpp>
#include <glimac/Image.hpp>
#include <glimac/Program.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/common.hpp>
#include <glimac/glm.hpp>
#include <glimac/MoonProgramDraw.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glimac/model.hpp>
#include <glimac/mesh.hpp>
#include <glimac/spline.hpp>


int           window_width  = 1280;

int           window_height = 720;
float         prevxpos;
float         prevypos;
bool          previous = true;

glm::vec3 li(1, 1, 1);
glm::vec3 ukd(0.31, 0.7, 0.97);
glm::vec3 uks(0.5, 0.5, 0.5);
float     sh = 200;

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}



float cubeVertices[] = {
    // positions          // normals
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};
float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};



FreeflyCamera camera = FreeflyCamera();



static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Move forward
    if (key == GLFW_KEY_W && action != GLFW_RELEASE) {
        camera.moveFront(0.1f);
    }
    // Move backward
    if (key == GLFW_KEY_S && action != GLFW_RELEASE) {
        camera.moveFront(-0.1f);
    }
    // Strafe left
    if (key == GLFW_KEY_A && action != GLFW_RELEASE) {
        camera.moveLeft(0.1f);
    }
    // Strafe right
    if (key == GLFW_KEY_D && action != GLFW_RELEASE) {
        camera.moveLeft(-0.1f);
    }
};

static void mouse_button_callback(GLFWwindow* /*window*/, int /* button*/, int /* action*/, int /* mods*/){};

static void scroll_callback(GLFWwindow* /* window*/, double /* xoffset*/, double /* yoffset*/){

};

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (previous) {
        prevxpos = (float)xpos;
        prevypos = (float)ypos;
        previous    = false;
    }

    double xoffset = xpos - prevxpos;
    double yoffset = ypos - prevypos ;

    prevxpos = (float)xpos;
    prevypos = (float)ypos;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        camera.rotateLeft((float)xoffset / 5);
        camera.rotateUp((float)yoffset / 5);
    }
};

static void size_callback(GLFWwindow* /*window*/, int /* width*/, int /* height*/){

};

int main(int argc, char** argv)
{
    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    /* Create a window and its OpenGL context */
#ifdef __APPLE__
    /* We need to explicitly ask for a 3.3 context on Mac */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Rollercoaster", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Intialize glad (loads the OpenGL functions) */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    /* Hook input callbacks */
    glfwSetKeyCallback(window, &key_callback);
    glfwSetMouseButtonCallback(window, &mouse_button_callback);
    glfwSetScrollCallback(window, &scroll_callback);
    glfwSetCursorPosCallback(window, &cursor_position_callback);
    glfwSetWindowSizeCallback(window, &size_callback);
    glimac::FilePath               applicationPath(argv[0]);
    glimac::Program                program = loadProgram(applicationPath.dirPath() + "/Rollercoaster/shaders/3D.vs.glsl",
                                          applicationPath.dirPath() + "/Rollercoaster/shaders/directionallight.fs.glsl");
   GLuint mvp = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
    GLuint mv  = glGetUniformLocation(program.getGLId(), "uMVMatrix");
   GLuint nm   = glGetUniformLocation(program.getGLId(), "uNormalMatrix");
    GLuint kd  = glGetUniformLocation(program.getGLId(), "uKd");
   GLuint  ks   = glGetUniformLocation(program.getGLId(), "uKs");
    GLuint shin   = glGetUniformLocation(program.getGLId(), "uShininess");
   GLuint  ldir = glGetUniformLocation(program.getGLId(), "uLightDir_vs");
    GLuint lint   = glGetUniformLocation(program.getGLId(), "uLightIntensity");
    glimac::Sphere sphere(1, 32, 16);
    MoonProgram sp(applicationPath,li,ukd,uks,sh);


    glimac::Program                program2 = loadProgram(applicationPath.dirPath() + "/Rollercoaster/shaders/merry.vs.glsl",
        applicationPath.dirPath() + "/Rollercoaster/shaders/merry.fs.glsl");
     GLuint mvpmerry = glGetUniformLocation(program2.getGLId(), "uMVPMatrix");
   GLuint mvmerry = glGetUniformLocation(program2.getGLId(), "uMVMatrix");
   GLuint nmmerry = glGetUniformLocation(program2.getGLId(), "uNormalMatrix");
   GLuint texmerry = glGetUniformLocation(program2.getGLId(), "texture_diffuse1");

    glimac::Program                program3 = loadProgram(applicationPath.dirPath() + "/Rollercoaster/shaders/6.2.skybox.vs.glsl",
        applicationPath.dirPath() + "/Rollercoaster/shaders/6.2.skybox.fs.glsl");




    GLuint p = glGetUniformLocation(program3.getGLId(), "projection");
    GLuint v = glGetUniformLocation(program3.getGLId(), "view"); 
    GLuint skybox = glGetUniformLocation(program3.getGLId(), "skybox");


  
    std::vector<std::string> faces
    {
        applicationPath.dirPath()+"Rollercoaster/assets/textures/skybox/right.jpg",
        applicationPath.dirPath()+"Rollercoaster/assets/textures/skybox/left.jpg",
        applicationPath.dirPath()+"Rollercoaster/assets/textures/skybox/top.jpg",
        applicationPath.dirPath()+"Rollercoaster/assets/textures/skybox/bottom.jpg",
        applicationPath.dirPath()+"Rollercoaster/assets/textures/skybox/front.jpg",
        applicationPath.dirPath()+"Rollercoaster/assets/textures/skybox/back.jpg",
    };
    unsigned int cubemapTexture = loadCubemap(faces);


    program3.use();
    glUniform1i(skybox, 0);

    std::vector<glm::vec3> controlPoints1 = {
    glm::vec3(0.f, -1.f, 0.f),
    glm::vec3(-1.f, 0.f, 0.0f),
    glm::vec3(0.f, 1.f, 0.f),
    glm::vec3(1.f, 0.f, 0.f),
     glm::vec3(0.f, -1.f, 0.f),

    };

    std::vector<glm::vec3> controlPoints2 = {
    glm::vec3(0.f, 0.f, -1.f),
    glm::vec3(1.f, 0.f, 0.0f),
    glm::vec3(0.f, 0.f, 1.f),
    glm::vec3(-1.f, 0.f, 0.f),
     glm::vec3(0.f, 0.f, -1.f),

    };

    glm::mat4 MVPMatrix, ProjMatrix, MVMatrix, NormalMatrix;
    ProjMatrix = glm::perspective(glm::radians(70.f), (GLfloat)window_width / (GLfloat)window_height, 0.1f, 100.f);
    glEnable(GL_DEPTH_TEST);
    float time=0.f;
    bool  c = true;
    std::vector<glm::vec3> pos;
    for (int i = 0; i < 32; i++) {

        pos.push_back(glm::sphericalRand(2.f));
    }
    Spline splineObj=Spline(li,ukd,uks,sh);
    std::vector<glm::vec3> spline = splineObj.generateSpline(controlPoints1);
    std::vector<glm::vec3> spline2= splineObj.generateSpline(controlPoints2);

    spline.insert(spline.end(), spline2.begin(), spline2.end());

    
    int j = 0;
    float elapsedTime = 0.0f;
    float lasttime = 0.f;
    splineObj.drawSquares2D(spline, 0.1f);
    splineObj.bindSpline();
    sp.bindSphere(sphere);


    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glm::mat4 viewMatrix = camera.getViewMatrix();

    glm::mat4  MVPgrass, MVgrass, Normalgrass;
    Model md(applicationPath.dirPath() + "Rollercoaster/assets/textures/merry.obj");
    Model mdgrass(applicationPath.dirPath() + "Rollercoaster/assets/textures/grass.obj");

    while (!glfwWindowShouldClose(window)) {

        time                 = (float)glfwGetTime();

        elapsedTime += abs(time - lasttime);
        if (j >= spline.size())j = 0;
        int numSplinePoints =(int) spline.size();
        int index = (int)(elapsedTime * 80.f) % numSplinePoints;
        glm::vec3 splinePos = spline[index];
        


        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //cube
        

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);

        //skybox
        program3.use();
        viewMatrix = glm::mat4(glm::mat3(viewMatrix));
        glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(p, 1, GL_FALSE, glm::value_ptr(ProjMatrix));
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
        viewMatrix=camera.getViewMatrix();


        program.use();
        splineObj.renderSpline(window, viewMatrix, ProjMatrix, mvp, mv, nm, kd, ks, lint, ldir, shin);
        MVMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));              
        MVMatrix = glm::scale(MVMatrix, glm::vec3(0.2, 0.2, 0.2));
        glm::mat4 MVMatrix2 = MVMatrix;
        MVMatrix = glm::rotate(MVMatrix, glm::radians(-90.f), glm::vec3(1, 0, 0));
        MVMatrix = glm::rotate(MVMatrix, glm::radians(-90.f), glm::vec3(0, 0, 1));
        
        
        MVMatrix = glm::translate(MVMatrix, splinePos);
        MVMatrix = glm::rotate(MVMatrix, glm::radians(+90.f), glm::vec3(1, 0, 0));
        if(splinePos.z==0)
        {
            MVMatrix = glm::rotate(MVMatrix, glm::radians(180.f), glm::vec3(0, 1, 0));
        }
        
        MVMatrix = glm::scale(MVMatrix, glm::vec3(0.1, 0.1, 0.1));
        j++;
        MVMatrix     = viewMatrix*MVMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        MVPMatrix           = ProjMatrix  * MVMatrix;
        glm::vec3 newlumdir = glm::vec3(viewMatrix*glm::vec4(1));

        sp.drawSphere(viewMatrix, ProjMatrix, newlumdir, sphere,time);
      
       program2.use();
        glUniformMatrix4fv(mvpmerry, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
        glUniformMatrix4fv(mvmerry, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(nmmerry, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        md.Draw(program2);
       
        MVgrass = glm::translate(glm::mat4(1.0f), glm::vec3(-0.01, -0.011, -0.05));
        MVgrass = glm::scale(MVgrass, glm::vec3(0.001, 0.001, 0.001));
        MVgrass= viewMatrix * MVgrass;
        MVPgrass= ProjMatrix * MVgrass;
        Normalgrass = glm::transpose(glm::inverse(MVgrass));
        glUniformMatrix4fv(mvpmerry, 1, GL_FALSE, glm::value_ptr(MVPgrass));
        glUniformMatrix4fv(mvmerry, 1, GL_FALSE, glm::value_ptr(MVgrass));
        glUniformMatrix4fv(nmmerry, 1, GL_FALSE, glm::value_ptr(Normalgrass));
        mdgrass.Draw(program2);
       


        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
        lasttime = time;
    }
    glDeleteBuffers(1, &sp.vbo);
    glDeleteVertexArrays(1, &sp.vao);
 
    glfwTerminate();
    return 0;
}