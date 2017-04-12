#include "../include/ShaderProgram.hpp"
#include "../include/Viewer.hpp"

#include "../include/FrameRenderable.hpp"
#include "../teachers/CylinderRenderable.hpp"
#include "../teachers/ConeRenderable.hpp"
#include "../teachers/MeshRenderable.hpp"


void initialize_practical_02_scene(Viewer& viewer)
{
    // create all shaders of this scene, then add them to the viewer
    ShaderProgramPtr flatShader
        = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl",
                                          "../shaders/flatFragment.glsl");
    viewer.addShaderProgram(flatShader);


    // create renderable objects
    viewer.addRenderable(std::make_shared<FrameRenderable>(flatShader));
    glm::mat4 scaleM(1.0);
    glm::mat4 translateM(1.0);

    // Tron du sapin
    std::shared_ptr<teachers::CylinderRenderable> Tron
        = std::make_shared<teachers::CylinderRenderable>(flatShader, true, 30);
    scaleM = glm::scale(glm::mat4(), glm::vec3(0.5, 0.5, 0.5));
    Tron->setLocalTransform(scaleM*Tron->getModelMatrix());
   

    //Haut du sapin
    std::shared_ptr<teachers::ConeRenderable> Cone1
        = std::make_shared<teachers::ConeRenderable>(flatShader, true, 30);
    translateM = glm::translate(glm::mat4(), glm::vec3(0.0, 0.0, 0.5));
    Cone1->setParentTransform(translateM);
    scaleM = glm::scale(glm::mat4(), glm::vec3(1.0,1.0,1.0));
    Cone1->setLocalTransform(scaleM);

    std::shared_ptr<teachers::ConeRenderable> Cone2
        = std::make_shared<teachers::ConeRenderable>(flatShader, true, 30);
    translateM = glm::translate(glm::mat4(), glm::vec3(0.0, 0.0, 1.0));
    Cone2->setParentTransform(translateM);
    scaleM = glm::scale(glm::mat4(), glm::vec3(0.85,0.85,1.0));
    Cone2->setLocalTransform(scaleM);

    std::shared_ptr<teachers::ConeRenderable> Cone3
        = std::make_shared<teachers::ConeRenderable>(flatShader, true, 30);
    translateM = glm::translate(glm::mat4(), glm::vec3(0.0, 0.0, 1.5));
    Cone3->setParentTransform(translateM);
    scaleM = glm::scale(glm::mat4(), glm::vec3(0.70,0.70,0.8));
    Cone3->setLocalTransform(scaleM);

    HierarchicalRenderable::addChild(Tron, Cone1);
    HierarchicalRenderable::addChild(Tron, Cone2);
    HierarchicalRenderable::addChild(Tron, Cone3);
    viewer.addRenderable(Tron);

    

}
