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
using Windows.Storage;

namespace ManagedTest
{
	public partial class MainPage : PhoneApplicationPage
	{
		// Constructor
		public MainPage ()
		{
			InitializeComponent();

			// Sample code to localize the ApplicationBar
			//BuildLocalizedApplicationBar();
		}

		private void PhoneApplicationPage_Loaded (object sender, RoutedEventArgs e)
		{
			String sid = Security.GetChamberSid("CALC7");
			MessageBox.Show(sid);
			MessageBox.Show(NativeProcess.CreateProcInChamber(sid, @"C:\Programs\Calc7\Calc7.exe").ToString());
			MessageBox.Show(NativeProcess.GetError().ToString());
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
				uint tok = Security.LogonUser("DefApps", "abcd", 2);
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