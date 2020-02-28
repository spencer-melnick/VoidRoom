#include "WaveGenerator.h"

#include "Containers/DynamicRHIResourceArray.h"
#include "RenderingThread.h"
#include "RenderGraphBuilder.h"
#include "PixelShaderUtils.h"

// #include "BoxMullerShader.h"
// #include "InitialComponentsShader.h"
#include "TestShader.h"

void FWaveGenerator::Initialize(FIntPoint Dimensions)
{
	BufferSize = Dimensions;
	FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(BufferSize, FTestShader::ThreadsPerGroupDimension);
	
	ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)
	([this, GroupCount]
	(FRHICommandListImmediate& CommandListImmediate)
	{
		FRDGBuilder GraphBuilder(CommandListImmediate);

		FRDGTextureDesc OutputTextureDesc = FRDGTextureDesc::Create2DDesc(BufferSize, EPixelFormat::PF_FloatRGBA, FClearValueBinding::BlackMaxAlpha,
			TexCreate_None,	TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable, false);
		FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(OutputTextureDesc, TEXT("Test Texture"));

		FTestShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FTestShader::FParameters>();
		PassParameters->OutputTexture = GraphBuilder.CreateUAV(OutputTexture);
		PassParameters->OutputTextureSize = BufferSize;

		TShaderMapRef<FTestShader> TestShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
		
		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Test Shader"), *TestShader,
			PassParameters, GroupCount);

		GraphBuilder.Execute();
	});
}

