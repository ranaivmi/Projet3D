#include "../include/Viewer.hpp"
#include "../include/FrameRenderable.hpp"

#include "../include/dynamics/DynamicSystem.hpp"
#include "../include/dynamics/DampingForceField.hpp"
#include "../include/dynamics/ConstantForceField.hpp"
#include "../include/dynamics/SpringForceField.hpp"
#include "../include/dynamics/EulerExplicitSolver.hpp"
#include "../include/CylinderRenderable.hpp"

#include "../include/dynamics_rendering/DynamicSystemRenderable.hpp"
#include "../include/dynamics_rendering/ParticleRenderable.hpp"
#include "../include/dynamics_rendering/ParticleListRenderable.hpp"
#include "../include/dynamics_rendering/ConstantForceFieldRenderable.hpp"
#include "../include/dynamics_rendering/SpringForceFieldRenderable.hpp"
#include "../include/dynamics_rendering/SpringListRenderable.hpp"
#include "../include/dynamics_rendering/ControlledForceFieldRenderable.hpp"
#include "../include/dynamics_rendering/QuadRenderable.hpp"

#include <cstdlib>

void practical07_particles(Viewer& viewer,
    DynamicSystemPtr& system, DynamicSystemRenderablePtr& systemRenderable);

void practical07_springs(Viewer& viewer,
    DynamicSystemPtr& system, DynamicSystemRenderablePtr& systemRenderable);

void practical07_collisions(Viewer& viewer,
    DynamicSystemPtr& system, DynamicSystemRenderablePtr& systemRenderable);

void practical07_playPool(Viewer& viewer,
    DynamicSystemPtr& system, DynamicSystemRenderablePtr& systemRenderable);

void snow_scene(Viewer& viewer,
    DynamicSystemPtr& system, DynamicSystemRenderablePtr& systemRenderable);

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
        case 71:
            practical07_particles(viewer, system, systemRenderable);
            break;
        case 72:
            practical07_springs(viewer, system, systemRenderable);
            break;
        case 73:
            practical07_collisions(viewer, system, systemRenderable);
            break;
        case 74:
            practical07_playPool(viewer, system, systemRenderable);
            break;
        case 75:
            snow_scene(viewer, system, systemRenderable);
            break;
        default:
            break;
    }

    //Finally, run the animation
    viewer.startAnimation();
}


void practical07_particles(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable)
{
    //Position the camera
    viewer.getCamera().setViewMatrix(
        glm::lookAt(glm::vec3(0, -4, 0), glm::vec3(0,0,0), glm::vec3(0,0,1)) );

    //Initialize a shader for the following renderables
    ShaderProgramPtr flatShader
        = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl","../shaders/flatFragment.glsl");
    viewer.addShaderProgram(flatShader);

    //We diminish the time step to be able to see what happens before particles go too far
    system->setDt(5e-4);

    //Deactivate collision detection
    system->setCollisionsDetection(false);

    glm::vec3 px,pv;
    float pm, pr;
    //Two particles with gravity and damping
    {
        //Initialize each particle with its position, velocity, mass and radius
        //and add it to the system
        px = glm::vec3(0.0, 0.0, 1.0);
        pv = glm::vec3(3.0, 0.0, 0.0);
        pr = 0.1;
        pm = 1.0;
        ParticlePtr particle1 = std::make_shared<Particle>(px, pv, pm, pr);
        system->addParticle(particle1);

        px = glm::vec3(0.0, 0.0, 1.5);
        pv = glm::vec3(6.0, 0.0, 0.0);
        pr = 0.1;
        pm = 1.0;
        ParticlePtr particle2 = std::make_shared<Particle>(px, pv, pm, pr);
        system->addParticle(particle2);

        //Initialize a force field that apply to all the particles of the system to simulate gravity
        //Add it to the system as a force field
        ConstantForceFieldPtr gravityForceField
            = std::make_shared<ConstantForceField>(system->getParticles(), glm::vec3{0, 0, -10} );
        system->addForceField(gravityForceField);

        //Create a particleRenderable for each particle of the system
        //DynamicSystemRenderable act as a hierarchical renderable
        //This allows easily applying a transformation on the visualization of a dynamicSystem
        ParticleRenderablePtr particleRenderable1
            = std::make_shared<ParticleRenderable>(flatShader, particle1);
        HierarchicalRenderable::addChild(systemRenderable, particleRenderable1);
        ParticleRenderablePtr particleRenderable2
            = std::make_shared<ParticleRenderable>(flatShader, particle2);
        HierarchicalRenderable::addChild(systemRenderable, particleRenderable2);
    }
}


void practical07_springs(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable)
{
    //Position the camera
    viewer.getCamera().setViewMatrix(
        glm::lookAt(glm::vec3(-10, 0, 2), glm::vec3(0,0,0), glm::vec3(0,0,1)) );

    //Initialize a shader for the following renderables
    ShaderProgramPtr flatShader
        = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl","../shaders/flatFragment.glsl");
    viewer.addShaderProgram(flatShader);

    //Initialize particle attributes (radius, mass)
    float pr = 0.1, pm = 10.0;
    glm::vec3 px(0.0,0.0,0.0), pv(0.0,0.0,0.0);

    //Create particles on a squared uniform grid starting at origin
    std::vector<ParticlePtr> particles;
    glm::vec3 origin(0,-2.0,1.0), displacement(0.0,0.0,0.0);
    int particlePerLine = 11;
    float gridWidth=4.0, gridHeight=4.0;
    float ystep = gridWidth / (float)(particlePerLine-1);
    float zstep = gridHeight / (float)(particlePerLine-1);
    particles.resize(particlePerLine*particlePerLine);
    for (size_t i = 0; i < particlePerLine; ++ i) {
        for (size_t j = 0; j < particlePerLine; ++ j) {
            displacement = glm::vec3(0, i*ystep, j*zstep);
            px = origin + displacement;
            particles[i*particlePerLine+j] = std::make_shared<Particle>( px, pv, pm, pr );
            system->addParticle( particles[i*particlePerLine+j] );
        }
    }

    //Fix particles on the y-borders
    for (size_t j = 0; j < particlePerLine; ++j) {
        particles[0 * particlePerLine + j]->setFixed(true);
        particles[(particlePerLine - 1) * particlePerLine + j]->setFixed(true);
    }

    //Initialize springs attributes (stiffness, rest length, damping)
    float stiffness = 4e3;
    float l0 = gridWidth / (particlePerLine-1);
    float damping = 0.0;

    //Create springs between particles of the grid, horizontally and vertically
    //Store them in a list
    std::list<SpringForceFieldPtr> springForceFields;
    //Interior
    for (size_t i = 1; i < particlePerLine; ++ i) {
        for (size_t j = 1; j < particlePerLine; ++ j) {
            SpringForceFieldPtr spring1 = std::make_shared<SpringForceField>( particles[(i-1)*particlePerLine+j], particles[i*particlePerLine+j], stiffness, l0, damping );
            springForceFields.push_back(spring1);
            system->addForceField(spring1);
            SpringForceFieldPtr spring2 = std::make_shared<SpringForceField>( particles[i*particlePerLine+(j-1)], particles[i*particlePerLine+j], stiffness, l0, damping );
            springForceFields.push_back(spring2);
            system->addForceField(spring2);
        }
    }
    //Border case 1
    for (size_t j = 1; j < particlePerLine; ++ j) {
        SpringForceFieldPtr spring = std::make_shared<SpringForceField>( particles[0*particlePerLine+j], particles[0*particlePerLine+(j-1)], stiffness, l0, damping );
        springForceFields.push_back(spring);
        system->addForceField(spring);
    }
    //Border case 2
    for (size_t i = 1; i < particlePerLine; ++ i) {
        SpringForceFieldPtr spring = std::make_shared<SpringForceField>( particles[(i-1)*particlePerLine+0], particles[i*particlePerLine+0], stiffness, l0, damping );
        springForceFields.push_back(spring);
        system->addForceField(spring);
    }

    //Initialize a force field that apply to all the particles of the system to simulate gravity
    //Add it to the system as a force field
    ConstantForceFieldPtr gravityForceField = std::make_shared<ConstantForceField>(system->getParticles(), glm::vec3{0, 0, -10});
    system->addForceField(gravityForceField);

    //Initialize a force field that apply to all the particles of the system to simulate vicosity (air friction)
    float dampingCoefficient = 0.0;
    DampingForceFieldPtr dampingForceField = std::make_shared<DampingForceField>(system->getParticles(), dampingCoefficient);
    system->addForceField(dampingForceField);


    //Create a particleListRenderable to efficiently visualize the particles of the system
    ParticleListRenderablePtr particleListRenderable = std::make_shared<ParticleListRenderable>( flatShader, particles);
    HierarchicalRenderable::addChild(systemRenderable, particleListRenderable);

    //Create a springListRenderable to efficiently visualize the springs of the system
    SpringListRenderablePtr springsRenderable = std::make_shared<SpringListRenderable>(flatShader, springForceFields);
    HierarchicalRenderable::addChild(systemRenderable, springsRenderable);

    //Display gravity
    ConstantForceFieldRenderablePtr gravityRenderable = std::make_shared<ConstantForceFieldRenderable>(flatShader, gravityForceField);
    HierarchicalRenderable::addChild(systemRenderable, gravityRenderable);
}


void practical07_collisions(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable)
{
    //Position the camera
    viewer.getCamera().setViewMatrix(
        glm::lookAt(glm::vec3(0, 2, 2), glm::vec3(0,0,0), glm::vec3(0,0,1)) );

    //Initialize a shader for the following renderables
    ShaderProgramPtr flatShader
        = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl","../shaders/flatFragment.glsl");
    viewer.addShaderProgram(flatShader);

    //Activate collision detection
    system->setCollisionsDetection(true);

    //Initialize the restitution coefficient for collision
    //1.0 = full elastic response
    //0.0 = full absorption
    system->setRestitution(0.6f);

    //Initialize a plane from 3 points and add it to the system as an obstacle
    glm::vec3 p1(-1.0, -1.0, 0.0);
    glm::vec3 p2(1.0, -1.0, 0.0);
    glm::vec3 p3(1.0, 1.0, 0.0);
    glm::vec3 p4(-1.0, 1.0, 0.0);
    PlanePtr plane = std::make_shared<Plane>(p1, p2, p3);
    system->addPlaneObstacle(plane);

    //Create a plane renderable to display the obstacle
    PlaneRenderablePtr planeRenderable = std::make_shared<QuadRenderable>(flatShader, p1,p2,p3,p4);
    HierarchicalRenderable::addChild( systemRenderable, planeRenderable );

    glm::vec3 px,pv;
    float pm, pr;
    //Particle vs Plane collision
    {

        //Initialize a particle with position, velocity, mass and radius and add it to the system
        px = glm::vec3(0.0, 0.0, 1.0);
        pv = glm::vec3(0.0, 0.0, 0.0);
        pr = 0.1;
        pm = 1.0;
        ParticlePtr particle = std::make_shared<Particle>(px, pv, pm, pr);
        system->addParticle(particle);

        //Create a particleRenderable for each particle of the system
        //DynamicSystemRenderable act as a hierarchical renderable
        //This which allows to easily apply transformation on the visualiazation of a dynamicSystem
        ParticleRenderablePtr particleRenderable = std::make_shared<ParticleRenderable>(flatShader, particle);
        HierarchicalRenderable::addChild(systemRenderable, particleRenderable);
    }

    //Particle vs Particle collision
    {
        //Initialize two particles with position, velocity, mass and radius and add it to the system
        //One of the particle is fixed
        px = glm::vec3(0.5, 0.0, 0.1);
        pv = glm::vec3(0.0, 0.0, 0.0);
        pr = 0.1;
        pm = 1000.0;
        ParticlePtr particle1 = std::make_shared<Particle>(px, pv, pm, pr);
        particle1->setFixed(true);
        system->addParticle(particle1);

        px = glm::vec3(0.5, 0.0, 1.0);
        pv = glm::vec3(0.0, 0.0, -0.5);
        pr = 0.1;
        pm = 1.0;
        ParticlePtr particle2 = std::make_shared<Particle>(px, pv, pm, pr);
        system->addParticle(particle2);

        //Create a particleRenderable for each particle of the system
        ParticleRenderablePtr particleRenderable1 = std::make_shared<ParticleRenderable>(flatShader, particle1);
        HierarchicalRenderable::addChild(systemRenderable, particleRenderable1);
        ParticleRenderablePtr particleRenderable2 = std::make_shared<ParticleRenderable>(flatShader, particle2);
        HierarchicalRenderable::addChild(systemRenderable, particleRenderable2);
    }

    //Initialize a force field that apply to all the particles of the system to simulate gravity
    //Add it to the system as a force field
    ConstantForceFieldPtr gravityForceField = std::make_shared<ConstantForceField>(system->getParticles(), glm::vec3{0,0,-10} );
    system->addForceField(gravityForceField);
}


void practical07_playPool(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr& systemRenderable)
{
    //Initialize a shader for the following renderables
    ShaderProgramPtr flatShader
        = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl","../shaders/flatFragment.glsl");
    viewer.addShaderProgram(flatShader);

    //Position the camera
    viewer.getCamera().setViewMatrix(
        glm::lookAt(glm::vec3(0, -15, 15), glm::vec3(0,0,0), glm::vec3(0,0,1)) );

    //Initialize two particles with position, velocity, mass and radius and add it to the system
    glm::vec3 px(0.0, 0.0, 0.0);
    glm::vec3 pv(0.0, 0.0, 0.0);
    float pm = 1.0, pr = 1.0;
    px = glm::vec3(0.0,0.0,1.0);
    ParticlePtr mobile = std::make_shared<Particle>( px, pv, pm, pr);
    system->addParticle( mobile );
    px = glm::vec3(0.0,5.0,1.0);
    ParticlePtr other = std::make_shared<Particle>( px, pv, pm, pr);
    system->addParticle( other );

    //Create a particleRenderable for each particle of the system
    //Add them to the system renderable
    ParticleRenderablePtr mobileRenderable = std::make_shared<ParticleRenderable>(flatShader, mobile);
    HierarchicalRenderable::addChild(systemRenderable, mobileRenderable);
    ParticleRenderablePtr otherRenderable = std::make_shared<ParticleRenderable>(flatShader, other);
    HierarchicalRenderable::addChild(systemRenderable, otherRenderable);

    //Initialize four planes to create walls arround the particles
    glm::vec3 planeNormal, planePoint;
    planeNormal = glm::vec3(-1, 0, 0);
    planePoint = glm::vec3(10, 0, 0);
    PlanePtr p0 = std::make_shared<Plane>(planeNormal, planePoint);
    system->addPlaneObstacle(p0);

    planeNormal = glm::vec3(1, 0, 0);
    planePoint = glm::vec3(-10, 0, 0);
    PlanePtr p1 = std::make_shared<Plane>(planeNormal, planePoint);
    system->addPlaneObstacle(p1);

    planeNormal = glm::vec3(0, -1, 0);
    planePoint = glm::vec3(0, 10, 0);
    PlanePtr p2 = std::make_shared<Plane>(planeNormal, planePoint);
    system->addPlaneObstacle(p2);

    planeNormal = glm::vec3(0, 1, 0);
    planePoint = glm::vec3(0, -10, 0);
    PlanePtr p3 = std::make_shared<Plane>(planeNormal, planePoint);
    system->addPlaneObstacle(p3);

    planeNormal = glm::vec3(0, 0, 1);
    planePoint = glm::vec3(0, 0, 0);
    PlanePtr floor = std::make_shared<Plane>( planeNormal, planePoint);
    system->addPlaneObstacle(floor);

    //Create  plane renderables to display each obstacle
    //Add them to the system renderable
    glm::vec3 x1, x2, x3, x4;
    glm::vec4 color;
    x1 = glm::vec3(10,  10, 5);
    x2 = glm::vec3(10,  10, 0);
    x3 = glm::vec3(10, -10, 0);
    x4 = glm::vec3(10, -10, 5);
    color = glm::vec4(0.4, 0.2, 0.2, 1.0);
    PlaneRenderablePtr p1Renderable = std::make_shared<QuadRenderable>(flatShader, x1, x2, x3, x4, color);
    HierarchicalRenderable::addChild(systemRenderable, p1Renderable);

    x1 = glm::vec3(-10, 10, 5);
    x2 = glm::vec3(-10, 10, 0);
    x3 = glm::vec3( 10, 10, 0);
    x4 = glm::vec3( 10, 10, 5);
    color = glm::vec4(0.4, 0.2, 0.2, 1.0);
    PlaneRenderablePtr p2Renderable = std::make_shared<QuadRenderable>(flatShader, x1, x2, x3, x4, color);
    HierarchicalRenderable::addChild(systemRenderable, p2Renderable);

    x1 = glm::vec3(-10, -10, 5);
    x2 = glm::vec3(-10, -10, 0);
    x3 = glm::vec3(-10,  10, 0);
    x4 = glm::vec3(-10,  10, 5);
    color = glm::vec4(0.2, 0.4, 0.4, 1.0);
    PlaneRenderablePtr p3Renderable = std::make_shared<QuadRenderable>(flatShader, x1, x2, x3, x4, color);
    HierarchicalRenderable::addChild(systemRenderable, p3Renderable);

    x1 = glm::vec3( 10, -10, 5);
    x2 = glm::vec3( 10, -10, 0);
    x3 = glm::vec3(-10, -10, 0);
    x4 = glm::vec3(-10, -10, 5);
    color = glm::vec4(0.2, 0.4, 0.4, 1.0);
    PlaneRenderablePtr p4Renderable = std::make_shared<QuadRenderable>(flatShader, x1, x2, x3, x4, color);
    HierarchicalRenderable::addChild(systemRenderable, p4Renderable);

    //Initialize a force field that apply only to the mobile particle
    glm::vec3 nullForce(0.0, 0.0, 0.0);
    std::vector<ParticlePtr> vParticle;
    vParticle.push_back(mobile);
    ConstantForceFieldPtr force = std::make_shared<ConstantForceField>(vParticle, nullForce);
    system->addForceField(force);

    //Initialize a renderable for the force field applied on the mobile particle.
    //This renderable allows to modify the attribute of the force by key/mouse events
    //Add this renderable to the systemRenderable.
    ControlledForceFieldRenderablePtr forceRenderable = std::make_shared<ControlledForceFieldRenderable>(flatShader, force);
    HierarchicalRenderable::addChild(systemRenderable, forceRenderable);

    //Add a damping force field to the mobile.
    DampingForceFieldPtr dampingForceField = std::make_shared<DampingForceField>(vParticle, 0.9);
    system->addForceField(dampingForceField);

    //Activate collision and set the restitution coefficient to 1.0
    system->setCollisionsDetection(true);
    system->setRestitution(1.0f);
}

double frand_a_b(double a, double b){
    return ( rand()/(double)RAND_MAX ) * (b-a) + a;
}

void snow_scene(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable)
{
    //Position the camera
    viewer.getCamera().setViewMatrix(
        glm::lookAt(glm::vec3(0, 2, 2), glm::vec3(0,0,0), glm::vec3(0,0,1)) );

    //Initialize a shader for the following renderables
    ShaderProgramPtr flatShader
        = std::make_shared<ShaderProgram>("../shaders/flatVertex.glsl","../shaders/flatFragment.glsl");
    viewer.addShaderProgram(flatShader);

    system->setDt(0.00125);

    //Activate collision detection
    system->setCollisionsDetection(true);

    //Initialize the restitution coefficient for collision
    //1.0 = full elastic response
    //0.0 = full absorption
    system->setRestitution(0.6f);

    //Initialize a plane from 3 points and add it to the system as an obstacle
    glm::vec3 p1(-1.0, -1.0, 0.0);
    glm::vec3 p2(1.0, -1.0, 0.0);
    glm::vec3 p3(1.0, 1.0, 0.0);
    glm::vec3 p4(-1.0, 1.0, 0.0);
    PlanePtr plane = std::make_shared<Plane>(p1, p2, p3);
    system->addPlaneObstacle(plane);

    //Create a plane renderable to display the obstacle
    PlaneRenderablePtr planeRenderable = std::make_shared<QuadRenderable>(flatShader, p1,p2,p3,p4);
    HierarchicalRenderable::addChild( systemRenderable, planeRenderable );

    glm::vec3 px,pv;
    float pm, pr;
    //Particle vs Plane collision
    {

        //Initialize a particle with position, velocity, mass and radius and add it to the system
        px = glm::vec3(0.0, 0.0, 1.0);
        pv = glm::vec3(0.0, 0.0, 0.0);
        pr = 0.1;
        pm = 1.0;
        ParticlePtr particle = std::make_shared<Particle>(px, pv, pm, pr);
        system->addParticle(particle);

        //Create a particleRenderable for each particle of the system
        //DynamicSystemRenderable act as a hierarchical renderable
        //This which allows to easily apply transformation on the visualiazation of a dynamicSystem
        ParticleRenderablePtr particleRenderable = std::make_shared<ParticleRenderable>(flatShader, particle);
        HierarchicalRenderable::addChild(systemRenderable, particleRenderable);
    }

    //Particle vs Particle collision
    {
        //Initialize two particles with position, velocity, mass and radius and add it to the system
        //One of the particle is fixed
        px = glm::vec3(0.5, 0.0, 0.1);
        pv = glm::vec3(0.0, 0.0, 0.0);
        pr = 0.1;
        pm = 1000.0;
        ParticlePtr particle1 = std::make_shared<Particle>(px, pv, pm, pr);
        particle1->setFixed(true);
        system->addParticle(particle1);

        px = glm::vec3(0.5, 0.0, 1.0);
        pv = glm::vec3(0.0, 0.0, -0.5);
        pr = 0.1;
        pm = 1.0;
        ParticlePtr particle2 = std::make_shared<Particle>(px, pv, pm, pr);
        system->addParticle(particle2);

        //Create a particleRenderable for each particle of the system
        ParticleRenderablePtr particleRenderable1 = std::make_shared<ParticleRenderable>(flatShader, particle1);
        HierarchicalRenderable::addChild(systemRenderable, particleRenderable1);
        ParticleRenderablePtr particleRenderable2 = std::make_shared<ParticleRenderable>(flatShader, particle2);
        HierarchicalRenderable::addChild(systemRenderable, particleRenderable2);

        // Initialization snow
        int nb_snowball = 300;
        ParticleRenderablePtr snowBallRenderable;
        for (int i = 2; i < nb_snowball; i++) {
            px = glm::vec3(frand_a_b(-1.0, 1.0), frand_a_b(-1.0, 1.0), frand_a_b(0.025, 3.0));
            pv = glm::vec3(0.0, 0.0, 0.0);
            pr = frand_a_b(0, 0.025);
            pm = frand_a_b(0, 0.005);
            ParticlePtr snowball = std::make_shared<Particle>(px, pv, pm, pr);
            system->addParticle(snowball);
            snowBallRenderable = std::make_shared<ParticleRenderable>(flatShader, snowball);
            HierarchicalRenderable::addChild(systemRenderable, snowBallRenderable);
        }
        // End Snow
    }
    //La cible 
    glm::mat4 scaleM(1.0);
    glm::mat4 rotationM(1.0);
    std::shared_ptr<formes::CylinderRenderable> Cible
        = std::make_shared<formes::CylinderRenderable>(flatShader, 30);
    scaleM = glm::scale(glm::mat4(), glm::vec3(0.5, 0.5, 0.1));
    rotationM = glm::rotate(glm::mat4(1.0), (float)(M_PI/2.0), glm::vec3(1.0,0.0,0.0));
    Cible->setModelMatrix(rotationM*scaleM);
    viewer.addRenderable(Cible);
    //Initialize a force field that apply to all the particles of the system to simulate gravity
    //Add it to the system as a force field
    ConstantForceFieldPtr gravityForceField = std::make_shared<ConstantForceField>(system->getParticles(), glm::vec3{0,0,-10} );
    system->addForceField(gravityForceField);
}
