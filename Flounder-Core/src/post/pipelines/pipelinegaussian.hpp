#pragma once

#include "../filters/filterblurhorizontal.hpp"
#include "../filters/filterblurvertical.hpp"
#include "../ipostpipeline.hpp"

namespace flounder
{
	class pipelinegaussian :
		public ipostpipeline
	{
	private:
		filterblurhorizontal *m_filterBlurHorizontal;
		filterblurvertical *m_filterBlurVertical;
	public:
		pipelinegaussian(const int &width, const int &height);

		pipelinegaussian(const float &sizeScalar);

		~pipelinegaussian();

		void renderPipeline(const int n_args, va_list args) override;

		fbo *getOutput() override;

		void setScaleValue(const float &scaleValue);
	};
}