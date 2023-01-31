class TrackballCamera {
private:
    float m_fDistance, m_fAngleX, m_fAngleY;

public:
    TrackballCamera()
        : m_fDistance{10.f}, m_fAngleX(0.f), m_fAngleY(0.f) { std::cout << m_fDistance; }

    void moveFront(float delta) {
        m_fDistance += delta;
    }

    void rotateLeft(float degrees) {
        m_fAngleY += glm::radians(degrees);
    }
    void rotateUp(float degrees) {
        m_fAngleX +=glm::radians( degrees);
    }

    glm::mat4 getViewMatrix() const {
       glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, m_fDistance));
        viewMatrix           = glm::rotate(viewMatrix, m_fAngleY, glm::vec3(1, 0, 0)); 
        viewMatrix           = glm::rotate(viewMatrix, m_fAngleX, glm::vec3(0, 1, 0)); 

        return viewMatrix;
    }
};