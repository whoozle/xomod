#ifndef BACKEND_H
#define BACKEND_H

#include <chip8/types.h>
#include <algorithm>

namespace chip8
{
	class Audio;
	class Framebuffer;

	class Backend
	{
	public:
		virtual ~Backend() { }
		virtual bool Render(Framebuffer & fb) = 0;
		virtual bool GetKeyState(u8 index) = 0;
		virtual void SetAudio(Audio *audio) = 0;

		static void CalculateZoom(int &num, int &denom, int & offsetX, int &offsetY, uint displayW, uint displayH, uint chipW, uint chipH)
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
				denom = 1 + std::max((chipW + displayW - 1) / displayW, (chipH + displayH - 1) / chipH);
				offsetX = (displayW - chipW / denom) / 2;
				offsetY = (displayH - chipH / denom) / 2;
			}
		}

	};
}


#endif /* BACKEND_H */

