#include <glib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419716939937510
#endif

void cube(glib& glib, float x, float y, float z, float sz)
{
	// front
	glib.push({ x - sz / 2,  y - sz / 2, z + sz / 2, 0xFF0000FF }, { x + sz / 2, y - sz / 2, z + sz / 2, 0xFF0000FF }, { x - sz / 2, y + sz / 2, z + sz / 2, 0xFF0000FF });
	glib.push({ x + sz / 2,  y + sz / 2, z + sz / 2, 0xFF0000FF }, { x - sz / 2, y + sz / 2, z + sz / 2, 0xFF0000FF }, { x + sz / 2, y - sz / 2, z + sz / 2, 0xFF0000FF });

	// back
	glib.push({ x - sz / 2,  y - sz / 2, z - sz / 2, 0xFF0000FF }, { x - sz / 2, y + sz / 2, z - sz / 2, 0xFF0000FF }, { x + sz / 2, y - sz / 2, z - sz / 2, 0xFF0000FF });
	glib.push({ x + sz / 2,  y + sz / 2, z - sz / 2, 0xFF0000FF }, { x + sz / 2, y - sz / 2, z - sz / 2, 0xFF0000FF }, { x - sz / 2, y + sz / 2, z - sz / 2, 0xFF0000FF });

	// top
	glib.push({ x - sz / 2,  y + sz / 2, z - sz / 2, 0x00FF00FF }, { x - sz / 2, y + sz / 2, z + sz / 2, 0x00FF00FF }, { x + sz / 2, y + sz / 2, z + sz / 2, 0x00FF00FF });
	glib.push({ x + sz / 2,  y + sz / 2, z - sz / 2, 0x00FF00FF }, { x - sz / 2, y + sz / 2, z - sz / 2, 0x00FF00FF }, { x + sz / 2, y + sz / 2, z + sz / 2, 0x00FF00FF });

	// bottom
	glib.push({ x - sz / 2, y - sz / 2, z - sz / 2, 0x00FF00FF }, { x + sz / 2, y - sz / 2, z + sz / 2, 0x00FF00FF }, { x - sz / 2, y - sz / 2, z + sz / 2, 0x00FF00FF });
	glib.push({ x + sz / 2, y - sz / 2, z - sz / 2, 0x00FF00FF }, { x + sz / 2, y - sz / 2, z + sz / 2, 0x00FF00FF }, { x - sz / 2, y - sz / 2, z - sz / 2, 0x00FF00FF });

	// right
	glib.push({ x + sz / 2, y - sz / 2, z - sz / 2, 0x0000FFFF }, { x + sz / 2, y + sz / 2, z - sz / 2, 0x0000FFFF }, { x + sz / 2, y + sz / 2, z + sz / 2, 0x0000FFFF });
	glib.push({ x + sz / 2, y - sz / 2, z + sz / 2, 0x0000FFFF }, { x + sz / 2, y - sz / 2, z - sz / 2, 0x0000FFFF }, { x + sz / 2, y + sz / 2, z + sz / 2, 0x0000FFFF });

	// left
	glib.push({ x - sz / 2, y - sz / 2, z - sz / 2, 0x0000FFFF }, { x - sz / 2, y + sz / 2, z + sz / 2, 0x0000FFFF }, { x - sz / 2, y + sz / 2, z - sz / 2, 0x0000FFFF });
	glib.push({ x - sz / 2, y - sz / 2, z + sz / 2, 0x0000FFFF }, { x - sz / 2, y + sz / 2, z + sz / 2, 0x0000FFFF }, { x - sz / 2, y - sz / 2, z - sz / 2, 0x0000FFFF });
}

int main()
{
	{
		glib glib;
		float tm = 0.0f;

		Mtx_PerspTilt(glib.projection(), C3D_AngleFromDegrees(40.0f), C3D_AspectRatioTop, 0.001f, 1000.0f, false);

		while (aptMainLoop())
		{
			hidScanInput();

			u32 kDown = hidKeysDown();
			if (kDown & KEY_START) { break; }

			glib.beginFrame();

			Mtx_Identity(glib.model());
			Mtx_Translate(glib.model(), 0.0f, 0.0f, -3.0f, true);
			Mtx_Rotate(glib.model(), FVec3_New(0.1f, 0.1f, 0.1f), 5.0f * tm, true);

			cube(glib, 0.0f, 0.0f, 0.0f, 1.0f + 0.4f * sinf(5.0f * tm));

			glib.endFrame();

			tm += 1.0f / 60.0f;
		}
	}

	return 0;
}