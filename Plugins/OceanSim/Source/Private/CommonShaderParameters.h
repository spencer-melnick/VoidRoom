#pragma once

#include "CoreMinimal.h"

#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"

BEGIN_SHADER_PARAMETER_STRUCT(FOceanShaderCommonParameters, )
	SHADER_PARAMETER(FIntPoint, BufferSize)
	SHADER_PARAMETER(float, Amplitude)
	SHADER_PARAMETER(float, PatchLength)
	SHADER_PARAMETER(float, Gravity)
	SHADER_PARAMETER(float, WindSpeed)
	SHADER_PARAMETER(FVector2D, WindDirection)
END_SHADER_PARAMETER_STRUCT()

enum class EFFTDirection
{
	FFT_Horizontal,
	FFT_Vertical
};
