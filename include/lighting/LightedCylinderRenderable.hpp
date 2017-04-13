#ifndef LIGHTED_CYLINDER_RENDERABLE_HPP
#define LIGHTED_CYLINDER_RENDERABLE_HPP

#include "./../lighting/Materiable.hpp"
#include "./../CylinderRenderable.hpp"

class LightedCylinderRenderable : public formes::CylinderRenderable, public Materiable
{
public:
    ~LightedCylinderRenderable();
    LightedCylinderRenderable(ShaderProgramPtr program, const MaterialPtr& material);

protected:
    void do_draw();
    void do_animate(float time);
};

typedef std::shared_ptr<LightedCylinderRenderable> LightedCylinderRenderablePtr;

#endif
