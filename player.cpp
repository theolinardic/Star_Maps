#include <player.h>

float sensitivity = 1.0f;
float movement_speed = 1.0f;
bool show_debug = true;

// Function to read player input and act based on it:
void read_player_input(GLFWwindow* star_maps_window, glm::vec3& camera_position, glm::vec3& camera_front, float& camera_yaw, float& camera_pitch, std::vector<game_object*> entitiys)
{
	if (glfwGetKey(star_maps_window, GLFW_KEY_W) == GLFW_PRESS) // Move forward
		camera_position += movement_speed * camera_front;
	if (glfwGetKey(star_maps_window, GLFW_KEY_S) == GLFW_PRESS) // Move backward
		camera_position -= movement_speed * camera_front;
	if (glfwGetKey(star_maps_window, GLFW_KEY_A) == GLFW_PRESS) // Move left
		camera_position -= glm::normalize(glm::cross(camera_front, glm::vec3(0.0f, 1.0f, 0.0f))) * movement_speed;
	if (glfwGetKey(star_maps_window, GLFW_KEY_D) == GLFW_PRESS) // Move right
		camera_position += glm::normalize(glm::cross(camera_front, glm::vec3(0.0f, 1.0f, 0.0f))) * movement_speed;
	if (glfwGetKey(star_maps_window, GLFW_KEY_TAB) == GLFW_PRESS) // Toggle debug menus
		show_debug = !show_debug;
	if (glfwGetKey(star_maps_window, GLFW_KEY_LEFT) == GLFW_PRESS) // Tilt Camera Left
		camera_yaw -= sensitivity;
	if (glfwGetKey(star_maps_window, GLFW_KEY_RIGHT) == GLFW_PRESS) // Tilt Camera Right
		camera_yaw += sensitivity;
	if (glfwGetKey(star_maps_window, GLFW_KEY_UP) == GLFW_PRESS) // Tilt Camera Up
		camera_pitch += sensitivity;
	if (glfwGetKey(star_maps_window, GLFW_KEY_DOWN) == GLFW_PRESS) // Tilt Camera Down
		camera_pitch -= sensitivity;

	// Clamp the camera pitch to keep it oriented correctly:
	if (camera_pitch > 89.0f)
		camera_pitch = 89.0f;
	if (camera_pitch < -89.0f)
		camera_pitch = -89.0f;

	// Raycast to check which object the player is trying to interact with:
	if (glfwGetMouseButton(star_maps_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) // Right Click
	{
		// Get resolution of the glfw window:
		int window_width, window_height;
		glfwGetWindowSize(star_maps_window, &window_width, &window_height);

		// Define the view and projection matrices for the camera, as well as the inverse of those matrices
		// which will be used for raycasting to find which object the player clicked on:
		glm::mat4 view_matrix = glm::lookAt(camera_position, camera_position + camera_front, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), window_width + 0.0f / window_height, 0.1f, 4000.0f);
		glm::mat4 inverse_projection_matrix = glm::inverse(projection_matrix);
		glm::mat4 inverse_view_matrix = glm::inverse(view_matrix);
		glm::vec3 out_direction = glm::vec3(-99999.0f);

		// Get mouse position on the glfw window:
		double mouseX, mouseY;
		glfwGetCursorPos(star_maps_window, &mouseX, &mouseY);

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

			// If the raycast enters the bounding box anywhere, set the bounding box to be visible. 
			// Mainly for testing purposes for now:
			if (tEnterMax <= tExitMin && tExitMin >= 0.0f)
				obj->render_bb = true;
			else
				obj->render_bb = false;
		}
	}
}
