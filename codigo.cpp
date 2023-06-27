#include <windows.h>
#include <stdio.h>
#include "dllmain.h"
#include <cmath>

// functions
/*

	PIT -> Y
	YAW -> X

*/

HMODULE hmBase = GetModuleHandle(TEXT("ac_client.exe"));

// Jogador --
class bPlayern
{
public:
	char pad_0000[4]; //0x0000
	float x; //0x0004
	float y; //0x0008
	float z; //0x000C
	char pad_0010[36]; //0x0010
	float yaw; //0x0034
	float pitch; //0x0038
	char pad_003C[176]; //0x003C
	int Vida; //0x00EC
	char pad_00F0[848]; //0x00F0
}; //Size: 0x0440
DWORD pbase = DWORD(hmBase) + 0x17E0A8;
bPlayern* player = (bPlayern*)*(DWORD*)pbase;

#define M_PI 3.14159265358979323846


float euclidean_distance(float x, float y) {
	return sqrtf((x * x) + (y * y));
}

void sub_main()
{
	AllocConsole();
	HWND hWnd = GetConsoleWindow();
	freopen("CONOUT$", "w", stdout);
	SetConsoleTitle(L"test");

	int id = 2;
	while (true)
	{

		if (GetKeyState(VK_RBUTTON) & 0x8000)
		{
			// aimbot
			float closest_player = -1.0f;
			float closest_yaw = 0.0f;
			float closest_pitch = 0.0f;

			//
			DWORD* max_players = (DWORD*)(DWORD(hmBase) + 0x18AC0C);
			for (int i = 0; i < *max_players; i++)
			{

				DWORD* enemy_list = (DWORD*)(DWORD(hmBase) + 0x18AC04);
				DWORD* enemy_offset = (DWORD*)(*enemy_list + (i * 4));
				bPlayern* enemy = (bPlayern*)(*enemy_offset);
				if (player != NULL && enemy != NULL && enemy->Vida > 0)
				{
					float abspos_x = enemy->x - player->x;
					float abspos_y = enemy->y - player->y;
					float abspos_z = enemy->z - player->z;

					float temp_distance = euclidean_distance(abspos_x, abspos_y);
					if (closest_player == -1.0f || temp_distance < closest_player) {
						closest_player = temp_distance;

						float azimuth_xy = atan2f(abspos_y, abspos_x);
						float yaw = (float)(azimuth_xy * (180.0 / M_PI));

						closest_yaw = yaw + 90;

						if (abspos_y < 0) {
							abspos_y *= -1;
						}
						if (abspos_y < 5) {
							if (abspos_x < 0) {
								abspos_x *= -1;
							}
							abspos_y = abspos_x;
						}
						float azimuth_z = atan2f(abspos_z, abspos_y);

						closest_pitch = (float)(azimuth_z * (180.0 / M_PI));
					}
				}
				player->yaw = closest_yaw;
				player->pitch = closest_pitch;
			}

		}
		Sleep(1);
	}
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)sub_main, 0, 0, 0);

		}
	}
	return TRUE;
}

