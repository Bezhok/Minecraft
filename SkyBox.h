#pragma once
#include "Box.h"
class SkyBox :
	public Box
{
public:
	SkyBox();
	~SkyBox();
	void bind_textures();
	void load_textures();

};

