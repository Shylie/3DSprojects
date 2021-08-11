#include "glib.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419716939937510
#endif

void cube(glib* glib, float x, float y, float z, float sz)
{
	// front
	glib->tri({ x - sz / 2,  y - sz / 2, z + sz / 2, 0xFFFFFFFF }, { x + sz / 2, y - sz / 2, z + sz / 2, 0xFFFFFFFF }, { x - sz / 2, y + sz / 2, z + sz / 2, 0xFFFFFFFF });
	glib->tri({ x + sz / 2,  y + sz / 2, z + sz / 2, 0xFFFFFFFF }, { x - sz / 2, y + sz / 2, z + sz / 2, 0xFFFFFFFF }, { x + sz / 2, y - sz / 2, z + sz / 2, 0xFFFFFFFF });

	// back
	glib->tri({ x - sz / 2,  y - sz / 2, z - sz / 2, 0xFFFFFFFF }, { x - sz / 2, y + sz / 2, z - sz / 2, 0xFFFFFFFF }, { x + sz / 2, y - sz / 2, z - sz / 2, 0xFFFFFFFF });
	glib->tri({ x + sz / 2,  y + sz / 2, z - sz / 2, 0xFFFFFFFF }, { x + sz / 2, y - sz / 2, z - sz / 2, 0xFFFFFFFF }, { x - sz / 2, y + sz / 2, z - sz / 2, 0xFFFFFFFF });

	// top
	glib->tri({ x - sz / 2,  y + sz / 2, z - sz / 2, 0xFFFFFFFF }, { x - sz / 2, y + sz / 2, z + sz / 2, 0xFFFFFFFF }, { x + sz / 2, y + sz / 2, z + sz / 2, 0xFFFFFFFF });
	glib->tri({ x + sz / 2,  y + sz / 2, z - sz / 2, 0xFFFFFFFF }, { x - sz / 2, y + sz / 2, z - sz / 2, 0xFFFFFFFF }, { x + sz / 2, y + sz / 2, z + sz / 2, 0xFFFFFFFF });

	// bottom
	glib->tri({ x - sz / 2, y - sz / 2, z - sz / 2, 0xFFFFFFFF }, { x + sz / 2, y - sz / 2, z + sz / 2, 0xFFFFFFFF }, { x - sz / 2, y - sz / 2, z + sz / 2, 0xFFFFFFFF });
	glib->tri({ x + sz / 2, y - sz / 2, z - sz / 2, 0xFFFFFFFF }, { x + sz / 2, y - sz / 2, z + sz / 2, 0xFFFFFFFF }, { x - sz / 2, y - sz / 2, z - sz / 2, 0xFFFFFFFF });

	// right
	glib->tri({ x + sz / 2, y - sz / 2, z - sz / 2, 0xFFFFFFFF }, { x + sz / 2, y + sz / 2, z - sz / 2, 0xFFFFFFFF }, { x + sz / 2, y + sz / 2, z + sz / 2, 0xFFFFFFFF });
	glib->tri({ x + sz / 2, y - sz / 2, z + sz / 2, 0xFFFFFFFF }, { x + sz / 2, y - sz / 2, z - sz / 2, 0xFFFFFFFF }, { x + sz / 2, y + sz / 2, z + sz / 2, 0xFFFFFFFF });

	// left
	glib->tri({ x - sz / 2, y - sz / 2, z - sz / 2, 0xFFFFFFFF }, { x - sz / 2, y + sz / 2, z + sz / 2, 0xFFFFFFFF }, { x - sz / 2, y + sz / 2, z - sz / 2, 0xFFFFFFFF });
	glib->tri({ x - sz / 2, y - sz / 2, z + sz / 2, 0xFFFFFFFF }, { x - sz / 2, y + sz / 2, z + sz / 2, 0xFFFFFFFF }, { x - sz / 2, y - sz / 2, z - sz / 2, 0xFFFFFFFF });
}

int main()
{
	{
		glib glib;
		float tm = 0.1f;

		Mtx_PerspTilt(glib.projection(), C3D_AngleFromDegrees(40.0f), C3D_AspectRatioTop, 0.001f, 1000.0f, false);

		while (aptMainLoop())
		{
			hidScanInput();

			u32 kDown = hidKeysDown();
			if (kDown & KEY_START) { break; }

			glib.beginFrame();

			Mtx_Identity(glib.model());
			Mtx_Translate(glib.model(), 0.0f, 0.0f, -3.0f, true);
			Mtx_Rotate(glib.model(), FVec3_New(-0.1f, 0.1f, 0.1f), tm, true);

			cube(&glib, 0.0f, 0.0f, 0.0f, 0.5f);
			glib.flush();

			glib.endFrame();

			tm += 1.0f / 60.0f;
		}
	}

	return 0;
}