// Fill out your copyright notice in the Description page of Project Settings.


#include "GymEnvConnectionManager.h"



GymEnvConnectionManager::GymEnvConnectionManager(FString IPAddress, int Port){
	FIPv4Address GymEnvIP;
	FIPv4Address::Parse(IPAddress, GymEnvIP); // transform the ip address into something unreal understands

	TSharedRef<FInternetAddr> GymEnvAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	GymEnvAddress->SetIp(GymEnvIP.Value);
	GymEnvAddress->SetPort(Port);

	Endpoint = FIPv4Endpoint(GymEnvAddress);

	FTimespan TimeSlept = FTimespan(0);
	GymEnvListener = new FTcpListener(Endpoint, TimeSlept, true);


	UE_LOG(LogTemp, Log, TEXT("GymEnvListener was created"))

	GymEnvListener->OnConnectionAccepted().BindRaw(this, &GymEnvConnectionManager::OnConnectionAccepted);

	if (GymEnvListener->Init()) {
		SocketListenerThread = FRunnableThread::Create(GymEnvListener, TEXT("TCP_Subsystem_Thread"), 0, TPri_BelowNormal);
		UE_LOG(LogTemp, Log, TEXT("Thread id: %d"), SocketListenerThread->GetThreadID());
	}
		

}

GymEnvConnectionManager::~GymEnvConnectionManager() {}


bool GymEnvConnectionManager::OnConnectionAccepted(FSocket* ClientSocket, const FIPv4Endpoint& ClientEndpoint)
{
	GymEnvClient = ClientSocket;
	IsConnected = true;

	UE_LOG(LogTemp, Log, TEXT("CONNECTION ACCEPTED!!"))
	return true;
}

bool GymEnvConnectionManager::DoesMessageToReceiveExist()
{
	uint32 PendingDataSize;
	try {
		GymEnvClient->HasPendingData(PendingDataSize);
	}
	catch (int e) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Error Message: %lld"), e));
		return false;
	}

	return PendingDataSize > 0;
}

FString GymEnvConnectionManager::ReceiveMessage()
{
	// TCP is limited to 1024*64 in one datagram, so try not to exceed that

	uint8 DataToReceiveLocation[1024 * 8];
	int32 NumberOfBytesRead;
	GymEnvClient->Recv(DataToReceiveLocation, 1024 * 8, NumberOfBytesRead);
	UE_LOG(LogTemp, Log, TEXT("Number of Bytes Read: %d"), NumberOfBytesRead)

		FString Message = FString(UTF8_TO_TCHAR(DataToReceiveLocation));
	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
	return Message;
}

FString GymEnvConnectionManager::SendMessage(FString Message)
{
	TCHAR* serializedChar = Message.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);
	int32 sent = 0;
	bool successful = GymEnvClient->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);
	return Message;
}

void GymEnvConnectionManager::CloseConnection()
{
	UE_LOG(LogTemp, Log, TEXT("Closing. Last thread's id: %d"), SocketListenerThread->GetThreadID())
	UE_LOG(LogTemp, Log, TEXT("Closing. Is last Listener still listening?: %s"), (GymEnvListener->IsActive() ? TEXT("true") : TEXT("false")))

	IsConnected = false;
	/*
	// Make sure the listener goes back to listening for another connection
	FTimespan TimeSlept = FTimespan(0);
	GymEnvListener = new FTcpListener(Endpoint, TimeSlept, true);

	UE_LOG(LogTemp, Log, TEXT("GymEnvListener was created"))

	GymEnvListener->OnConnectionAccepted().BindRaw(this, &GymEnvConnectionManager::OnConnectionAccepted);

	if (GymEnvListener->Init()) {
		SocketListenerThread = FRunnableThread::Create(GymEnvListener, TEXT("TCP_Subsystem_Thread"), 0, TPri_BelowNormal);
		UE_LOG(LogTemp, Log, TEXT("Thread id: %d"), SocketListenerThread->GetThreadID());
	}
	*/
}





