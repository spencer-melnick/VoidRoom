#pragma once

#include "CoreMinimal.h"

#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"

#include "WaveGenerator.h"

template <FWaveGenerator::EFFTDirection FFTDirection>
class FBitReverseCopyShader : public FGlobalShader
{
public:
	static const int32 ThreadsPerGroupDimension = 32;

	DECLARE_GLOBAL_SHADER(FBitReverseCopyShader)
	SHADER_USE_PARAMETER_STRUCT(FBitReverseCopyShader, FGlobalShader)

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_UAV(float2, DataBuffer)
		SHADER_PARAMETER_SRV(uint2, BitReversalLookup)
		SHADER_PARAMETER(FIntPoint, BufferSize)
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

		if (FFTDirection == FWaveGenerator::EFFTDirection::FFT_Vertical)
		{
			OutEnvironment.SetDefine(TEXT("DO_FFT_VERTICAL"), true);
		}
		else if (FFTDirection == FWaveGenerator::EFFTDirection::FFT_Horizontal)
		{
			OutEnvironment.SetDefine(TEXT("DO_FFT_HORIZONTAL"), true);
		}
	}
};

using FBitReverseCopyShaderHorizontal = FBitReverseCopyShader<FWaveGenerator::EFFTDirection::FFT_Horizontal>;
using FBitReverseCopyShaderVertical = FBitReverseCopyShader<FWaveGenerator::EFFTDirection::FFT_Vertical>;

IMPLEMENT_GLOBAL_SHADER(FBitReverseCopyShader<FWaveGenerator::EFFTDirection::FFT_Horizontal>, "/OceanShaders/Private/BitReverseCopy.usf", "main", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FBitReverseCopyShader<FWaveGenerator::EFFTDirection::FFT_Vertical>, "/OceanShaders/Private/BitReverseCopy.usf", "main", SF_Compute);

