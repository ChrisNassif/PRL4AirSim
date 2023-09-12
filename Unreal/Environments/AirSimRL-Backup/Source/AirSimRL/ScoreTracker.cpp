// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreTracker.h"

AScoreTracker::AScoreTracker(){PrimaryActorTick.bCanEverTick = true;}
void AScoreTracker::BeginPlay(){Super::BeginPlay();}
void AScoreTracker::Tick(float DeltaTime){Super::Tick(DeltaTime);}


//float AScoreTracker::HoopCount = 0;
int AScoreTracker::GatesPassedCount = 0;
int AScoreTracker::GatesRepassedCount = 0;

void AScoreTracker::AddGatePassed()
{
	GatesPassedCount += 1;
	UE_LOG(LogTemp, Log, TEXT("Added Gate Passed"));
}

void AScoreTracker::AddGateRepassed()
{
	GatesRepassedCount += 1;
	UE_LOG(LogTemp, Log, TEXT("Added Gate Repassed"));
}
int AScoreTracker::GetGatesPassedCount()
{
	return GatesPassedCount; 
}

int AScoreTracker::GetGatesRepassedCount()
{
	return GatesRepassedCount;
}

void AScoreTracker::ResetScoreTracker()
{
	GatesPassedCount = 0;
	GatesRepassedCount = 0;
}
