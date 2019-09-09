#pragma once

#include "Maths/Matrix4.hpp"

namespace acid
{
/**
 * @brief Class that represents a joint in a "skeleton". It contains the index of the joint which determines where in the vertex shader uniform array the joint matrix for this joint is loaded up to.
 * It also contains the name of the bone, and a list of all the child joints.
 *
 * The "animatedTransform" matrix is the joint transform.
 * This is the transform that gets loaded up to the vertex shader and is used to transform vertices.
 * It is a model-space transform that transforms the joint from it's bind (original position, no animation applied) position to it's current position in the current pose.
 * Changing this transform changes the position/rotation of the joint in the animated entity.
 *
 * The two other matrices are transforms that are required to calculate the "animatedTransform" in the <seealso cref="Animator"/> class.
 * It also has the local bind transform which is the original (no pose/animation applied) transform of the joint relative to the parent joint (in bone-space).
 *
 * The "localBindTransform" is the original (bind) transform of the joint relative to its parent (in bone-space).
 * The "inverseBindTransform" is that bind transform in model-space, but inversed.
 **/
class ACID_EXPORT Joint
{
public:
	/**
	 * Creates a new skeleton joint.
	 **/
	Joint() = default;

	/**
	 * Creates a new skeleton joint.
	 * @param index The joint's index (ID).
	 * @param name The name of the joint. This is how the joint is named in the collada file, and so is used to identify which joint a joint transform in an animation keyframe refers to.
	 * @param bindLocalTransform The bone-space transform of the joint in the bind position.
	 **/
	Joint(uint32_t index, std::string name, const Matrix4 &bindLocalTransform);

	/**
	 * This is called during set-up, after the joints hierarchy has been created. This calculates the model-space bind transform of this joint like so:
	 *
	 * {@code bindTransform = parentBindTransform * localBindTransform}
	 *
	 * Where "bindTransform" is the model-space bind transform of this joint, "parentBindTransform" is the model-space bind transform of the parent joint,
	 * and "localBindTransform" is the bone-space bind transform of this joint. It the calculates and stores the inverse of this model-space bind transform,
	 * for use when calculating the final animation transform each frame. It then recursively calls the method for all of the children joints,
	 * so that they too calculate and store their inverse bind-pose transform.
	 * @param parentBindTransform The model-space bind transform of the parent joint.
	 **/
	void CalculateInverseBindTransform(const Matrix4 &parentBindTransform);

	uint32_t GetIndex() const { return m_index; }

	void SetIndex(uint32_t index) { m_index = index; }

	const std::string &GetName() const { return m_name; }

	void SetName(const std::string &name) { m_name = name; }

	const std::vector<Joint> &GetChildren() const { return m_children; }

	/**
	 * Adds a child joint to this joint. Used during the creation of the joint hierarchy. Joints can have multiple children,
	 * which is why they are stored in a list (e.g. a "hand" joint may have multiple "finger" children joints).
	 * @param child The new child joint of this joint.
	 **/
	void AddChild(const Joint &child);

	const Matrix4 &GetLocalBindTransform() const { return m_localBindTransform; }

	void SetLocalBindTransform(const Matrix4 &localBindTransform) { m_localBindTransform = localBindTransform; }

	/**
	 * This returns the inverted model-space bind transform.
	 * The bind transform is the original model-space transform of the joint (when no animation is applied).
	 * This returns the inverse of that, which is used to calculate the animated transform matrix which gets used to transform vertices in the shader.
	 * @return The inverse of the joint's bind transform (in model-space).
	 **/
	const Matrix4 &GetInverseBindTransform() const { return m_inverseBindTransform; }

	void SetInverseBindTransform(const Matrix4 &inverseBindTransform) { m_inverseBindTransform = inverseBindTransform; }

	friend const Node &operator>>(const Node &node, Joint &joint);

	friend Node &operator<<(Node &node, const Joint &joint);

private:
	uint32_t m_index{};
	std::string m_name;
	std::vector<Joint> m_children;

	Matrix4 m_localBindTransform;
	Matrix4 m_inverseBindTransform;
};
}
