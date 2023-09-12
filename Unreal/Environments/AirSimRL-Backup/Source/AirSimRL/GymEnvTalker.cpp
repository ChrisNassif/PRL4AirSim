// Fill out your copyright notice in the Description page of Project Settings.


#include "GymEnvTalker.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Common/TcpListener.h"
#include "JsonUtilities.h"
//#include "Unix/UnixPlatformProcess.h"


AGymEnvTalker::AGymEnvTalker() {PrimaryActorTick.bCanEverTick = true;}
void AGymEnvTalker::Tick(float DeltaTime) {Super::Tick(DeltaTime);}

void AGymEnvTalker::BeginPlay() {Super::BeginPlay();}

//FTcpListener* GymEnvListener = nullptr;
//FRunnableThread* GymEnvListenerThread;
//FSocket* AGymEnvTalker::GymEnvTalkerClient = nullptr;
GymEnvConnectionManager* AGymEnvTalker::SimulationConnection = nullptr;
TMap<int, AActor*> AGymEnvTalker::SpawnedObjects;

void AGymEnvTalker::ResetEnvironment()
{
	SpawnedObjects.Reset();
}

// returns the port that the GymEnv is currently trying to connect to
int AGymEnvTalker::GetClientPortNumFromSettingsFile(FString SettingsFilePath) {
	FString JsonString; //Json converted to FString

	FFileHelper::LoadFileToString(JsonString, *SettingsFilePath);

	//Create a json object to store the information from the json string
	//The json reader is used to deserialize the json object later on
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
	
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid()) {
		return JsonObject->GetIntegerField("GymEnvServerPort");
	}
	return 0;
}


void AGymEnvTalker::InitializeSocketListener(FString IPAddress, int Port)
{
	SimulationConnection = new GymEnvConnectionManager(IPAddress, Port);
}


bool AGymEnvTalker::DoesMessageToReceiveExist()
{
	return SimulationConnection->DoesMessageToReceiveExist();
}

void AGymEnvTalker::CloseConnection()
{
	SimulationConnection->CloseConnection();
}

bool AGymEnvTalker::IsConnected()
{
	UE_LOG(LogTemp, Log, TEXT("Is Connected?: %s"), (SimulationConnection->IsConnected) ? TEXT("TRUE") : TEXT("FALSE"))
	return SimulationConnection->IsConnected;
}


FString AGymEnvTalker::ReceiveMessage()
{
	if (SimulationConnection == nullptr) return "";

	return SimulationConnection->ReceiveMessage();
}



FString AGymEnvTalker::SendGatePassData(class AActor* GateReference) {
	int IDToSend = *SpawnedObjects.FindKey(GateReference);
	FString Message = FString::FromInt(IDToSend);
	Message += ";";

	return SimulationConnection->SendMessage(Message);
}

/*FString AGymEnvTalker::SendGatePassData(int GatesPassedCount, int GatesDoublePassedCount)
{
	// GatesDoublePassedCount is the number of times that the same gate has been passed more than once by the drone
	// When the drone passes through the same gate more than once, we want to punish the drone because it won't try to pass all of the other gates
	if (SimulationConnection == nullptr) return "";

	FString Message = FString::FromInt(GatesPassedCount);
	Message += ",";
	Message += FString::FromInt(GatesDoublePassedCount);
	Message += ";";

	return SimulationConnection->SendMessage(Message);
}*/






TArray<FString> AGymEnvTalker::SplitMultipleMessages(FString Message)
{
	TArray<FString> Result;
	Message.ParseIntoArray(Result, TEXT(";"), true);
	return Result;
}

FString AGymEnvTalker::ParseCommand(FString Message)
{
	TArray<FString> Arguments;
	Message.ParseIntoArray(Arguments, TEXT(", "), true);
	return Arguments[0];
}

void AGymEnvTalker::ParseSpawnMessage(FString Message, FString& ObjectToSpawnString, int& id, float& xPos, float& yPos, float& zPos, float& xScale, float& yScale, float& zScale, float& pitch, float& yaw, float& roll)
{
	TArray<FString> Arguments;
	Message.ParseIntoArray(Arguments, TEXT(", "), true);

	if (Arguments.Num() != 12) return;

	id = FCString::Atoi(*Arguments[1]);
	ObjectToSpawnString = Arguments[2];
	xPos = FCString::Atof(*Arguments[3]);
	yPos = FCString::Atof(*Arguments[4]);
	zPos = FCString::Atof(*Arguments[5]);
	xScale = FCString::Atof(*Arguments[6]);
	yScale = FCString::Atof(*Arguments[7]);
	zScale = FCString::Atof(*Arguments[8]);
	pitch = FCString::Atof(*Arguments[9]);
	yaw = FCString::Atof(*Arguments[10]);
	roll = FCString::Atof(*Arguments[11]);

}

void AGymEnvTalker::ParseDestroyMessage(FString Message, int& id)
{
	TArray<FString> Arguments;
	Message.ParseIntoArray(Arguments, TEXT(", "), true);

	if (Arguments.Num() != 2) return;

	id = FCString::Atoi(*Arguments[1]);
}

void AGymEnvTalker::ParseTransformMessage(FString Message, int& id, float& xPos, float& yPos, float& zPos, float& xScale, float& yScale, float& zScale, float& pitch, float& yaw, float& roll)
{
	TArray<FString> Arguments;
	Message.ParseIntoArray(Arguments, TEXT(", "), true);

	if (Arguments.Num() != 11) return;

	id = FCString::Atoi(*Arguments[1]);
	xPos = FCString::Atof(*Arguments[2]);
	yPos = FCString::Atof(*Arguments[3]);
	zPos = FCString::Atof(*Arguments[4]);
	xScale = FCString::Atof(*Arguments[5]);
	yScale = FCString::Atof(*Arguments[6]);
	zScale = FCString::Atof(*Arguments[7]);
	pitch = FCString::Atof(*Arguments[8]);
	yaw = FCString::Atof(*Arguments[9]);
	roll = FCString::Atof(*Arguments[10]);
}





TSubclassOf<AActor> AGymEnvTalker::GetObjectToSpawnFromString(FString ObjectToSpawnString, 
								TSubclassOf<AActor> PlaygroundReference, TSubclassOf<AActor> DroneGateReference)
{
	if (ObjectToSpawnString == "Playground") return PlaygroundReference;
	if (ObjectToSpawnString == "DroneGate") return DroneGateReference;
	//if (ObjectToSpawnString == "Box") return;
	return TSubclassOf<AActor>();
}



AActor* AGymEnvTalker::SpawnObject(TSubclassOf<AActor> ObjectToSpawnActor, int id, float xPos, float yPos, float zPos, float xScale, float yScale, float zScale, float pitch, float yaw, float roll)
{
	FVector Location = FVector(xPos, yPos, zPos);
	FRotator Rotation = FRotator(pitch, yaw, roll);

	UWorld* World = GEngine->GameViewport->GetWorld();
	AActor* SpawnedObject = World->SpawnActor<AActor>(ObjectToSpawnActor, Location, Rotation);

	SpawnedObject->SetActorScale3D(FVector(xScale, yScale, zScale));

	SpawnedObjects.Add(id, SpawnedObject);

	return SpawnedObject;
}

bool AGymEnvTalker::CanDestroyObject(int id)
{
	return SpawnedObjects.Contains(id);
}

AActor* AGymEnvTalker::GetActorToDestroy(int id)
{
	AActor* result = SpawnedObjects[id];
	SpawnedObjects.Remove(id);
	return result;
	
	
}

AActor* AGymEnvTalker::TransformObject(int id, float xPos, float yPos, float zPos, float xScale, float yScale, float zScale, float pitch, float yaw, float roll)
{
	AActor* Object = SpawnedObjects[id];
	Object->SetActorLocation(FVector(xPos, yPos, zPos));
	Object->SetActorRotation(FRotator(pitch, yaw, roll));
	Object->SetActorScale3D(FVector(xScale, yScale, zScale));
	return Object;
}

TMap<int, AActor*> AGymEnvTalker::GetSpawnedObjects()
{
	return SpawnedObjects;
}

void AGymEnvTalker::PrintSpawnedObjects()
{
	UE_LOG(LogTemp, Log, TEXT("PRINTING OUT THE SPAWNED OBJECTS"))
	for (const TPair<int, AActor*>& pair : SpawnedObjects) {
		UE_LOG(LogTemp, Log, TEXT("Spawned in Object Id: %d"), pair.Key)
		UE_LOG(LogTemp, Log, TEXT("Spawned in Object Name: %s"), pair.Value)
		UE_LOG(LogTemp, Log, TEXT(""))
	}
}

/*int AGymEnvTalker::GetCurrentProcessID()
{
	return FUnixPlatformProcess::GetCurrentProcessId();
}*/


