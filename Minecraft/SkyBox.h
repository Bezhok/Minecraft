#pragma once
#include "pch.h"
#include "Box.h"

namespace World {
	class SkyBox :
		public Base::Box
	{
	private:
		std::array<GLuint, 6> m_box;

	public:
		/* nothing */
		SkyBox();
		~SkyBox();

		/* eponymous */
		void bind_textures();
		void load_textures();
	private:
		GLuint load_texture(sf::String name);
	};
}
