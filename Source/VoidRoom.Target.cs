// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class VoidRoomTarget : TargetRules
{
	public VoidRoomTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

        ExtraModuleNames.AddRange( new string[] { "VoidRoom" } );

		DefaultBuildSettings = BuildSettingsVersion.V2;
	}
}
