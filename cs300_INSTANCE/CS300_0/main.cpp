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

#include "RenderableMeshObject.h"

static int     winID;
static GLsizei WIDTH = 1280;
static GLsizei HEIGHT = 720;
float PIValue = glm::atan(1) * 4;
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
    GLuint theProgram = ShaderUtils::CreateShaderProgram("Vertex.vert", "Fragment.frag");
    return theProgram;
}

void CleanUpObjectAndBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh)
{
    // Delete the VBOs
    glDeleteBuffers(1, &vbo);
    // Delete the VAO
    glDeleteVertexArrays(1, &vao);
    mesh.CleanupAndReset(); 
}
void ChangeSlices(GLuint& vbo, GLuint& vao, RenderableMeshObject & RenderableMesh, int new_slices, MeshType & t, GLuint & normal_vbo, GLuint & normal_vao, GLuint& average_normal_vbo, GLuint& average_normal_vao)
{
    Mesh& mesh = RenderableMesh.GetMesh();
    //reset the Mesh object and clean up the buffers
    CleanUpObjectAndBuffers(vbo, vao, mesh);
    CleanUpObjectAndBuffers(normal_vbo, normal_vao, mesh);
    CleanUpObjectAndBuffers(average_normal_vbo, average_normal_vao, mesh);
    //reconstruct mesh with new slices
    switch (t)
    {
    case MeshType::CYLINDER:
        mesh.ConstructCylinder(new_slices);
        break;
    case MeshType::CONE:
        mesh.ConstructCone(new_slices);
        break;
    case MeshType::SPHERE:
        mesh.ConstructSphere(new_slices);
        break;
    default:
        break;
    }
    
    RenderableMesh.Renderable_InitializeMeshBuffers(vbo, vao, mesh);
    mesh.GenerateNormalLines();
    mesh.GenerateAveragedNormalLines();
    RenderableMesh.Renderable_InitializeNormalBuffers(normal_vbo, normal_vao, mesh);
    RenderableMesh.Renderable_InitializeAveragedNormalBuffers(average_normal_vbo, average_normal_vao, mesh);
}

GLuint makeNormalMapTexture(const std::string& filename)
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
    GLuint shaderProgram = InitializeProgram();
   
    //TODO CHANGE TO GRASS
    //Make a normal map for the height maps
    GLuint mNormalMap = makeNormalMapTexture("./Textures/normal_map_flippedY.png");

    /*******************************************************************************************************************************************/
    //create matrices
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 20.0f, 20.0f));
    //model matrix
    glm::mat4 ModelMatrix = translationMatrix * rotationMatrix * scaleMatrix;//world space
    int current_slices = 20;
    //create objects to swap when pressing buttons
    RenderableMeshObject planeObject(MeshType::PLANE, current_slices, ModelMatrix);

    /*******************************************************************************************************************************************/
    //view matrix
    glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 ViewDirection = glm::vec3(0.0f, 0.0f, 0.0f) - glm::vec3(0.0f, 0.0f, 50.0f);
    ViewDirection = glm::normalize(ViewDirection);

    float CameraRadius = 50.f;  //radius of the camera from origin
    float CameraRadius_increment = 0.5;  //radius of the camera from origin
    float alpha_rad = 0.0f;     //angle of the camera on the xz plane, y axis
    float gamma_rad = 0.0f;   //angle of the camera from the top
    float alpha_increment = 0.05f;
    float gamma_increment = 0.05f;
    
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
                else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
                {
                    glm::mat4& mPlane = planeObject.GetModelRefference();
                    mPlane = glm::rotate(planeObject.GetModel(), glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));             
                    
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_UP)
                {
                    glm::mat4& mPlane = planeObject.GetModelRefference();
                    mPlane = glm::rotate(planeObject.GetModel(), glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
                {
                    glm::mat4& mPlane = planeObject.GetModelRefference();
                    mPlane = glm::rotate(planeObject.GetModel(), glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
                {
                    glm::mat4& mPlane = planeObject.GetModelRefference();
                    mPlane = glm::rotate(planeObject.GetModel(), glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
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
                break;
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

        //

        SDL_GL_SwapWindow(window);    
        
    }

    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}