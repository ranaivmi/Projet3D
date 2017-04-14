#include "./../../include/lighting/LightedMeshRenderable.hpp"
#include "./../../include/gl_helper.hpp"

#include <glm/gtc/type_ptr.hpp>

#define WIND 0.3
#define HIGH 50.0
#define VAR 30.0

LightedMeshRenderable::~LightedMeshRenderable()
{
}

LightedMeshRenderable::LightedMeshRenderable(ShaderProgramPtr shaderProgram,
        const std::string& filename, const MaterialPtr& material) :
    MeshRenderable(shaderProgram, filename), Materiable(material)
{
    z = frand_a_b(0.0, 50.0);
    last_time = 0.0;
}

void LightedMeshRenderable::LightedMeshRenderable::do_draw()
{
    //Send material to GPU as uniform
    Material::sendToGPU(m_shaderProgram, getMaterial());

    //NIT matrix
    int nitLocation = m_shaderProgram->getUniformLocation("NIT");
    if (nitLocation != ShaderProgram::null_location) {
        glcheck(glUniformMatrix3fv(nitLocation, 1, GL_FALSE,
            glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(getModelMatrix()))))));
    }

    // Call the super method to bind other uniforms/attributes & draw
    formes::MeshRenderable::do_draw();

    if (nitLocation != ShaderProgram::null_location) {
        glcheck(glDisableVertexAttribArray(nitLocation));
    }
}

void LightedMeshRenderable::do_animate(float time)
{
    float dz = 0.5;
    float dt = time - last_time;
    float x(0.0), y(0.0), newZ(0.0);
    newZ -= dt * dz;
    if (newZ < 0.0) {
        x = frand_a_b(-VAR, VAR);
        y = frand_a_b(-VAR, VAR);
        newZ = -newZ;
        newZ = HIGH - (newZ - (((float) ((int) newZ) / HIGH) * HIGH));
    }
    glm::mat4 transformation(1.0);
    transformation =  glm::translate(glm::mat4(1.0), glm::vec3(frand_a_b(-WIND*dt, WIND*dt), frand_a_b(-WIND*dt, WIND*dt), newZ - z));
    setLocalTransform(transformation);
    z = newZ;
    last_time = time;
}
