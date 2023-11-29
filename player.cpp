#include <player.h>
#include <random>

float sensitivity = 1.0f;
float movement_speed = 1.0f;
bool show_debug = true;

// Function to read player input and process it appropriatley.
// Note: will not be called when interacting with debug menu.
glm::vec3 read_player_input(GLFWwindow* star_maps_window, glm::vec3& camera_position, glm::vec3& camera_front, float& camera_yaw, float& camera_pitch, std::vector<game_object*> entitiys)
{
	if (glfwGetKey(star_maps_window, GLFW_KEY_W) == GLFW_PRESS) {
		camera_position += movement_speed * camera_front;  // Move forward
	}
	if (glfwGetKey(star_maps_window, GLFW_KEY_S) == GLFW_PRESS) {
		camera_position -= movement_speed * camera_front;  // Move backward
	}
	if (glfwGetKey(star_maps_window, GLFW_KEY_A) == GLFW_PRESS) {
		glm::vec3 camera_right = glm::normalize(glm::cross(camera_front, glm::vec3(0.0f, 1.0f, 0.0f)));
		camera_position -= camera_right * movement_speed;  // Move left
	}
	if (glfwGetKey(star_maps_window, GLFW_KEY_D) == GLFW_PRESS) {
		glm::vec3 camera_right = glm::normalize(glm::cross(camera_front, glm::vec3(0.0f, 1.0f, 0.0f)));
		camera_position += camera_right * movement_speed;  // Move right
	}
	if (glfwGetKey(star_maps_window, GLFW_KEY_TAB) == GLFW_PRESS) {
		show_debug = !show_debug;
	}
	if (glfwGetKey(star_maps_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		camera_yaw -= sensitivity;
	}
	if (glfwGetKey(star_maps_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		camera_yaw += sensitivity;
	}
	if (glfwGetKey(star_maps_window, GLFW_KEY_UP) == GLFW_PRESS) {
		camera_pitch += sensitivity;
	}
	if (glfwGetKey(star_maps_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		camera_pitch -= sensitivity;
	}
	if (glfwGetKey(star_maps_window, GLFW_KEY_1) == GLFW_PRESS) {
		// Find the currently rendered bounding box and toggle it off
		for (game_object* obj : entitiys) {
			if (obj->render_bb) {
				obj->render_bb = false;
				break;
			}
		}

		// Move to the next object and toggle its bounding box on
		static size_t currentEntityIndex = 0; // Keep track of the current entity
		currentEntityIndex = (currentEntityIndex + 1) % entitiys.size();
		entitiys[currentEntityIndex]->render_bb = true;
	}

	// Clamp the camera pitch to avoid flipping upside down
	if (camera_pitch > 89.0f) {
		camera_pitch = 89.0f;
	}
	if (camera_pitch < -89.0f) {
		camera_pitch = -89.0f;
	}

	glm::mat4 viewMatrix = glm::lookAt(camera_position, camera_position + camera_front, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 4000.0f);
	glm::mat4 inverseProjectionMatrix = glm::inverse(projectionMatrix);
	glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);

	glm::vec3 out_direction = glm::vec3(-99999.0f);

	if (glfwGetMouseButton(star_maps_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		double mouseX, mouseY;
		glfwGetCursorPos(star_maps_window, &mouseX, &mouseY);

		// Get window size
		int windowWidth, windowHeight;
		glfwGetWindowSize(star_maps_window, &windowWidth, &windowHeight);

		// Convert mouse coordinates to NDC (Normalized Device Coordinates)
		float ndcX = (2.0f * mouseX) / windowWidth - 1.0f;
		float ndcY = 1.0f - (2.0f * mouseY) / windowHeight;

		glm::vec4 rayStart_NDC = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
		glm::vec4 rayEnd_NDC = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

		glm::vec4 rayStart_world = inverseProjectionMatrix * rayStart_NDC;
		rayStart_world /= rayStart_world.w;

		glm::vec4 rayEnd_world = inverseProjectionMatrix * rayEnd_NDC;
		rayEnd_world /= rayEnd_world.w;

		rayStart_world = inverseViewMatrix * rayStart_world;
		rayEnd_world = inverseViewMatrix * rayEnd_world;

		// Calculate the direction of the ray
		glm::vec3 rayDirection = glm::normalize(glm::vec3(rayEnd_world - rayStart_world));

		// Loop through objects and their bounding boxes to check for intersection
		for (game_object* obj : entitiys) {
			glm::vec3 invDirection = 1.0f / rayDirection;

			glm::vec3 bboxMin = obj->boundingBoxMin + obj->position;
			glm::vec3 bboxMax = obj->boundingBoxMax + obj->position;

			glm::vec3 tMin = (bboxMin - glm::vec3(rayStart_world)) * invDirection;
			glm::vec3 tMax = (bboxMax - glm::vec3(rayStart_world)) * invDirection;

			glm::vec3 tEnter = glm::min(tMin, tMax);
			glm::vec3 tExit = glm::max(tMin, tMax);

			float tEnterMax = glm::max(glm::max(tEnter.x, tEnter.y), tEnter.z);
			float tExitMin = glm::min(glm::min(tExit.x, tExit.y), tExit.z);

			if (tEnterMax <= tExitMin && tExitMin >= 0.0f) {
				obj->render_bb = true;
				return obj->position;
			}
			else {
				obj->render_bb = false;
			}
		}
	}
	return glm::vec3(-999999.0f);
}


// Initialize json interpreter for save data and user settings and set up a struct of the default
// settings to be used if the settings.json file is missing.
using json = nlohmann::json;
json default_settings = {
	{"res_width", 1280},
	{"res_height", 720},
	{"fullscreen", false},
	// 1=low, 2=medium, 3=high
	{"graphics_preset", "high"},
	{"fps_cap", 0}
};

// Function to rest all of the settings to the default values defined above.
void reset_settings() {
	std::ofstream settings_file(SETTINGS_FILE_LOCATION);
	settings_file << default_settings.dump(4);
	settings_file.close();
}

// Function to get the user settings from the SETTINGS_FILE_LOCATION and return them.
json get_settings()
{
	json user_settings;
	std::ifstream settings_file(SETTINGS_FILE_LOCATION);
	// If the file opened correctly, read in to user_settings.
	if (settings_file.is_open())
	{
		settings_file >> user_settings;
		settings_file.close();
	}
	// If no settings file is found, create a new one with default settings from above.
	else
	{
		reset_settings();

		// Reload settings after creating a new file
		return get_settings();
	}

	return user_settings;
}

// Function to change a setting to a new value.
void change_setting(const std::string setting, const json new_value) {
	json user_settings = get_settings();
	user_settings[setting] = new_value;

	std::ofstream settings_file(SETTINGS_FILE_LOCATION);
	settings_file << user_settings.dump(4);
	settings_file.close();
}

// Function to generate the data for a new save file based on the difficulty and galaxy name.
json generate_new_save_data(const std::string difficulty, const std::string galaxy_name) {
	using json = nlohmann::json;
	std::ifstream file(PLANET_NAME_JSON);
	json data;
	file >> data;

	std::vector<std::string> picked_names;
	std::vector<std::string> planet_names;
	for (const auto& planet : data["planet_names"])
		planet_names.push_back(planet);

	std::random_device rd;
	std::mt19937 gen(rd());

	while (picked_names.size() < 6)
	{
		std::uniform_int_distribution<> dis(0, planet_names.size() - 1);
		int random = dis(gen);
		std::string picked = planet_names[random];

		auto it = std::find(picked_names.begin(), picked_names.end(), picked);
		if (it == picked_names.end())
		{
			picked_names.push_back(picked);
			planet_names.erase(planet_names.begin() + random);
		}
	}

	int money = 0;
	if (difficulty == "easy") {
		money = 100000000;
	}
	else if (difficulty == "medium") {
		money = 10000000;
	}
	else if (difficulty == "hard") {
		money = 1000000;
	}
	else {
		std::cout << difficulty << std::endl;
		money = -99999999999999;
	}

	json entities;
	entities.push_back({
		{"obj_type", 0},
		{"texture", 0},
		{"parent", -1},
		{"location", {{"X", 0}, {"Y", 0}, {"Z", 0}}}
		});

	for (size_t i = 0; i < picked_names.size(); ++i) {
		entities.push_back({
			{"obj_type", 1},
			{"texture", 1},
			{"parent", 1},
			{"location", {{"X", -15.2f * i}, {"Y", 30.8f * i}, {"Z", 12.0f * i}}}
			});
	}

	return {
		{"current_sol", 0},
		{"current_time", "05H_05M_16.589310S"},
		{"difficulty", difficulty},
		{"galaxy_name", galaxy_name},
		{"money", money},
		{"status_1_percent", 100.0},
		{"status_2_percent", 100.0},
		{"status_3_percent", 100.0},
		{"total_time_played", "02H_08M_12.238135S"},
		{"entities", entities}
	};
}

// Function to create a new save file with the supplied galaxy name and difficulty.
void create_new_save(const std::string difficulty, const std::string galaxy_name) {
	// Read in the existing save files in the save data directory.
	std::vector<std::string> existing_saves;
	std::string save_folder = "save_data/";
	for (const auto& file : std::filesystem::directory_iterator(save_folder)) {
		if (file.path().extension() == ".json") {
			existing_saves.push_back(file.path().filename().string());
		}
	}
	// Find the highest index save file so that the numbering can be kept consistent (save_1, save_2, etc.)
	int new_file_index = 1;
	if (!existing_saves.empty()) {
		std::vector<int> indexes;
		std::transform(existing_saves.begin(), existing_saves.end(), std::back_inserter(indexes),
			[](const std::string& filename) {
				return std::stoi(filename.substr(5, filename.find('.') - 5));
			});
		new_file_index = *std::max_element(indexes.begin(), indexes.end()) + 1;
	}
	// Create a new save file at the found next index.
	std::string new_save_location = save_folder + "save_" + std::to_string(new_file_index) + ".json";
	std::ofstream save_file(new_save_location);
	if (save_file.is_open()) {
		json data = generate_new_save_data(difficulty, galaxy_name);
		save_file << std::setw(4) << data << std::endl;
		save_file.close();
	}
}

// Function to delete an existing save file.
void delete_save(int delete_index) {
	std::string save_folder = "save_data/";
	std::string save_to_delete = save_folder + "save_" + std::to_string(delete_index) + ".json";
	std::remove(save_to_delete.c_str());
	// Find the number of saves already in the file so they can all be readjusted and the +1 numbering pattern is kept.
	for (const auto& file : std::filesystem::directory_iterator(save_folder)) {
		int current_index = std::stoi(file.path().filename().string().substr(5, file.path().filename().string().find('.') - 5));
		if (current_index > delete_index) {
			std::string new_name = save_folder + "save_" + std::to_string(current_index - 1) + ".json";
			std::filesystem::rename(file.path(), new_name);
		}
	}
}

// Function to get some preview data from all of the current save files for the load save screen.
// This will help distinguish between the save files at a glance easier.
// TO-DO: Try to implement a screenshot of the galaxy on the save load menu if possible.
std::vector<json> preview_saves() {
	std::vector<json> save_previews;
	std::string save_folder = "save_data/";
	for (const auto& file : std::filesystem::directory_iterator(save_folder)) {
		std::string save_file_path = file.path().string();
		std::ifstream file_stream(save_file_path);
		if (file_stream.is_open()) {
			json save_data;
			file_stream >> save_data;

			json save_preview = {
				{"galaxy_name", save_data.value("galaxy_name", "")},
				{"difficulty", save_data.value("difficulty", "")},
				{"current_sol", save_data.value("current_sol", 0)},
				{"current_time", save_data.value("current_time", "")},
				{"total_time_played", save_data.value("total_time_played", "")},
				{"save_index", std::stoi(file.path().filename().string().substr(5, file.path().filename().string().find('.') - 5))}
			};
			save_previews.push_back(save_preview);
			file_stream.close();
		}
	}
	return save_previews;
}

// Function to load all dave from the save file selected and return it to the main game loop.
json load_save_file(int index) {
	std::string save_folder = "save_data/";
	std::string save_file_path = save_folder + "save_" + std::to_string(index) + ".json";
	std::ifstream file(save_file_path);
	json save_data;
	if (file.is_open()) {
		file >> save_data;
		file.close();
	}
	return save_data;
}

void update_save(int index, const json& new_data)
{
	std::string save_folder = "save_data/";
	std::string save_file_path = save_folder + "save_" + std::to_string(index) + ".json";
	std::ofstream file(save_file_path);
	if (file.is_open()) {
		file << std::setw(4) << new_data << std::endl;
		file.close();
		std::cout << "Save file updated" << std::endl;
	}
	else
		std::cerr << "Unable to open the save file" << std::endl;
}