struct PosNorm { glm::vec3 pos; glm::vec3 norm; };

struct result { std::vector<PosNorm> vec; std::vector<uint32_t> indexes; };
class Spline
{
private:
    GLuint vao, vbo,ibo;
    result spline;
    glm::vec3 li, ukd, uks;
    float sh;

public:

    Spline(glm::vec3 pli, glm::vec3 pukd, glm::vec3 puks,float psh) {
        li = pli;
        ukd = pukd;
        uks = puks;
        sh = psh;
      
    }
    // take a set of control points to generate a spline
    std::vector<glm::vec3> generateSpline(std::vector<glm::vec3>& controlPoints) {
        std::vector<glm::vec3> splinePoints;
        int numControlPoints = (int)controlPoints.size();
        for (int i = 0; i < numControlPoints - 3; i++) {
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
                float y = (float)(0.5 * ((2 * p1.y) +
                    (-p0.y + p2.y) * t +
                    (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t2 +
                    (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t3));
                float z = (float)(0.5 * ((2 * p1.z) +
                    (-p0.z + p2.z) * t +
                    (2 * p0.z - 5 * p1.z + 4 * p2.z - p3.z) * t2 +
                    (-p0.z + 3 * p1.z - 3 * p2.z + p3.z) * t3));
                splinePoints.push_back({ x, y, z });
            }

        }

        return splinePoints;
    }


    void bindSpline()
    {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, spline.vec.size() * sizeof(PosNorm), spline.vec.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &ibo);

        //// => We bind on GL_ELEMENT_ARRAY_BUFFER, target reserved for IBOs
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, spline.indexes.size() * sizeof(uint32_t), spline.indexes.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
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
            sizeof(PosNorm), (const GLvoid*)offsetof(PosNorm, norm));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void drawSquares2D(std::vector<glm::vec3> centers, float s) {
        float halfSize = s / 2.0f;
        glm::vec3 p1, p2, p3, p4;
        PosNorm pos, pos2, pos3, pos4;


        for (long unsigned int i = 0; i < centers.size() - 1; i++) {
            glm::vec3 center = centers[i];
            glm::vec3 center2 = centers[i + 1];
            p1 = center + glm::vec3(halfSize, -halfSize, 0);
            p2 = center + glm::vec3(-halfSize, -halfSize, 0);

            p3 = center2 + glm::vec3(halfSize, -halfSize, 0);
            p4 = center2 + glm::vec3(-halfSize, -halfSize, 0);

            pos.pos = p1;
            pos.norm = glm::normalize(p1);
            spline.vec.push_back(pos);
            pos2.pos = p2;
            pos2.norm = glm::normalize(p2);
            spline.vec.push_back(pos2);
            pos3.pos = p3;
            pos3.norm = glm::normalize(p3);
            spline.vec.push_back(pos3);
            pos4.pos = p4;
            pos4.norm = glm::normalize(p4);
            spline.vec.push_back(pos4);


            spline.indexes.push_back(i * 4 + 0);
            spline.indexes.push_back(i * 4 + 1);
            spline.indexes.push_back(i * 4 + 2);
            spline.indexes.push_back(i * 4 + 2);
            spline.indexes.push_back(i * 4 + 3);
            spline.indexes.push_back(i * 4 + 1);

        }


    }


    void renderSpline(glm::mat4 viewMatrix, glm::mat4 ProjMatrix, GLuint mvp, GLuint mv, GLuint nm,
        GLuint kd, GLuint ks, GLuint lint, GLuint ldir, GLuint shin)
    {


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




        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, (GLsizei)spline.indexes.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

    }

};

