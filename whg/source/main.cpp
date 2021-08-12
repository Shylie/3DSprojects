#include <glib.h>

int main()
{
	{
		glib glib;

		while (aptMainLoop())
		{
			hidScanInput();

			u32 kDown = hidKeysDown();
			if (kDown & KEY_START) { break; }

			glib.beginFrame();

			glib.rect(20.0f, 20.0f, 20.0f, 20.0f, 0xFF0000FF);

			glib.endFrame();
		}
	}

	return 0;
}