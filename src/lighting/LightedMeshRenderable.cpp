#include "./../../include/lighting/LightedMeshRenderable.hpp"
#include "./../../include/gl_helper.hpp"

#include <glm/gtc/type_ptr.hpp>

#define WIND 0.0
#define HIGH 100.0
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
    //std::cout << "do_draw\n";
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
    //std::cout << "do_animate\n";
    float dz = 0.5;
    float dt = (time - last_time)*10;
    glm::mat4 transformation(1.0);
    if (z < 0.0) {
        z = HIGH;
        transformation =  glm::translate(getParentTransform(), glm::vec3(0.0, 0.0, HIGH));
    } else {
        transformation =  glm::translate(getParentTransform(), glm::vec3(0.0, 0.0, -dz));
    }
    setParentTransform(transformation);
    //setParentTransform(transformation);
    z -= dz;
    last_time = time;
}
