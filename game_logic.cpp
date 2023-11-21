#include <game_logic.h>


// Time Ratio:
// 1 Second IRL = 1 Minute In-Game
// 1 Minute IRL = 1 Hour In-Game
// 1 Hour IRL = 2.5 Days In-Game

star_maps_game::star_maps_game(bool p)
{
	this->paused = p;
	this->game_speed_multiplier = 1.0;
	this->sols_passed = 0;
	this->in_game_hour = 0;
	this->in_game_minute = 0;
	this->in_game_second = 0.0;

	// Load all these from the file after the user picks a save.
	this->player_money = 0.00;
	this->current_save_idx = 0;
	this->current_save_name = "no save loaded";
	this->num_entitys = 0;

	this->orbit_rings = new rings();
}

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

void star_maps_game::load_save(int index)
{
	using json = nlohmann::json;
	json data = load_save_file(index);

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

	if (data.find("entities") != data.end()) {
		for (const auto& entity : data["entities"]) {
			int obj_type = entity["obj_type"];
			int texture_id = entity["texture"];
			int parent = entity["parent"];
			double locX = entity["location"]["X"];
			double locY = entity["location"]["Y"];
			double locZ = entity["location"]["Z"];

			spawn_entity(obj_type, texture_id, parent, glm::vec3(locX, locY, locZ));
		}
	}
}

void star_maps_game::save_game()
{
	using json = nlohmann::json;

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

	update_save(this->current_save_idx, updated_data);
}

void star_maps_game::give_money(int amount)
{
	this->player_money += amount;
}

void star_maps_game::update_debug_ingame_clock(float time_to_add)
{
	// Modify to handle all debug HUD.

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
	if (ImGui::Button("Give $1,000,000")) {
		this->give_money(1000000);
	}
	if (ImGui::Button("Subtract $1,000,000")) {
		this->give_money(-1000000);
	}
	if (ImGui::Button("Load save 1")) {
		this->load_save(1);
	}
	if (ImGui::Button("Load save 2")) {
		this->load_save(2);
	}
	if (ImGui::Button("Save Game")) {
		this->save_game();
	}
	if (ImGui::Button("Gen New Save")) {
		create_new_save("easy", "test");
	}
	if (ImGui::Button("load save 3")) {
		this->load_save(3);
	}
	if (ImGui::Button("Clear All Ents"))
		this->despawn_all_entities();
}

void star_maps_game::spawn_entity(int type, int texture_id, int parent_in, glm::vec3 location)
{
	std::cout << "test" << std::endl;
	this->num_entitys = this->entitiys.size();
	long int new_entity_id = ++this->num_entitys;
	
	std::cout << "spawning new entity of type " << type << ". - ID: " << std::endl;
	game_object* new_ent = new game_object();

	switch (type) {
	case 0:
		new_ent->shader = load_shader("shaders/planets/vert.glsl", "shaders/planets/frag.glsl");
		new_ent->LoadObject("assets/objects/planets/planet.obj");
		new_ent->orbit_center = glm::vec3(0.0f, 0.0f, 0.0f);
		new_ent->orbit_radius = 0;
		new_ent->orbit_speed = 0;
		new_ent->size_adjust = 10;
		break;
	case 1:
		std::cout << "X " << new_entity_id << std::endl;
		new_ent->shader = load_shader("shaders/planets/vert.glsl", "shaders/planets/frag.glsl");
		new_ent->LoadObject("assets/objects/planets/planet.obj");
		new_ent->orbit_center = glm::vec3(0.0f, 0.0f, 0.0f);
		new_ent->orbit_radius = 25;
		new_ent->size_adjust = 5;
		new_ent->orbit_speed = 0.5;
		break;
	case 2:
		new_ent->shader = load_shader("shaders/ships/vert.glsl", "shaders/ships/frag.glsl");
		new_ent->LoadObject("assets/objects/ships/ship_1/ship_1_cent.obj");
		new_ent->orbit_center = glm::vec3(0.0f, 0.0f, 0.0f);
		new_ent->orbit_radius = 15;
		new_ent->orbit_speed = 0.5f;
		new_ent->size_adjust = 1;
		break;
	default:
		std::cout << "Error: invalid type" << std::endl;
		--this->num_entitys;
	}
	switch (texture_id) {
	case 0:
		new_ent->LoadTexture("assets/textures/planets/planet_texture1.png");
		break;
	case 1:
		new_ent->LoadTexture("assets/textures/planets/earth.jpg");
		break;
	case 2:
		new_ent->LoadTexture("assets/objects/ships/ship_1/textures/ship_1.png");
		//new_ent->LoadTexture("assets / objects / test / Low_Poly_Vehicles_carPolice.obj");
		break;
	default:
		std::cout << "Error: invalid type" << std::endl;
	}
	float orbit_speeds[7] = { 0.05, 0.15, 0.02, 0.25, 0.09, 0.05, 0.12};
	float size_changes[7] = {.25, 0.45, 0.35, 0.15, 0.75, 0.45, 0.30};
	float radiuses[7] = { 80, 70, 130, 380, 98, 205, 250 };
	if (new_entity_id > 1 && new_entity_id < 8)
	{
		new_ent->orbit_radius = radiuses[new_entity_id - 2];
		new_ent->orbit_speed = orbit_speeds[this->num_entitys - 1];
		new_ent->size_adjust = size_changes[new_entity_id - 2];
	}
	std::cout << this->num_entitys << std::endl;
	glUseProgram(new_ent->shader);

	int success;
	glGetShaderiv(new_ent->shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(new_ent->shader, 512, NULL, infoLog);
		std::cout << "Shader compilation error: " << infoLog << std::endl;
	}

	new_ent->planet_entity_id = new_entity_id;
	new_ent->parent = parent_in;
	new_ent->position = location;

	for (game_object* obj : this->entitiys)
		if (obj->planet_entity_id == parent_in)
			new_ent->parent_planet = obj;

	entitiys.push_back(new_ent);
	//this->entitiys[new_entity_id].LoadObject();
}

void star_maps_game::despawn_all_entities()
{
	for (game_object* obj : this->entitiys) {
		delete obj;
	}
}

void star_maps_game::despawn_entity(int entity_id)
{
	long int new_entity_id = --this->num_entitys;
	std::cout << "despawning entity with id: " << entity_id << std::endl;
}

void star_maps_game::entity_manager(const glm::vec3& cameraPosition, const glm::vec3& cameraFront, float game_speed)
{
	if (this->paused)
	{
		game_speed_multiplier = 0.0f;
	}
	for (game_object* obj : this->entitiys) {
		obj->Render(cameraPosition, cameraFront, game_speed);
	}

	if (this->entitiys.size() != 0)
		this->orbit_rings->render();
			
}

void star_maps_game::check_click(glm::vec3 camera_position, glm::vec3 mouse_pos)
{
	for (game_object* obj : this->entitiys)
	{
		bool isClicked = obj->IsMouseOnObject(camera_position, mouse_pos, obj->get_center(), obj->get_radius());
		if (isClicked)
			std::cout << obj->planet_entity_id << std::endl;
	}
}

void star_maps_game::close_game()
{
	save_game();

	// close main window
	//star_maps
}