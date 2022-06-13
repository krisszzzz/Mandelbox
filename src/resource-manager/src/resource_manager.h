/** @file resource_manager.h
 *  @brief something
 * 
 */

#ifndef SHADER_HANDLER_H_
#define SHADER_HANDLER_H_

#include <stdarg.h>
#include <glad/glad.h>
#include <murmurhash.h>
#include <stdio.h>

const int MAX_SHADER_TYPES = 6;

/** Structure to simplify working with shaders. */

struct shader
{
        char*     shader_path; /**< path to the shader source file */
        hash_t    shader_hash; /**< hash of current shader. Used to search shader
                                *   in shader_program.  
                                */
        GLenum    shader_type; /**< One of this shader type: GL_VERTEX_SHADER,
                                *   GL_FRAGMENT_SHADER, GL_GEOMENTRY_SHADER,
                                *   GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER.
                                *   For more see GLFW documentation.
                                */
        GLuint    shader_id;   /**< shader_id is return value of glCreateShader() function.
                                *   For more see GLFW documentation.
                                */

        long long status;      /**< Flag used to check the shader status, used in printing debug info
                                */

};


/** Structure that contain array of shaders (shader structure objects), and info from GLFW functions */

struct shader_program
{
        shader      shaders[MAX_SHADER_TYPES]; /**< array of shader objects. Can be used to print debug info */
        hash_t      shader_prog_hash;          /**< hash of this shader program. Used to search shader_program
                                                *   in resource_manager.
                                                */
        GLuint      shader_prog_id;            /**<  return ovalue of glCreateProgram().
                                                *    For more see GLFW documentation.
                                                */
        long long   status;                    /**<  Flag for debug info*/
        
};

/** Structure to control your resource such as shaders. */

struct resource_manager
{
        char*             work_dir;  /**< Save current work directory. This is the path where all resource
                                      *   will be checked. Note that in function make_shader_prog() all
                                      *   pathes are relative.
                                      */
        char*             shader_names_chunk;  /**< save all relative path to shaders. Used to print debug
                                                *   info.
                                                */
        int               shader_names_offset; /**< offset shader_names_chunk buffer. Used to save relative
                                                *   path of shaders.
                                                */
        int               shader_names_size;  /**<  used to control shader_name_chunk size */

        shader_program*   programs;

        int               shader_program_size;
        int               shader_program_count;

        long long         status;

        int               is_binded;
        int               is_initialized;
};

enum RESOURCE_MANAGER_ERRORS
{
        RME_NULLPTR = -0xDED,
        RME_NO_BIND,
        RME_INCCORECT_MANAGER,
        RME_MALLOC_ERROR,
};

enum SHADER_STATUS
{
// GLOBAL STATUS
        SHADERS_DESTROYED   = -1,
        SHADERS_NO_STATUS   = 0,
//        SHADERS_INITIALIZED = 0,
        SHADERS_COMPILED,
        SHADERS_LINKED,

        SHADERS_COMPILATION_FILE_LOAD_ERROR,
        SHADERS_COMPILATION_SYNTAX_ERROR,

        SHADERS_LINKING_COMPILE_ERROR,
        SHADERS_LINKING_GL_ATTACH_ERROR,

// BIN_SHADERS LINKING ERRORS
};

enum SHADER_PROGRAM_STATUS
{
        SHADER_PROG_DESTROYED = -1,
        SHADER_PROG_NO_STATUS,
        SHADER_PROG_BUILDED,
        SHADER_PROG_BUILDING_ERROR,
        SHADER_PROG_GL_VALIDATE_ERROR,
        SHADER_PROG_LOG_MALLOC_ERROR
};

enum RESOURCE_MANAGER_STATUS
{
        RES_MAN_NO_STATUS,
        RES_MAN_INITIALIZED,
        RES_MAN_BINDED,
        RES_MAN_SHADER_PROG_MALLOC_ERROR,
};

extern shader_program* NOT_FOUNDED;


enum RET_VAL_ERRORS
{
        NULLPTR = -0xEBAFF,
        ERROR_RET
};

char*   file_to_buffer(FILE* source, int* buffer_size);
char*   load_file_source(const char *const src_file_path);
//int     validate_shader(shader* curr_shader, const char* shader_path);

int     init_resource_manager(resource_manager* res_manager, const char* exec_path);
int     bind_resource_manager(resource_manager res_manager);

int     make_shader_prog_(const char* prog_name, int binary_count, ...);
int     shader_prog_log(shader_program* prog);
int     destroy_programs();

int     destroy_resource_manager();
int     resource_manager_log();

int     create_shader_prog(const char* const shader_prog_name,     const char* const vert_s = nullptr,
                           const char* const frag_s     = nullptr, const char* const geom_s = nullptr,
                           const char* const tess_ctl_s = nullptr, const char* const tess_eval_s = nullptr,
                           const char* const comp_s     = nullptr);
int    resource_manager_shader_log();

GLuint          get_shader_prog_id(const char* prog_name);

shader_program* find_shader_prog(const char* prog_name);


#endif // SHADER_HANDLER_H_
