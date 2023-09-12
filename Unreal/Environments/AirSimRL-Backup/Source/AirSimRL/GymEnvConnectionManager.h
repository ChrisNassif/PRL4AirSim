// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Common/TcpListener.h"
#include "Sockets.h"
/**
 * 
 */
class AIRSIMRL_API GymEnvConnectionManager 
{

	public:
		GymEnvConnectionManager(FString IPAddress, int Port);
		~GymEnvConnectionManager();


		FIPv4Endpoint Endpoint;
		FRunnableThread* SocketListenerThread;
		FTcpListener* GymEnvListener;
		FSocket* GymEnvClient;
		bool IsConnected = false;

		
		bool OnConnectionAccepted(FSocket* ClientSocket, const FIPv4Endpoint& Endpoint);

		bool DoesMessageToReceiveExist();

		FString ReceiveMessage();

		FString SendMessage(FString Message);

		void CloseConnection();

};