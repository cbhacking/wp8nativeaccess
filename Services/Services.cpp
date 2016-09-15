// Services.cpp
#include "pch.h"
#include "Services.h"

using namespace Services;
using namespace Platform;

NativeServices::NativeServices()
{
}

bool NativeServices::EnumServices (ServiceType types, ServiceStateClass state, String ^group, Array<ServiceStatus> ^*services)
{
	bool retval = false;
	SC_HANDLE scm = ::OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	if (!scm)
	{
		goto Cleanup;
	}

	retval = true;
Cleanup:
	if (scm)
		::CloseServiceHandle(scm);
	return retval;
}

bool NativeServices::GetServiceStatus (String ^service, ServiceStatus *status)
{
	bool retval = false;
	SC_HANDLE scm = NULL;
	SC_HANDLE svc = NULL;

	scm = ::OpenSCManager(NULL, NULL, 0x0);
	if (!scm)
	{
		goto Cleanup;
	}
	svc = ::OpenService(scm, service->Data(), GENERIC_READ);
	if (!svc)
	{
		goto Cleanup;
	}

	retval = true;
Cleanup:
	if (svc)
		::CloseServiceHandle(svc);
	if (scm)
		::CloseServiceHandle(scm);
	return retval;
}

uint32 NativeServices::GetError ()
{
	return ::GetLastError();
}