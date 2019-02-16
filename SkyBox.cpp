#include "SkyBox.h"



SkyBox::SkyBox()
{
	m_box = {
		load_texture("resources/textures/sky/cloud1/cloud1_05.gif"),
		load_texture("resources/textures/sky/cloud1/cloud1_03.gif"),//
		load_texture("resources/textures/sky/cloud1/cloud1_04.gif"),
		load_texture("resources/textures/sky/cloud1/cloud1_06.gif"),
		load_texture("resources/textures/sky/cloud1/cloud1_01.gif"),//
		load_texture("resources/textures/sky/cloud1/cloud1_02.gif")//
	};
}


SkyBox::~SkyBox()
{
}
