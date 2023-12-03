#include <game_logic.h>

// Time Ratio (NOT FINAL):
// 1 Second IRL = 1 Minute In-Game
// 1 Minute IRL = 1 Hour In-Game
// 1 Hour IRL = 2.5 Days In-Game

// Class initialization function for star_maps_game, the main logic of the entire game.
star_maps_game::star_maps_game(bool p, GLFWwindow* window, HUD* ui)
{
	this->paused = p;
	this->game_speed_multiplier = 1.0;
	this->sols_passed = 0;
	this->in_game_hour = 0;
	this->in_game_minute = 0;
	this->in_game_second = 0.0;
	this->window = window;
	this->game_ui = ui;

	// Load all these from the file after the user picks a save:
	this->player_money = 0.00;
	this->current_save_idx = 0;
	this->current_save_name = "no save loaded";
	this->num_entitys = 0;

	// Load planet orbit path rings:
	this->orbit_rings = new rings();

	// Set up ImGui debug tools:
	ImGuiIO& io = ImGui::GetIO(); (void)io;
}
// Function to reset all of the settings to the default 'safe' values.
void star_maps_game::reset_settings() {
	json default_settings = {
	{"res_width", 1280},
	{"res_height", 720},
	{"fullscreen", false},
	// 1=low, 2=medium, 3=high
	{"graphics_preset", "high"},
	{"fps_cap", 0}
	};

	std::ofstream settings_file(SETTINGS_FILE_LOCATION);
	settings_file << default_settings.dump(4);
	settings_file.close();
}

// Function to pull user settings from the local settings file.
json star_maps_game::get_settings()
{
	json user_settings;
	std::ifstream settings_file(SETTINGS_FILE_LOCATION);
	if (settings_file.is_open())
	{
		settings_file >> user_settings;
		settings_file.close();
	}
	// If no settings file is found, create a new one with default settings as defined in reset_settings():
	else
	{
		reset_settings();
		return get_settings();
	}
	return user_settings;
}

// Function to change any setting to a new value.
void star_maps_game::change_setting(const std::string setting, const json new_value) {
	json user_settings = get_settings();
	user_settings[setting] = new_value;
	std::ofstream settings_file(SETTINGS_FILE_LOCATION);
	settings_file << user_settings.dump(4);
	settings_file.close();
}

// Function to load the save file at passed in index and render all objects from the save data.
void star_maps_game::load_save(int index)
{
	// Load the json values stored in the save file:
	std::string save_file = "save_data/save_" + std::to_string(index) + ".json";
	std::ifstream file(save_file);
	json data;

	if (file.is_open()) {
		file >> data;
		file.close();
	}
	// Save the appropriate values from the save file to the game logic:
	std::cout << "loaded save " << index << "with data: \n" << data << std::endl;
	this->current_save_idx = index;
	this->current_save_name = data["galaxy_name"];
	this->player_money = data["money"];
	this->sols_passed = data["current_sol"];
	std::string in_game_time = data["current_time"];
	this->in_game_hour = std::stoi(in_game_time.substr(0, 2));
	this->in_game_minute = std::stoi(in_game_time.substr(4, 6));
	this->in_game_second = std::stoi(in_game_time.substr(8, 10));
	std::string total_played = data["total_time_played"];
	this->total_played_hour = std::stoi(total_played.substr(0, 2));
	this->total_played_minute = std::stoi(total_played.substr(4, 6));
	this->total_played_second = std::stoi(total_played.substr(8, 10));
	this->difficulty = diff_text_to_int(data["difficulty"]);
	this->status_1_percent = data["status_1_percent"];
	this->status_2_percent = data["status_2_percent"];
	this->status_3_percent = data["status_3_percent"];

	// Spawn in all game objects from the save data:
	if (data.find("entities") != data.end())
		for (const auto& entity : data["entities"])
			spawn_entity(entity["obj_type"], entity["texture"], entity["parent"], glm::vec3(entity["location"]["X"], entity["location"]["Y"], entity["location"]["Z"]));

	this->loaded_save = true;
}

// Function to save the game in its current state to a json file.
void star_maps_game::save_game()
{
	// Get the correct formatting for all of the strings in the save file:
	std::string in_game_time_out = (this->in_game_hour < 10) ? "0" + std::to_string(this->in_game_hour) : std::to_string(this->in_game_hour);
	in_game_time_out.append("H_");
	in_game_time_out.append((this->in_game_minute < 10) ? "0" + std::to_string(this->in_game_minute) : std::to_string(this->in_game_minute));
	in_game_time_out.append("M_");
	in_game_time_out.append((this->in_game_second < 10) ? "0" + std::to_string(this->in_game_second) : std::to_string(this->in_game_second));
	in_game_time_out.append("S");

	std::string total_time_out = (this->total_played_hour < 10) ? "0" + std::to_string(this->total_played_hour) : std::to_string(this->total_played_hour);
	total_time_out.append("H_");
	total_time_out.append((this->total_played_minute < 10) ? "0" + std::to_string(this->total_played_minute) : std::to_string(this->total_played_minute));
	total_time_out.append("M_");
	total_time_out.append((this->total_played_second < 10) ? "0" + std::to_string(this->total_played_second) : std::to_string(this->total_played_second));
	total_time_out.append("S");

	json updated_data = {
	   {"galaxy_name", this->current_save_name},
	   {"money", this->player_money},
	   {"current_sol", this->sols_passed},
	   {"current_time", in_game_time_out},
	   {"total_time_played", total_time_out},
	   {"difficulty", diff_int_to_text(this->difficulty)},
	   {"status_1_percent", this->status_1_percent},
	   {"status_2_percent", this->status_2_percent},
	   {"status_3_percent", this->status_3_percent}
	};

	// Call update_save() with the new data to save it to the exising save file:
	update_save(this->current_save_idx, updated_data);
}

// Function to generate the data for a new save file based on the passed in difficulty and galaxy name.
json star_maps_game::generate_new_save_data(const std::string difficulty, const std::string galaxy_name) {
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
void star_maps_game::create_new_save(const std::string difficulty, const std::string galaxy_name) {
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
void star_maps_game::delete_save(int delete_index) {
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
std::vector<json> star_maps_game::preview_saves() {
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

// Function to update the players save file with all of the new information passed in.
void star_maps_game::update_save(int index, const json& new_data)
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

// Function to give or take away money from the player (for testing purposes).
void star_maps_game::give_money(int amount)
{
	this->player_money += amount;
}

// Function to handle debug tools inside the game logic.
void star_maps_game::update_ingame_clock(float time_to_add, HUD* game_ui)
{
	// Update save data played timer if a save file is loaded and the game isn't paused:
	if (loaded_save && !paused)
	{
		this->total_played_second += (time_to_add);
		if (this->total_played_second >= 60.0)
		{
			this->total_played_minute++;
			this->total_played_second -= 60.0;
		}
		if (this->total_played_minute >= 60)
		{
			this->total_played_hour++;
			this->total_played_minute -= 60;
		}
	}
	// Update in game timer if the game isn't paused:
	if (!paused)
	{
		this->in_game_second += (time_to_add * this->game_speed_multiplier);
		if (this->in_game_second >= 60.0)
		{
			this->in_game_minute++;
			this->in_game_second -= 60.0;
		}
		if (this->in_game_minute == 60)
		{
			this->in_game_hour++;
			this->in_game_minute -= 60;
		}
		if (this->in_game_hour == 24)
		{
			this->sols_passed++;
			this->in_game_hour = 0;
		}
	}
	// Format output for UI clock correctly:
	std::stringstream hour;
	hour << std::setfill('0') << std::setw(2) << static_cast<int>(this->in_game_hour);
	std::stringstream min;
	min << std::setfill('0') << std::setw(2) << static_cast<int>(this->in_game_minute);
	std::stringstream sec;
	sec << std::setfill('0') << std::setw(2) << static_cast<int>(this->in_game_second);
	std::stringstream formatted_time;
	formatted_time << hour.str() << ":" << min.str() << ":" << sec.str();
	std::string clock_text = "Day " + std::to_string(this->sols_passed) + "        " + formatted_time.str();

	// Format output for UI money display correctly:
	std::string money = std::to_string(static_cast<int>(this->player_money));
	for (int i = money.length() - 3; i > 0; i -= 3)
		money.insert(i, ",");

	game_ui->update_text_element(0, clock_text);
	game_ui->update_text_element(1, money);

	// Set up debug values for development tests:
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::Text("Spawned Entitys: %d", this->num_entitys);
	ImGui::Text("Sols passed: %d", this->sols_passed);
	ImGui::Text("Clock: %d:%d:%.2f", this->in_game_hour, this->in_game_minute, this->in_game_second);
	ImGui::SliderFloat("Game Speed", &this->game_speed_multiplier, 1.0, 1000.0);
	ImGui::Text("Galaxy Name: %s", this->current_save_name.c_str());
	ImGui::Text("Save file idx: %d", this->current_save_idx);
	ImGui::Text("Time played on save file: %dH:%dM:%.2fS", this->total_played_hour, this->total_played_minute, this->total_played_second);
	ImGui::Text("Player Money: %.2f", this->player_money);
	ImGui::SliderFloat("Cam Sens:", &sensitivity, 1.0f, 4.0f);
	ImGui::SliderFloat("Move sens:", &movement_speed, 0.1f, 2.0f);
	if (ImGui::Button("Give $100,000,000"))
		this->give_money(100000000);
	if (ImGui::Button("Subtract $100,000,000"))
		this->give_money(-100000000);
	if (ImGui::Button("Gen New Save"))
		create_new_save("easy", "test");
	if (ImGui::Button("Load save 1"))
		this->load_save(1);
	if (ImGui::Button("Load save 2"))
		this->load_save(2);
	if (ImGui::Button("load save 3"))
		this->load_save(3);
	if (ImGui::Button("Save Game"))
		this->save_game();
	if (ImGui::Button("Clear All Ents"))
		this->despawn_all_entities();
}

// Function that handles the game speed and rendering of all game objects.
void star_maps_game::entity_manager(const glm::vec3& camera_position, const glm::vec3& camera_front, float game_speed)
{
	if (this->paused)
		game_speed_multiplier = 0.0f;
	for (game_object* obj : this->entitiys)
		obj->render(camera_position, camera_front, game_speed, this->orbit_rings->ring_positions);
	if (this->entitiys.size() != 0)
		this->orbit_rings->render(camera_position, camera_front);
}

// Function to spawn a new game object with the passed in values.
void star_maps_game::spawn_entity(int type, int texture_id, int parent_in, glm::vec3 location)
{
	this->num_entitys = this->entitiys.size();
	long int new_entity_id = ++this->num_entitys;
	
	std::cout << "Spawning new entity of type " << type << ". - ID: " << new_entity_id << std::endl;
	game_object* new_ent = new game_object(this->window);

	// Depending on the object type passed in, different shaders and obj files will be loaded:
	switch (type) {
	case -1: // Temporary Item When picking a placement spot
		new_ent->shader = load_shader("shaders/placing_item/vert.glsl", "shaders/placing_item/frag.glsl");
		new_ent->load_object("assets/objects/planets/planet.obj");
		new_ent->orbit_center = glm::vec3(0.0f, 0.0f, 0.0f);
		new_ent->orbit_radius = 0;
		new_ent->orbit_speed = 0;
		new_ent->size_adjust = 10;
		break;
	case 0: // Sun
		new_ent->shader = load_shader("shaders/planets/vert.glsl", "shaders/planets/frag.glsl");
		new_ent->load_object("assets/objects/planets/planet.obj");
		new_ent->orbit_center = glm::vec3(0.0f, 0.0f, 0.0f);
		new_ent->orbit_radius = 0;
		new_ent->orbit_speed = 0;
		new_ent->size_adjust = 10;
		break;
	case 1: // Planet
		new_ent->shader = load_shader("shaders/planets/vert.glsl", "shaders/planets/frag.glsl");
		new_ent->load_object("assets/objects/planets/planet.obj");
		new_ent->orbit_center = glm::vec3(0.0f, 0.0f, 0.0f);
		new_ent->orbit_radius = 25;
		new_ent->size_adjust = 5;
		new_ent->orbit_speed = 0.5;
		break;
	case 2: // Ship
		new_ent->shader = load_shader("shaders/ships/vert.glsl", "shaders/ships/frag.glsl");
		new_ent->load_object("assets/objects/ships/ship_1/ship_1_cent.obj");
		new_ent->orbit_center = glm::vec3(0.0f, 0.0f, 0.0f);
		new_ent->orbit_radius = 15;
		new_ent->orbit_speed = 0.5f;
		new_ent->size_adjust = 1;
		break;
	case 3: // Holo Drive-In
		new_ent->shader = load_shader("shaders/ships/vert.glsl", "shaders/ships/frag.glsl");
		new_ent->load_object("assets/objects/stations/holodrivein/holodrivein.obj");
		new_ent->orbit_center = glm::vec3(0.0f, 0.0f, 0.0f);
		new_ent->orbit_radius = 15;
		new_ent->orbit_speed = 0.5f;
		new_ent->size_adjust = 1;
		break;
	case 4: // Road Start/End
		new_ent->shader = load_shader("shaders/ships/vert.glsl", "shaders/ships/frag.glsl");
		new_ent->load_object("assets/objects/stations/roadgate/road_gate.obj");
		new_ent->orbit_center = glm::vec3(0.0f, 0.0f, 0.0f);
		new_ent->orbit_radius = 15;
		new_ent->orbit_speed = 0.5f;
		new_ent->size_adjust = 1;
		break;
	default:
		std::cout << "Error: invalid type: " << type << std::endl;
		--this->num_entitys;
		return;
	}
	// Load the correct texture for the game_object based on the passed in texture_id:
	switch (texture_id) {
	case 0:
		new_ent->load_texture("assets/textures/planets/sun.jpg");
		break;
	case 1:
		new_ent->load_texture("assets/textures/planets/earth.jpg");
		break;
	case 2:
		new_ent->load_texture("assets/textures/planets/p1.jpg");
		break;
	case 3:
		new_ent->load_texture("assets/textures/planets/p2.jpg");
		break;
	case 4:
		new_ent->load_texture("assets/textures/planets/p3.jpg");
		break;
	case 5:
		new_ent->load_texture("assets/textures/planets/p4.jpg");
		break;
	case 6:
		new_ent->load_texture("assets/objects/ships/ship_1/textures/ship_1.png");
		break;
	default:
		std::cout << "Error: invalid texture id: " << texture_id << std::endl;
		return;
	}
	// If the game object being spawned is a planet (not the sun), set the correct values for 
	// its orbit speed, orbit radius, and scale adjustment:
	float orbit_speeds[7] = { 0.05, 0.02, 0.025, 0.045, 0.09, 0.005, 0.009};
	float size_changes[7] = {.25, 0.45, 0.35, 0.15, 0.75, 0.45, 0.30};
	float radiuses[7] = { 80, 70, 130, 380, 98, 205, 250 };
	if (new_entity_id > 1 && new_entity_id < 8)
	{
		new_ent->orbit_radius = radiuses[new_entity_id - 2];
		new_ent->orbit_speed = orbit_speeds[new_entity_id - 2];
		new_ent->size_adjust = size_changes[new_entity_id - 2];
	}
	// Make sure the shader loaded correctly; print out errors if any are found:
	glUseProgram(new_ent->shader);
	int success;
	glGetShaderiv(new_ent->shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(new_ent->shader, 512, NULL, infoLog);
		std::cout << "Shader compilation error on entity " << new_entity_id << ": " << infoLog << std::endl;
	}

	// Give new_ent the final unique values it needs and then add it to the array of all spawned game_objects:
	new_ent->planet_entity_id = new_entity_id;
	new_ent->parent = parent_in;
	new_ent->position = location;
	for (game_object* obj : this->entitiys)
		if (obj->planet_entity_id == parent_in)
			new_ent->parent_planet = obj;
	entitiys.push_back(new_ent);
}

// Function that despawns a particular game object based on its id.
void star_maps_game::despawn_entity(int entity_id)
{
	long int new_entity_id = --this->num_entitys;
	std::cout << "Attempting to despawn entity with id: " << entity_id << std::endl;
}

// Function that clears all spawned entities from the game world.
void star_maps_game::despawn_all_entities()
{
	for (game_object* obj : this->entitiys) {
		delete obj;
	}
}

// Function to switch the camera mode from top down to spinning around the system.
void star_maps_game::switch_camera_mode(glm::vec3& camera_position, glm::vec3& camera_front, float& camera_yaw, float& camera_pitch, int view_switch)
{
	// If passed in view_switch is 0, switch to the side 'spinning view':
	if (view_switch == 0)
	{
		camera_position = glm::vec3(642.0f, 692.0f, -1220.0f);
		camera_front = glm::vec3(0.0f, 1.0f, 0.0f);
		camera_yaw = 115.0f;
		camera_pitch = -35.0f;
	}
	// If passed in view_switch is 1, switch to the top down view.
	else
	{
		camera_position = glm::vec3(0.0f, 1200.0f, 3.0f);
		camera_front = glm::vec3(0.0f, -1.0f, 0.0f);
		camera_yaw = -90.0f;
		camera_pitch = -90.0f;
	}
}

// Function that will save the current game to the correct save file and return to the main menu.
void star_maps_game::save_and_return_to_menu()
{
	save_game();
	exit(0);
	// To-Do: change exit(0) to function to return to main menu.
}

// Function that will close the app completely without saving the game data.
void star_maps_game::close_game()
{
	exit(0);
}

// Function to convert text value of difficulty to an integer.
int diff_text_to_int(std::string diff)
{
	if (diff == "easy")
		return 1;
	else if (diff == "medium")
		return 2;
	else if (diff == "hard")
		return 3;
	else
		return 4;
}

// Function to convert int value of difficulty to a string.
std::string diff_int_to_text(int diff)
{
	if (diff == 1)
		return "easy";
	else if (diff == 2)
		return "medium";
	else if (diff == 3)
		return "hard";
	else
		return "dev";
}

// Function to change between fullscreen and windowed mode.
void switch_display_type(GLFWwindow* window, bool fullscreen)
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* monitor_settings = glfwGetVideoMode(monitor);
	if (fullscreen)
		glfwSetWindowMonitor(window, monitor, 0, 0, monitor_settings->width, monitor_settings->height, monitor_settings->refreshRate);
	else
		glfwSetWindowMonitor(window, nullptr, 0, 30, monitor_settings->width, monitor_settings->height - 71, monitor_settings->refreshRate);
}

float sensitivity = 1.0f;
float movement_speed = 1.0f;
bool show_debug = true;

// Function to read player input and act based on it:
void star_maps_game::read_player_input(glm::vec3& camera_position, glm::vec3& camera_front, float& camera_yaw, float& camera_pitch)
{
	if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) // Move forward
		camera_position += movement_speed * camera_front;
	if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS) // Move backward
		camera_position -= movement_speed * camera_front;
	if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS) // Move left
		camera_position -= glm::normalize(glm::cross(camera_front, glm::vec3(0.0f, 1.0f, 0.0f))) * movement_speed;
	if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS) // Move right
		camera_position += glm::normalize(glm::cross(camera_front, glm::vec3(0.0f, 1.0f, 0.0f))) * movement_speed;
	if (glfwGetKey(this->window, GLFW_KEY_TAB) == GLFW_PRESS) // Toggle debug menus
		show_debug = !show_debug;
	if (glfwGetKey(this->window, GLFW_KEY_LEFT) == GLFW_PRESS) // Tilt Camera Left
		camera_yaw -= sensitivity;
	if (glfwGetKey(this->window, GLFW_KEY_RIGHT) == GLFW_PRESS) // Tilt Camera Right
		camera_yaw += sensitivity;
	if (glfwGetKey(this->window, GLFW_KEY_UP) == GLFW_PRESS) // Tilt Camera Up
		camera_pitch += sensitivity;
	if (glfwGetKey(this->window, GLFW_KEY_DOWN) == GLFW_PRESS) // Tilt Camera Down
		camera_pitch -= sensitivity;

	if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		last_frame_down = 1;
		double mx, my;
		glfwGetCursorPos(this->window, &mx, &my);

		// If about to click on save or exit, give slight visual confirmation by changing texture:
		if (mx > 840 && mx < 975 && my > 957 && my < 1067) // select tool
			game_ui->update_element(1, 5);
		else if (mx > 1000 && mx < 1129 && my > 957 && my < 1067) // delete tool
			game_ui->update_element(1, 6);
		else if (mx > 1167 && mx < 1292 && my > 957 && my < 1067) // save tool
			game_ui->update_element(1, 3);
		else if (mx > 1320 && mx < 1445 && my > 957 && my < 1067) // exit tool
			game_ui->update_element(1, 4);
	}

	if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && last_frame_down == 1)
	{
		last_frame_down = 0;
		double mx, my;
		glfwGetCursorPos(this->window, &mx, &my);

		if (mx > 840 && mx < 975 && my > 957 && my < 1067) // select tool
		{
			game_ui->update_element(1, 1);

		}

		else if (mx > 1000 && mx < 1129 && my > 957 && my < 1067) // delete tool
			game_ui->update_element(1, 2);
		else if (mx > 1167 && mx < 1292 && my > 957 && my < 1067) // save tool
			game_ui->update_element(1, 0);
		else if (mx > 1320 && mx < 1445 && my > 957 && my < 1067) // exit tool
			glfwSetWindowShouldClose(this->window, 1);
		else if (mx > 100 && mx < 155 && my > 950 && my < 1005) // tile 1
			game_ui->update_element(0, 5);
		else if (mx > 197 && mx < 251 && my > 950 && my < 1005) // tile 2
			game_ui->update_element(0, 7);
		else if (mx > 293 && mx < 357 && my > 950 && my < 1005) // tile 3
			game_ui->update_element(0, 9);
		else if (mx > 389 && mx < 444 && my > 950 && my < 1005) // tile 4
			game_ui->update_element(0, 11);
		else if (mx > 484 && mx < 540 && my > 950 && my < 1005) // tile 5
			game_ui->update_element(0, 12);
		else if (mx > 580 && mx < 636 && my > 950 && my < 1005) // tile 6
			game_ui->update_element(0, 15);
		else if (mx > 100 && mx < 155 && my > 1015 && my < 1070) // tile 7
			game_ui->update_element(0, 6);
		else if (mx > 197 && mx < 251 && my > 1015 && my < 1070) // tile 8
			game_ui->update_element(0, 8);
		else if (mx > 293 && mx < 357 && my > 1015 && my < 1070) // tile 9
			game_ui->update_element(0, 10);
		else if (mx > 389 && mx < 444 && my > 1015 && my < 1070) // tile 10
			game_ui->update_element(0, 13);
		else if (mx > 484 && mx < 540 && my > 1015 && my < 1070) // tile 11
			game_ui->update_element(0, 14);
		else if (mx > 580 && mx < 636 && my > 1015 && my < 1070) // tile 12
			game_ui->update_element(0, 16);
		else if (mx > 1145 && mx < 1166 && my >= 0 && my < 25) // pause
		{
			paused = true;
			game_ui->update_element(3, 1);
		}
		else if (mx > 1186 && mx < 1208 && my >= 0 && my < 25) // play
		{
			paused = false;
			this->game_speed_multiplier = 1.0f;
			game_ui->update_element(3, 2);
		}
		else if (mx > 1220 && mx < 1245 && my >= 0 && my < 25) // ffwd
		{
			if (paused)
			{
				paused = false;
				this->game_speed_multiplier = 1.0f;
			}
			this->game_speed_multiplier *= 2.0f;
			game_ui->update_element(3, 3);
		}
		// Reset tool bar to place in case player started to click on save/quit but then moved the mouse:
		else
			game_ui->update_element(1, 1);
	}

	double mx, my;
	glfwGetCursorPos(this->window, &mx, &my);

	if (mx > 100 && mx < 155 && my > 950 && my < 1005) // tile 1
		game_ui->add_element(5);
	else if (mx > 197 && mx < 251 && my > 950 && my < 1005) // tile 2
		game_ui->add_element(7);
	else if (mx > 293 && mx < 357 && my > 950 && my < 1005) // tile 3
		game_ui->add_element(9);
	else if (mx > 389 && mx < 444 && my > 950 && my < 1005) // tile 4
		game_ui->add_element(11);
	else if (mx > 484 && mx < 540 && my > 950 && my < 1005) // tile 5
		game_ui->add_element(13);
	else if (mx > 580 && mx < 636 && my > 950 && my < 1005) // tile 6
		game_ui->add_element(15);
	else if (mx > 100 && mx < 155 && my > 1015 && my < 1070) // tile 7
		game_ui->add_element(6);
	else if (mx > 197 && mx < 251 && my > 1015 && my < 1070) // tile 8
		game_ui->add_element(8);
	else if (mx > 293 && mx < 357 && my > 1015 && my < 1070) // tile 9
		game_ui->add_element(10);
	else if (mx > 389 && mx < 444 && my > 1015 && my < 1070) // tile 10
		game_ui->add_element(12);
	else if (mx > 484 && mx < 540 && my > 1015 && my < 1070) // tile 11
		game_ui->add_element(14);
	else if (mx > 580 && mx < 636 && my > 1015 && my < 1070) // tile 12
		game_ui->add_element(16);
	else
		game_ui->delete_preview();

	// Clamp the camera pitch to keep it oriented correctly:
	if (camera_pitch > 89.0f)
		camera_pitch = 89.0f;
	if (camera_pitch < -89.0f)
		camera_pitch = -89.0f;

	if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		last_frame_down = 2;
	// Raycast to check which object the player is trying to interact with:
	if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && last_frame_down == 2) // Right Click
	{
		last_frame_down = 0;
		// Get resolution of the glfw window:
		int window_width, window_height;
		glfwGetWindowSize(this->window, &window_width, &window_height);

		// Define the view and projection matrices for the camera, as well as the inverse of those matrices
		// which will be used for raycasting to find which object the player clicked on:
		glm::mat4 view_matrix = glm::lookAt(camera_position, camera_position + camera_front, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 4000.0f);
		glm::mat4 inverse_projection_matrix = glm::inverse(projection_matrix);
		glm::mat4 inverse_view_matrix = glm::inverse(view_matrix);
		glm::vec3 out_direction = glm::vec3(-99999.0f);

		// Get mouse position on the glfw window:
		double mouseX, mouseY;
		glfwGetCursorPos(this->window, &mouseX, &mouseY);

		// Convert mouse position to normalized device coordinates (essentially the starting spot on the window):
		float ndcX = (2.0f * mouseX) / window_width - 1.0f;
		float ndcY = 1.0f - (2.0f * mouseY) / window_height;

		// Begin to define the start and end position of the raycast based on the mouse coordinates
		glm::vec4 ray_start = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
		glm::vec4 ray_end = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

		// Find the start and end positions for the ray relative to the window resolution and camera FOV
		// by multiplying the ray start and end from above with the inverse projection matrix:
		glm::vec4 ray_start_world = inverse_projection_matrix * ray_start;
		ray_start_world /= ray_start_world.w;
		glm::vec4 ray_end_world = inverse_projection_matrix * ray_end;
		ray_end_world /= ray_end_world.w;

		// Find the actual start and end positions of the ray in world coordinates by multipling the previous
		// result value with the inverse view matrix, which lets us take the cameras view and flip it, essentially:
		ray_start_world = inverse_view_matrix * ray_start_world;
		ray_end_world = inverse_view_matrix * ray_end_world;

		// Calculate the direction of the ray:
		glm::vec3 ray_direction = glm::normalize(glm::vec3(ray_end_world - ray_start_world));

		// Loop through objects and their bounding boxes to check for intersection along the line of the raycast:
		for (game_object* obj : entitiys) {
			glm::vec3 inv_direction = 1.0f / ray_direction;

			// Find the real world coordinates for the bounding box corners after taking into account
			// the objects changes through out its time existing in the game world:
			glm::vec3 bboxMin = obj->boundingBoxMin + obj->position;
			glm::vec3 bboxMax = obj->boundingBoxMax + obj->position;
			glm::vec3 tMin = (bboxMin - glm::vec3(ray_start_world)) * inv_direction;
			glm::vec3 tMax = (bboxMax - glm::vec3(ray_start_world)) * inv_direction;
			glm::vec3 tEnter = glm::min(tMin, tMax);
			glm::vec3 tExit = glm::max(tMin, tMax);
			float tEnterMax = glm::max(glm::max(tEnter.x, tEnter.y), tEnter.z);
			float tExitMin = glm::min(glm::min(tExit.x, tExit.y), tExit.z);

			// If the raycast enters the bounding box anywhere, set the bounding box to be visible,
			// Mainly for testing purposes for now:
			if (tEnterMax <= tExitMin && tExitMin >= 0.0f)
				obj->render_bb = true;
			else
				obj->render_bb = false;
		}
	}
}
