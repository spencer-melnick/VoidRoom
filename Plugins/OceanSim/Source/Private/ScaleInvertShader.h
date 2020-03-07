#pragma once

#pragma once

#include "CoreMinimal.h"

#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"

class FScaleInvertShader : public FGlobalShader
{
public:
	static const int32 ThreadsPerGroupDimension = 8;

	DECLARE_GLOBAL_SHADER(FScaleInvertShader)
	SHADER_USE_PARAMETER_STRUCT(FScaleInvertShader, FGlobalShader)

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float2>, HeightBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float2>, DisplacementXBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float2>, DisplacementYBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float2>, SlopeXBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float2>, SlopeYBuffer)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, DisplacementTexture)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture<float2>, SlopeTexture)
		SHADER_PARAMETER(FIntPoint, BufferSize)
		SHADER_PARAMETER(float, FoamLambda)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), ThreadsPerGroupDimension);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), ThreadsPerGroupDimension);
	}
};

IMPLEMENT_GLOBAL_SHADER(FScaleInvertShader, "/OceanShaders/Private/ScaleInvert.usf", "main", SF_Compute);

