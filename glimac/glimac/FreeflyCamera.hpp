#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class FreeflyCamera {
private:
    glm::vec3 m_Position;
    float     m_fPhi;
    float     m_fTheta;
    glm::vec3 m_FrontVector;
    glm::vec3 m_LeftVector;
    glm::vec3 m_UpVector;

    void computeDirectionVectors()
    {
        m_FrontVector = glm::vec3(cos(m_fTheta) * sin(m_fPhi), sin(m_fTheta), cos(m_fTheta) * cos(m_fPhi));
        m_LeftVector  = glm::vec3(sin(m_fPhi + glm::half_pi<float>()), 0, cos(m_fPhi + glm::half_pi<float>()));
        m_UpVector    = glm::cross(m_FrontVector, m_LeftVector);
    }

public:
    FreeflyCamera()
        : m_Position(0.1, 0.1, 0.5), m_fPhi(glm::pi<float>()), m_fTheta(0.f)
    {
        computeDirectionVectors();
    }

    void moveLeft(float t)
    {
        m_Position += t * m_LeftVector;
    }

    void moveFront(float t)
    {
        m_Position += t * m_FrontVector;
    }

    void rotateLeft(float degrees)
    {
        m_fPhi += glm::radians(degrees);
        computeDirectionVectors();
    }

    void rotateUp(float degrees)
    {
        m_fTheta += glm::radians(degrees);
        computeDirectionVectors();
    }

    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(m_Position, m_Position + m_FrontVector, m_UpVector);
    }
};