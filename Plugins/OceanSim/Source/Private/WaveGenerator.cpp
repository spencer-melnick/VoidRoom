#include "WaveGenerator.h"

#include "Containers/DynamicRHIResourceArray.h"
#include "RenderingThread.h"
#include "RenderGraphBuilder.h"
#include "PixelShaderUtils.h"

#include "BoxMullerShader.h"
// #include "InitialComponentsShader.h"
#include "CopyShader.h"

void FWaveGenerator::Initialize(FIntPoint Dimensions)
{
	BufferSize = Dimensions;
	uint32 NumElements = BufferSize.X * BufferSize.Y * 4;
	FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(BufferSize, FCopyShader::ThreadsPerGroupDimension);

	InputData.Init(FFloat16(), sizeof(FFloat16) * 4 * NumElements);

	for (uint32 i = 0; i < NumElements; i++)
	{
		InputData[i] = FMath::FRandRange(0, 1);
	}
	
	ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)
	([this, GroupCount, NumElements]
	(FRHICommandListImmediate& CommandListImmediate)
	{
		FRDGBuilder GraphBuilder(CommandListImmediate);

		FRHIResourceCreateInfo CreateInfo;
		FTexture2DRHIRef UniformNoiseTexture = RHICreateTexture2D(BufferSize.X, BufferSize.Y, PF_FloatRGBA, 1, 1,
			TexCreate_ShaderResource, CreateInfo);
		FShaderResourceViewRHIRef UniformNoiseTextureSRV = RHICreateShaderResourceView(UniformNoiseTexture, 0);

		FTexture2DRHIRef GaussianNoiseTexture = RHICreateTexture2D(BufferSize.X, BufferSize.Y, PF_FloatRGBA, 1, 1,
			TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
		FUnorderedAccessViewRHIRef GaussianNoiseTextureUAV = RHICreateUnorderedAccessView(GaussianNoiseTexture);
		FShaderResourceViewRHIRef GaussianNoiseTextureSRV = RHICreateShaderResourceView(GaussianNoiseTexture, 0);

		// Copy uniform noise data to texture
		uint32 Stride;
		FFloat16* TextureData = static_cast<FFloat16*>(RHILockTexture2D(UniformNoiseTexture, 0, EResourceLockMode::RLM_WriteOnly, Stride, false));
		FMemory::Memcpy(TextureData, InputData.GetData(), NumElements * sizeof(FFloat16));
		RHIUnlockTexture2D(UniformNoiseTexture, 0, false);

		FRDGTextureDesc OutputTextureDesc = FRDGTextureDesc::Create2DDesc(BufferSize, EPixelFormat::PF_FloatRGBA, FClearValueBinding::BlackMaxAlpha,
			TexCreate_None,	TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable, false);
		FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(OutputTextureDesc, TEXT("Wave Debug Texture"));

		FBoxMullerShader::FParameters* BoxMullerPassParameters = GraphBuilder.AllocParameters<FBoxMullerShader::FParameters>();
		BoxMullerPassParameters->InputTexture = UniformNoiseTextureSRV;
		BoxMullerPassParameters->OutputTexture = GaussianNoiseTextureUAV;

		TShaderMapRef<FBoxMullerShader> BoxMullerShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Box Muller Pass"), *BoxMullerShader, BoxMullerPassParameters, GroupCount);

		FCopyShader::FParameters* CopyPassParameters = GraphBuilder.AllocParameters<FCopyShader::FParameters>();
		CopyPassParameters->InputTexture = GaussianNoiseTextureSRV;
		CopyPassParameters->OutputTexture = GraphBuilder.CreateUAV(OutputTexture);
		CopyPassParameters->Bias = 0.5;
		CopyPassParameters->Scale = 1 / 2.5;

		TShaderMapRef<FCopyShader> CopyShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Copy Texture Pass"), *CopyShader, CopyPassParameters, GroupCount);

		GraphBuilder.Execute();
	});
}

