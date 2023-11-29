#include <debug_menu.h>

void debug::start()
{
	this->launch_time = glfwGetTime();
	this->should_render = true;

	ImGuiIO& io = ImGui::GetIO(); (void)io;
}

void debug::add_text_float(std::string label, float* value) {
    // If label already exists, just update value.
    for (auto& item : this->all_debug_floats) {
        if (item->label == label) {
            item->value = value;
            return;
        }
    }
    // Item not found, create a new one
    debug_item_float* new_item = new debug_item_float{ label, value };
    this->all_debug_floats.push_back(new_item);
}

void debug::add_text_int(std::string label, int* value) {
    for (auto& item : this->all_debug_ints) {
        if (item->label == label) {
            item->value = value;
            return;
        }
    }
    debug_item_int* new_item = new debug_item_int{ label, value };
    this->all_debug_ints.push_back(new_item);
}

void debug::add_checkbox(std::string label, bool* value) {
    for (auto& item : this->all_debug_checks) {
        if (item->label == label) {
            item->value = value;
            return;
        }
    }
    debug_item_check* new_item = new debug_item_check{ label, value };
    this->all_debug_checks.push_back(new_item);
}

void debug::add_slider(std::string label, float* value)
{
    for (auto& item : this->all_debug_sliders) {
        if (item->label == label) {
            item->value = value;
            return;
        }
    }
    debug_item_slider* new_item = new debug_item_slider{ label, value };
    this->all_debug_sliders.push_back(new_item);
}


void debug::render()
{
	float time_open_sec = glfwGetTime() - this->launch_time;
	int time_open_min = time_open_sec / 60;
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug Window");
	if (time_open_min >= 1)
		ImGui::Text("Time open: %dm %.1fs", time_open_min, (time_open_sec - (60 * time_open_min)));
	else
		ImGui::Text("Time open: %.2fs", time_open_sec);

	for (debug_item_float* item : this->all_debug_floats)
		ImGui::Text(item->label.c_str(), item->value);
	for (debug_item_int* item : this->all_debug_ints)
		ImGui::Text(item->label.c_str(), item->value);
	for (debug_item_check* item : this->all_debug_checks)
		ImGui::Checkbox(item->label.c_str(), item->value);
    for (debug_item_slider* item : this->all_debug_sliders)
        ImGui::SliderFloat(item->label.c_str(), item->value, item->min, item->max);

	ImGui::End();
	ImGui::Render();
	if (this->should_render)
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}