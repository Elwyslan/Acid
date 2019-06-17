#include "MaterialSkybox.hpp"

#include "Models/VertexDefault.hpp"
#include "Scenes/Scenes.hpp"

namespace acid
{
MaterialSkybox::MaterialSkybox(std::shared_ptr<ImageCube> image, const Colour &baseColour) :
	m_image{std::move(image)},
	m_baseColour{baseColour},
	m_blend{1.0f},
	m_fogLimits{-10000.0f}
{
}

void MaterialSkybox::Start()
{
	m_pipelineMaterial = PipelineMaterial::Create({1, 0}, {{"Shaders/Skyboxes/Skybox.vert", "Shaders/Skyboxes/Skybox.frag"}, {VertexDefault::GetVertexInput()}, {}, 
		PipelineGraphics::Mode::Mrt, PipelineGraphics::Depth::None, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT});
}

void MaterialSkybox::Update()
{
}

void MaterialSkybox::PushUniforms(UniformHandler &uniformObject)
{
	if (auto transform{GetEntity()->GetComponent<Transform>()}; transform != nullptr)
	{
		uniformObject.Push("transform", transform->GetWorldMatrix());
		uniformObject.Push("fogLimits", transform->GetScale().m_y * m_fogLimits);
	}

	uniformObject.Push("baseColour", m_baseColour);
	uniformObject.Push("fogColour", m_fogColour);
	uniformObject.Push("blendFactor", m_blend);
}

void MaterialSkybox::PushDescriptors(DescriptorsHandler &descriptorSet)
{
	descriptorSet.Push("samplerColour", m_image);
}

const Metadata &operator>>(const Metadata &metadata, MaterialSkybox &material)
{
	metadata.GetChild("image", material.m_image);
	metadata.GetChild("baseColour", material.m_baseColour);
	return metadata;
}

Metadata &operator<<(Metadata &metadata, const MaterialSkybox &material)
{
	metadata.SetChild("image", material.m_image);
	metadata.SetChild("baseColour", material.m_baseColour);
	return metadata;
}
}
