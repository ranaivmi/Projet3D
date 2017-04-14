#ifndef CONTROLLED_FORCE_CANON_RENDERABLE_HPP
#define CONTROLLED_FORCE_CANON_RENDERABLE_HPP

#include "../HierarchicalRenderable.hpp"
#include "../dynamics/ConstantForceField.hpp"

#include "../dynamics/DynamicSystem.hpp"
#include "../dynamics/DynamicSystem.hpp"
#include "./../../include/Viewer.hpp"
#include "./../../include/dynamics_rendering/DynamicSystemRenderable.hpp"

/**@brief Status of a ControlledForceCanon.
 *
 * This status holds important variables that are used to control a force
 * field thanks to user inputs. You can (and should) adapt this class
 * and its default value to the kind of behavior you want in your project.
 */
class ControlledForceCanonStatus
{
public:
    ControlledForceCanonStatus();
    ControlledForceCanonStatus(const glm::vec3& initial_direction);
    ~ControlledForceCanonStatus();
    void clear();

public:
    glm::vec3 initial;
    glm::vec3 movement;
    float angle;
    float last_time;
    float intensity;
    float min_intensity;
    float max_intensity;
    float acceleration;
    float deacceleration;
    float angularSpeed;
    float dampingFactor;

    bool accelerating;
    bool deaccelerating;
    bool turning_left;
    bool turning_right;

    // Rana
    float angle2;
    float angularSpeed2;
    bool move_high;
    bool move_down;

};


/**@brief Implement a force field controlled by user input.
 *
 * This class is an example of what you could do to control a dynamic component
 * (e.g. the kart in your project) thanks to user inputs. This is done by modifying
 * the force of a constant force field applied to the mobile(s) you want to control.
 *
 * We derive from a renderable to be able to react to user input. This might look
 * nasty, but since you dealt with renderables during all previous praticales, we think
 * this is the easiest way to do so. We derive from a hierarchical renderable to be able
 * to use the same local frame as the dynamic system using the force field (see DynamicSystemRenderable).
 */
class ControlledForceCanonRenderable : public HierarchicalRenderable
{
public:
  /**@brief Build a new controlled force field renderable.
   *
   * Build a new controlled force field by user inputs.
   * @param program The shader program used to render the force applied to particles.
   * @param forceCanon The force field to control with user inputs.
   */
  ControlledForceCanonRenderable(ShaderProgramPtr program,ConstantForceFieldPtr forceCanon, DynamicSystemPtr system, DynamicSystemRenderablePtr systemRenderable);
  ~ControlledForceCanonRenderable();

private:

  virtual void do_keyPressedEvent(sf::Event& e);
  virtual void do_keyReleasedEvent(sf::Event& e);
  virtual void do_animate(float time);
  virtual void do_draw();

  ControlledForceCanonStatus m_status;
  ConstantForceFieldPtr m_force;
  DynamicSystemPtr m_system;
  ShaderProgramPtr m_program;
  DynamicSystemRenderablePtr m_systemRenderable;

  std::vector<glm::vec3> m_positions;
  std::vector<glm::vec4> m_colors;
  std::vector<glm::vec3> m_normals;

  unsigned int m_pBuffer;
  unsigned int m_cBuffer;
  unsigned int m_nBuffer;
};

typedef std::shared_ptr<ControlledForceCanonRenderable> ControlledForceCanonRenderablePtr;

#endif //CONTROLLED_FORCE_CANON_RENDERABLE_HPP
