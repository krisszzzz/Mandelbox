#include <WinMain.h>

#define WINDOW_WIDTH 1920 
#define WINDOW_HEIGHT 1080

float x_pos    = 0.17963f;
float y_pos    = 0.099261f;
float z_pos    = -1.3678434f;
float scale    = -2.81f;

void glfw_key_callback(GLFWwindow* p_window, int key, int scancode, int action, int mode)
{
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(p_window, GLFW_TRUE);
        } else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
                z_pos -= 0.001f;
        } else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
                z_pos += 0.001f;
        } else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
                x_pos -= 0.001f;
        } else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
                x_pos += 0.001f;
        } else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
                y_pos -= 0.001f;
        } else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
                y_pos += 0.001f;
        } else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
                if (mode == GLFW_MOD_CONTROL) 
                    scale += 0.01;
                else 
                    scale -= 0.01;
        }
}

int make_ultra_shader(GLuint* p_shader_prog, GLuint* p_vao, const char* shader_prog_name)
{
        GLfloat point[] = {          // Vertex of triangle
                 -1.0f, -1.0f,
                 -1.0f,  1.0f,      // See OpenGL info abount normalized coordinates
                  1.0f, -1.0f,
                  1.0f,  1.0f
        };

        GLuint shader_program = get_shader_prog_id(shader_prog_name);

        int    buffer_count = 1;
        GLuint points_vbo   = 0;

        // Vertex buffer object
        glGenBuffers(buffer_count, &points_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);

        // Make buffer to contain points in videomemory
        // Vertex array object
        GLuint vao = 0;

        glGenVertexArrays(buffer_count, &vao); // Create VAO
        glBindVertexArray(vao);    // Bind VAO
//        printf("Color = %u, Point = %u, VAO = %u, source = %s\n", colors_vbo, points_vbo, vao, vs_buffer);

        int vertex_position = 0;
        int vertex_count    = 2;
        int stride          = 0;

        glEnableVertexAttribArray(vertex_position); // layout(location = 0); See vertex_shader.txt
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo); // Vertex proccessing

        glVertexAttribPointer(vertex_position, vertex_count, GL_FLOAT,
                              GL_FALSE,        stride,       nullptr);

        *p_shader_prog = shader_program;
        *p_vao         = vao;

        return 0;
}

GLFWwindow* make_fullscreen_window()
{
    /* Initialize the library */
        if (!glfwInit()) {
                ErrorPrint("GLFW library initializitation error ocurred\n");
                return nullptr;
        }

    // Required minimum 4.6 OpenGL
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

   /* Create a windowed mode window and its OpenGL context */
        GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH,
                                              WINDOW_HEIGHT,
                                              "Hello World",
                                              NULL,
                                              NULL);

        if (!window) {
                ErrorPrint("Window creating error\n");
                glfwTerminate();
                return nullptr;
        }

        glfwSetKeyCallback(window, glfw_key_callback);
   /* Make the window's context current */
        glfwMakeContextCurrent(window);

        if (!gladLoadGL()) {
                ErrorPrint("Fatal error: GLAD cannot be loaded\n");
                return nullptr;
        }

        printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
        glClearColor(0, 0, 0, 1);

        return window;

}

int make_shad_prog_n_res_man(const char *const work_dir, const char *const shader_prog_name)
{
        resource_manager res_man = {};
        init_resource_manager(&res_man, work_dir);
        bind_resource_manager(res_man);

        create_shader_prog(shader_prog_name, "vertex_shader.glsl",
                                             "fragment_shader.glsl");
        resource_manager_shader_log();

        glfwSetTime(1);

        return 0;

}

void WinMain(GLFWwindow* window, GLuint shader_program, GLuint vao)
{
        int vertex_indx_start = 0;
        int vertex_count      = 4;

        while (!glfwWindowShouldClose(window)) {
                /* Render here */
                glClear(GL_COLOR_BUFFER_BIT);

                GLint resolution = glGetUniformLocation(shader_program, "iResolution");
                GLint z_coord    = glGetUniformLocation(shader_program, "z_pos");
                GLint y_coord    = glGetUniformLocation(shader_program, "y_pos");
                GLint x_coord    = glGetUniformLocation(shader_program, "x_pos");
                GLint mouse      = glGetUniformLocation(shader_program, "iMouse");
                GLint gl_scale   = glGetUniformLocation(shader_program, "scale");

                glUniform2f(resolution, WINDOW_WIDTH, WINDOW_HEIGHT);
                glUniform1f(z_coord, z_pos);
                glUniform1f(y_coord, y_pos);
                glUniform1f(x_coord, x_pos);
                glUniform1f(gl_scale, scale);

                double x_pos = 0.0, y_pos = 0.0;
                glfwGetCursorPos(window, &x_pos, &y_pos);
                glUniform2f(mouse, x_pos, y_pos);

                glUseProgram(shader_program);  // Use shader program that was
                                               //
//                glBindVertexArray(vao);        // generated bu glfw_make_triangle
                //                                // Bind current VAO
                glDrawArrays(GL_TRIANGLE_STRIP, vertex_indx_start,
                             vertex_count);           // Draw current VAO
        /* Swap front and back buffers */
                glfwSwapBuffers(window);
                /* Poll for and process events */
                glfwPollEvents();
        }

}
