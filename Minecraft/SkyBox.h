#pragma once
#include "pch.h"
#include "Box.h"

namespace World {
	class SkyBox :
		public Base::Box
	{
	public:
		SkyBox();
		~SkyBox();
		void bind_textures();
		void load_textures();
	private:
		std::array<GLuint, 6> m_box;
	};
}
