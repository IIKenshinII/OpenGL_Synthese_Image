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
#include <glimac/SphereProgramDraw.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glimac/model.hpp>
#include <glimac/mesh.hpp>


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


std::vector<glm::vec3> generateSpline(std::vector<glm::vec3> &controlPoints) {
    std::vector<glm::vec3> splinePoints;
    int numControlPoints = (int)controlPoints.size();
    for (int i = 0; i < numControlPoints -3; i++) {
        glm::vec3 p0 = controlPoints[i];
        glm::vec3 p1 = controlPoints[i + 1];
        glm::vec3 p2 = controlPoints[i + 2];
        glm::vec3 p3 = controlPoints[i + 3];
        for (float t = 0.f; t < 1.f; t += 0.01f) {
            float t2 = t * t;
            float t3 = t2 * t;
            float x = (float)(0.5 * ((2 * p1.x) +
                (-p0.x + p2.x) * t +
                (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t2 +
                (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t3));
            float y =(float)( 0.5 * ((2 * p1.y) +
                (-p0.y + p2.y) * t +
                (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t2 +
                (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t3));
            float z =(float) (0.5 * ((2 * p1.z) +
                (-p0.z + p2.z) * t +
                (2 * p0.z - 5 * p1.z + 4 * p2.z - p3.z) * t2 +
                (-p0.z + 3 * p1.z - 3 * p2.z + p3.z) * t3));
            splinePoints.push_back({ x, y, z });
        }
    
    }

    return splinePoints;
}

struct PosNorm { glm::vec3 pos; glm::vec3 norm; };
struct result { std::vector<PosNorm> vec; std::vector<uint32_t> indexes;  };
result drawSquares2D(std::vector<glm::vec3> centers, float s) {
    float halfSize = s / 2.0f;
    result res;
    glm::vec3 p1, p2, p3, p4;
    PosNorm pos,pos2,pos3,pos4;


    for (auto i = 0; i < centers.size() - 1; i++) {
        glm::vec3 center = centers[i];
        glm::vec3 center2 = centers[i+1];
        p1 = center + glm::vec3(halfSize, -halfSize, 0);
        p2 = center + glm::vec3(-halfSize, -halfSize, 0);
        
        p3 = center2 + glm::vec3(halfSize, -halfSize, 0);
        p4 = center2 + glm::vec3(-halfSize, -halfSize, 0);

        pos.pos=p1;
        pos.norm=glm::normalize(p1);
        res.vec.push_back(pos);
        pos2.pos = p2;
        pos2.norm = glm::normalize(p2);
        res.vec.push_back(pos2);
        pos3.pos = p3;
        pos3.norm = glm::normalize(p3);
        res.vec.push_back(pos3);
        pos4.pos = p4;
        pos4.norm = glm::normalize(p4);
        res.vec.push_back(pos4);
       

        res.indexes.push_back(i*4+ 0);
        res.indexes.push_back(i*4 + 1);
        res.indexes.push_back(i*4 + 2);
        res.indexes.push_back(i*4 + 2);
        res.indexes.push_back(i*4 + 3);
        res.indexes.push_back(i*4 + 1);
           
    }


    return res;
}


void renderSpline(GLFWwindow* window, glm::mat4 viewMatrix,glm::mat4 ProjMatrix,GLuint mvp, GLuint mv, GLuint nm,
    GLuint kd, GLuint ks, GLuint lint, GLuint ldir, GLuint shin,result spline)
{

    

    GLuint vbo, ibo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, spline.vec.size() * sizeof(PosNorm), spline.vec.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &ibo);

    //// => On bind sur GL_ELEMENT_ARRAY_BUFFER, cible reservée pour les IBOs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, spline.indexes.size() * sizeof(uint32_t), spline.indexes.data(), GL_STATIC_DRAW);
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
        sizeof(PosNorm), (const GLvoid*)offsetof(PosNorm, pos));
    glVertexAttribPointer(1,
        3, GL_FLOAT, GL_FALSE,
        sizeof(PosNorm), (const GLvoid*)offsetof(PosNorm,norm));
    



   glm::mat4 MVMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));  
    MVMatrix = glm::scale(MVMatrix, glm::vec3(0.2, 0.2, 0.2));
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
    



    //glDrawArrays(GL_TRIANGLES, 0, (int)trail.size());

    glDrawElements(GL_TRIANGLES,(GLsizei) spline.indexes.size(), GL_UNSIGNED_INT, 0);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

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
    SphereProgram sp(applicationPath);


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

    std::vector<glm::vec3> spline = generateSpline(controlPoints1);
    std::vector<glm::vec3> spline2= generateSpline(controlPoints2);

    spline.insert(spline.end(), spline2.begin(), spline2.end());

    
    int j = 0;
    float elapsedTime = 0.0f;
    float lasttime = 0.f;
    result res = drawSquares2D(spline, 0.1f);
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

    glm::mat4 MVPMoon, MVMoon, NormalMoon, MVPgrass, MVgrass, Normalgrass;
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
        renderSpline(window, viewMatrix, ProjMatrix, mvp, mv, nm, kd, ks, lint, ldir, shin,res);
        MVMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));              //position my sphere
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


       sp.m_Program.use();
       MVMoon = glm::translate(glm::mat4(1.0f), glm::vec3(0.5, 0.5, -0.5));
       //MVMoon = glm::rotate(MVMoon, glm::radians(time * 4), glm::vec3(0, 1, 0)); // Translation * Rotation

       MVMoon = glm::scale(MVMoon, glm::vec3(0.1, 0.1, 0.1)); 
       MVMoon = viewMatrix * MVMoon;
       MVPMoon = ProjMatrix * MVMoon;
       NormalMoon = glm::transpose(glm::inverse(MVMoon));
       glActiveTexture(GL_TEXTURE0);
       glBindTexture(GL_TEXTURE_2D, sp.texture_map);
        glUniformMatrix4fv(sp.mvp, 1, GL_FALSE, glm::value_ptr(MVPMoon));
        glUniformMatrix4fv(sp.mv, 1, GL_FALSE, glm::value_ptr(MVMoon));
        glUniformMatrix4fv(sp.nm, 1, GL_FALSE, glm::value_ptr(NormalMoon));
        glUniform1i(sp.tex, 0);

        glBindVertexArray(sp.vao);
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
       program2.use();
        glUniformMatrix4fv(mvpmerry, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
        glUniformMatrix4fv(mvmerry, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(nmmerry, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        md.Draw(program2);
       
        MVgrass = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.011, 0));
        MVgrass = glm::scale(MVgrass, glm::vec3(0.001, 0.001, 0.001));
        MVgrass= viewMatrix * MVgrass;
        MVPgrass= ProjMatrix * MVgrass;
        Normalgrass = glm::transpose(glm::inverse(MVgrass));
        glUniformMatrix4fv(mvpmerry, 1, GL_FALSE, glm::value_ptr(MVPgrass));
        glUniformMatrix4fv(mvmerry, 1, GL_FALSE, glm::value_ptr(MVgrass));
        glUniformMatrix4fv(nmmerry, 1, GL_FALSE, glm::value_ptr(Normalgrass));
        mdgrass.Draw(program2);
       

       

        //int i = 0;
       

        /*for (int i = 0; i < 32; i++) {
            MVMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));

            MVMatrix     = glm::rotate(MVMatrix, glm::radians(time * 4), glm::vec3(0, 1, 0)); // Translation * Rotation
            MVMatrix     = glm::translate(MVMatrix, pos[i]);                                  // Translation * Rotation * Translation
            MVMatrix     = glm::scale(MVMatrix, glm::vec3(0.2, 0.2, 0.2));                    // Translation * Rotation * Translation * Scale
            MVMatrix     = viewMatrix * MVMatrix;
            MVPMatrix    = ProjMatrix  * MVMatrix;
            NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
            glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
            glUniformMatrix4fv(mv, 1, GL_FALSE, glm::value_ptr(MVMatrix));
            glUniformMatrix4fv(nm, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
            glUniform3fv(kd, 1, glm::value_ptr(ukd));
            glUniform3fv(ks, 1, glm::value_ptr(uks));
            glUniform3fv(lint, 1, glm::value_ptr(li));
            glUniform3fv(ldir, 1, glm::value_ptr(glm::vec3(newlumdir)));
            glUniform1f(shin, sh);
            glBindVertexArray(vao);
            
            glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());//these sphere don't have the good color
            
            glBindVertexArray(0);
        }*/

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