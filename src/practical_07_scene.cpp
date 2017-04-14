#include "../include/Viewer.hpp"
#include "../include/FrameRenderable.hpp"
#include "../include/CylinderRenderable.hpp"
#include "../include/ConeRenderable.hpp"
#include "../teachers/MeshRenderable.hpp"

#include "../include/dynamics/DynamicSystem.hpp"
#include "../include/dynamics/DampingForceField.hpp"
#include "../include/dynamics/ConstantForceField.hpp"
#include "../include/dynamics/SpringForceField.hpp"
#include "../include/dynamics/EulerExplicitSolver.hpp"

#include "../include/dynamics_rendering/DynamicSystemRenderable.hpp"
#include "../include/dynamics_rendering/SnowballParticleRenderable.hpp"
#include "../include/dynamics_rendering/ParticleListRenderable.hpp"
#include "../include/dynamics_rendering/ConstantForceFieldRenderable.hpp"
#include "../include/dynamics_rendering/SpringForceFieldRenderable.hpp"
#include "../include/dynamics_rendering/SpringListRenderable.hpp"
#include "../include/dynamics_rendering/ControlledForceFieldRenderable.hpp"
#include "../include/dynamics_rendering/QuadRenderable.hpp"
#include "../include/texturing/TexturedPlaneRenderable.hpp"
#include "../include/lighting/LightedMeshRenderable.hpp"
#include "../include/texturing/TexturedMeshRenderable.hpp"
#include "../include/lighting/DirectionalLightRenderable.hpp"
#include "../include/keyframes/GeometricTransformation.hpp"

#include <cstdlib>

#define PS 50.0
#define PL 25.0
#define LPL PL/5
#define SNOWBALL_RADIUS 0.25
#define PH 3.0
#define COS_45 0.70710678118
#define FIR_SIZE 4.0
#define WIND 10.0

void snow_scene(Viewer& viewer,
    DynamicSystemPtr& system, DynamicSystemRenderablePtr& systemRenderable);

void createFir(DynamicSystemRenderablePtr &systemRenderable, glm::mat4 FinaltranslateM , ShaderProgramPtr flatShader ){

    glm::mat4 scaleM(1.0);
    glm::mat4 translateM(1.0);

    // Tron du sapin
    std::shared_ptr<formes::CylinderRenderable> Tron
        = std::make_shared<formes::CylinderRenderable>(flatShader, 30);
    scaleM = glm::scale(glm::mat4(), glm::vec3(0.5, 0.5, 0.5));
      Tron->setLocalTransform(scaleM*Tron->getModelMatrix());
      Tron->setModelMatrix(FinaltranslateM);

    //Haut du sapin
    std::shared_ptr<formes::ConeRenderable> Cone1
        = std::make_shared<formes::ConeRenderable>(flatShader, 30);
    translateM = glm::translate(glm::mat4(), glm::vec3(0.0, 0.0, 0.5));
    Cone1->setParentTransform(translateM);
    scaleM = glm::scale(glm::mat4(), glm::vec3(1.0,1.0,1.0));
    Cone1->setLocalTransform(scaleM);

    std::shared_ptr<formes::ConeRenderable> Cone2
        = std::make_shared<formes::ConeRenderable>(flatShader, 30);
    translateM = glm::translate(glm::mat4(), glm::vec3(0.0, 0.0, 1.0));
    Cone2->setParentTransform(translateM);
    scaleM = glm::scale(glm::mat4(), glm::vec3(0.85,0.85,1.0));
    Cone2->setLocalTransform(scaleM);

    std::shared_ptr<formes::ConeRenderable> Cone3
        = std::make_shared<formes::ConeRenderable>(flatShader, 30);
    translateM = glm::translate(glm::mat4(), glm::vec3(0.0, 0.0, 1.5));
    Cone3->setParentTransform(translateM);
    scaleM = glm::scale(glm::mat4(), glm::vec3(0.70,0.70,0.8));
    Cone3->setLocalTransform(scaleM);

    HierarchicalRenderable::addChild(Tron, Cone1);
    HierarchicalRenderable::addChild(Tron, Cone2);
    HierarchicalRenderable::addChild(Tron, Cone3);
    //viewer.addRenderable(Tron);
    HierarchicalRenderable::addChild( systemRenderable, Tron );
}

void initialize_practical_07_scene(Viewer& viewer, unsigned int scene_to_load)
{
    //Set up a shader and add a 3D frame.
    ShaderProgramPtr flatShader =
        std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl",
                                        "../shaders/flatFragment.glsl");
    viewer.addShaderProgram(flatShader);
    FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);

    //Initialize a dynamic system (Solver, Time step, Restitution coefficient)
    DynamicSystemPtr system = std::make_shared<DynamicSystem>();
    EulerExplicitSolverPtr solver = std::make_shared<EulerExplicitSolver>();
    system->setSolver(solver);
    system->setDt(0.01);
    //Create a renderable associated to the dynamic system
    //This renderable is responsible for calling DynamicSystem::computeSimulationStep()in the animate() function
    //It also handles some of the key/mouse events
    DynamicSystemRenderablePtr systemRenderable = std::make_shared<DynamicSystemRenderable>(system);
    viewer.addRenderable(systemRenderable);

    //Populate the dynamic system with particles, forcefields
    //and create renderables associated to them for visualization.
    switch (scene_to_load) {
        case 75:
            snow_scene(viewer, system, systemRenderable);
            break;
        default:
            snow_scene(viewer, system, systemRenderable);
            break;
    }

    //Finally, run the animation
    viewer.startAnimation();
}

void snow_scene(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable)
{
    //Initialize a shader for the following renderables
    ShaderProgramPtr flatShader
        = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl","../shaders/flatFragment.glsl");
    ShaderProgramPtr texShader
        = std::make_shared<ShaderProgram>("../shaders/textureVertex.glsl", "../shaders/textureFragment.glsl");
    ShaderProgramPtr phongShader
            = std::make_shared<ShaderProgram>("../shaders/phongVertex.glsl", "../shaders/phongFragment.glsl");
    viewer.addShaderProgram(flatShader);
    viewer.addShaderProgram(texShader);
    viewer.addShaderProgram(phongShader);

    system->setDt(0.0250);

    //Activate collision detection
    system->setCollisionsDetection(true);

    //Initialize the restitution coefficient for collision
    //1.0 = full elastic response
    //0.0 = full absorption
    system->setRestitution(0.6f);

    glm::mat4 scaleTransformation(1.0), localTransformation(1.0);
    std::string filename;
    MaterialPtr pearl = Material::Pearl();
    MaterialPtr emerald = Material::Emerald();

    glm::vec3 d_direction = glm::normalize(glm::vec3(0.0,0.0,-1.0));
    glm::vec3 d_ambient(1.0,1.0,1.0), d_diffuse(1.0,1.0,0.8), d_specular(1.0,1.0,1.0);
    DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
    //Add a renderable to display the light and control it via mouse/key event
    glm::vec3 lightPosition(0.0,0.0,50.0);
    DirectionalLightRenderablePtr directionalLightRenderable = std::make_shared<DirectionalLightRenderable>(flatShader, directionalLight, lightPosition);
    localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.5,0.5,0.5));
    directionalLightRenderable->setLocalTransform(localTransformation);
    viewer.setDirectionalLight(directionalLight);
    viewer.addRenderable(directionalLightRenderable);

    //Initialize a plane from 3 points and add it to the system as an obstacle
    // main plane, flat => -PS, -PS, 0 - PS, -PS, 0 - -PS, 0, 0 - PS, 0, 0
    // Flat plane
    glm::vec3 p1(-PS, -PS, 0.0);
    glm::vec3 p2(PS, -PS, 0.0);
    // Flat & Leaning plane
    glm::vec3 p3(-PS, 0.0, 0.0);
    glm::vec3 p4(PS, 0.0, 0.0);
    // Leaning & High plane
    glm::vec3 p5(-PS, PS/2, PH);
    glm::vec3 p6(PS, PS/2, PH);
    // High plane
    glm::vec3 p7(-PS, PS, PH);
    glm::vec3 p8(PS, PS, PH);

    // normal vectors to define planes
    glm::vec3 normalFlat(0.0, 0.0, 1.0);
    glm::vec3 normalLeaning(0.0, -2.0, 3.0);

    glm::vec4 color(0.5, 0.5, 0.5, 1.0);
    PlanePtr flat_plane = std::make_shared<Plane>(p1, normalFlat);
    system->addPlaneObstacle(flat_plane);

    PlanePtr leaning_plane = std::make_shared<Plane>(p3, normalLeaning);
    system->addPlaneObstacle(leaning_plane);

    PlanePtr high_plane = std::make_shared<Plane>(p5, normalFlat);
    system->addPlaneObstacle(high_plane);

    filename = "../textures/snow_tex2.png";
    TexturedPlaneRenderablePtr flatPlaneRenderable = std::make_shared<TexturedPlaneRenderable>(texShader, filename);
    scaleTransformation = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -PL/2, 0.0)) * glm::scale(glm::mat4(1.0), glm::vec3(PS, PL, 10.0));
    flatPlaneRenderable->setParentTransform(scaleTransformation);
    flatPlaneRenderable->setMaterial(pearl);
    HierarchicalRenderable::addChild( systemRenderable, flatPlaneRenderable );

    TexturedPlaneRenderablePtr leaningPlaneRenderable = std::make_shared<TexturedPlaneRenderable>(texShader, filename);
    scaleTransformation = glm::rotate(glm::mat4(1.0), (float) (M_PI/4.0), glm::vec3(1.0, 0.0, 0.0)) * glm::translate(glm::mat4(1.0), glm::vec3(0.0, LPL/2, 0.0)) * glm::scale(glm::mat4(1.0), glm::vec3(PS, LPL, 10.0));
    leaningPlaneRenderable->setParentTransform(scaleTransformation);
    leaningPlaneRenderable->setMaterial(pearl);
    HierarchicalRenderable::addChild( systemRenderable, leaningPlaneRenderable );

    TexturedPlaneRenderablePtr highPlaneRenderable = std::make_shared<TexturedPlaneRenderable>(texShader, filename);
    scaleTransformation = glm::translate(glm::mat4(1.0), glm::vec3(0.0, PL/2+LPL*COS_45, LPL*COS_45)) * glm::scale(glm::mat4(1.0), glm::vec3(PS, PL, 10.0));
    highPlaneRenderable->setParentTransform(scaleTransformation);
    highPlaneRenderable->setMaterial(pearl);
    HierarchicalRenderable::addChild( systemRenderable, highPlaneRenderable );

    //Position the camera
    viewer.getCamera().setViewMatrix(
        glm::lookAt(glm::vec3(0, -PL, 10), glm::vec3(0, PL/2 + LPL*COS_45, LPL*COS_45), glm::vec3(0,0,1)) );

    glm::mat4 translateM(1.0);

    // Firs on flatPlane
    for (int i = 5; i > 0; i--) {
        for (int j = 0; j < i; j++) {
            translateM = glm::translate(glm::mat4(), glm::vec3(+ i*FIR_SIZE, -PL + (j+1)*FIR_SIZE, 0));
            createFir(systemRenderable, translateM, flatShader);
            translateM = glm::translate(glm::mat4(), glm::vec3(- i*FIR_SIZE, -PL + (j+1)*FIR_SIZE, 0));
            createFir(systemRenderable, translateM, flatShader);
        }
    }

    // Firs on highPlane
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 11; j++) {
            translateM = glm::translate(glm::mat4(), glm::vec3(-PS/2 +  (j+1)*FIR_SIZE, PL + LPL*COS_45 - (i+1)*FIR_SIZE, LPL*COS_45));
            createFir(systemRenderable, translateM, flatShader);
        }
    }

    glm::vec3 px,pv;
    float pm, pr;
    //Particle vs Plane collision
    {

        //Initialize a particle with position, velocity, mass and radius and add it to the system
        px = glm::vec3(0.0, -PL, 1.0);
        pv = glm::vec3(0.0, 0.0, 0.0);
        pr = 0.1;
        pm = 1.0;
        ParticlePtr particle = std::make_shared<Particle>(px, pv, pm, pr);
        system->addParticle(particle);

        //Create a particleRenderable for each particle of the system
        //DynamicSystemRenderable act as a hierarchical renderable
        //This which allows to easily apply transformation on the visualiazation of a dynamicSystem
        SnowballParticleRenderablePtr particleRenderable = std::make_shared<SnowballParticleRenderable>(flatShader, particle);
        HierarchicalRenderable::addChild(systemRenderable, particleRenderable);
    }

    //Particle vs Particle collision
    {
        // Initialization snow

        std::string objFilename = "../meshes/farmhouse.obj";
        std::string texFilename = "../textures/farmhouse_texture.jpg";
        TexturedMeshRenderablePtr house = std::make_shared<TexturedMeshRenderable>(texShader, objFilename, texFilename);
        scaleTransformation = glm::rotate(glm::mat4(1.0), (float) (M_PI/2.0),glm::vec3(1.0, 0.0, 0.0)) * glm::translate(glm::mat4(1.0), glm::vec3(-15.0, LPL*COS_45, -11.0)) * glm::scale(glm::mat4(1.0), glm::vec3(SNOWBALL_RADIUS, SNOWBALL_RADIUS, SNOWBALL_RADIUS));
        scaleTransformation = glm::rotate(scaleTransformation, (float) (M_PI/2.0),glm::vec3(0.0, -1.0, 0.0));
        //meshBall->setLocalTransform(scaleTransformation);
        //house->setMaterial(pearl);
        house->setParentTransform(scaleTransformation);
        HierarchicalRenderable::addChild(systemRenderable, house);
        //viewer.addRenderable(house);


        objFilename = "../meshes/sphere.obj";
        LightedMeshRenderablePtr meshBall = std::make_shared<LightedMeshRenderable>(flatShader, objFilename, pearl);
        scaleTransformation = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -PL + 5.0, 3.0)) * glm::scale(glm::mat4(1.0), glm::vec3(SNOWBALL_RADIUS, SNOWBALL_RADIUS, SNOWBALL_RADIUS));
        meshBall->setParentTransform(scaleTransformation);
        HierarchicalRenderable::addChild(systemRenderable, meshBall);
        viewer.addRenderable(meshBall);

        int nb_snowball = 100;
        SnowballParticleRenderablePtr snowBallRenderable;
        for (int i = 1; i < nb_snowball; i++) {
            px = glm::vec3(frand_a_b(-PS, PS), frand_a_b(-PS, PS), frand_a_b(0.0, 50.0));
            pv = glm::vec3(frand_a_b(-WIND, WIND), frand_a_b(-WIND, WIND), -10.0);
            pr = frand_a_b(0, SNOWBALL_RADIUS);
            pm = frand_a_b(0, 0.05);
            ParticlePtr snowball = std::make_shared<Particle>(px, pv, pm, pr);
            //snowball->setForce(glm::vec3(0.0, 0.0, 0.0));
            system->addParticle(snowball);
            snowBallRenderable = std::make_shared<SnowballParticleRenderable>(flatShader, snowball);
            HierarchicalRenderable::addChild(systemRenderable, snowBallRenderable);
        }
        // End Snow
    }

    //Initialize a force field that apply to all the particles of the system to simulate gravity
    //Add it to the system as a force field
    // ConstantForceFieldPtr gravityForceField = std::make_shared<ConstantForceField>(system->getParticles(), glm::vec3{0,0,-10} );
    // system->addForceField(gravityForceField);
}
