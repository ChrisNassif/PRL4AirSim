// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GymEnvConnectionManager.h"
#include "GymEnvTalker.generated.h"


UCLASS()
class AIRSIMRL_API AGymEnvTalker : public AActor
{
	GENERATED_BODY()
	
	public:	
		// Sets default values for this actor's properties
		AGymEnvTalker();

	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;


	public:	
		// Called every frame
		virtual void Tick(float DeltaTime) override;
		

		//FRunnableThread* GymEnvListenerThread;
		static GymEnvConnectionManager* SimulationConnection;
		//static FSocket* GymEnvTalkerClient;
		static TMap<int, AActor*> SpawnedObjects;


		UFUNCTION(BlueprintCallable, Category = "Editing Environment")
			static void ResetEnvironment();


		UFUNCTION(BlueprintCallable, Category = "Networking")
			static int GetClientPortNumFromSettingsFile(FString SettingsFilePath);

		UFUNCTION(BlueprintCallable, Category = "Networking")
			static void InitializeSocketListener(FString IPAddress, int Port);

		UFUNCTION(BlueprintCallable, Category = "Networking")
			static void CloseConnection();

		UFUNCTION(BlueprintCallable, Category = "Networking")
			static bool IsConnected();

		UFUNCTION(BlueprintCallable, Category = "Networking")
			static bool DoesMessageToReceiveExist();

		UFUNCTION(BlueprintCallable, Category = "Networking")
			static FString ReceiveMessage();


		UFUNCTION(BlueprintCallable, Category = "Networking")
			static FString SendGatePassData(class AActor* GateReference);

		//UFUNCTION(BlueprintCallable, Category = "Networking")
			//static FString SendGatePassData(int GatesPassedCount, int GatesDoublePassedCount);


		UFUNCTION(BlueprintCallable, Category = "Parsing")
			static TArray<FString> SplitMultipleMessages(FString Message);

		UFUNCTION(BlueprintCallable, Category = "Parsing")
			static FString ParseCommand(FString Message);

		UFUNCTION(BlueprintCallable, Category = "Parsing")
			static void ParseSpawnMessage(FString Message,
										  FString& ObjectToSpawnString, int& id,
										  float& xPos, float& yPos, float& zPos,
										  float& xScale, float& yScale, float& zScale,
										  float& pitch, float& yaw, float& roll);
		UFUNCTION(BlueprintCallable, Category = "Parsing")
			static void ParseDestroyMessage(FString Message, int& id);

		UFUNCTION(BlueprintCallable, Category = "Parsing")
			static void ParseTransformMessage(FString Message,
											  int& id,
											  float& xPos, float& yPos, float& zPos,
											  float& xScale, float& yScale, float& zScale,
											  float& pitch, float& yaw, float& roll);




		UFUNCTION(BlueprintCallable, Category = "EditingEnvironment")
			static TSubclassOf<AActor> GetObjectToSpawnFromString(FString ObjectToSpawnString, TSubclassOf<AActor> PlaygroundReference, TSubclassOf<AActor> DroneGateReference);

		UFUNCTION(BlueprintCallable, Category = "EditingEnvironment")
			static AActor* SpawnObject(TSubclassOf<AActor> ObjectToSpawn, int id,
									   float xPos, float yPos, float zPos,
									   float xScale, float yScale, float zScale,
									   float pitch, float yaw, float roll);


		UFUNCTION(BlueprintCallable, Category = "EditingEnvironment")
			static bool CanDestroyObject(int id);

		UFUNCTION(BlueprintCallable, Category = "EditingEnvironment")
			static AActor* GetActorToDestroy(int id);

		UFUNCTION(BlueprintCallable, Category = "EditingEnvironment")
			static AActor* TransformObject(int id,
										   float xPos, float yPos, float zPos,
										   float xScale, float yScale, float zScale,
										   float pitch, float yaw, float roll);




		UFUNCTION(BlueprintCallable, Category = "EditingEnvironment")
			static TMap<int, AActor*> GetSpawnedObjects();

		UFUNCTION(BlueprintCallable, Category = "EditingEnvironment")
			static void PrintSpawnedObjects();

};
