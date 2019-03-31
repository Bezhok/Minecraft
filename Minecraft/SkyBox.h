#pragma once
#include "pch.h"
#include "Box.h"

namespace World {
	using std::array;
	using namespace Base;

	class SkyBox :
		public Box
	{
	public:
		SkyBox();
		~SkyBox();
		void bind_textures();
		void load_textures();
	private:
		array<GLuint, 6> m_box;
	};
}
