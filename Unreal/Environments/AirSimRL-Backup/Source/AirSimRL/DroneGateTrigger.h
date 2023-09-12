// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "DroneGateTrigger.generated.h"

/**
 * 
 */
UCLASS()
class AIRSIMRL_API ADroneGateTrigger : public ATriggerBox
{
	GENERATED_BODY()

	protected:
		virtual void BeginPlay() override;

	public:
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			bool IsActivated = false;
			
		AActor* GateFrame;
		AActor* DroneThatPassedThisTrigger;


		ADroneGateTrigger();

		UFUNCTION(BlueprintCallable)
			bool GetIsActivated();

		//UFUNCTION(BlueprintCallable)
		//	AActor* GetDroneThatPassedThisTrigger(bool& IsNullPointer);

		UFUNCTION()
			void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

		UFUNCTION(BlueprintCallable, Category = "Drone Score")
			void Reset();

};
