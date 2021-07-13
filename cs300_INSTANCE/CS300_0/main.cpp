#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GL/GL.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "OGLDebug.h"
#include "ShaderUtils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "InstanceMesh.h"

static int     winID;
static GLsizei WIDTH = 1280;
static GLsizei HEIGHT = 720;
float PIValue = glm::atan(1) * 4;
//declare vertex as a struct
glm::mat4 RandomTranslate(int x_area_width, int z_area_height, glm::mat4 inputMat)
{
    int x_offset = rand() % 100;
    x_offset -= (x_area_width / 2);
    int z_offset = rand() % 100;
    z_offset -= (z_area_height / 2);
    glm::mat4 resultMat = glm::translate(glm::mat4(1.0f), glm::vec3(x_offset, 0.0f, z_offset));
    return resultMat;
}
GLuint CreateShader(GLenum eShaderType, const std::string& strShaderFile)
{
    GLuint       shader = glCreateShader(eShaderType);
    const char* strFileData = strShaderFile.c_str();
    glShaderSource(shader, 1, &strFileData, NULL);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        auto dummyvar = infoLogLength + 1;
        GLchar* strInfoLog = new GLchar[dummyvar];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

        const char* strShaderType = NULL;
        switch (eShaderType)
        {
        case GL_VERTEX_SHADER:
            strShaderType = "vertex";
            break;
        case GL_GEOMETRY_SHADER:
            strShaderType = "geometry";
            break;
        case GL_FRAGMENT_SHADER:
            strShaderType = "fragment";
            break;
        }

        fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
        delete[] strInfoLog;
    }

    return shader;
}
GLuint CreateProgram(const std::vector<GLuint>& shaderList)
{
    GLuint program = glCreateProgram();

    for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
        glAttachShader(program, shaderList[iLoop]);

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        auto dummyvar = infoLogLength + 1;
        GLchar* strInfoLog = new GLchar[dummyvar];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }

    for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
        glDetachShader(program, shaderList[iLoop]);

    return program;
}
GLuint InitializeProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("Instance.vert", "Instance.frag");
    return theProgram;
}
GLuint makeTextureFromFile(const std::string& filename)
{
    //load texture from filename
    SDL_Surface* normalMap = IMG_Load(filename.c_str());
    if (normalMap == nullptr)
    {
        std::cout << "Could not load texture: " + filename << std::endl;
        return 0;
    }
    GLuint texture;
    // Create texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Select pixel format from surface
    int pixelFormat = GL_RGB;
    if (normalMap->format->BytesPerPixel == 4)
    {
        pixelFormat = GL_RGBA;
    }
    // Give pixel data to opengl
    glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, normalMap->w, normalMap->h, 0, pixelFormat, GL_UNSIGNED_BYTE, normalMap->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Free surface because OpenGL already has the data
    SDL_FreeSurface(normalMap);

    return texture;
}
void displayInstancedMesh(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& shader, GLuint& texture, GLuint& VAO, InstanceMesh mesh, unsigned int qCount)
{
    ////////////////////////////////////////////////////////////////////////////////
    // Bind the glsl program and this object's VAO
    glUseProgram(shader);
    GLint view = glGetUniformLocation(shader, "u_V");
    glUniformMatrix4fv(view, 1, GL_FALSE, &(ViewMatrix[0][0]));
    GLint projection = glGetUniformLocation(shader, "u_P");
    glUniformMatrix4fv(projection, 1, GL_FALSE, &(ProjectionMatrix[0][0]));

    //texture stuff
    glActiveTexture(GL_TEXTURE0); //activate bucket 0
    glBindTexture(GL_TEXTURE_2D, texture);  //fill bucket 0
    GLuint loc = glGetUniformLocation(shader, "texture_data");   //get uniform of frag shader
    glUniform1i(loc, 0);    //use stuff from bucket 0

    glBindVertexArray(VAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.GetVertexNum(), qCount);
}
#undef main
int main(int argc, char* args[])
{
    //init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }
    //Create THe window witth the parameters
    SDL_Window* window = SDL_CreateWindow("CS300", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
    if (window == nullptr)
    {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
    //ASK ABOUT THIS
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //CREATE the Context, ask about this
    SDL_GLContext context_ = SDL_GL_CreateContext(window);
    if (context_ == nullptr)
    {
        SDL_DestroyWindow(window);
        std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        SDL_GL_DeleteContext(context_);
        SDL_DestroyWindow(window);
        std::cout << "GLEW Error: Failed to init" << std::endl;
        SDL_Quit();
        exit(1);
    }

#if _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif
    // print GPU data
    std::cout << "GL_VENDOR: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "GL_RENDERER: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;
    
    glEnable(GL_DEPTH_TEST);
    /*******************************************************************************************************************************************/
    GLuint shaderProgram = InitializeProgram();
    GLuint mGrassTexture = makeTextureFromFile("./Textures/grass.png");
    int AreaWidth = 100;    //x
    int AreaHeight = 100;   //z
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    //Base model matrix before making it all random
    glm::mat4 ModelMatrixBase = scaleMatrix;//world space
    //create 50 initial positions from the base model matrix in the area
    unsigned int quad_count = 1000;
    std::vector<glm::mat4> quad_positions;
    for (int i = 0; i < quad_count; i++)
    {
        glm::mat4 r = RandomTranslate(AreaWidth, AreaHeight, ModelMatrixBase);
        quad_positions.push_back(r);
    }
    /*******************************************************************************************************************************************/
    unsigned long Vertex_Stride = sizeof(Vertex);
    InstanceMesh GrassQuads = InstanceMesh();
    GLuint GrassVAO;    // create buffer for VAO
    glGenVertexArrays(1, &GrassVAO);
    GLuint GrassQuadVBO;
    glGenBuffers(1, &GrassQuadVBO);
    GLuint GrassInstanceMatricesVBO;
    glGenBuffers(1, &GrassInstanceMatricesVBO);

    glBindVertexArray(GrassVAO);                 //bind so we are now doing stuff to the vao
    glBindBuffer(GL_ARRAY_BUFFER, GrassQuadVBO); // bind so we are now doing stuff to the vao of the grass quad with its positions and UVs
    glBufferData(GL_ARRAY_BUFFER, (sizeof(Vertex) * GrassQuads.GetVertexNum()), GrassQuads.GetVertices(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);               //positions
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, Vertex_Stride, 0);  
    glEnableVertexAttribArray(1);               //normals
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, Vertex_Stride, reinterpret_cast<void*>(offsetof(Vertex, normal))); 
    glEnableVertexAttribArray(2);               //uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, Vertex_Stride, reinterpret_cast<void*>(offsetof(Vertex, UV)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(GrassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, GrassInstanceMatricesVBO);
    glBufferData(GL_ARRAY_BUFFER, quad_count * sizeof(glm::mat4), &quad_positions[0], GL_STATIC_DRAW);
    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    /*******************************************************************************************************************************************/

    //view matrix
    glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 ViewDirection = glm::vec3(0.0f, 0.0f, 0.0f) - glm::vec3(0.0f, 0.0f, 50.0f);
    ViewDirection = glm::normalize(ViewDirection);

    float CameraRadius = 50.f;  //radius of the camera from origin
    float CameraRadius_increment = 0.5;  //radius of the camera from origin
    float alpha_rad = 0.0f;     //angle of the camera on the xz plane, y axis
    float gamma_rad = 0.0f;   //angle of the camera from the top
    float alpha_increment = 0.01f;
    float gamma_increment = 0.01f;
    
    float cam_x = CameraRadius * glm::cos(gamma_rad) * glm::sin(alpha_rad);
    float cam_y = CameraRadius * glm::sin(gamma_rad);
    float cam_z = CameraRadius * glm::cos(gamma_rad) * glm::cos(alpha_rad);
    glm::vec3 cam_pos(cam_x, cam_y, cam_z);
    //projection matrix
    float aspect = (float)WIDTH / HEIGHT;
    glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 150.0f);
    /*******************************************************************************************************************************************/

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    SDL_Event event;
    bool      quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                {
                    quit = true;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_W)
                {
                    gamma_rad += gamma_increment;
                    if (gamma_rad > (PIValue / 2.0f))
                    {
                        gamma_rad = (PIValue / 2.0f);
                    }
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_S)
                {
                    gamma_rad -= gamma_increment;
                    if (gamma_rad < -(PIValue / 2.0f))
                    {
                        gamma_rad = -(PIValue / 2.0f);
                    }
                    glm::clamp(gamma_rad, -(PIValue / 2.0f), (PIValue / 2.0f));  
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_A)
                {
                    alpha_rad -= alpha_increment;
                   
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_D)
                {
                    alpha_rad += alpha_increment;
                    
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_Q)
                {
                    //closer
                    CameraRadius -= CameraRadius_increment;
                    if (CameraRadius < 0.1f)
                    {
                        CameraRadius = 0.1f;
                    }
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_E)
                {
                    //further
                    CameraRadius += CameraRadius_increment;
                    if (CameraRadius > 100.0f)
                    {
                        CameraRadius = 100.0f;
                    }
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_KP_PLUS)
                {
                    //further
                    quad_count += 1;
                    glm::mat4 r = RandomTranslate(AreaWidth, AreaHeight, ModelMatrixBase);
                    quad_positions.push_back(r);
                    glBindVertexArray(GrassVAO);
                    glBindBuffer(GL_ARRAY_BUFFER, GrassInstanceMatricesVBO);
                    glBufferData(GL_ARRAY_BUFFER, quad_count * sizeof(glm::mat4), &quad_positions[0], GL_STATIC_DRAW);
                    std::size_t vec4Size = sizeof(glm::vec4);
                    glEnableVertexAttribArray(3);
                    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
                    glEnableVertexAttribArray(4);
                    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
                    glEnableVertexAttribArray(5);
                    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
                    glEnableVertexAttribArray(6);
                    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

                    glVertexAttribDivisor(3, 1);
                    glVertexAttribDivisor(4, 1);
                    glVertexAttribDivisor(5, 1);
                    glVertexAttribDivisor(6, 1);

                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_KP_MINUS)
                {
                    //further
                    quad_count -= 1;
                    if (quad_count < 0)
                    {
                        quad_count = 0;
                    }
                    else
                    {
                        quad_positions.pop_back();
                        glBindVertexArray(GrassVAO);
                        glBindBuffer(GL_ARRAY_BUFFER, GrassInstanceMatricesVBO);
                        glBufferData(GL_ARRAY_BUFFER, quad_count * sizeof(glm::mat4), &quad_positions[0], GL_STATIC_DRAW);
                        std::size_t vec4Size = sizeof(glm::vec4);
                        glEnableVertexAttribArray(3);
                        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
                        glEnableVertexAttribArray(4);
                        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
                        glEnableVertexAttribArray(5);
                        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
                        glEnableVertexAttribArray(6);
                        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

                        glVertexAttribDivisor(3, 1);
                        glVertexAttribDivisor(4, 1);
                        glVertexAttribDivisor(5, 1);
                        glVertexAttribDivisor(6, 1);

                        glBindBuffer(GL_ARRAY_BUFFER, 0);
                        glBindVertexArray(0);
                    }
                }
                break;

                //SDL_SCANCODE_KP_PLUS
            }
        }
        ////////////////////////////////////////////////////////////////////////////////
        //update camera
        cam_x = CameraRadius * glm::cos(gamma_rad) * glm::sin(alpha_rad);
        cam_y = CameraRadius * glm::sin(gamma_rad);
        cam_z = CameraRadius * glm::cos(gamma_rad) * glm::cos(alpha_rad);
        glm::vec3 cam_pos(cam_x, cam_y, cam_z);
        glm::mat4 ViewMatrix2 = glm::lookAt(cam_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ViewMatrix = ViewMatrix2;
        ////////////////////////////////////////////////////////////////////////////////
        //change shader program to receive matrices as inputs
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ////////////////////////////////////////////////////////////////////////////////       
        //render instance
        displayInstancedMesh(ViewMatrix, ProjectionMatrix, shaderProgram, mGrassTexture, GrassVAO, GrassQuads, quad_count);
        SDL_GL_SwapWindow(window);         
    }

    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}