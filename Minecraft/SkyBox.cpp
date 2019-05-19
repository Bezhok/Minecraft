#include "game_constants.h"
#include "SkyBox.h"

using namespace World;

SkyBox::SkyBox()
{
}

SkyBox::~SkyBox()
{
}

GLuint SkyBox::load_texture(sf::String name)
{


	return 1;
}

void SkyBox::bind_textures()
{

}

void SkyBox::load_textures()
{
	m_box = {
		load_texture("resources/textures/sky/cloud1/cloud1_05.gif"),
		load_texture("resources/textures/sky/cloud1/cloud1_03.gif"),
		load_texture("resources/textures/sky/cloud1/cloud1_04.gif"),
		load_texture("resources/textures/sky/cloud1/cloud1_06.gif"),
		load_texture("resources/textures/sky/cloud1/cloud1_01.gif"),
		load_texture("resources/textures/sky/cloud1/cloud1_02.gif")
	};
}

