#include "BlendingMode.hh"
#include "RasterTests.hh"

namespace Gfx
{
	const BlendingMode BlendingMode::Opaque(BlendEquation::SrcPlusDst,
											BlendEquation::SrcPlusDst,
											BlendFunction::One,
											BlendFunction::One,
											BlendFunction::Zero,
											BlendFunction::Zero);
	const BlendingMode BlendingMode::Translucent(BlendEquation::SrcPlusDst,
												 BlendEquation::SrcPlusDst,
												 BlendFunction::SrcAlpha,
												 BlendFunction::SrcAlpha,
												 BlendFunction::OneMinusSrcAlpha,
												 BlendFunction::OneMinusSrcAlpha);
	const BlendingMode BlendingMode::Additive(BlendEquation::SrcPlusDst,
											  BlendEquation::SrcPlusDst,
											  BlendFunction::SrcAlpha,
											  BlendFunction::SrcAlpha,
											  BlendFunction::One,
											  BlendFunction::One);

	const RasterTests RasterTests::NoDepthTest(FaceCulling::Back, DepthFunction::Always, false);
	const RasterTests RasterTests::RegularDepthTest(FaceCulling::Back, DepthFunction::Less, true);

	const RasterTests RasterTests::TwoSidedNoDepthTest(FaceCulling::None, DepthFunction::Always, false);
	const RasterTests RasterTests::TwoSidedRegularDepthTest(FaceCulling::None, DepthFunction::Less, true);
}
