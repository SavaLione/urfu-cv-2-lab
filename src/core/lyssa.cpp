// clang-format off
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
// clang-format on

#include "core/shader.h"

#include <spdlog/spdlog.h>

int main(int argc, char *argv[])
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		spdlog::error("Error: {}", SDL_GetError());
		return -1;
	}

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char *glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
	const char *glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	SDL_Window *window	  = SDL_CreateWindow("OpenGL Test", 0, 0, 800, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	// SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	// SDL_GL_MakeCurrent(window, gl_context);

	bool exit = false;

	while(!exit)
	{
		SDL_Event event;

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					exit = true;
					break;
				case SDL_QUIT:
					exit = true;
					break;
				default:
					break;
			}
		}

		glViewport(0, 0, 800, 800);
		glClearColor(1.f, 0.f, 1.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(window);
	}
	// SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////

// // clang-format off
// #include <GL/glew.h>
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_opengl.h>
// // clang-format on

// #include <spdlog/spdlog.h>

// typedef float t_mat4x4[16];

// static inline void mat4x4_ortho(t_mat4x4 out, float left, float right, float bottom, float top, float znear, float zfar)
// {
// #define T(a, b) (a * 4 + b)

// 	out[T(0, 0)] = 2.0f / (right - left);
// 	out[T(0, 1)] = 0.0f;
// 	out[T(0, 2)] = 0.0f;
// 	out[T(0, 3)] = 0.0f;

// 	out[T(1, 1)] = 2.0f / (top - bottom);
// 	out[T(1, 0)] = 0.0f;
// 	out[T(1, 2)] = 0.0f;
// 	out[T(1, 3)] = 0.0f;

// 	out[T(2, 2)] = -2.0f / (zfar - znear);
// 	out[T(2, 0)] = 0.0f;
// 	out[T(2, 1)] = 0.0f;
// 	out[T(2, 3)] = 0.0f;

// 	out[T(3, 0)] = -(right + left) / (right - left);
// 	out[T(3, 1)] = -(top + bottom) / (top - bottom);
// 	out[T(3, 2)] = -(zfar + znear) / (zfar - znear);
// 	out[T(3, 3)] = 1.0f;

// #undef T
// }

// static const char *vertex_shader = "#version 130\n"
// 								   "in vec2 i_position;\n"
// 								   "in vec4 i_color;\n"
// 								   "out vec4 v_color;\n"
// 								   "uniform mat4 u_projection_matrix;\n"
// 								   "void main() {\n"
// 								   "    v_color = i_color;\n"
// 								   "    gl_Position = u_projection_matrix * vec4( i_position, 0.0, 1.0 );\n"
// 								   "}\n";

// static const char *fragment_shader = "#version 130\n"
// 									 "in vec4 v_color;\n"
// 									 "out vec4 o_color;\n"
// 									 "void main() {\n"
// 									 "    o_color = v_color;\n"
// 									 "}\n";

// typedef enum t_attrib_id
// {
// 	attrib_position,
// 	attrib_color
// } t_attrib_id;

// int main(int argc, char *argv[])
// {
// 	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
// 	{
// 		spdlog::error("Error: {}", SDL_GetError());
// 		return -1;
// 	}

// 	// Decide GL+GLSL versions
// #if defined(IMGUI_IMPL_OPENGL_ES2)
// 	// GL ES 2.0 + GLSL 100
// 	const char *glsl_version = "#version 100";
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
// #elif defined(__APPLE__)
// 	// GL 3.2 Core + GLSL 150
// 	const char *glsl_version = "#version 150";
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
// #else
// 	// GL 3.0 + GLSL 130
// 	const char *glsl_version = "#version 130";
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
// #endif

// 	// SDL_Init(SDL_INIT_VIDEO);
// 	// SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
// 	// SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
// 	// SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
// 	// SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
// 	// SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
// 	// SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

// 	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
// 	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
// 	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

// 	static const int width	= 800;
// 	static const int height = 600;

// 	SDL_Window *window =
// 		SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
// 	SDL_GLContext context = SDL_GL_CreateContext(window);

// 	GLuint vs, fs, program;

// 	vs = glCreateShader(GL_VERTEX_SHADER);
// 	fs = glCreateShader(GL_FRAGMENT_SHADER);

// 	int length = strlen(vertex_shader);
// 	glShaderSource(vs, 1, (const GLchar **)&vertex_shader, &length);
// 	glCompileShader(vs);

// 	GLint status;
// 	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
// 	if(status == GL_FALSE)
// 	{
// 		fprintf(stderr, "vertex shader compilation failed\n");
// 		return 1;
// 	}

// 	length = strlen(fragment_shader);
// 	glShaderSource(fs, 1, (const GLchar **)&fragment_shader, &length);
// 	glCompileShader(fs);

// 	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
// 	if(status == GL_FALSE)
// 	{
// 		fprintf(stderr, "fragment shader compilation failed\n");
// 		return 1;
// 	}

// 	program = glCreateProgram();
// 	glAttachShader(program, vs);
// 	glAttachShader(program, fs);

// 	glBindAttribLocation(program, attrib_position, "i_position");
// 	glBindAttribLocation(program, attrib_color, "i_color");
// 	glLinkProgram(program);

// 	glUseProgram(program);

// 	glDisable(GL_DEPTH_TEST);
// 	glClearColor(0.5, 0.0, 0.0, 0.0);
// 	glViewport(0, 0, width, height);

// 	GLuint vao, vbo;

// 	glGenVertexArrays(1, &vao);
// 	glGenBuffers(1, &vbo);
// 	glBindVertexArray(vao);
// 	glBindBuffer(GL_ARRAY_BUFFER, vbo);

// 	glEnableVertexAttribArray(attrib_position);
// 	glEnableVertexAttribArray(attrib_color);

// 	glVertexAttribPointer(attrib_color, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
// 	glVertexAttribPointer(attrib_position, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)(4 * sizeof(float)));

// 	const GLfloat g_vertex_buffer_data[] = {/*  R, G, B, A, X, Y  */
// 											1, 0, 0, 1, 0, 0, 0, 1, 0, 1, width, 0,		 0, 0, 1, 1, width, height,

// 											1, 0, 0, 1, 0, 0, 0, 0, 1, 1, width, height, 1, 1, 1, 1, 0,		height};

// 	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

// 	t_mat4x4 projection_matrix;
// 	mat4x4_ortho(projection_matrix, 0.0f, (float)width, (float)height, 0.0f, 0.0f, 100.0f);
// 	glUniformMatrix4fv(glGetUniformLocation(program, "u_projection_matrix"), 1, GL_FALSE, projection_matrix);

// 	for(;;)
// 	{
// 		glClear(GL_COLOR_BUFFER_BIT);

// 		SDL_Event event;
// 		while(SDL_PollEvent(&event))
// 		{
// 			switch(event.type)
// 			{
// 				case SDL_KEYUP:
// 					if(event.key.keysym.sym == SDLK_ESCAPE)
// 						return 0;
// 					break;
// 			}
// 		}

// 		glBindVertexArray(vao);
// 		glDrawArrays(GL_TRIANGLES, 0, 6);

// 		SDL_GL_SwapWindow(window);
// 		SDL_Delay(1);
// 	}

// 	SDL_GL_DeleteContext(context);
// 	SDL_DestroyWindow(window);
// 	SDL_Quit();

// 	return 0;
// }

////////////////////////////////////////////////////////////////////////////////////

// int main(int argc, char *argv[])
// {
// 	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
// 	{
// 		spdlog::error("Error: {}", SDL_GetError());
// 		return -1;
// 	}

// 	// Decide GL+GLSL versions
// #if defined(IMGUI_IMPL_OPENGL_ES2)
// 	// GL ES 2.0 + GLSL 100
// 	const char *glsl_version = "#version 100";
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
// #elif defined(__APPLE__)
// 	// GL 3.2 Core + GLSL 150
// 	const char *glsl_version = "#version 150";
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
// #else
// 	// GL 3.0 + GLSL 130
// 	const char *glsl_version = "#version 130";
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
// 	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
// #endif

// 	// Create window with graphics context
// 	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
// 	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
// 	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
// 	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
// 	SDL_Window *window			 = SDL_CreateWindow(
// 		  "Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
// 	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
// 	SDL_GL_MakeCurrent(window, gl_context);
// 	SDL_GL_SetSwapInterval(1); // Enable vsync

// 	// Setup Dear ImGui context
// 	IMGUI_CHECKVERSION();
// 	ImGui::CreateContext();
// 	ImGuiIO &io = ImGui::GetIO();
// 	(void)io;
// 	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
// 	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

// 	ImGui::StyleColorsDark();

// 	// Setup Platform/Renderer backends
// 	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
// 	ImGui_ImplOpenGL3_Init(glsl_version);

// 	// Our state
// 	bool show_demo_window	 = true;
// 	bool show_another_window = false;
// 	ImVec4 clear_color		 = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
// 	//GLuint program_id = load_shader("simple_vertex_shader.vert", "simple_fragment_shader.frag");

// 	// Main loop
// 	bool done = false;
// 	while(!done)
// 	{
// 		// Poll and handle events (inputs, window resize, etc.)
// 		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// 		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// 		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// 		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
// 		SDL_Event event;
// 		while(SDL_PollEvent(&event))
// 		{
// 			ImGui_ImplSDL2_ProcessEvent(&event);
// 			if(event.type == SDL_QUIT)
// 				done = true;
// 			if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
// 			   event.window.windowID == SDL_GetWindowID(window))
// 				done = true;
// 		}

// 		// Start the Dear ImGui frame
// 		ImGui_ImplOpenGL3_NewFrame();
// 		ImGui_ImplSDL2_NewFrame();
// 		ImGui::NewFrame();

// 		if(show_demo_window)
// 			ImGui::ShowDemoWindow(&show_demo_window);

// 		{
// 			static float f	   = 0.0f;
// 			static int counter = 0;

// 			ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

// 			ImGui::Text("This is some useful text.");		   // Display some text (you can use a format strings too)
// 			ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
// 			ImGui::Checkbox("Another Window", &show_another_window);

// 			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);			 // Edit 1 float using a slider from 0.0f to 1.0f
// 			ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color

// 			if(ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
// 				counter++;
// 			ImGui::SameLine();
// 			ImGui::Text("counter = %d", counter);

// 			ImGui::Text(
// 				"Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
// 			ImGui::End();
// 		}

// 		if(show_another_window)
// 		{
// 			ImGui::Begin("Another Window", &show_another_window);
// 			ImGui::Text("Hello from another window!");
// 			if(ImGui::Button("Close Me"))
// 				show_another_window = false;
// 			ImGui::End();
// 		}

// 		// Rendering
// 		ImGui::Render();
// 		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
// 		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

// 		// OpenGL
// 		// GLuint vertex_array_id;

// 		// glGenVertexArrays(1, &vertex_array_id);
// 		// glBindVertexArray(vertex_array_id);

// 		// static const GLfloat g_vertex_buffer_data[] = {
// 		// 	-1.0f,
// 		// 	-1.0f,
// 		// 	0.0f,
// 		// 	1.0f,
// 		// 	-1.0f,
// 		// 	0.0f,
// 		// 	0.0f,
// 		// 	1.0f,
// 		// 	0.0f,
// 		// };

// 		// GLuint vertexbuffer;
// 		// glGenBuffers(1, &vertexbuffer);
// 		// glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
// 		// glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
// 		// OpenGL end

// 		glClear(GL_COLOR_BUFFER_BIT);

// 		// OpenGL
// 		// glUseProgram(program_id);

// 		// // 1rst attribute buffer : vertices
// 		// glEnableVertexAttribArray(0);
// 		// glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
// 		// glVertexAttribPointer(
// 		// 	0,		  // attribute 0. No particular reason for 0, but must match the layout in the shader.
// 		// 	3,		  // size
// 		// 	GL_FLOAT, // type
// 		// 	GL_FALSE, // normalized?
// 		// 	0,		  // stride
// 		// 	(void *)0 // array buffer offset
// 		// );

// 		// // Draw the triangle !
// 		// glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

// 		// glDisableVertexAttribArray(0);
// 		// OpenGL end

// 		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
// 		SDL_GL_SwapWindow(window);
// 	}

// 	// Cleanup
// 	ImGui_ImplOpenGL3_Shutdown();
// 	ImGui_ImplSDL2_Shutdown();
// 	ImGui::DestroyContext();

// 	SDL_GL_DeleteContext(gl_context);
// 	SDL_DestroyWindow(window);
// 	SDL_Quit();

// 	return 0;
// }