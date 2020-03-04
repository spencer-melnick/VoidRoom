#pragma once

#include "CoreMinimal.h"

#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"

#include "CommonShaderParameters.h"

class FRealtimeComponentsShader : public FGlobalShader
{
public:
	static const int32 ThreadsPerGroupDimension = 1;

	DECLARE_GLOBAL_SHADER(FRealtimeComponentsShader)
	SHADER_USE_PARAMETER_STRUCT(FRealtimeComponentsShader, FGlobalShader)

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_SRV(Texture2D<float4>, InitialComponents)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<float2>, HeightComponentsBuffer)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<float2>, SlopeXBuffer)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<float2>, SlopeYBuffer)
		SHADER_PARAMETER(float, Time)
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
	}
};

IMPLEMENT_GLOBAL_SHADER(FRealtimeComponentsShader, "/OceanShaders/Private/RealtimeComponents.usf", "main", SF_Compute);
