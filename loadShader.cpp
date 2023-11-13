#include <loadShader.h>

GLchar* read_shader_text(const char* file_path)
{
    // Open shader file and read in the content.
    FILE* shader_file;
    errno_t error = fopen_s(&shader_file, file_path, "r");
    if (error == '0')
    {
        fclose(shader_file);
        return NULL;
    }
    if (!shader_file)
    {
        std::cout << "Error in opening shader: " << file_path << "." << std::endl;
        return NULL;
    }

    // Find the length of the shader file and allocate mem to the shader char array.
    fseek(shader_file, 0, SEEK_END);
    long file_len = ftell(shader_file);
    fseek(shader_file, 0, SEEK_SET);
    GLchar* shader_content = (GLchar*)malloc(file_len + 1);

    // Read the contents of the shader file into the allocated memory, remove the empty char at the end, and close the file.
    fread(shader_content, 1, file_len, shader_file);
    shader_content[file_len] = 0;
    fclose(shader_file);

    return shader_content;
}
// Function to load vertex and fragment shaders from a file:
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

