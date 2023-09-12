// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScoreTracker.generated.h"

UCLASS()
class AIRSIMRL_API AScoreTracker : public AActor
{
	GENERATED_BODY()
	
public:	
	AScoreTracker();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;


	static int GatesPassedCount;
	static int GatesRepassedCount;

	UFUNCTION(BlueprintCallable, Category = "Score")
		static void AddGatePassed();

	UFUNCTION(BlueprintCallable, Category = "Score")
		static void AddGateRepassed();

	UFUNCTION(BlueprintCallable, Category = "Drone Score")
		static int GetGatesPassedCount();

	UFUNCTION(BlueprintCallable, Category = "Drone Score")
		static int GetGatesRepassedCount();

	UFUNCTION(BlueprintCallable, Category = "Drone Score")
		static void ResetScoreTracker();

};
