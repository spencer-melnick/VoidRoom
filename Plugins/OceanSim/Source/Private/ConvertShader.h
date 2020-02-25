#pragma once

#include "CoreMinimal.h"

#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"

class FConvertShader: public FGlobalShader
{
public:
	static const int32 ThreadsPerGroupDimension = 32;
	
	DECLARE_GLOBAL_SHADER(FConvertShader)
	SHADER_USE_PARAMETER_STRUCT(FConvertShader, FGlobalShader)

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_UAV(RWTexture2D<float4>, OutputTexture)
		SHADER_PARAMETER_SRV(StructuredBuffer<float4>, InputBuffer)
		SHADER_PARAMETER(FIntPoint, InputBufferSize)
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
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
	}
};

IMPLEMENT_GLOBAL_SHADER(FConvertShader, "/OceanShaders/Private/Convert.usf", "main", SF_Compute);
