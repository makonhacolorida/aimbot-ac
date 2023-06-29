#include <windows.h>
#include <stdio.h>
#include "dllmain.h"
#include <cmath>

HMODULE hmBase = GetModuleHandle(TEXT("ac_client.exe"));


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
	char pad_00ED[3936]; //0x00ED
}; //Size: 0x104D
DWORD pbase = DWORD(hmBase) + 0x17E0A8;
bPlayern* player = (bPlayern*)*(DWORD*)pbase;

#define M_PI 3.14159265358979323846


float euclidean_distance(float x, float y) {
	return sqrtf((x * x) + (y * y));
}

void sub_main()
{
	while (true)
	{
		if (GetKeyState(VK_RBUTTON) & 0x8000)
		{
			float enemyprox = -1.0f;

			DWORD* max_players = (DWORD*)(DWORD(hmBase) + 0x18AC0C);
			for (int i = 0; i < *max_players; i++)
			{

				DWORD* enemy_list = (DWORD*)(DWORD(hmBase) + 0x18AC04);
				DWORD* enemy_offset = (DWORD*)(*enemy_list + (i * 4));
				bPlayern* enemy = (bPlayern*)(*enemy_offset);

				if (player != NULL && enemy != NULL && enemy->Vida > 0)
				{
					float ax = enemy->x - player->x;
					float ay = enemy->y - player->y;
					float az = enemy->z - player->z;

					float temp_distance = euclidean_distance(ax, ay);
					if (enemyprox == -1.0f || temp_distance < enemyprox)
					{
						enemyprox = temp_distance;
						
						// Camera X
						float yx = atan2f(ay, ax);
						float cameraX = (float)(yx * (180.0 / M_PI))+90;
						player->yaw = cameraX;

						// Camera Y
						if (ay < 0) ay *= -1;
						if (ay < 5) {
							if (ax < 0) ax *= -1;
							ay = ax;
						}
						float zy = atan2f(az, ay);
						player->pitch = (float)(zy * (180.0 / M_PI));
					}
				}
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

