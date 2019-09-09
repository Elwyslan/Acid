#pragma once

#include "Serialized/Node.hpp"
#include "Joint.hpp"

namespace acid
{
class ACID_EXPORT SkeletonLoader
{
public:
	SkeletonLoader(const Node *libraryControllers, std::vector<std::string> boneOrder, const Matrix4 &correction);

	uint32_t GetJointCount() const { return m_jointCount; }

	const Joint &GetHeadJoint() const { return m_headJoint; }

private:
	Joint LoadJointData(const Node *jointNode, const bool &isRoot);

	Joint ExtractMainJointData(const Node *jointNode, const bool &isRoot);

	std::optional<uint32_t> GetBoneIndex(const std::string &name);

	const Node *m_armatureData{};
	std::vector<std::string> m_boneOrder;
	Matrix4 m_correction;

	uint32_t m_jointCount{};
	Joint m_headJoint;
};
}
