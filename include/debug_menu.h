#pragma once
#include <vector>
#include <string>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>

struct debug_item_float
{
	std::string label;
	float* value;
};

struct debug_item_int
{
	std::string label;
	int* value;
};

struct debug_item_check
{
	std::string label;
	bool* value;
};

struct debug_item_slider
{
	std::string label;
	float* value;
	float min;
	float max;
};

class debug
{
public:
	bool should_render;
	float launch_time;
	std::vector<debug_item_float*> all_debug_floats;
	std::vector<debug_item_int*> all_debug_ints;
	std::vector<debug_item_check*> all_debug_checks;
	std::vector<debug_item_slider*> all_debug_sliders;

	void start();
	void add_text_float(std::string label, float* value);
	void add_text_int(std::string label, int* value);
	void add_checkbox(std::string label, bool* value);
	void add_slider(std::string label, float* value);
	void render();
};
