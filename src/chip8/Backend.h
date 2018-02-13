#ifndef BACKEND_H
#define BACKEND_H

namespace chip8
{
	class Framebuffer;

	class Backend
	{
	public:
		virtual void Render(const Framebuffer & fb) = 0;
		virtual ~Backend() { }
	};
}


#endif /* BACKEND_H */

