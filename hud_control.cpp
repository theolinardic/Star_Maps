#include "hud_control.h"

HUD::HUD()
{
	// Load initial default hud elements.
	glGenTextures(1, &this->tiles_texture_id);
	std::ifstream infile("assets/textures/ui/main_game/tiles_empty.png");
	glBindTexture(GL_TEXTURE_2D, this->tiles_texture_id);
	if (infile.good() != 1)
		std::cout << "Err: " << infile.good() << std::endl;
	int w, h;
	unsigned char* texture_img = SOIL_load_image("assets/textures/ui/main_game/tiles_empty.png", &w, &h, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_img);
	SOIL_free_image_data(texture_img);

}

void HUD::render()
{

}
