#ifndef KEYFRAMED_CYLINDER_RENDERABLE_HPP
#define KEYFRAMED_CYLINDER_RENDERABLE_HPP

#include "../texturing/TexturedCylinderRenderable.hpp"
#include "KeyframeCollection.hpp"

#include <glm/glm.hpp>

class GeometricTransformation;

class KeyframedCylinderRenderable : public TexturedCylinderRenderable
{
public:
    KeyframedCylinderRenderable(ShaderProgramPtr program, const std::string& textureFilename);
    ~KeyframedCylinderRenderable();

    void addParentTransformKeyframe(float time, const GeometricTransformation& transformation);

    void addLocalTransformKeyframe(float time, const GeometricTransformation& transformation);

protected:
    void do_animate(float time);

private:
    KeyframeCollection m_localKeyframes; 
    KeyframeCollection m_parentKeyframes; 
};

typedef std::shared_ptr<KeyframedCylinderRenderable> KeyframedCylinderRenderablePtr;

#endif
