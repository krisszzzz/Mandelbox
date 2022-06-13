#include <WinMain.h>

int main(int argc, char** argv)
{
        GLFWwindow*       window    = make_fullscreen_window();
        const char*       prog_name = "prog1";
        GLuint            shad_prog = 0;

        make_shad_prog_n_res_man("/mnt/Asm/OpenGL-shaders/mandelbox/res/Shaders", prog_name);

        GLuint vao = get_shader_prog_id(prog_name);
        make_ultra_shader(&shad_prog, &vao, prog_name);
        WinMain(window, shad_prog, vao);


        destroy_resource_manager();
        glfwTerminate();

        return 0;
}
