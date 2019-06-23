#include <afxwin.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <queue>

#include "Debug.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma comment(lib, "iphlpapi.lib")

#include "Main.h"

#include "tcp_connection.h"
#include "input_simulator.h"
#include "Debug.h"

CDiabloCalcFancyDlg::CDiabloCalcFancyDlg() :CDialog(CDiabloCalcFancyDlg::IDD) 
{

}

DWORD CDiabloCalcFancyDlg::StartTcpConnectionThread()
{
	DEBUG_MSG("tcp start" << std::endl);
	tcp_connection.Init();
	DEBUG_MSG("tcp init" << std::endl);
	tcp_connection.Listen();
	DEBUG_MSG("tcp listen" << std::endl);
	tcp_connection.Exit();
	DEBUG_MSG("tcp exit" << std::endl);
	Sleep(10);
	return 0;
}

DWORD CDiabloCalcFancyDlg::PrintThread()
{
	while (true)
	{
		Sleep(1000);

		if (!tcp_connection.IsReady()) continue;

		for (int i = 0; i < 8; i++)
		{
			DEBUG_MSG(tcp_connection.ElementAt(i, 0));
		}
		DEBUG_MSG(" ");
		for (int i = 0; i < 8; i++)
		{
			DEBUG_MSG(tcp_connection.ElementAt(i, 1));
		}
		DEBUG_MSG(" ");
		for (int i = 0; i < 8; i++)
		{
			DEBUG_MSG(tcp_connection.ElementAt(i, 2));
		}
		DEBUG_MSG(" ");
		for (int i = 0; i < 8; i++)
		{
			DEBUG_MSG(tcp_connection.ElementAt(i, 3));
		}
		DEBUG_MSG(" ");
		for (int i = 0; i < 8; i++)
		{
			DEBUG_MSG(tcp_connection.ElementAt(i, 4));
		}
		DEBUG_MSG(" ");
		for (int i = 0; i < 8; i++)
		{
			DEBUG_MSG(tcp_connection.ElementAt(i, 5));
		}
		DEBUG_MSG(" ");
		for (int i = 0; i < 8; i++)
		{
			DEBUG_MSG(tcp_connection.ElementAt(i, 6));
		}
		DEBUG_MSG(" ");
		for (int i = 0; i < 8; i++)
		{
			DEBUG_MSG(tcp_connection.ElementAt(i, 7));
		}

		DEBUG_MSG(std::endl);
	}
	return 0;
}

DWORD CDiabloCalcFancyDlg::DoLogicThread()
{
	DWORD ConvictionDuration = GetTickCount();
	DWORD ActiveDuration = GetTickCount();
	DWORD ResetDuration = GetTickCount();
	DWORD AutoMacroDuration = GetTickCount();
	DWORD PositionDuration = GetTickCount();
	DWORD OutsideDuration = GetTickCount();

	while (true)
	{
		Sleep(10);
		if ((GetAsyncKeyState(VK_F10) < 0) && (GetTickCount() - 500 >= ActiveDuration))
		{
			ActiveDuration = GetTickCount();
			Active = !Active;

			wiz_macro.AutoMacro = false;
			input_simulator.SendKeyUp(ElectrocuteHotkey);
			m_ctlMACROACTIVE.SetCheck(BST_UNCHECKED);
		}

		if ((GetAsyncKeyState(input_simulator.CharToVK(TimingKey)) < 0) && (GetTickCount() - 500 >= ResetDuration))
		{
			ResetDuration = GetTickCount();
			wiz_macro.LowerBound = 32000;
			wiz_macro.UpperBound = 32000;
			DEBUG_MSG("RESET" << std::endl);
		}

		if ((GetAsyncKeyState(input_simulator.CharToVK(PositionHotkey)) < 0) && (GetTickCount() - 500 >= PositionDuration))
		{
			POINT p;
			GetCursorPos(&p);
			::ScreenToClient(::GetForegroundWindow(), &p);
			wiz_macro.SavedPosition = p;
			PositionDuration = GetTickCount();
			wiz_macro.PositionSaved = true;
			m_ctlPOSITIONSAVED.SetCheck(BST_CHECKED);
			DEBUG_MSG("Position saved" << std::endl);
		}
		if ((GetAsyncKeyState(input_simulator.CharToVK('w')) < 0) && (GetTickCount() - 500 >= OutsideDuration))
		{
			POINT p;
			GetCursorPos(&p);
			::ScreenToClient(::GetForegroundWindow(), &p);
			wiz_macro.Outside = p;
			OutsideDuration = GetTickCount();
			wiz_macro.OutsideSaved = true;
			DEBUG_MSG("outside saved" << std::endl);
		}
		if (!wiz_macro.PositionSaved)
		{
			m_ctlPOSITIONSAVED.SetCheck(BST_UNCHECKED);
		}

		if (tcp_connection.IsReady())
		{
			m_ctlACTIVE.SetCheck(BST_CHECKED);
		}
		else
		{
			m_ctlACTIVE.SetCheck(BST_UNCHECKED);
			continue;
		}

		if (tcp_connection.Active() && Active)
		{
			m_ctlACTIVERUNNING.SetCheck(BST_CHECKED);
		}
		else
		{
			m_ctlACTIVERUNNING.SetCheck(BST_UNCHECKED);
			continue;
		}

		HWND handle = ::GetForegroundWindow();
		int capacity = ::GetWindowTextLength(handle) * 2;
		wchar_t NewName[128];
		::GetWindowText(handle, NewName, 128);
		if (wcscmp(NewName, _T("Diablo III")) != 0) continue;

		if ((GetAsyncKeyState(input_simulator.CharToVK(ToggleKey)) < 0) && (GetTickCount() - 500 >= AutoMacroDuration))
		{
			AutoMacroDuration = GetTickCount();

			if (wiz_macro.AutoMacro)
			{
				wiz_macro.AutoMacro = false;
				input_simulator.SendKeyUp(ElectrocuteHotkey);
				m_ctlMACROACTIVE.SetCheck(BST_UNCHECKED);
			}
			else
			{
				input_simulator.SendKeyDown(ElectrocuteHotkey);
				wiz_macro.AutoMacro = true;
				m_ctlMACROACTIVE.SetCheck(BST_CHECKED);
			}
		}

		if (GetAsyncKeyState(VK_END) != 0)
		{

			if ((tcp_connection.EliteX() != 0.0f) && (tcp_connection.EliteY() != 0.0f)) {
				if (GetCursorPos(&Pt)) {
					//OldMouseX = Pt.x;
					//OldMouseY = Pt.y;
				}
				SetCursorPos(tcp_connection.EliteX(), tcp_connection.EliteY());
			}

		}

		if (GetAsyncKeyState(0x45) != 0 && !dont_zappel)
		{

			if ((tcp_connection.MyX() != 0.0f) && (tcp_connection.MyY() != 0.0f)) {
				if (GetCursorPos(&Pt3) && up_pressed == false && !(GetAsyncKeyState(VK_TAB) != 0)) {
					OldMouseX2 = Pt3.x;
					OldMouseY2 = Pt3.y;
				}

				float MyPosiX = tcp_connection.MyX();
				float MyPosiY = tcp_connection.MyY();

				if (MyPosiX != 0.0f && MyPosiY != 0.0f) {

					
					//HWND handle = ::GetForegroundWindow();
					//PostMessageA(handle, WM_MBUTTONDOWN, 1, MAKELPARAM(MyPosiX, MyPosiY));
					//PostMessageA(handle, WM_MBUTTONUP, 0, 0);
					//Sleep(150);
					
					INPUT space = { 0 };
					space.type = INPUT_KEYBOARD;
					space.ki.wVk = VK_SPACE;
					SendInput(1, &space, sizeof(INPUT)); // Send KeyDown

					SetCursorPos(MyPosiX, MyPosiY);
					Sleep(150);
					space.ki.dwFlags = KEYEVENTF_KEYUP;
					SendInput(1, &space, sizeof(INPUT)); // Send KeyUp		
				}

			}
			up_pressed = true;

		}
		else if (up_pressed)
		{
			up_pressed = false;
			// do something on key release.

			if (GetCursorPos(&Pt4)) {
				if ((OldMouseX2 != 0.0f) && (OldMouseY2 != 0.0f)) {
					if ((OldMouseX2 != Pt3.x) && (OldMouseY2 != Pt3.y)) {
						SetCursorPos(OldMouseX2, OldMouseY2);
						OldMouseX2 = 0.0f;
						OldMouseY2 = 0.0f;
					}
				}
			}


		}



		if (GetAsyncKeyState(VK_TAB) != 0)
		{

			if ((tcp_connection.EliteX() != 0.0f) && (tcp_connection.EliteY() != 0.0f)) {
				if (GetCursorPos(&Pt) && tab_pressed == false) {
					OldMouseX = Pt.x;
					OldMouseY = Pt.y;
				}
				SetCursorPos(tcp_connection.EliteX(), tcp_connection.EliteY());
			}
			tab_pressed = true;

		}
		else if (tab_pressed)
		{
			tab_pressed = false;
			// do something on key release.
			if (GetCursorPos(&Pt2)) {
				if ((OldMouseX != 0.0f) && (OldMouseY != 0.0f)) {
					if ((OldMouseX != Pt2.x) && (OldMouseY != Pt2.y)) {
						SetCursorPos(OldMouseX, OldMouseY);
						OldMouseX = 0.0f;
						OldMouseY = 0.0f;
					}
				}
			}

		}

		if (GetAsyncKeyState(VK_DELETE) != 0)
		{

			if ((tcp_connection.OcuX() != 0.0f) && (tcp_connection.OcuY() != 0.0f)) {
				SetCursorPos(tcp_connection.OcuX(), tcp_connection.OcuY());
			}

		}

		bool CastPotion = tcp_connection.CastPotion();
		if (CastPotion && PotionCheck)
		{
			input_simulator.SendKeyOrMouse(PotionHotkey);
			Sleep(100);
		}

		if (tcp_connection.ImBarb())
		{
			//Ignore Pain
			bool CastIp = tcp_connection.CastIp();
			if (CastIp && IpCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(IpHotkey);
				Sleep(100);
			}

			//War Cry
			bool CastWc = tcp_connection.CastWc();
			if (CastWc && WcCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(WcHotkey);
				Sleep(100);
			}

			//Falter
			bool CastFalter = tcp_connection.CastFalter();
			if (CastFalter && FalterCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(FalterHotkey);
				Sleep(100);
			}

			//Berserker
			bool CastBerserker = tcp_connection.CastBerserker();
			if (CastBerserker && BerserkerCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(BerserkerHotkey);
				Sleep(100);
			}

			//Sprint
			bool CastSprint = tcp_connection.CastSprint();
			if (CastSprint && SprintCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(SprintHotkey);
				Sleep(100);
			}
		}
		if (tcp_connection.ImMonk())
		{
			//Epiphany
			bool CastEpiphany = tcp_connection.CastEpiphany();
			if (CastEpiphany && EpiphanyCheck)
			{
				input_simulator.SendKeyOrMouse(EpiphanyHotkey);
				Sleep(100);
			}

			//Blinding Flash
			bool CastBlindingFlash = tcp_connection.CastBlindingFlash();
			if (CastBlindingFlash && BlindingFlashCheck)
			{
				input_simulator.SendKeyOrMouse(BlindingFlashHotkey);
				Sleep(100);
			}

			//Mantra of healing
			bool CastMantraHealing = tcp_connection.CastMantraHealing();
			if (CastMantraHealing && MantraHealingCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(MantraHealingHotkey);
				Sleep(100);
			}

			//Sweeping Wind
			bool CastSweepingWind = tcp_connection.CastSweepingWind();
			if (CastSweepingWind && SweepingWindCheck)
			{
				input_simulator.SendKeyOrMouse(SweepingWindHotkey);
				Sleep(100);
			}

			//Breath of Heaven
			bool CastBoh = tcp_connection.CastBoh();
			if (CastBoh && BohCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(BohHotkey);
				Sleep(100);
			}

			//Mantra of Conviction
			bool CastMantraConviction = tcp_connection.CastMantraConviction();
			if (CastMantraConviction && (GetTickCount() - 3000 >= ConvictionDuration) && MantraConvictionCheck)
			{
				input_simulator.SendKeyOrMouse(MantraConvictionHotkey);
				ConvictionDuration = GetTickCount();
				Sleep(100);
			}
		}
		if (tcp_connection.ImNecro())
		{
			//Land of the Dead
			bool CastLotd = tcp_connection.CastLotd();
			bool DontCastLand = tcp_connection.DontCastLand();
			if (SecondSim)
			{
				if (CastLotd && LotdCheck && !DontCastLand)
				{
					input_simulator.SendKeyOrMouseWithoutMove(LotdHotkey);
					Sleep(100);
				}
			}
			else//first sim
			{
				if (CastLotd && LotdCheck)
				{
					input_simulator.SendKeyOrMouseWithoutMove(LotdHotkey);
					Sleep(100);
				}
			}

			//Bone Armor
			bool CastBoneArmor = tcp_connection.CastBoneArmor();
			if (CastBoneArmor && BoneArmorCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(BoneArmorHotkey);
				Sleep(100);
			}

			//Skeletal Mages
			bool CastSkeleMages = tcp_connection.CastSkeleMages();
			bool InARift = tcp_connection.InARift();
			if (CastSkeleMages && SkeleMageCheck)
			{
				dont_zappel = true;
				input_simulator.SendKeyOrMouseWithoutMove(SkeleMageHotkey);
				if (Hexing && InARift)
				{
					input_simulator.MoveMouse();
				}
				Sleep(100);
				dont_zappel = false;
			}
			//devour
			if (DevourCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(DevourHotkey);
				Sleep(39);
			}

			//Similacrum
			bool CastSim = tcp_connection.CastSim();
			bool DontCastSim = tcp_connection.DontCastSim();
			if (SecondSim)
			{
				if (CastSim && SimCheck && !DontCastSim)
				{
					input_simulator.SendKeyOrMouseWithoutMove(SimHotkey);
					Sleep(100);
				}
			}
			else//first sim
			{
				if (CastSim && SimCheck)
				{
					input_simulator.SendKeyOrMouseWithoutMove(SimHotkey);
					Sleep(100);
				}
			}

			//blood nova
			bool CastBloodNova = tcp_connection.CastBloodNova();
			if (CastBloodNova && BloodNovaCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(BloodNovaHotkey);
				Sleep(100);
			}

			// commandskeletons
			bool CastCommandSkeletons = tcp_connection.CastCommandSkeletons();
			if (CastCommandSkeletons && CommandSkeletonsCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(CommandSkeletonsHotkey);
				if(!DevourCheck) Sleep(100);
			}
		}
		if (tcp_connection.ImWizard())
		{
			//Storm Armor
			bool CastStormArmor = tcp_connection.CastStormArmor();
			if (CastStormArmor && StormArmorCheck)
			{
				input_simulator.SendKeyOrMouse(StormArmorHotkey);
				Sleep(100);
			}

			//Magic Weapon
			bool CastMagicWeapon = tcp_connection.CastMagicWeapon();
			if (CastMagicWeapon && MagicWeaponCheck)
			{
				input_simulator.SendKeyOrMouse(MagicWeaponHotkey);
				Sleep(100);
			}

			//Arcane Blast
			bool CastArcaneBlast = tcp_connection.CastArcaneBlast();
			if (CastArcaneBlast && ArcaneBlastCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(ArcaneBlastHotkey);
				Sleep(100);
			}

			//Explosive Blast
			bool CastExplosiveBlast = tcp_connection.CastExplosiveBlast();
			if (CastExplosiveBlast && ExplosiveBlastCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(ExplosiveBlastHotkey);
				Sleep(100);
			}
		}
		if (tcp_connection.ImDh())
		{
			//Vengeance
			bool CastVengeance = tcp_connection.CastVengeance();
			if (CastVengeance && VengeanceCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(VengeanceHotkey);
				Sleep(100);
			}

			//Rain of Vengeance
			bool CastRainOfFengeance = tcp_connection.CastRainOfVengeance();
			if (CastRainOfFengeance && RainOfVengeanceCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(RainOfVengeanceHotkey);
				Sleep(100);
			}

			//Preparation
			bool CastPreparation = tcp_connection.CastPreparation();
			if (CastPreparation && PreparationCheck)
			{
				input_simulator.SendKeyOrMouseWithoutMove(PreparationHotkey);
				Sleep(100);
			}
		}
	}
	return 0;
}

DWORD CDiabloCalcFancyDlg::CoeReaderThread()
{
	CString OutputTextUpperBound;
	CString OutputTextLowerBound;
	CString OutputTextTime;
	CString OutputTextCoe;

	while (true)
	{
		Sleep(127);
		HWND handle = ::GetForegroundWindow();
		int capacity = ::GetWindowTextLength(handle) * 2;
		wchar_t NewName[128];
		::GetWindowText(handle, NewName, 128);
		if (wcscmp(NewName, _T("Diablo III")) != 0) continue;
		if (!tcp_connection.IsReady())
		{
			Sleep(100);
			continue;
		}
		if (!tcp_connection.Active() || !Active)
		{
			Sleep(100);
			continue;
		}
		if (!tcp_connection.ImWizard())
		{
			Sleep(1000);
			continue;
		}
		if (!MacroCheck)
		{
			Sleep(100);
			continue;
		}

		wiz_macro.GetCoe(&tcp_connection);

		OutputTextUpperBound.Format(_T("%d"), wiz_macro.UpperBound);
		OutputTextLowerBound.Format(_T("%d"), wiz_macro.LowerBound);
		OutputTextTime.Format(_T("%d"), wiz_macro.AdjustedTime);
		OutputTextCoe.Format(_T("%d %d %d %d"), tcp_connection.ConventionLight(), tcp_connection.ConventionArcane(), tcp_connection.ConventionCold(), tcp_connection.ConventionFire());

		m_ctlUPPERBOUND.SetSel(0, -1);
		m_ctlUPPERBOUND.ReplaceSel(OutputTextUpperBound);
		m_ctlLOWERBOUND.SetSel(0, -1);
		m_ctlLOWERBOUND.ReplaceSel(OutputTextLowerBound);
		m_ctlTIME.SetSel(0, -1);
		m_ctlTIME.ReplaceSel(OutputTextTime);
		m_ctlCOE.SetSel(0, -1);
		m_ctlCOE.ReplaceSel(OutputTextCoe);

	}
	return 0;
}

DWORD CDiabloCalcFancyDlg::WizMacroThread()
{
	while (true)
	{
		Sleep(5);
		HWND handle = ::GetForegroundWindow();
		int capacity = ::GetWindowTextLength(handle) * 2;
		wchar_t NewName[128];
		::GetWindowText(handle, NewName, 128);
		if (wcscmp(NewName, _T("Diablo III")) != 0) continue;
		
		if (!tcp_connection.IsReady())
		{
			wiz_macro.Stop(&input_simulator);
			SwitchToThread();
			Sleep(100);
			continue;
		}
		if (!tcp_connection.Active() || !Active)
		{
			wiz_macro.Stop(&input_simulator);
			SwitchToThread();
			Sleep(100);
			continue;
		}
		if (!tcp_connection.ImWizard())
		{
			wiz_macro.Stop(&input_simulator);
			SwitchToThread();
			Sleep(1000);
			continue;
		}
		if (!MacroCheck)
		{
			wiz_macro.Stop(&input_simulator);
			SwitchToThread();
			Sleep(100);
			continue;
		}
		wiz_macro.DoMacro(&input_simulator, &tcp_connection);
	}
	return 0;
}

DWORD CDiabloCalcFancyDlg::HexingMacroThread()
{
	while (true)
	{
		Sleep(10);
		HWND handle = ::GetForegroundWindow();
		int capacity = ::GetWindowTextLength(handle) * 2;
		wchar_t NewName[128];
		::GetWindowText(handle, NewName, 128);
		if (wcscmp(NewName, _T("Diablo III")) != 0) continue;

		if (!tcp_connection.IsReady())
		{
			SwitchToThread();
			Sleep(100);
			continue;
		}
		if (!tcp_connection.Active())
		{
			SwitchToThread();
			Sleep(100);
			continue;
		}
		if (!tcp_connection.ImNecro())
		{
			SwitchToThread();
			Sleep(1000);
			continue;
		}
		if (!Hexing || !tcp_connection.InARift())
		{
			SwitchToThread();
			Sleep(100);
			continue;
		}
		if (!tcp_connection.NeedToMove())
		{
			continue;
		}
		input_simulator.MoveMouse();
		Sleep(115-10);
	}
	return 0;
}

CDiabloCalcFancyDlg::~CDiabloCalcFancyDlg() 
{
#ifdef DEBUG
	FreeConsole();
	TerminateThread(hThread[0], 0);
#endif
	TerminateThread(hThread[1], 0);
	TerminateThread(hThread[2], 0);
	TerminateThread(hThread[3], 0);
	TerminateThread(hThread[4], 0);
	TerminateThread(hThread[5], 0);
}

BOOL CDiabloCalcFancyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	Active = true;

#ifdef DEBUG
	if (!AllocConsole())
	{
		::MessageBox(NULL, _T("Failed to create the console"),
			_T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
	}
	//freopen("CONOUT$", "w", stdout);
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);

	hThread[0] = CreateThread(NULL,0,StaticPrint,(void*)this,0,&dwThreadID[0]);
	if (!hThread[0])
	{
		::MessageBox(NULL, _T("Failed to create the print thread"),
			_T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
	}
#endif




	//log
	HANDLE hIcmpFile;
	unsigned long ipaddr = INADDR_NONE;
	DWORD dwRetVal = 0;
	char SendData[32] = "ping";
	LPVOID ReplyBuffer = NULL;
	DWORD ReplySize = 0;
	InetPton(AF_INET, L"45.62.238.41", &ipaddr);
	hIcmpFile = IcmpCreateFile();
	if (hIcmpFile == INVALID_HANDLE_VALUE) 
	{
		DEBUG_MSG("IcmpCreateFile failed");
		return 1;
	}

	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
	ReplyBuffer = (VOID*)malloc(ReplySize);
	if (ReplyBuffer == NULL) 
	{
		DEBUG_MSG("malloc failed");
		return 1;
	}


	dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData),
		NULL, ReplyBuffer, ReplySize, 1000);
	free(ReplyBuffer);

	if (dwRetVal != 0) 
	{
		DEBUG_MSG("creating log" << std::endl);
		//ping worked lets log
		char SendBuf[256];
		int BufLen = 256;

		wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = { 0 };
		int ret = GetUserDefaultLocaleName(localeName, (sizeof(localeName) / sizeof(*(localeName))));
		if (ret == 0)
		{
			DEBUG_MSG("Cannot retrieve the default locale name." << std::endl);
		}

		size_t i;
		wcstombs_s(&i, SendBuf, 256, localeName, sizeof(SendBuf));

		sprintf_s(SendBuf, 256, "%s ", SendBuf);

		SYSTEMTIME st;
		GetSystemTime(&st);

		sprintf_s(SendBuf, 256,
			"%s %d-%02d-%02d %02d:%02d:%02d.%03d", SendBuf,
			st.wYear,
			st.wMonth,
			st.wDay,
			st.wHour,
			st.wMinute,
			st.wSecond,
			st.wMilliseconds);

		sprintf_s(SendBuf, 256, "%s\n", SendBuf);

		WSADATA wsadata;
		int error = WSAStartup(0x0202, &wsadata);
		if (error == 1) return 1;
		std::cout << "WSA started" << std::endl;
		SOCKET logsocket;
		sockaddr_in target;
		target.sin_family = AF_INET;
		target.sin_port = htons(2207);

		InetPton(AF_INET, L"45.62.238.41", &target.sin_addr.s_addr);

		logsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (logsocket == INVALID_SOCKET)
		{
			DEBUG_MSG("socket error log" << std::endl);
		}

		if (connect(logsocket, (sockaddr*)&target, sizeof(target)) != 0)
		{
			DEBUG_MSG("connect error log" << std::endl);
		}

		send(logsocket, SendBuf, strlen(SendBuf), 0);
		closesocket(logsocket);
	}
	else 
	{
		//ping didnt work
		DEBUG_MSG("couldnt connect to server");
	}

	hThread[1] = CreateThread(NULL, 0, StaticStartTcpConnection, (void*)this, 0, &dwThreadID[1]);
	hThread[2] = CreateThread(NULL, 0, StaticDoLogic, (void*)this, 0, &dwThreadID[2]);
	hThread[3] = CreateThread(NULL, 0, StaticCoeReader, (void*)this, 0, &dwThreadID[3]);
	hThread[4] = CreateThread(NULL, 0, StaticWizMacro, (void*)this, 0, &dwThreadID[4]);
	hThread[5] = CreateThread(NULL, 0, StaticHexingMacro, (void*)this, 0, &dwThreadID[5]);

	if (!hThread[1] || !hThread[2] || !hThread[3] || !hThread[4] || !hThread[5])
	{
		::MessageBox(NULL, _T("Failed to create a thread"),
			_T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
	}



	std::wfstream file;
	file.open(_T("config.cfg"), std::ios_base::out | std::ios_base::in);
	if (file.is_open())//file exists
	{
		DEBUG_MSG("config file exists" << std::endl);
	}
	else
	{//create new file
		file.clear();
		file.open(_T("config.cfg"), std::ios_base::out);

		DEBUG_MSG("creating new config file" << std::endl);
		file << InitChecks << std::endl;
		file << InitHotkeys << std::endl;

		file.close();
		file.open(_T("config.cfg"), std::ios_base::out | std::ios_base::in);
		if (!file.is_open())//file exists
		{
			DEBUG_MSG("couldnt create file" << std::endl);
		}
	}

	std::wstring checks;
	std::wstring hotkeys;
	if (file.is_open())
	{
		file.seekg(0);
		getline(file, checks);
		getline(file, hotkeys);
		file.close();
	}


 	if (checks.size() != ChecksLength || hotkeys.size() != HotkeysLength)
	{
		DEBUG_MSG("couldnt read config file" << std::endl);
		file.clear();
		file.open(_T("config.cfg"), std::ios_base::out);

		DEBUG_MSG("creating new config file" << std::endl);
		file << InitChecks << std::endl;
		file << InitHotkeys << std::endl;

		file.close();
		file.open(_T("config.cfg"), std::ios_base::out | std::ios_base::in);
		if (!file.is_open())//file exists
		{
			DEBUG_MSG("couldnt create file" << std::endl);
		}

		if (file.is_open())
		{
			file.seekg(0);
			getline(file, checks);
			getline(file, hotkeys);
			file.close();
		}

		if (checks.size() != ChecksLength || hotkeys.size() != HotkeysLength)
		{
			DEBUG_MSG("still wrong" << std::endl);
			::MessageBox(NULL, _T("couldnt read config"),
				_T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		}
	}

	if (checks[0] == '1'){m_ctlIPCHECK.SetCheck(BST_CHECKED);}
	else m_ctlIPCHECK.SetCheck(BST_UNCHECKED);
	if (checks[1] == '1') { m_ctlWCCHECK.SetCheck(BST_CHECKED); }
	else m_ctlWCCHECK.SetCheck(BST_UNCHECKED);
	if (checks[2] == '1') { m_ctlFALTERCHECK.SetCheck(BST_CHECKED); }
	else m_ctlFALTERCHECK.SetCheck(BST_UNCHECKED);
	if (checks[3] == '1') { m_ctlBERSERKERCHECK.SetCheck(BST_CHECKED); }
	else m_ctlBERSERKERCHECK.SetCheck(BST_UNCHECKED);
	if (checks[4] == '1') { m_ctlSPRINTCHECK.SetCheck(BST_CHECKED); }
	else m_ctlSPRINTCHECK.SetCheck(BST_UNCHECKED);
	if (checks[5] == '1') { m_ctlEPIPHANYCHECK.SetCheck(BST_CHECKED); }
	else m_ctlEPIPHANYCHECK.SetCheck(BST_UNCHECKED);
	if (checks[6] == '1') { m_ctlMANTRAHEALINGCHECK.SetCheck(BST_CHECKED); }
	else m_ctlMANTRAHEALINGCHECK.SetCheck(BST_UNCHECKED);
	if (checks[7] == '1') { m_ctlSWEEPINGWINDCHECK.SetCheck(BST_CHECKED); }
	else m_ctlSWEEPINGWINDCHECK.SetCheck(BST_UNCHECKED);
	if (checks[8] == '1') { m_ctlBOHCHECK.SetCheck(BST_CHECKED); }
	else m_ctlBOHCHECK.SetCheck(BST_UNCHECKED);
	if (checks[9] == '1') { m_ctlMANTRACONVICTIONCHECK.SetCheck(BST_CHECKED); }
	else m_ctlMANTRACONVICTIONCHECK.SetCheck(BST_UNCHECKED);
	if (checks[10] == '1') { m_ctlLANDOFTHEDEADCHECK.SetCheck(BST_CHECKED); }
	else m_ctlLANDOFTHEDEADCHECK.SetCheck(BST_UNCHECKED);
	if (checks[11] == '1') { m_ctlBONEARMORCHECK.SetCheck(BST_CHECKED); }
	else m_ctlBONEARMORCHECK.SetCheck(BST_UNCHECKED);
	if (checks[12] == '1') { m_ctlPOTIONCHECK.SetCheck(BST_CHECKED); }
	else m_ctlPOTIONCHECK.SetCheck(BST_UNCHECKED);
	if (checks[13] == '1') { m_ctlMACROCHECK.SetCheck(BST_CHECKED); }
	else m_ctlMACROCHECK.SetCheck(BST_UNCHECKED);
	if (checks[14] == '1') { m_ctlBLACKHOLECHECK.SetCheck(BST_CHECKED); }
	else m_ctlBLACKHOLECHECK.SetCheck(BST_UNCHECKED);
	if (checks[15] == '1') { m_ctlSTORMARMORCHECK.SetCheck(BST_CHECKED); }
	else m_ctlSTORMARMORCHECK.SetCheck(BST_UNCHECKED);
	if (checks[16] == '1') { m_ctlMAGICWEAPONCHECK.SetCheck(BST_CHECKED); }
	else m_ctlMAGICWEAPONCHECK.SetCheck(BST_UNCHECKED);
	if (checks[17] == '1') { m_ctlVENGEANCECHECK.SetCheck(BST_CHECKED); }
	else m_ctlVENGEANCECHECK.SetCheck(BST_UNCHECKED);
	if (checks[18] == '1') { m_ctlRAINOFVENGEANCECHECK.SetCheck(BST_CHECKED); }
	else m_ctlRAINOFVENGEANCECHECK.SetCheck(BST_UNCHECKED);
	if (checks[19] == '1') { m_ctlPREPARATIONCHECK.SetCheck(BST_CHECKED); }
	else m_ctlPREPARATIONCHECK.SetCheck(BST_UNCHECKED);
	if (checks[20] == '1') { m_ctlSKELEMAGECHECK.SetCheck(BST_CHECKED); }
	else m_ctlSKELEMAGECHECK.SetCheck(BST_UNCHECKED);
	if (checks[21] == '1') { m_ctlDEVOURCHECK.SetCheck(BST_CHECKED); }
	else m_ctlDEVOURCHECK.SetCheck(BST_UNCHECKED);
	if (checks[22] == '1') { m_ctlSIMCHECK.SetCheck(BST_CHECKED); }
	else m_ctlSIMCHECK.SetCheck(BST_UNCHECKED);
	if (checks[23] == '1') { m_ctlSECONDSIM.SetCheck(BST_CHECKED); }
	else m_ctlSECONDSIM.SetCheck(BST_UNCHECKED);
	if (checks[24] == '1') { m_ctlHEXING.SetCheck(BST_CHECKED); }
	else m_ctlHEXING.SetCheck(BST_UNCHECKED);
	if (checks[25] == '1') { m_ctlARCHONCHECK.SetCheck(BST_CHECKED); }
	else m_ctlARCHONCHECK.SetCheck(BST_UNCHECKED);
	if (checks[26] == '1') { m_ctlARCANEBLASTCHECK.SetCheck(BST_CHECKED); }
	else m_ctlARCANEBLASTCHECK.SetCheck(BST_UNCHECKED);
	if (checks[27] == '1') { m_ctlEXPLOSIVEBLASTCHECK.SetCheck(BST_CHECKED); }
	else m_ctlEXPLOSIVEBLASTCHECK.SetCheck(BST_UNCHECKED);
	if (checks[28] == '1') { m_ctlBLOODNOVACHECK.SetCheck(BST_CHECKED); }
	else m_ctlBLOODNOVACHECK.SetCheck(BST_UNCHECKED);
	if (checks[29] == '1') { m_ctlBLINDINGFLASHCHECK.SetCheck(BST_CHECKED); }
	else m_ctlBLINDINGFLASHCHECK.SetCheck(BST_UNCHECKED);
	if (checks[30] == '1') { m_ctlCOMMANDSKELETONSCHECK.SetCheck(BST_CHECKED); }
	else m_ctlCOMMANDSKELETONSCHECK.SetCheck(BST_UNCHECKED);

	m_ctlMACROACTIVE.SetCheck(BST_UNCHECKED);

	wchar_t str[2];
	str[1] = L'\0';


	str[0] = hotkeys[0];
	m_ctlIPHOTKEY.SetWindowText(str);
	str[0] = hotkeys[1];
	m_ctlWCHOTKEY.SetWindowText(str);
	str[0] = hotkeys[2];
	m_ctlFALTERHOTKEY.SetWindowText(str);
	str[0] = hotkeys[3];
	m_ctlBERSERKERHOTKEY.SetWindowText(str);
	str[0] = hotkeys[4];
	m_ctlSPRINTHOTKEY.SetWindowText(str);
	str[0] = hotkeys[5];
	m_ctlEPIPHANYHOTKEY.SetWindowText(str);
	str[0] = hotkeys[6];
	m_ctlMANTRAHEALINGHOTKEY.SetWindowText(str);
	str[0] = hotkeys[7];
	m_ctlSWEEPINGWINDHOTKEY.SetWindowText(str);
	str[0] = hotkeys[8];
	m_ctlBOHHOTKEY.SetWindowText(str);
	str[0] = hotkeys[9];
	m_ctlMANTRACONVICTIONHOTKEY.SetWindowText(str);
	str[0] = hotkeys[10];
	m_ctlLANDOFTHEDEADHOTKEY.SetWindowText(str);
	str[0] = hotkeys[11];
	m_ctlBONEARMORHOTKEY.SetWindowText(str);
	str[0] = hotkeys[12];
	m_ctlPOTIONHOTKEY.SetWindowText(str);
	str[0] = hotkeys[13];
	m_ctlWAVEOFFORCEHOTKEY.SetWindowText(str);
	str[0] = hotkeys[14];
	m_ctlELECTROCUTEHOTKEY.SetWindowText(str);
	str[0] = hotkeys[15];
	m_ctlMETEORHOTKEY.SetWindowText(str);
	str[0] = hotkeys[16];
	m_ctlDISINTEGRATEHOTKEY.SetWindowText(str);
	str[0] = hotkeys[17];
	m_ctlBLACKHOLEHOTKEY.SetWindowText(str);
	str[0] = hotkeys[18];
	m_ctlSTORMARMORHOTKEY.SetWindowText(str);
	str[0] = hotkeys[19];
	m_ctlMAGICWEAPONHOTKEY.SetWindowText(str);
	str[0] = hotkeys[20];
	m_ctlMACROHOTKEY.SetWindowText(str);
	str[0] = hotkeys[21];
	m_ctlTIMINGKEY.SetWindowText(str);
	str[0] = hotkeys[22];
	m_ctlTOGGLEKEY.SetWindowText(str);
	str[0] = hotkeys[23];
	m_ctlVENGEANCEHOTKEY.SetWindowText(str);
	str[0] = hotkeys[24];
	m_ctlRAINOFVENGEANCEHOTKEY.SetWindowText(str);
	str[0] = hotkeys[25];
	m_ctlPREPARATIONHOTKEY.SetWindowText(str);
	str[0] = hotkeys[26];
	m_ctlSKELEMAGEHOTKEY.SetWindowText(str);
	str[0] = hotkeys[27];
	m_ctlDEVOURHOTKEY.SetWindowText(str);
	str[0] = hotkeys[28];
	m_ctlSIMHOTKEY.SetWindowText(str);
	str[0] = hotkeys[29];
	m_ctlARCHONHOTKEY.SetWindowText(str);
	str[0] = hotkeys[30];
	m_ctlARCANEBLASTHOTKEY.SetWindowText(str);
	str[0] = hotkeys[31];
	m_ctlEXPLOSIVEBLASTHOTKEY.SetWindowText(str);
	str[0] = hotkeys[32];
	m_ctlBLOODNOVAHOTKEY.SetWindowText(str);
	str[0] = hotkeys[33];
	if (hotkeys[34] == (wchar_t)SpecialHotkey::Key)//Letter (a-z) or number (0-9)
	{
		m_ctlFORCESTANDSTILLHOTKEY.SetWindowText(str);
	}
	else if (hotkeys[34] == (wchar_t)SpecialHotkey::Shift)
	{
		m_ctlFORCESTANDSTILLHOTKEY.SetWindowText(L"Shift");
	}
	else if (hotkeys[34] == (wchar_t)SpecialHotkey::Alt)
	{
		m_ctlFORCESTANDSTILLHOTKEY.SetWindowText(L"Alt");
	}
	else if (hotkeys[34] == (wchar_t)SpecialHotkey::Space)
	{
		m_ctlFORCESTANDSTILLHOTKEY.SetWindowText(L"Space");
	}
	str[0] = hotkeys[35];
	m_ctlPOSITIONHOTKEY.SetWindowText(str);
	str[0] = hotkeys[36];
	m_ctlBLINDINGFLASHHOTKEY.SetWindowText(str);
	str[0] = hotkeys[37];
	m_ctlCHANNELHOTKEY.SetWindowText(str);
	str[0] = hotkeys[38];
	m_ctlCOMMANDSKELETONSHOTKEY.SetWindowText(str);
	str[0] = hotkeys[39];
	m_ctlFORCEMOVEHOTKEY.SetWindowText(str);
	return TRUE;
}

BOOL CDiabloCalcFancy::InitInstance() 
{
	CDiabloCalcFancyDlg myDlg;
	m_pMainWnd = &myDlg;
	myDlg.DoModal();

	return TRUE;
}

CDiabloCalcFancy MyApp;

BEGIN_MESSAGE_MAP(CDiabloCalcFancyDlg, CDialog)
	ON_BN_CLICKED(IDC_IPCHECK, Update)
	ON_BN_CLICKED(IDC_WCCHECK, Update)
	ON_BN_CLICKED(IDC_FALTERCHECK, Update)
	ON_BN_CLICKED(IDC_BERSERKERCHECK, Update)
	ON_BN_CLICKED(IDC_SPRINTCHECK, Update)
	ON_BN_CLICKED(IDC_EPIPHANYCHECK, Update)
	ON_BN_CLICKED(IDC_MANTRAHEALINGCHECK, Update)
	ON_BN_CLICKED(IDC_SWEEPINGWINDCHECK, Update)
	ON_BN_CLICKED(IDC_BOHCHECK, Update)
	ON_BN_CLICKED(IDC_MANTRACONVICTIONCHECK, Update)
	ON_BN_CLICKED(IDC_LANDOFTHEDEADCHECK, Update)
	ON_BN_CLICKED(IDC_BONEARMORCHECK, Update)
	ON_BN_CLICKED(IDC_POTIONCHECK, Update)
	ON_BN_CLICKED(IDC_MACROCHECK, Update)
	ON_BN_CLICKED(IDC_BLACKHOLECHECK, Update)
	ON_BN_CLICKED(IDC_STORMARMORCHECK, Update)
	ON_BN_CLICKED(IDC_MAGICWEAPONCHECK, Update)
	ON_BN_CLICKED(IDC_VENGEANCECHECK, Update)
	ON_BN_CLICKED(IDC_RAINOFVENGEANCECHECK, Update)
	ON_BN_CLICKED(IDC_PREPARATIONCHECK, Update)
	ON_BN_CLICKED(IDC_SKELEMAGECHECK, Update)
	ON_BN_CLICKED(IDC_DEVOURCHECK, Update)
	ON_BN_CLICKED(IDC_SIMCHECK, Update)
	ON_BN_CLICKED(IDC_SECONDSIM, Update)
	ON_BN_CLICKED(IDC_HEXING, Update)
	ON_BN_CLICKED(IDC_ARCHONECHECK, Update)
	ON_BN_CLICKED(IDC_ARCANEBLASTCHECK, Update)
	ON_BN_CLICKED(IDC_EXPLOSIVEBLASTCHECK, Update)
	ON_BN_CLICKED(IDC_BLOODNOVACHECK, Update)
	ON_BN_CLICKED(IDC_BLINDINGFLASHCHECK, Update)
	ON_BN_CLICKED(IDC_COMMANDSKELETONSCHECK, Update)

	ON_EN_CHANGE(IDC_IPHOTKEY, Update)
	ON_EN_CHANGE(IDC_WCHOTKEY, Update)
	ON_EN_CHANGE(IDC_FALTERHOTKEY, Update)
	ON_EN_CHANGE(IDC_BERSERKERHOTKEY, Update)
	ON_EN_CHANGE(IDC_SPRINTHOTKEY, Update)
	ON_EN_CHANGE(IDC_EPIPHANYHOTKEY, Update)
	ON_EN_CHANGE(IDC_MANTRAHEALINGHOTKEY, Update)
	ON_EN_CHANGE(IDC_SWEEPINGWINDHOTKEY, Update)
	ON_EN_CHANGE(IDC_BOHHOTKEY, Update)
	ON_EN_CHANGE(IDC_MANTRACONVICTIONHOTKEY, Update)
	ON_EN_CHANGE(IDC_LANDOFTHEDEADHOTKEY, Update)
	ON_EN_CHANGE(IDC_BONEARMORHOTKEY, Update)
	ON_EN_CHANGE(IDC_POTIONTHOTKEY, Update)
	ON_EN_CHANGE(IDC_WAVEOFFORCEHOTKEY, Update)
	ON_EN_CHANGE(IDC_ELECTROCUTEHOTKEY, Update)
	ON_EN_CHANGE(IDC_METEORHOTKEY, Update)
	ON_EN_CHANGE(IDC_DISINTEGRATEHOTKEY, Update)
	ON_EN_CHANGE(IDC_BLACKHOLEHOTKEY, Update)
	ON_EN_CHANGE(IDC_STORMARMORHOTKEY, Update)
	ON_EN_CHANGE(IDC_MAGICWEAPONHOTKEY, Update)
	ON_EN_CHANGE(IDC_VENGEANCEHOTKEY, Update)
	ON_EN_CHANGE(IDC_RAINOFVENGEANCEHOTKEY, Update)
	ON_EN_CHANGE(IDC_PREPARATIONHOTKEY, Update)
	ON_EN_CHANGE(IDC_SKELEMAGEHOTKEY, Update)
	ON_EN_CHANGE(IDC_DEVOURHOTKEY, Update)
	ON_EN_CHANGE(IDC_SIMHOTKEY, Update)
	ON_EN_CHANGE(IDC_ARCHONHOTKEY, Update)
	ON_EN_CHANGE(IDC_ARCANEBLASTHOTKEY, Update)
	ON_EN_CHANGE(IDC_EXPLOSIVEBLASTHOTKEY, Update)
	ON_EN_CHANGE(IDC_BLOODNOVAHOTKEY, Update)
	ON_EN_CHANGE(IDC_FORCESTANDSTILLHOTKEY, Update)
	ON_EN_CHANGE(IDC_POSITIONHOTKEY, Update)
	ON_EN_CHANGE(IDC_BLINDINGFLASHHOTKEY, Update)
	ON_EN_CHANGE(IDC_CHANNELHOTKEY, Update)
	ON_EN_CHANGE(IDC_COMMANDSKELETONSHOTKEY, Update)

	ON_EN_CHANGE(IDC_MACROHOTKEY, Update)
	ON_EN_CHANGE(IDC_TIMINGKEY, Update)
	ON_EN_CHANGE(IDC_TOGGLEKEY, Update)
	ON_EN_CHANGE(IDC_FORCEMOVEHOTKEY, Update)

END_MESSAGE_MAP()

void CDiabloCalcFancyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ACTIVE, m_ctlACTIVE);
	DDX_Control(pDX, IDC_ACTIVERUNNING, m_ctlACTIVERUNNING);
	DDX_Control(pDX, IDC_MACROACTIVE, m_ctlMACROACTIVE);
	DDX_Control(pDX, IDC_POSITIONSAVED, m_ctlPOSITIONSAVED);

	DDX_Control(pDX, IDC_IPCHECK, m_ctlIPCHECK);
	DDX_Control(pDX, IDC_WCCHECK, m_ctlWCCHECK);
	DDX_Control(pDX, IDC_FALTERCHECK, m_ctlFALTERCHECK);
	DDX_Control(pDX, IDC_BERSERKERCHECK, m_ctlBERSERKERCHECK);
	DDX_Control(pDX, IDC_SPRINTCHECK, m_ctlSPRINTCHECK);
	DDX_Control(pDX, IDC_EPIPHANYCHECK, m_ctlEPIPHANYCHECK);
	DDX_Control(pDX, IDC_MANTRAHEALINGCHECK, m_ctlMANTRAHEALINGCHECK);
	DDX_Control(pDX, IDC_SWEEPINGWINDCHECK, m_ctlSWEEPINGWINDCHECK);
	DDX_Control(pDX, IDC_BOHCHECK, m_ctlBOHCHECK);
	DDX_Control(pDX, IDC_MANTRACONVICTIONCHECK, m_ctlMANTRACONVICTIONCHECK);
	DDX_Control(pDX, IDC_LANDOFTHEDEADCHECK, m_ctlLANDOFTHEDEADCHECK);
	DDX_Control(pDX, IDC_BONEARMORCHECK, m_ctlBONEARMORCHECK);
	DDX_Control(pDX, IDC_POTIONCHECK, m_ctlPOTIONCHECK);
	DDX_Control(pDX, IDC_MACROCHECK, m_ctlMACROCHECK);
	DDX_Control(pDX, IDC_BLACKHOLECHECK, m_ctlBLACKHOLECHECK);
	DDX_Control(pDX, IDC_STORMARMORCHECK, m_ctlSTORMARMORCHECK);
	DDX_Control(pDX, IDC_MAGICWEAPONCHECK, m_ctlMAGICWEAPONCHECK);
	DDX_Control(pDX, IDC_VENGEANCECHECK, m_ctlVENGEANCECHECK);
	DDX_Control(pDX, IDC_RAINOFVENGEANCECHECK, m_ctlRAINOFVENGEANCECHECK);
	DDX_Control(pDX, IDC_PREPARATIONCHECK, m_ctlPREPARATIONCHECK);
	DDX_Control(pDX, IDC_SKELEMAGECHECK, m_ctlSKELEMAGECHECK);
	DDX_Control(pDX, IDC_DEVOURCHECK, m_ctlDEVOURCHECK);
	DDX_Control(pDX, IDC_SIMCHECK, m_ctlSIMCHECK);
	DDX_Control(pDX, IDC_SECONDSIM, m_ctlSECONDSIM);
	DDX_Control(pDX, IDC_HEXING, m_ctlHEXING);
	DDX_Control(pDX, IDC_ARCHONECHECK, m_ctlARCHONCHECK);
	DDX_Control(pDX, IDC_ARCANEBLASTCHECK, m_ctlARCANEBLASTCHECK);
	DDX_Control(pDX, IDC_EXPLOSIVEBLASTCHECK, m_ctlEXPLOSIVEBLASTCHECK);
	DDX_Control(pDX, IDC_BLOODNOVACHECK, m_ctlBLOODNOVACHECK);
	DDX_Control(pDX, IDC_BLINDINGFLASHCHECK, m_ctlBLINDINGFLASHCHECK);
	DDX_Control(pDX, IDC_COMMANDSKELETONSCHECK, m_ctlCOMMANDSKELETONSCHECK);

	DDX_Control(pDX, IDC_IPHOTKEY, m_ctlIPHOTKEY);
	DDX_Control(pDX, IDC_WCHOTKEY, m_ctlWCHOTKEY);
	DDX_Control(pDX, IDC_FALTERHOTKEY, m_ctlFALTERHOTKEY);
	DDX_Control(pDX, IDC_BERSERKERHOTKEY, m_ctlBERSERKERHOTKEY);
	DDX_Control(pDX, IDC_SPRINTHOTKEY, m_ctlSPRINTHOTKEY);
	DDX_Control(pDX, IDC_EPIPHANYHOTKEY, m_ctlEPIPHANYHOTKEY);
	DDX_Control(pDX, IDC_MANTRAHEALINGHOTKEY, m_ctlMANTRAHEALINGHOTKEY);
	DDX_Control(pDX, IDC_SWEEPINGWINDHOTKEY, m_ctlSWEEPINGWINDHOTKEY);
	DDX_Control(pDX, IDC_BOHHOTKEY, m_ctlBOHHOTKEY);
	DDX_Control(pDX, IDC_MANTRACONVICTIONHOTKEY, m_ctlMANTRACONVICTIONHOTKEY);
	DDX_Control(pDX, IDC_LANDOFTHEDEADHOTKEY, m_ctlLANDOFTHEDEADHOTKEY);
	DDX_Control(pDX, IDC_BONEARMORHOTKEY, m_ctlBONEARMORHOTKEY);
	DDX_Control(pDX, IDC_POTIONTHOTKEY, m_ctlPOTIONHOTKEY);
	DDX_Control(pDX, IDC_WAVEOFFORCEHOTKEY, m_ctlWAVEOFFORCEHOTKEY);
	DDX_Control(pDX, IDC_ELECTROCUTEHOTKEY, m_ctlELECTROCUTEHOTKEY);
	DDX_Control(pDX, IDC_METEORHOTKEY, m_ctlMETEORHOTKEY);
	DDX_Control(pDX, IDC_DISINTEGRATEHOTKEY, m_ctlDISINTEGRATEHOTKEY);
	DDX_Control(pDX, IDC_BLACKHOLEHOTKEY, m_ctlBLACKHOLEHOTKEY);
	DDX_Control(pDX, IDC_STORMARMORHOTKEY, m_ctlSTORMARMORHOTKEY);
	DDX_Control(pDX, IDC_MAGICWEAPONHOTKEY, m_ctlMAGICWEAPONHOTKEY);
	DDX_Control(pDX, IDC_MACROHOTKEY, m_ctlMACROHOTKEY);
	DDX_Control(pDX, IDC_TIMINGKEY, m_ctlTIMINGKEY);
	DDX_Control(pDX, IDC_TOGGLEKEY, m_ctlTOGGLEKEY);
	DDX_Control(pDX, IDC_VENGEANCEHOTKEY, m_ctlVENGEANCEHOTKEY);
	DDX_Control(pDX, IDC_RAINOFVENGEANCEHOTKEY, m_ctlRAINOFVENGEANCEHOTKEY);
	DDX_Control(pDX, IDC_PREPARATIONHOTKEY, m_ctlPREPARATIONHOTKEY);
	DDX_Control(pDX, IDC_SKELEMAGEHOTKEY, m_ctlSKELEMAGEHOTKEY);
	DDX_Control(pDX, IDC_DEVOURHOTKEY, m_ctlDEVOURHOTKEY);
	DDX_Control(pDX, IDC_SIMHOTKEY, m_ctlSIMHOTKEY);
	DDX_Control(pDX, IDC_ARCHONHOTKEY, m_ctlARCHONHOTKEY);
	DDX_Control(pDX, IDC_ARCANEBLASTHOTKEY, m_ctlARCANEBLASTHOTKEY);
	DDX_Control(pDX, IDC_EXPLOSIVEBLASTHOTKEY, m_ctlEXPLOSIVEBLASTHOTKEY);
	DDX_Control(pDX, IDC_BLOODNOVAHOTKEY, m_ctlBLOODNOVAHOTKEY);
	DDX_Control(pDX, IDC_FORCESTANDSTILLHOTKEY, m_ctlFORCESTANDSTILLHOTKEY);
	DDX_Control(pDX, IDC_POSITIONHOTKEY, m_ctlPOSITIONHOTKEY);
	DDX_Control(pDX, IDC_BLINDINGFLASHHOTKEY, m_ctlBLINDINGFLASHHOTKEY);
	DDX_Control(pDX, IDC_CHANNELHOTKEY, m_ctlCHANNELHOTKEY);
	DDX_Control(pDX, IDC_COMMANDSKELETONSHOTKEY, m_ctlCOMMANDSKELETONSHOTKEY);

	DDX_Control(pDX, IDC_UPPERBOUND, m_ctlUPPERBOUND);
	DDX_Control(pDX, IDC_LOWERBOUND, m_ctlLOWERBOUND);
	DDX_Control(pDX, IDC_TIME, m_ctlTIME);
	DDX_Control(pDX, IDC_COE, m_ctlCOE);
	DDX_Control(pDX, IDC_FORCEMOVEHOTKEY, m_ctlFORCEMOVEHOTKEY);
}

void CDiabloCalcFancyDlg::Update()
{
	//Reset
	CString Blanc;
	Blanc.Format(_T(""));
	int len;
	CString strText;
	LPTSTR buffer;



	IpCheck = m_ctlIPCHECK.GetCheck();
	WcCheck = m_ctlWCCHECK.GetCheck();
	FalterCheck = m_ctlFALTERCHECK.GetCheck();
	BerserkerCheck = m_ctlBERSERKERCHECK.GetCheck();
	SprintCheck = m_ctlSPRINTCHECK.GetCheck();
	EpiphanyCheck = m_ctlEPIPHANYCHECK.GetCheck();
	MantraHealingCheck = m_ctlMANTRAHEALINGCHECK.GetCheck();
	SweepingWindCheck = m_ctlSWEEPINGWINDCHECK.GetCheck();
	BohCheck = m_ctlBOHCHECK.GetCheck();
	MantraConvictionCheck = m_ctlMANTRACONVICTIONCHECK.GetCheck();
	LotdCheck = m_ctlLANDOFTHEDEADCHECK.GetCheck();
	BoneArmorCheck = m_ctlBONEARMORCHECK.GetCheck();
	PotionCheck = m_ctlPOTIONCHECK.GetCheck();
	MacroCheck = m_ctlMACROCHECK.GetCheck();
	BlackholeCheck = m_ctlBLACKHOLECHECK.GetCheck();
	StormArmorCheck = m_ctlSTORMARMORCHECK.GetCheck();
	MagicWeaponCheck = m_ctlMAGICWEAPONCHECK.GetCheck();
	VengeanceCheck = m_ctlVENGEANCECHECK.GetCheck();
	RainOfVengeanceCheck = m_ctlRAINOFVENGEANCECHECK.GetCheck();
	PreparationCheck = m_ctlPREPARATIONCHECK.GetCheck();
	SkeleMageCheck = m_ctlSKELEMAGECHECK.GetCheck();
	DevourCheck = m_ctlDEVOURCHECK.GetCheck();
	SimCheck = m_ctlSIMCHECK.GetCheck();
	SecondSim = m_ctlSECONDSIM.GetCheck();
	Hexing = m_ctlHEXING.GetCheck();
	ArchonCheck = m_ctlARCHONCHECK.GetCheck();
	ArcaneBlastCheck = m_ctlARCANEBLASTCHECK.GetCheck();
	ExplosiveBlastCheck = m_ctlEXPLOSIVEBLASTCHECK.GetCheck();
	BloodNovaCheck = m_ctlBLOODNOVACHECK.GetCheck();
	BlindingFlashCheck = m_ctlBLINDINGFLASHCHECK.GetCheck();
	CommandSkeletonsCheck = m_ctlCOMMANDSKELETONSCHECK.GetCheck();

	wiz_macro.BlackholeCheck = BlackholeCheck;
	wiz_macro.ArchonCheck = ArchonCheck;

	len = m_ctlIPHOTKEY.LineLength(m_ctlIPHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlIPHOTKEY.GetLine(0, buffer, len);
		IpHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		IpHotkey = ' ';
	}

	len = m_ctlWCHOTKEY.LineLength(m_ctlWCHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlWCHOTKEY.GetLine(0, buffer, len);
		WcHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		WcHotkey = ' ';
	}

	len = m_ctlFALTERHOTKEY.LineLength(m_ctlFALTERHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlFALTERHOTKEY.GetLine(0, buffer, len);
		FalterHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		FalterHotkey = ' ';
	}

	len = m_ctlBERSERKERHOTKEY.LineLength(m_ctlBERSERKERHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlBERSERKERHOTKEY.GetLine(0, buffer, len);
		BerserkerHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		BerserkerHotkey = ' ';
	}

	len = m_ctlSPRINTHOTKEY.LineLength(m_ctlSPRINTHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlSPRINTHOTKEY.GetLine(0, buffer, len);
		SprintHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		SprintHotkey = ' ';
	}

	len = m_ctlEPIPHANYHOTKEY.LineLength(m_ctlEPIPHANYHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlEPIPHANYHOTKEY.GetLine(0, buffer, len);
		EpiphanyHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		EpiphanyHotkey = ' ';
	}

	len = m_ctlMANTRAHEALINGHOTKEY.LineLength(m_ctlMANTRAHEALINGHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlMANTRAHEALINGHOTKEY.GetLine(0, buffer, len);
		MantraHealingHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		MantraHealingHotkey = ' ';
	}

	len = m_ctlSWEEPINGWINDHOTKEY.LineLength(m_ctlSWEEPINGWINDHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlSWEEPINGWINDHOTKEY.GetLine(0, buffer, len);
		SweepingWindHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		SweepingWindHotkey = ' ';
	}

	len = m_ctlBOHHOTKEY.LineLength(m_ctlBOHHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlBOHHOTKEY.GetLine(0, buffer, len);
		BohHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		BohHotkey = ' ';
	}

	len = m_ctlMANTRACONVICTIONHOTKEY.LineLength(m_ctlMANTRACONVICTIONHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlMANTRACONVICTIONHOTKEY.GetLine(0, buffer, len);
		MantraConvictionHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		MantraConvictionHotkey = ' ';
	}

	len = m_ctlLANDOFTHEDEADHOTKEY.LineLength(m_ctlLANDOFTHEDEADHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlLANDOFTHEDEADHOTKEY.GetLine(0, buffer, len);
		LotdHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		LotdHotkey = ' ';
	}

	len = m_ctlBONEARMORHOTKEY.LineLength(m_ctlBONEARMORHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlBONEARMORHOTKEY.GetLine(0, buffer, len);
		BoneArmorHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		BoneArmorHotkey = ' ';
	}

	len = m_ctlPOTIONHOTKEY.LineLength(m_ctlPOTIONHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlPOTIONHOTKEY.GetLine(0, buffer, len);
		PotionHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		PotionHotkey = ' ';
	}

	len = m_ctlWAVEOFFORCEHOTKEY.LineLength(m_ctlWAVEOFFORCEHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlWAVEOFFORCEHOTKEY.GetLine(0, buffer, len);
		WaveOfForceHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		WaveOfForceHotkey = ' ';
	}

	len = m_ctlELECTROCUTEHOTKEY.LineLength(m_ctlELECTROCUTEHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlELECTROCUTEHOTKEY.GetLine(0, buffer, len);
		ElectrocuteHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		ElectrocuteHotkey = ' ';
	}

	len = m_ctlMETEORHOTKEY.LineLength(m_ctlMETEORHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlMETEORHOTKEY.GetLine(0, buffer, len);
		MeteorHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		MeteorHotkey = ' ';
	}

	len = m_ctlDISINTEGRATEHOTKEY.LineLength(m_ctlDISINTEGRATEHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlDISINTEGRATEHOTKEY.GetLine(0, buffer, len);
		DisintegrateHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		DisintegrateHotkey = ' ';
	}

	len = m_ctlBLACKHOLEHOTKEY.LineLength(m_ctlBLACKHOLEHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlBLACKHOLEHOTKEY.GetLine(0, buffer, len);
		BlackholeHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		BlackholeHotkey = ' ';
	}

	len = m_ctlSTORMARMORHOTKEY.LineLength(m_ctlSTORMARMORHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlSTORMARMORHOTKEY.GetLine(0, buffer, len);
		StormArmorHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		StormArmorHotkey = ' ';
	}

	len = m_ctlMAGICWEAPONHOTKEY.LineLength(m_ctlMAGICWEAPONHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlMAGICWEAPONHOTKEY.GetLine(0, buffer, len);
		MagicWeaponHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		MagicWeaponHotkey = ' ';
	}

	len = m_ctlMACROHOTKEY.LineLength(m_ctlMACROHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlMACROHOTKEY.GetLine(0, buffer, len);
		MacroHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		MacroHotkey = ' ';
	}

	len = m_ctlTIMINGKEY.LineLength(m_ctlTIMINGKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlTIMINGKEY.GetLine(0, buffer, len);
		TimingKey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		TimingKey = ' ';
	}

	len = m_ctlTOGGLEKEY.LineLength(m_ctlTOGGLEKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlTOGGLEKEY.GetLine(0, buffer, len);
		ToggleKey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		ToggleKey = ' ';
	}

	len = m_ctlVENGEANCEHOTKEY.LineLength(m_ctlVENGEANCEHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlVENGEANCEHOTKEY.GetLine(0, buffer, len);
		VengeanceHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		VengeanceHotkey = ' ';
	}

	len = m_ctlRAINOFVENGEANCEHOTKEY.LineLength(m_ctlRAINOFVENGEANCEHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlRAINOFVENGEANCEHOTKEY.GetLine(0, buffer, len);
		RainOfVengeanceHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		RainOfVengeanceHotkey = ' ';
	}

	len = m_ctlPREPARATIONHOTKEY.LineLength(m_ctlPREPARATIONHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlPREPARATIONHOTKEY.GetLine(0, buffer, len);
		PreparationHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		PreparationHotkey = ' ';
	}

	len = m_ctlSKELEMAGEHOTKEY.LineLength(m_ctlSKELEMAGEHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlSKELEMAGEHOTKEY.GetLine(0, buffer, len);
		SkeleMageHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		SkeleMageHotkey = ' ';
	}

	len = m_ctlDEVOURHOTKEY.LineLength(m_ctlDEVOURHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlDEVOURHOTKEY.GetLine(0, buffer, len);
		DevourHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		DevourHotkey = ' ';
	}

	len = m_ctlSIMHOTKEY.LineLength(m_ctlSIMHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlSIMHOTKEY.GetLine(0, buffer, len);
		SimHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		SimHotkey = ' ';
	}

	len = m_ctlARCHONHOTKEY.LineLength(m_ctlARCHONHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlARCHONHOTKEY.GetLine(0, buffer, len);
		ArchonHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		ArchonHotkey = ' ';
	}

	len = m_ctlARCANEBLASTHOTKEY.LineLength(m_ctlARCANEBLASTHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlARCANEBLASTHOTKEY.GetLine(0, buffer, len);
		ArcaneBlastHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		ArcaneBlastHotkey = ' ';
	}

	len = m_ctlEXPLOSIVEBLASTHOTKEY.LineLength(m_ctlEXPLOSIVEBLASTHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlEXPLOSIVEBLASTHOTKEY.GetLine(0, buffer, len);
		ExplosiveBlastHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		ExplosiveBlastHotkey = ' ';
	}

	len = m_ctlBLOODNOVAHOTKEY.LineLength(m_ctlBLOODNOVAHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlBLOODNOVAHOTKEY.GetLine(0, buffer, len);
		BloodNovaHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		BloodNovaHotkey = ' ';
	}
	len = m_ctlPOSITIONHOTKEY.LineLength(m_ctlPOSITIONHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlPOSITIONHOTKEY.GetLine(0, buffer, len);
		PositionHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		PositionHotkey = ' ';
	}
	len = m_ctlBLINDINGFLASHHOTKEY.LineLength(m_ctlBLINDINGFLASHHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlBLINDINGFLASHHOTKEY.GetLine(0, buffer, len);
		BlindingFlashHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		BlindingFlashHotkey = ' ';
	}
	len = m_ctlCHANNELHOTKEY.LineLength(m_ctlCHANNELHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlCHANNELHOTKEY.GetLine(0, buffer, len);
		ChannelHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		ChannelHotkey = ' ';
	}
	len = m_ctlCOMMANDSKELETONSHOTKEY.LineLength(m_ctlCOMMANDSKELETONSHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlCOMMANDSKELETONSHOTKEY.GetLine(0, buffer, len);
		CommandSkeletonsHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		CommandSkeletonsHotkey = ' ';
	}
	len = m_ctlFORCEMOVEHOTKEY.LineLength(m_ctlFORCEMOVEHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlFORCEMOVEHOTKEY.GetLine(0, buffer, len);
		ForcemoveHotkey = strText[0];
		strText.ReleaseBuffer(len);
	}
	else
	{
		ForcemoveHotkey = ' ';
	}

	wiz_macro.WaveOfForceHotkey = WaveOfForceHotkey;
	wiz_macro.ElectrocuteHotkey = ElectrocuteHotkey;
	wiz_macro.MeteorHotkey = MeteorHotkey;
	wiz_macro.DisintegrateHotkey = DisintegrateHotkey;
	wiz_macro.BlackholeHotkey = BlackholeHotkey;
	wiz_macro.MacroHotkey = MacroHotkey;
	wiz_macro.ArchonHotkey = ArchonHotkey;
	wiz_macro.ChannelHotkey = ChannelHotkey;
	wiz_macro.ForcemoveHotkey = ForcemoveHotkey;

	len = m_ctlFORCESTANDSTILLHOTKEY.LineLength(m_ctlFORCESTANDSTILLHOTKEY.LineIndex(0));
	if (len > 0)
	{
		buffer = strText.GetBuffer(len);
		m_ctlFORCESTANDSTILLHOTKEY.GetLine(0, buffer, len);
		buffer[len] = 0;
		if (strText == L"Shift" || strText == L"shift")
		{
			ForceStandStillHotkey = '0';
			ForceStandStillSpecialHotkey = (wchar_t)SpecialHotkey::Shift;
			input_simulator.ForceStandStill = VK_SHIFT;
		}
		else if (strText == L"Alt" || strText == L"alt")
		{
			ForceStandStillHotkey = '0';
			ForceStandStillSpecialHotkey = (wchar_t)SpecialHotkey::Alt;
			input_simulator.ForceStandStill = VK_MENU;
		}
		else if (strText == L"Space" || strText == L"space")
		{
			ForceStandStillHotkey = '0';
			ForceStandStillSpecialHotkey = (wchar_t)SpecialHotkey::Space;
			input_simulator.ForceStandStill = VK_SPACE;
		}
		else
		{
			ForceStandStillHotkey = strText[0];
			ForceStandStillSpecialHotkey = (wchar_t)SpecialHotkey::Key;
			input_simulator.ForceStandStill = input_simulator.CharToVK(strText[0]);
		}
		strText.ReleaseBuffer(len);
	}
	else
	{
		ForceStandStillHotkey = '0';
		ForceStandStillSpecialHotkey = (wchar_t)SpecialHotkey::Shift;
	}


	std::wstring checks;
	std::wstring hotkeys;

	if (IpCheck) checks += '1';
	else checks += '0';
	if (WcCheck) checks += '1';
	else checks += '0';
	if (FalterCheck) checks += '1';
	else checks += '0';
	if (BerserkerCheck) checks += '1';
	else checks += '0';
	if (SprintCheck) checks += '1';
	else checks += '0';
	if (EpiphanyCheck) checks += '1';
	else checks += '0';
	if (MantraHealingCheck) checks += '1';
	else checks += '0';
	if (SweepingWindCheck) checks += '1';
	else checks += '0';
	if (BohCheck) checks += '1';
	else checks += '0';
	if (MantraConvictionCheck) checks += '1';
	else checks += '0';
	if (LotdCheck) checks += '1';
	else checks += '0';
	if (BoneArmorCheck) checks += '1';
	else checks += '0';
	if (PotionCheck) checks += '1';
	else checks += '0';
	if (MacroCheck) checks += '1';
	else checks += '0';
	if (BlackholeCheck) checks += '1';
	else checks += '0';
	if (StormArmorCheck) checks += '1';
	else checks += '0';
	if (MagicWeaponCheck) checks += '1';
	else checks += '0';
	if (VengeanceCheck) checks += '1';
	else checks += '0';
	if (RainOfVengeanceCheck) checks += '1';
	else checks += '0';
	if (PreparationCheck) checks += '1';
	else checks += '0';
	if (SkeleMageCheck) checks += '1';
	else checks += '0';
	if (DevourCheck) checks += '1';
	else checks += '0';
	if (SimCheck) checks += '1';
	else checks += '0';
	if (SecondSim) checks += '1';
	else checks += '0';
	if (Hexing) checks += '1';
	else checks += '0';
	if (ArchonCheck) checks += '1';
	else checks += '0';
	if (ArcaneBlastCheck) checks += '1';
	else checks += '0';
	if (ExplosiveBlastCheck) checks += '1';
	else checks += '0';
	if (BloodNovaCheck) checks += '1';
	else checks += '0';
	if (BlindingFlashCheck) checks += '1';
	else checks += '0';
	if (CommandSkeletonsCheck) checks += '1';
	else checks += '0';

	hotkeys += IpHotkey;
	hotkeys += WcHotkey;
	hotkeys += FalterHotkey;
	hotkeys += BerserkerHotkey;
	hotkeys += SprintHotkey;
	hotkeys += EpiphanyHotkey;
	hotkeys += MantraHealingHotkey;
	hotkeys += SweepingWindHotkey;
	hotkeys += BohHotkey;
	hotkeys += MantraConvictionHotkey;
	hotkeys += LotdHotkey;
	hotkeys += BoneArmorHotkey;
	hotkeys += PotionHotkey;
	hotkeys += WaveOfForceHotkey;
	hotkeys += ElectrocuteHotkey;
	hotkeys += MeteorHotkey;
	hotkeys += DisintegrateHotkey;
	hotkeys += BlackholeHotkey;
	hotkeys += StormArmorHotkey;
	hotkeys += MagicWeaponHotkey;
	hotkeys += MacroHotkey;
	hotkeys += TimingKey;
	hotkeys += ToggleKey;
	hotkeys += VengeanceHotkey;
	hotkeys += RainOfVengeanceHotkey;
	hotkeys += PreparationHotkey;
	hotkeys += SkeleMageHotkey;
	hotkeys += DevourHotkey;
	hotkeys += SimHotkey;
	hotkeys += ArchonHotkey;
	hotkeys += ArcaneBlastHotkey;
	hotkeys += ExplosiveBlastHotkey;
	hotkeys += BloodNovaHotkey;
	hotkeys += ForceStandStillHotkey;
	hotkeys += ForceStandStillSpecialHotkey;
	hotkeys += PositionHotkey;
	hotkeys += BlindingFlashHotkey;
	hotkeys += ChannelHotkey;
	hotkeys += CommandSkeletonsHotkey;
	hotkeys += ForcemoveHotkey;

	std::wofstream file;
	file.open(_T("config.cfg"), std::wofstream::out | std::wofstream::trunc);
	file << checks << std::endl;
	file << hotkeys << std::endl;
	file.close();

	DEBUG_MSG("Info Updated" << std::endl);
}

