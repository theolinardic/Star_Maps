// loadFiles serves to load local user files including their settings
// file and all save files.

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

// Constant definitions:
#define SETTINGS_FILE_LOCATION "settings.json"
#define SAVE_FOLDER_LOCATION "save_data/"
#define DEFAULT_RES_WIDTH 1280
#define DEFAULT_RES_HEIGHT 720
#define DEFAULT_SHOULD_FULLSCREEN false;
#define DEFAULT_GRAPHICS_PRESET "high"
#define DEFAULT_FPS_CAP 0

struct user_settings {
	int res_width;
	int res_height;
	int should_fullscreen;
	int graphics_preset;
	int fps_cap;
};

struct entity {
	std::string type;
	std::string orbit_location;
	int orbit_placement_x;
	int orbit_placement_y;
	int orbit_placement_z;
};

struct save_data_values {
	std::string system_name;
	int money;
	int current_sol;
	std::string current_time;
	std::string total_time_played;
	int difficulty;
	float status_1_percent;
	float status_2_percent;
	float status_3_percent;
	std::vector<entity> stations;
	std::vector<entity> planets;
};

struct preview_save_data_values {
	std::string system_name;
	int money;
	int current_sol;
	std::string total_time_played;
	int difficulty;
};


// Function declarations:
struct user_settings loadSettings(void);
void changeSetting(std::string setting, int new_value);
void setDefaultSettings(void);
struct save_data_values generateNewSaveData(int diff, std::string name);
void createNewSave(int diff, std::string name);
void deleteSave(int delete_index);
struct preview_save_data_values previewSaves();
struct save_data_values loadSave(int save_index);