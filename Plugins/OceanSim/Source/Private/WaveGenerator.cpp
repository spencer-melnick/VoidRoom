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
	uint32 NumElements = BufferSize.X * BufferSize.Y * 4;
	FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(BufferSize, FTestShader::ThreadsPerGroupDimension);

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
		FTexture2DRHIRef InputTexture = RHICreateTexture2D(BufferSize.X, BufferSize.Y, PF_FloatRGBA, 1, 1,
			TexCreate_ShaderResource, CreateInfo);
		FShaderResourceViewRHIRef InputTextureSRV = RHICreateShaderResourceView(InputTexture, 0);

		FRDGTextureDesc OutputTextureDesc = FRDGTextureDesc::Create2DDesc(BufferSize, EPixelFormat::PF_FloatRGBA, FClearValueBinding::BlackMaxAlpha,
			TexCreate_None,	TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable, false);
		FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(OutputTextureDesc, TEXT("Test Texture"));

		FTestShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FTestShader::FParameters>();
		PassParameters->OutputTexture = GraphBuilder.CreateUAV(OutputTexture);
		PassParameters->OutputTextureSize = BufferSize;
		PassParameters->InputTexture = InputTextureSRV;

		TShaderMapRef<FTestShader> TestShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		uint32 Stride;
		FFloat16* TextureData = static_cast<FFloat16*>(RHILockTexture2D(InputTexture, 0, EResourceLockMode::RLM_WriteOnly, Stride, false));
		FMemory::Memcpy(TextureData, InputData.GetData(), NumElements * sizeof(FFloat16));
		RHIUnlockTexture2D(InputTexture, 0, false);

		GraphBuilder.AddPass(RDG_EVENT_NAME("Test Shader Pass"), PassParameters, ERDGPassFlags::Compute,
		[&TestShader, &PassParameters, &InputTexture, GroupCount]
		(FRHICommandList& CommandList)
		{
			FComputeShaderUtils::Dispatch(CommandList, *TestShader, *PassParameters, GroupCount);
		});

		GraphBuilder.Execute();
	});
}

