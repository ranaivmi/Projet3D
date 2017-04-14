#include "../include/ShaderProgram.hpp"
#include "../include/Viewer.hpp"

#include "../include/FrameRenderable.hpp"
#include "../teachers/CubeRenderable.hpp"
#include "../teachers/IndexedCubeRenderable.hpp"
#include "../teachers/MeshRenderable.hpp"

#include "../include/FrameRenderable.hpp"
#include "../include/lighting/DirectionalLightRenderable.hpp"

#include "../include/texturing/TexturedPlaneRenderable.hpp"
#include "../include/texturing/TexturedCubeRenderable.hpp"
#include "../include/texturing/TexturedCylinderRenderable.hpp"
#include "../include/texturing/MultiTexturedCubeRenderable.hpp"
#include "../include/texturing/MipMapCubeRenderable.hpp"
#include "../include/texturing/TexturedMeshRenderable.hpp"
#include "../teachers/Geometries.hpp"
#include "../include/keyframes/KeyframedCylinderRenderable.hpp"

void initialize_practical_01_scene(Viewer& viewer)
{
    // create all shaders of this scene, then add them to the viewer
    ShaderProgramPtr defaultShader
        = std::make_shared<ShaderProgram>("../shaders/defaultVertex.glsl",
                                          "../shaders/defaultFragment.glsl");
    ShaderProgramPtr flatShader
        = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl",
                                          "../shaders/flatFragment.glsl");
    viewer.addShaderProgram(defaultShader);
    viewer.addShaderProgram(flatShader);

    // create renderable objects
    viewer.addRenderable(std::make_shared<FrameRenderable>(defaultShader));

    /*std::shared_ptr<teachers::CubeRenderable> teachersCube
        = std::make_shared<teachers::CubeRenderable>(flatShader);
    teachersCube->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(2.0, 0.0, 0.0)));
    viewer.addRenderable(teachersCube);

    std::shared_ptr<teachers::IndexedCubeRenderable> teachersIndexedCube
        = std::make_shared<teachers::IndexedCubeRenderable>(flatShader);
    teachersIndexedCube->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(-2.0, 0.0, 0.0)));
    viewer.addRenderable(teachersIndexedCube);*/
    ShaderProgramPtr texShader
        = std::make_shared<ShaderProgram>("../shaders/textureVertex.glsl",
                                          "../shaders/textureFragment.glsl");
    viewer.addShaderProgram(texShader);
  
          //Define a directional light for the whole scene
	  glm::vec3 d_direction = glm::normalize(glm::vec3(0.0,-1.0,0.0));
	  glm::vec3 d_ambient(1.0,1.0,1.0), d_diffuse(1.0,1.0,1.0), d_specular(1.0,1.0,1.0);
	  DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
          glm::vec3 lightPosition(0.0,0.0,0.0);
	  DirectionalLightRenderablePtr directionalLightRenderable = std::make_shared<DirectionalLightRenderable>(flatShader, directionalLight, lightPosition);
	  directionalLightRenderable->setLocalTransform(glm::scale(glm::mat4(1.0), glm::vec3(0.5,0.5,0.5)));
          viewer.setDirectionalLight(directionalLight);
	  viewer.addRenderable(directionalLightRenderable);

	  std::string filename;
	  filename = "../textures/cible.png";
	  auto cylinder = std::make_shared<KeyframedCylinderRenderable>(texShader,filename);
          cylinder->setLocalTransform((glm::rotate(glm::mat4(1.0), (float)(M_PI), glm::vec3(0,0,1))* glm::rotate(glm::mat4(1.0), (float)(M_PI/2.0), glm::vec3(1,0,0))) * glm::scale(glm::mat4(1.0), glm::vec3(0.5,0.5,0.125)) * glm::translate(glm::mat4(1.0), glm::vec3(0.0,1.0,-10.0)));
          cylinder->setParentTransform(glm::mat4(1.0));
          int n = 5;
          for (int i = 0; i < n; i++) {
              cylinder->addParentTransformKeyframe(0.5 + 2*i, GeometricTransformation(glm::vec3(2.0, 0.0, 0.0)));
              cylinder->addParentTransformKeyframe(1.5 + 2*i, GeometricTransformation(glm::vec3(-2.0, 0.0, 0.0)));
	      }
	  cylinder->setMaterial(Material::Pearl());
          viewer.addRenderable(cylinder);
}


#include "../include/ShaderProgram.hpp"
#include "../include/Viewer.hpp"
#include "../include/FrameRenderable.hpp"
#include "../include/lighting/DirectionalLightRenderable.hpp"

#include "../include/texturing/TexturedPlaneRenderable.hpp"
#include "../include/texturing/TexturedCubeRenderable.hpp"
#include "../include/texturing/TexturedCylinderRenderable.hpp"
#include "../include/texturing/MultiTexturedCubeRenderable.hpp"
#include "../include/texturing/MipMapCubeRenderable.hpp"
#include "../include/texturing/TexturedMeshRenderable.hpp"
