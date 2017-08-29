#pragma once

#include "../renderer/irenderer.hpp"

#include "skyboxes.hpp"

namespace flounder
{
	class rendererskyboxes :
		public irenderer
	{
	private:
		shader *m_shader;
	public:
		rendererskyboxes();

		~rendererskyboxes();

		void render(const vector4 &clipPlane, const icamera &camera) override;
	private:
		void prepareRendering(const vector4 &clipPlane, const icamera &camera);

		void renderSkybox(skybox *object);

		void endRendering();
	};
}