#pragma once

#include "CoreMinimal.h"

#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"

#include "CommonShaderParameters.h"

template <EFFTDirection FFTDirection>
class FButterflyShader : public FGlobalShader
{
public:
	static const int32 ThreadsPerGroupDimension = 32;

	DECLARE_GLOBAL_SHADER(FButterflyShader)
	SHADER_USE_PARAMETER_STRUCT(FButterflyShader, FGlobalShader)

		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<float2>, DataBuffer)
		SHADER_PARAMETER_SRV(StructuredBuffer<ButterflyOperation>, Operations)
		SHADER_PARAMETER(FIntPoint, BufferSize)
		SHADER_PARAMETER(uint32, OperationStartIndex)
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

		if (FFTDirection == EFFTDirection::FFT_Vertical)
		{
			OutEnvironment.SetDefine(TEXT("DO_FFT_VERTICAL"), true);
		}
		else if (FFTDirection == EFFTDirection::FFT_Horizontal)
		{
			OutEnvironment.SetDefine(TEXT("DO_FFT_HORIZONTAL"), true);
		}
	}
};

using FButterflyShaderHorizontal = FButterflyShader<EFFTDirection::FFT_Horizontal>;
using FButterflyShaderVertical = FButterflyShader<EFFTDirection::FFT_Vertical>;

IMPLEMENT_GLOBAL_SHADER(FButterflyShader<EFFTDirection::FFT_Horizontal>, "/OceanShaders/Private/Butterfly.usf", "main", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FButterflyShader<EFFTDirection::FFT_Vertical>, "/OceanShaders/Private/Butterfly.usf", "main", SF_Compute);
