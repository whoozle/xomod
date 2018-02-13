#ifndef BACKEND_H
#define BACKEND_H

#include <chip8/types.h>
#include <algorithm>

namespace chip8
{
	class Framebuffer;

	class Backend
	{
	public:
		virtual void Render(const Framebuffer & fb) = 0;
		virtual ~Backend() { }


		static void CalculateZoom(int &num, int &denom, int & offsetX, int &offsetY, int displayW, int displayH, int chipW, int chipH)
		{
			if (displayW >= chipW && displayH >= chipH) //zoom out
			{
				num = std::min(displayW / chipW, displayH / chipH);
				denom = 1;
				offsetX = (displayW - chipW * num) / 2;
				offsetY = (displayH - chipH * num) / 2;
			}
			else {
				//test me
				num = 1;
				denom = std::max((chipW + displayW - 1) / displayW, (displayH + chipH - 1) / chipH);
				offsetX = (displayW - chipW / denom) / 2;
				offsetY = (displayH - chipH / denom) / 2;
			}
		}

	};
}


#endif /* BACKEND_H */

