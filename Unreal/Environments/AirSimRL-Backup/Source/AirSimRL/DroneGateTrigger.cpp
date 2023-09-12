// Fill out your copyright notice in the Description page of Project Settings.

#include "DroneGateTrigger.h"
#include "ScoreTracker.h"


void ADroneGateTrigger::BeginPlay(){}

ADroneGateTrigger::ADroneGateTrigger()
{
	OnActorBeginOverlap.AddDynamic(this, &ADroneGateTrigger::OnOverlapBegin);
	GateFrame = nullptr;
	DroneThatPassedThisTrigger = nullptr;
}

bool ADroneGateTrigger::GetIsActivated() {
	return IsActivated;
}

/*AActor* ADroneGateTrigger::GetDroneThatPassedThisTrigger(bool& IsNullPointer)
{
	if (DroneThatPassedThisTrigger == nullptr) {
		IsNullPointer = true;
		UE_LOG(LogTemp, Log, TEXT("Drone That Passed This Trigger Was A Null Pointer!???!!"))
	}
	else
		IsNullPointer = false;

	return DroneThatPassedThisTrigger;
}*/

void ADroneGateTrigger::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	GateFrame = GetParentActor();
	
	if (!IsActivated && OtherActor != GateFrame)
	{
		IsActivated = true;
		DroneThatPassedThisTrigger = OtherActor;
	}

}


void ADroneGateTrigger::Reset()
{
	IsActivated = false;
}
