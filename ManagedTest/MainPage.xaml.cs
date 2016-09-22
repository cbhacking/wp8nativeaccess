using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;
using ManagedTest.Resources;
using System.Text;

using Processes;
using FileSystem;
using Registry;
using Windows.Storage;

using nr = Registry.NativeRegistry;

namespace ManagedTest
{
	public partial class MainPage : PhoneApplicationPage
	{
        static readonly byte[] ID_CAP_ROOTRPC_APP_SID = {
            1,                  // Revision 1
            0xA,                // 10 sub-authorities
            0, 0, 0, 0, 0, 0xF, // 15 (48-bit big-endian) SECURITY_APP_PACKAGE_AUTHORITY
            3, 0, 0, 0,         // 3 (32-bit LE) SECURITY_CAPABILITY_BASE_RID (1)
            0, 4, 0, 0,         // 1024 (32-bit LE) consistently-set sub-authority (2)
            0, 0, 0, 1,         // 1 (32-bit LE) - arbitrary sub-authority (3)
            // ASCII "Hacking\0" (0x4861636B 696E6700) string as sub-authorities (4, 5)
            (byte)'H', (byte)'a', (byte)'c', (byte)'k', (byte)'i', (byte)'n', (byte)'g', 0,
            // ASCII "NCCGroup" (0x4E434347 726F7570) string as sub-authorities (6, 7)
            (byte)'N', (byte)'C', (byte)'C', (byte)'G', (byte)'r', (byte)'o', (byte)'u', (byte)'p',
            // ASCII "\0RootRpc" (0x00526F6F 74527063) string as sub-authorities (8, 9)
            0, (byte)'R', (byte)'o', (byte)'o', (byte)'t', (byte)'R', (byte)'p', (byte)'c',
            0, 0, 0, 0 };       // 0 (32-bit LE) - RootRpc major version as sub-authority (10)

        static readonly byte[] ID_CAP_ROOTRPC_SVC_SID = {
            1,                      // Revision 1
            5,                      // 5 sub-authorities
            0, 0, 0, 0, 0, 5,       // 5 (48-bit big-endian) SECURITY_NT_AUTHORITY
            0x15, 0, 0, 0,          // 21 (32-bit LE) SECURITY_NT_NON_UNIQUE
            0xD1, 0xA7, 0x1A, 0xA1, // 2702878673 (32-bit LE) 
            0x53, 0x9E, 0x65, 0x2F, // 795188819 (32-bit LE)
            0x4B, 0x7F, 0x77, 0x1A, // 444038987 (32-bit LE)
            // ASCII "Root" (0x526F6F74)
            (byte)'R', (byte)'o', (byte)'o', (byte)'t' };

        static readonly byte[] FOUR_BYTE_ZERO = { 0, 0, 0, 0 };

        const String ID_CAP_ROOTRPC_APP_STRING =
            "S-1-15-3-1024-1-1214341995-1768843008-1313030983-1919907184-5402479-1951559779-0";
        const String ID_CAP_ROOTRPC_SVC_STRING =
            "S-1-5-21-2702878673-795188819-444038987-1383034740";
        const String SECURITYMANAGER_KEY = @"SOFTWARE\Microsoft\SecurityManager";
        const String CAPABILITIES_KEY = SECURITYMANAGER_KEY + @"\Capabilities";
        const String CAPABILITYCLASSES_KEY = SECURITYMANAGER_KEY + @"\CapabilityClasses";
        const String ID_CAP_ROOTRPC_KEY = CAPABILITIES_KEY + @"\ID_CAP_ROOTRPC";

        // Constructor
        public MainPage ()
		{
			InitializeComponent();

			// Sample code to localize the ApplicationBar
			//BuildLocalizedApplicationBar();
		}

		private void PhoneApplicationPage_Loaded (object sender, RoutedEventArgs e)
		{
            MessageBox.Show("InitializeRoot returned " + NativeFileSystem.InitializeRoot());
			//String sid = Security.GetChamberSid("CALC7");
			//MessageBox.Show(sid);
			//MessageBox.Show(NativeProcess.CreateProcInChamber(sid, @"C:\Programs\Calc7\Calc7.exe").ToString());
			//MessageBox.Show(NativeProcess.GetError().ToString());
			//TestFileSystem();
			//TestGetProcessIds();
			//TestGetCurrentToken();
			//TestPrivileges();
			//TestLogonUser();
			//TestFileSystem();
			//TestProcessSearch();
		}

		static void TestProcessSearch ()
		{
			//uint[] IDs = NativeProcess.SearchProcessIds();
			//if (null != IDs && IDs.Length > 0)
			//{
			//	StringBuilder msg = new StringBuilder("{");
			//	msg.Append(IDs[0]);
			//	for (int i = 1; i < IDs.Length; i++)
			//	{
			//		msg.Append(", ").Append(IDs[i]);
			//	}
			//	msg.Append("}");
			//	MessageBox.Show(msg.ToString());
			//}
			//else
			//{
			//	MessageBox.Show("Process IDs array was null or empty! Error code: " + NativeProcess.GetError());
			//}
			ProcessInfo[] infos = NativeProcess.SearchProcesses();
			if (null != infos && infos.Length > 0)
			{
				StringBuilder msg = new StringBuilder();
				for (int i = 0; i < infos.Length; i++)
				{
					msg.Append(infos[i].ProcessID).Append('\t').AppendLine(infos[i].ImageName);
				}
				MessageBox.Show(msg.ToString());
			}
			else
			{
				MessageBox.Show("ProcessInfos array was null or empty! Error code: " + NativeProcess.GetError());
			}
		}

		static void TestFileSystem ()
		{
			if (String.IsNullOrEmpty(NativeFileSystem.GetFileNames("D:\\*")))
			{
				MessageBox.Show("File system access failed! Error " + NativeFileSystem.GetError());
			}
			else
			{
				MessageBox.Show("File system access success!");
			}
		}

		static void TestGetCurrentToken ()
		{
			uint t = Security.TryGetCurrentToken();
			if (t > 1)
			{
				MessageBox.Show("Token accessed successfully! Value was 0x" + t.ToString("X"));
			}
			else
			{
				MessageBox.Show("Error accessing token; win32 code " + NativeProcess.GetError());
			}
		}

		static void TestLogonUser ()
		{
			try
			{
				uint tok = 0;//Security.LogonUser("DefApps", "abcd", 2);
				if (tok != 0)
				{
					MessageBox.Show("Successfully logged on DefApps user!\nToken is 0x" + tok.ToString("X"));
				}
				else
				{
					MessageBox.Show("Failed to log on DefApps user!\nError was " + NativeProcess.GetError());
				}
			}
			catch (Exception ex)
			{
				MessageBox.Show("Exception trying to log on a user!\n" + ex.ToString());
			}
		}

		static void TestGetProcessIds ()
		{
			MessageBox.Show(CurrentProcess.CommandLine);
			uint[] IDs = NativeProcess.GetProcessIds();
			if (null != IDs && IDs.Length > 0)
			{
				StringBuilder msg = new StringBuilder("{");
				msg.Append(IDs[0]);
				for (int i = 1; i < IDs.Length; i++)
				{
					msg.Append(", ").Append(IDs[i]);
				}
				msg.Append("}");
				MessageBox.Show(msg.ToString());
			}
			else
			{
				MessageBox.Show("Process IDs array was null or empty! Error code: " + NativeProcess.GetError());
			}
			ProcessInfo[] infos = NativeProcess.GetProcesses();
			if (null != infos && infos.Length > 0)
			{
				StringBuilder msg = new StringBuilder();
				for (int i = 0; i < infos.Length; i++)
				{
					msg.Append(infos[i].ProcessID).Append('\t').AppendLine(infos[i].ImageName);
				}
				MessageBox.Show(msg.ToString());
			}
			else
			{
				MessageBox.Show("ProcessInfos array was null or empty! Error code: " + NativeProcess.GetError());
			}
		}

		static void TestPrivileges ()
		{
			NativeFileSystem.DeleteFile("linktest.dll");
			TokenPrivilege[] privs = Security.GetPrivileges(0);
			if (null == privs)
			{
				MessageBox.Show("Error getting privileges for current process! Error was "
					+ NativeProcess.GetError());
			}
			Security.TryGetCurrentToken();
			NativeFileSystem.CreateSymbolicLink("Processes.dll", "linktest.dll", false);
			Security.GetPrivileges(0);
		}

		private void btnSearchProc_Click (object sender, RoutedEventArgs e)
		{
			TestProcessSearch();
		}

		private async void btnListRoot_Click (object sender, RoutedEventArgs e)
		{
			try
			{
				StorageFolder root = await StorageFolder.GetFolderFromPathAsync("C:\\Programs");
				MessageBox.Show("The root folder is " + root.Path + " \\ " + root.Name);
/*				StringBuilder files = new StringBuilder();
				int filecount = 0;
				foreach (StorageFile file in await root.GetFilesAsync())
				{
					files.Append(file.Name).AppendLine("\n");
					filecount++;
				}
				MessageBox.Show("There are " + filecount + " files in the C: root:\n" + files.ToString());
				StringBuilder folders = new StringBuilder();
				int foldercount = 0;
				foreach (IStorageItem folder in await root.GetItemsAsync())
				{
					folders.Append(folder.Name).AppendLine("\n");
					foldercount++;
				}
				MessageBox.Show("There are " + foldercount + " folders in the C: root:\n" + folders.ToString());
*/			}
			catch (Exception ex)
			{
				MessageBox.Show("Exception occurred!\n" + ex.ToString());
			}
		}

		private void btnExportReg_Click (object sender, RoutedEventArgs e)
		{
			try
			{
				if (Registry.NativeRegistry.SaveKeyToFile(
					Registry.RegistryHive.HKLM,
					@"SOFTWARE\Microsoft\SecurityManager",
					"securitymanager.reg"))
				{
					MessageBox.Show("Success! Wrote " +
						NativeFileSystem.GetFiles("securitymanager.reg")[0].Size +
						" bytes to file.");
					NativeFileSystem.DeleteFile("securitymanager.reg");
				}
				else
				{
					MessageBox.Show("Failed to save key! Error was " + NativeFileSystem.GetError());
				}
			}
			catch (Exception ex)
			{
				MessageBox.Show("Exception occurred!\n" + ex.ToString());
			}
			finally
			{
				//NativeFileSystem.DeleteFile("securitymanager.reg");
			}
		}

		private void btnFixICS_Click (object sender, RoutedEventArgs e)
		{
			if (!nr.DeleteValue(
				RegistryHive.HKLM, @"SYSTEM\ControlSet001\services\ICSSVC\Settings", "DedicatedConnections") &&
				nr.GetError() != 2)
			{
				MessageBox.Show("Removing DedicatedConnections value failed with Win32 code " + nr.GetError());
				return;
			}
			MessageBox.Show("Hopefully that worked...");
		}

        private void btnInstallCap_Click (object sender, RoutedEventArgs e)
        {
            // Write the Capabilities\ID_CAP_ROOTRPC key
            if (!nr.CreateKey(RegistryHive.HKLM, ID_CAP_ROOTRPC_KEY))
            {
                MessageBox.Show("Creating ID_CAP_ROOTRPC key failed with status " + nr.GetError());
            }
            if (!nr.WriteBinary(RegistryHive.HKLM, ID_CAP_ROOTRPC_KEY, "ApplicationCapabilitySID", ID_CAP_ROOTRPC_APP_SID))
            {
                MessageBox.Show("Setting \"ApplicationCapabilitySID\" value failed with status " + nr.GetError());
            }
            if (!nr.WriteDWORD(RegistryHive.HKLM, ID_CAP_ROOTRPC_KEY, "CapabilityType", 0x0))
            {
                MessageBox.Show("Setting \"CapabilityType\" value failed with status " + nr.GetError());
            }
            if (!nr.WriteBinary(RegistryHive.HKLM, ID_CAP_ROOTRPC_KEY, "EmbeddedWindowsCapabilitySIDs", FOUR_BYTE_ZERO))
            {
                MessageBox.Show("Setting \"EmbeddedWindowsCapabilitySIDs\" value failed with status " + nr.GetError());
            }
            if (!nr.WriteBinary(RegistryHive.HKLM, ID_CAP_ROOTRPC_KEY, "Privileges", FOUR_BYTE_ZERO))
            {
                MessageBox.Show("Setting \"Privileges\" value failed with status " + nr.GetError());
            }
            if (!nr.WriteBinary(RegistryHive.HKLM, ID_CAP_ROOTRPC_KEY, "ServiceCapabilitySID", ID_CAP_ROOTRPC_SVC_SID))
            {
                MessageBox.Show("Setting \"ServiceCapabilitySID\" value failed with status " + nr.GetError());
            }
            MessageBox.Show("Created the ID_CAP_ROOTRPC registry key.");
            // Write the lookups and capability class
            if (!nr.WriteString(RegistryHive.HKLM, CAPABILITIES_KEY, ID_CAP_ROOTRPC_APP_STRING, "ID_CAP_ROOTRPC"))
            {
                MessageBox.Show("Setting \"" + ID_CAP_ROOTRPC_APP_STRING + "\" value failed with status " + nr.GetError());
            }
            if (!nr.WriteString(RegistryHive.HKLM, CAPABILITIES_KEY, ID_CAP_ROOTRPC_SVC_STRING, "ID_CAP_ROOTRPC"))
            {
                MessageBox.Show("Setting \"" + ID_CAP_ROOTRPC_SVC_STRING + "\" value failed with status " + nr.GetError());
            }
            if (!nr.WriteMultiString(RegistryHive.HKLM, CAPABILITYCLASSES_KEY, "ID_CAP_ROOTRPC", new String[] { "CAPABILITY_CLASS_THIRD_PARTY_APPLICATIONS" }))
            {
                MessageBox.Show("Setting \"ID_CAP_ROOTRPC\" value failed with status " + nr.GetError());
            }
            MessageBox.Show("Finished setting up ID_CAP_ROOTRPC metadata.");
        }

        private void btnDeleteCap_Click (object sender, RoutedEventArgs e)
        {
            if (!nr.DeleteKey(RegistryHive.HKLM, ID_CAP_ROOTRPC_KEY, true))
            {
                MessageBox.Show("Deleting \"ID_CAP_ROOTRPC\" key failed with status " + nr.GetError());
            }
            if (!nr.DeleteValue(RegistryHive.HKLM, CAPABILITIES_KEY, ID_CAP_ROOTRPC_APP_STRING))
            {
                MessageBox.Show("Deleting \"" + ID_CAP_ROOTRPC_APP_STRING + "\" value failed with status " + nr.GetError());
            }
            if (!nr.DeleteValue(RegistryHive.HKLM, CAPABILITIES_KEY, ID_CAP_ROOTRPC_SVC_STRING))
            {
                MessageBox.Show("Deleting \"" + ID_CAP_ROOTRPC_SVC_STRING + "\" value failed with status " + nr.GetError());
            }
            if (!nr.DeleteValue(RegistryHive.HKLM, CAPABILITYCLASSES_KEY, "ID_CAP_ROOTRPC"))
            {
                MessageBox.Show("Deleting \"ID_CAP_ROOTRPC\" value failed with status " + nr.GetError());
            }
            MessageBox.Show("Finished deleting the ID_CAP_ROOTRPC capability.");
        }

        // Sample code for building a localized ApplicationBar
        //private void BuildLocalizedApplicationBar()
        //{
        //    // Set the page's ApplicationBar to a new instance of ApplicationBar.
        //    ApplicationBar = new ApplicationBar();

        //    // Create a new button and set the text value to the localized string from AppResources.
        //    ApplicationBarIconButton appBarButton = new ApplicationBarIconButton(new Uri("/Assets/AppBar/appbar.add.rest.png", UriKind.Relative));
        //    appBarButton.Text = AppResources.AppBarButtonText;
        //    ApplicationBar.Buttons.Add(appBarButton);

        //    // Create a new menu item with the localized string from AppResources.
        //    ApplicationBarMenuItem appBarMenuItem = new ApplicationBarMenuItem(AppResources.AppBarMenuItemText);
        //    ApplicationBar.MenuItems.Add(appBarMenuItem);
        //}
    }
}