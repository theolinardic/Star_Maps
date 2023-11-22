#include <load_object.h>

// Function to take in the name of an obj file and references of where to save the vertex uv and normals of the rendered object.
// Returns 0 if there is any error loading the object file. Otherwise return 1.
// TO-DO: change to void function and have a debug message pop up instead.
int load_object_file(const char* file_name, std::vector <glm::vec3> & v_out,
	std::vector <glm::vec2> & uv_out, std::vector <glm::vec3> & n_out)
{
	FILE* obj_file;
	

	return 1;
}

// Class definition for a planet object. Needs a name and refrence to a texture file when created.
planet::planet(std::string planet_name, const char* texture_file_name)
{
	this->name = planet_name;
	this->visible = true;

	std::vector<glm::vec3> v, n;
	std::vector<glm::vec2> uv;

	int status = load_object_file("assets/objects/planets/planet.obj", v, uv, n);

	if (!status)
	{
		std::cout << "Error loading: assets/objects/planets/planet.obj" << std::endl;
	}

	load_texture(texture_file_name);
}

// Load texture from the passed in file name and apply it itself.
void planet::load_texture(const char* file_name)
{
	glGenTextures(1, &this->texture_id);
	std::ifstream infile(file_name);
	if (infile.good() != 1)
	{
		std::cout << "Err: " << infile.good() << std::endl;
	}
	std::cout << this->texture_id << std::endl;
	
	std::cout << this->texture_id << std::endl;
	glBindTexture(GL_TEXTURE_2D, this->texture_id);

	int w, h;
	unsigned char* texture_img = SOIL_load_image(file_name, &w, &h, 0, SOIL_LOAD_RGB);

	if (!texture_img)
	{
		std::cout << "Error loading texture: " << file_name << ". err: " << SOIL_last_result() << std::endl;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_img);
	SOIL_free_image_data(texture_img);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// Simple function to test if planet init worked succsessfully...
void planet::print_name()
{
	std::cout << "Planet name: " << this->name << std::endl;
}

// Rendering loop function to keep the planet visible and moving in the correct way.
void planet::render(GLuint vao, GLuint shader)
{
	// Only render to the openGL window if the planet is currently set to visible.
	// TO-DO: change visible to swapping the texture to a black texture to implement FOG-Of-WAR type function
	// for game progress.
	if (this->visible)
	{
		glBindVertexArray(vao);
		glUseProgram(shader);
	}

	glm::mat4 test2 = glm::mat4(1.0f);
	test2 = glm::translate(test2, glm::vec3(0.0f, 0.0f, 3.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader, "translate"), 1, GL_FALSE, glm::value_ptr(test2));

	glBindTexture(0, this->texture_id);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}


// Function to read in the text of the suppied shader file.
GLchar* read_shader_text(const char* file_path)
{
	// Open shader file and read in the content.
	FILE* shader_file;
	errno_t error = fopen_s(&shader_file, file_path, "rb");
	if (error == '0' || !shader_file)
	{
		std::cout << "Error in opening shader: " << file_path << "." << std::endl;
		fclose(shader_file);
		return NULL;
	}
	// Find the length of the shader file and allocate mem to the shader char array.
	fseek(shader_file, 0, SEEK_END);
	long int file_len = ftell(shader_file);
	rewind(shader_file);
	GLchar* shader_content = (GLchar*)malloc(file_len + 1);

	// Read the contents of the shader file into the allocated memory, remove the empty char at the end, and close the file.
	fread(shader_content, 1, file_len, shader_file);
	shader_content[file_len] = '\0';
	fclose(shader_file);

	return shader_content;
}
// Function to load vertex and fragment shaders from a file.
GLuint load_shader(const char* vertex_file_path, const char* frag_file_path)
{
	GLchar* vertex_shader_content = read_shader_text(vertex_file_path);
	GLchar* frag_shader_content = read_shader_text(frag_file_path);

	// Create the vertex shader in OpenGL, pass it the correct source file, compile it, and free the char array of the input file:
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_content, NULL);
	glCompileShader(vertex_shader);
	free(vertex_shader_content);

	// Do the same for the fragment shader:
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &frag_shader_content, NULL);
	glCompileShader(fragment_shader);
	free(frag_shader_content);

	// Create shader program with the compiled vertex and fragment shaders:
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	// Delete the vertex and fragment shaders as they are in the shader pgrogram and do not need to be kept seperate.
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	return shader_program;
}
