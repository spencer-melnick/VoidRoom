#pragma once

#include "CoreMinimal.h"

#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"

#include "CommonShaderParameters.h"

class FInitialComponentsShader: public FGlobalShader
{
public:
	static const int32 ThreadsPerGroupDimension = 32;

	DECLARE_GLOBAL_SHADER(FInitialComponentsShader)
	SHADER_USE_PARAMETER_STRUCT(FInitialComponentsShader, FGlobalShader)

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float4>, NoiseBuffer)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<float4>, OutputBuffer)
		SHADER_PARAMETER_STRUCT_INCLUDE(FOceanShaderCommonParameters, CommonParameters)
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

IMPLEMENT_GLOBAL_SHADER(FInitialComponentsShader, "/OceanShaders/Private/InitialComponents.usf", "main", SF_Compute);
