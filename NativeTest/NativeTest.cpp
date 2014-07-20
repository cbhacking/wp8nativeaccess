#include "pch.h"
#include "NativeTest.h"
#include "BasicTimer.h"
//#include "..\Registry\Registry.h"

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace concurrency;

using namespace Platform;
using namespace Registry;
using namespace FileSystem;

NativeTest::NativeTest() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

void NativeTest::Initialize(CoreApplicationView^ applicationView)
{
	applicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &NativeTest::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &NativeTest::OnSuspending);

	CoreApplication::Resuming +=
		ref new EventHandler<Platform::Object^>(this, &NativeTest::OnResuming);

	m_renderer = ref new CubeRenderer();
}

void NativeTest::SetWindow(CoreWindow^ window)
{
	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &NativeTest::OnVisibilityChanged);

	window->Closed += 
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &NativeTest::OnWindowClosed);

	window->PointerPressed +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &NativeTest::OnPointerPressed);

	window->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &NativeTest::OnPointerMoved);

	window->PointerReleased +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &NativeTest::OnPointerReleased);

	m_renderer->Initialize(CoreWindow::GetForCurrentThread());
}

void NativeTest::Load(Platform::String^ entryPoint)
{
}

void NativeTest::Run()
{
	BasicTimer^ timer = ref new BasicTimer();

	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			timer->Update();
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			m_renderer->Update(timer->Total, timer->Delta);
			m_renderer->Render();
			m_renderer->Present(); // This call is synchronized to the display frame rate.
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

void NativeTest::Uninitialize()
{
}

void NativeTest::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void NativeTest::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
}

void NativeTest::OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
	// Insert your code here.
}

void NativeTest::OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
{
	// Insert your code here.
}

void NativeTest::OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
{
	// Insert your code here.
}

void NativeTest::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	CoreWindow::GetForCurrentThread()->Activate();
}

void NativeTest::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();
	m_renderer->ReleaseResourcesForSuspending();

	create_task([this, deferral]()
	{
		// Insert your code here.

		deferral->Complete();
	});
}
 
void NativeTest::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.
	 m_renderer->CreateWindowSizeDependentResources();
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new NativeTest();
}

unsigned RPCCompWriteMulti (PWCHAR value)
{
	size_t len = wcslen(value);
	PWCHAR buf = new WCHAR[len+2];
	wcscpy_s(buf, (len+2), value);
	buf[len+1] = L'\0';
	String ^val = ref new String(buf, (len+2));

	unsigned foo = 0x0;
	RPCComponent::CRPCComponent::Initialize();
	RPCComponent::CRPCComponent::Registry_SetString(
		(unsigned)(Registry::RegistryHive::HKLM),
		ref new Platform::String(L"SYSTEM\\CurrentControlSet\\Control\\CI\\TRSData"),//SOFTWARE\\Microsoft\\SecurityManager\\CapabilityClasses"),
		ref new Platform::String(L"TestPath"),//ID_CAP_BUILTIN_TCB"),
//		ref new Platform::String(L"ID_CAP_DEVELOPERUNLOCK_API"),
		val,
		&foo);
	return foo;
}

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	String ^cmd = Processes::CurrentProcess::CommandLine;
	Platform::Array<String^> ^infos;
	infos = FileSystem::NativeFileSystem::GetDriveLetters();
//	Registry::NativeRegistry::ReadMultiString(Registry::RegistryHive::HKLM,
//		ref new Platform::String(L"SOFTWARE\\Microsoft\\SecurityManager\\Applications\\TELCPL"),
//		ref new Platform::String(L"RequiredCapabilities"), &infos);
//	Registry::NativeRegistry::GetSubKeyNames(Registry::RegistryHive::HKLM,
//		ref new Platform::String(L"SOFTWARE\\Microsoft\\SecurityManagerrr"),
//		&infos);
//	Platform::String ^val = ref new String(L"CAPABILITY_CLASS_THIRD_PARTY_APPLICATIONS\0\0\0\0", 43);
	unsigned foo = RPCCompWriteMulti(L"\\PROGRAMS\\CALC7");
	Array<uint8> ^data;
	RegistryType type = RegistryType::IntegerBigEndian;
	NativeRegistry::QueryValue(RegistryHive::HKLM,
		ref new String(L"SECURITY\\Policy\\SecDesc"),
		nullptr,
		&type,
		&data);
	uint32 err = NativeRegistry::GetError();
//	RPCComponent::CRPCComponent::Registry_SetString(
//		(unsigned)(Registry::RegistryHive::HKLM),
//		ref new Platform::String(L"SOFTWARE\\Microsoft\\SecurityManager\\CapabilityClasses"),
//		ref new Platform::String(L"ID_CAP_BUILTIN_TCB"),
//		ref new Platform::String(L"ID_CAP_DEVELOPERUNLOCK_API"),
//		val,
//		&foo);

	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}