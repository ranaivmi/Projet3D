#include "./../../include/dynamics/SpringForceField.hpp"

SpringForceField::SpringForceField(const ParticlePtr p1, const ParticlePtr p2,
                                   float stiffness, float equilibriumLength,
                                   float damping) :
    m_p1(p1),
    m_p2(p2),
    m_stiffness(stiffness),
    m_equilibriumLength(equilibriumLength),
    m_damping(damping)
{}

void SpringForceField::do_addForce()
{
    //TODO: Implement a damped spring
    //Functions to use:
    //glm::length( vec3 ): Return the length of a vector
    //glm::normalize( vec3 ): Return the normalization of a vector
    //Particle::getVelocity(), Particle::getForce(), Particle::setForce()
    //
    //Nb:   Compute force ONLY IF the displacement length is above std::numeric_limits<float>::epsilon()
    //      Otherwise the computation is useless
    glm::vec3 p1 = m_p1->getPosition();
    glm::vec3 p2 = m_p2->getPosition();

    glm::vec3 f1 = - m_stiffness * (glm::length(p1 - p2) - m_equilibriumLength) * glm::normalize(p1 - p2);
    glm::vec3 f2 = - m_damping * (dot(m_p1->getVelocity() - m_p2->getVelocity(), glm::normalize(p1 - p2))) * glm::normalize(p1 - p2);
    m_p1->setForce( f1 + f2 + m_p1->getForce());
    m_p2->setForce(- (f1 + f2) + m_p2->getForce());
}

ParticlePtr SpringForceField::getParticle1() const
{
    return m_p1;
}

ParticlePtr SpringForceField::getParticle2() const
{
    return m_p2;
}
