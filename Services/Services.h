/*
 * Services\Services.h
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.0.1
 *
 * This file defines the WinRT-visible NativeServices class, which enables Service Control Manager access.
 */

#pragma once
#include "pch.h"

using namespace Platform;

namespace Services
{
	[Metadata::FlagsAttribute()]
	public enum class ServiceType : uint32
	{
		KernelDriver = SERVICE_KERNEL_DRIVER,
		FileSystemDriver = SERVICE_FILE_SYSTEM_DRIVER,
		Driver = SERVICE_DRIVER,
		OwnProcess = SERVICE_WIN32_OWN_PROCESS,
		ShareProcess = SERVICE_WIN32_SHARE_PROCESS,
		Win32Process = SERVICE_WIN32,
		Interactive = SERVICE_INTERACTIVE_PROCESS
	};

	[Metadata::FlagsAttribute()]
	public enum class ServiceState : uint32
	{
		Stopped = SERVICE_STOPPED,
		StartPending = SERVICE_START_PENDING,
		StopPending = SERVICE_STOP_PENDING,
		Running = SERVICE_RUNNING,
		ContinuePending = SERVICE_CONTINUE_PENDING,
		PausePending = SERVICE_PAUSE_PENDING,
		Paused = SERVICE_PAUSED
	};

	[Metadata::FlagsAttribute()]
	public enum class ServiceStateClass : uint32
	{
		Active = SERVICE_ACTIVE,
		Inactive = SERVICE_INACTIVE,
		All = SERVICE_STATE_ALL
	};

	[Metadata::FlagsAttribute()]
	public enum class ServiceControlsAccepted : uint32
	{
		Stop = SERVICE_ACCEPT_STOP,
		PauseContinue = SERVICE_ACCEPT_PAUSE_CONTINUE,
		Shutdown = SERVICE_ACCEPT_SHUTDOWN,
		ParamChange = SERVICE_ACCEPT_PARAMCHANGE,
		NetBindChange = SERVICE_ACCEPT_NETBINDCHANGE,
		HardwareProfileChange = SERVICE_ACCEPT_HARDWAREPROFILECHANGE,
		PowerEvent = SERVICE_ACCEPT_POWEREVENT,
		SessionChange = SERVICE_ACCEPT_SESSIONCHANGE,
		PreShutdown = SERVICE_ACCEPT_PRESHUTDOWN,
		TimeChange = SERVICE_ACCEPT_TIMECHANGE,
		TriggerEvent = SERVICE_ACCEPT_TRIGGEREVENT
	};

	public value struct ServiceStatus
	{
		String ^Name;
		String ^DisplayName;
		ServiceType Type;
		ServiceState State;
		ServiceControlsAccepted ControlsAccepted;
		uint32 Win32ExitCode;
		uint32 ServiceSpecificExitCode;
		uint32 Checkpoint;
		uint32 WaitHint;
		uint32 ProcessId;
		bool RunsInSystemProcess;
	};

    public ref class NativeServices sealed
    {
	private:
        NativeServices();
    public:
		static bool EnumServices (ServiceType types, ServiceStateClass state, String ^group, Array<ServiceStatus> ^*services);
		static bool GetServiceStatus (String ^service, ServiceStatus *status);
		static uint32 GetError ();
	};
}