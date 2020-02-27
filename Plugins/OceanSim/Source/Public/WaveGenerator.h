#pragma once

#include "CoreMinimal.h"

#include "RHI.h"

class FWaveGenerator
{
public:
	void Initialize(FIntPoint Dimensions);

private:
	FIntPoint BufferSize;
	FStructuredBufferRHIRef InitialComponentsBuffer;
	FUnorderedAccessViewRHIRef InitialComponentsBufferUAV;
};
