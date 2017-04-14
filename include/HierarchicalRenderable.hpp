#ifndef HIERARCHICAL_RENDERABLE_HPP
#define HIERARCHICAL_RENDERABLE_HPP


#include "Renderable.hpp"
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class HierarchicalRenderable;
typedef std::shared_ptr<HierarchicalRenderable> HierarchicalRenderablePtr;


class HierarchicalRenderable : public Renderable
{
public :
   
    virtual ~HierarchicalRenderable();

    
    HierarchicalRenderable(ShaderProgramPtr shaderProgram);

    static void addChild(HierarchicalRenderablePtr parent, HierarchicalRenderablePtr child);


    std::vector<HierarchicalRenderablePtr> & getChildren();

    void updateModelMatrix();

    glm::mat4 computeTotalParentTransform() const;

    const glm::mat4& getParentTransform() const;

    void setParentTransform( const glm::mat4& parentTransform );

    const glm::mat4& getLocalTransform() const;

    void setLocalTransform(const glm::mat4& localTransform);

    void setModelMatrix(const glm::mat4& model);

private:

    HierarchicalRenderablePtr m_parent;

    std::vector<HierarchicalRenderablePtr> m_children;

    glm::mat4 m_parentTransform;

    glm::mat4 m_localTransform;

    virtual void beforeDraw();

    virtual void afterDraw();

    virtual void afterAnimate( float time );

};
typedef std::shared_ptr<HierarchicalRenderable> HierarchicalRenderablePtr;

#endif
