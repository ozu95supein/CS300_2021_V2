#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GL/GL.h>
#include <SDL2/SDL.h>

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
GLuint InitializeNormalProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("NormalVertex.vert", "NormalFragment.frag");
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
GLuint& makeTexture(GLuint& t)
{
    //make the colored texture array?
    glm::vec4 blue(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 cyan(0.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 green(0.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 yellow(1.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 red(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 purple(1.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 textureColorArray[] =
    {
        blue, cyan, green, yellow, red, purple,
        cyan, green, yellow, red, purple, blue,
        green, yellow, red, purple, blue, cyan,
        yellow, red, purple, blue, cyan, green,
        red, purple, blue, cyan, green, yellow,
        purple, blue, cyan, green, yellow, red
    };

    // Create texture
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);

    int pixelFormat = GL_RGBA;

    // Give pixel data to opengl
    glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, 6, 6, 0, pixelFormat, GL_FLOAT, textureColorArray);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return t;
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

    GLuint texture = makeTexture(texture);
    GLuint shaderProgram = InitializeProgram();
    GLuint NormalshaderProgram = InitializeNormalProgram();

    /*******************************************************************************************************************************************/

    //create matrices
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
    //model matrix
    glm::mat4 ModelMatrix = translationMatrix * rotationMatrix * scaleMatrix;//world space
    int current_slices = 6;
    //create objects to swap when pressing buttons
    RenderableMeshObject planeObject(MeshType::PLANE, current_slices, ModelMatrix);
    RenderableMeshObject cubeObject(MeshType::CUBE, current_slices, ModelMatrix);
    RenderableMeshObject cylinderObject(MeshType::CYLINDER, current_slices, ModelMatrix);
    RenderableMeshObject coneObject(MeshType::CONE, current_slices, ModelMatrix);
    RenderableMeshObject sphereObject(MeshType::SPHERE, current_slices, ModelMatrix);
    //GROUND
    glm::mat4 GROUND_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -15.0f, 0.0f));
    glm::mat4 GROUND_rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 GROUND_scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 50.0f, 50.0f));
    //model matrix
    glm::mat4 GROUND_ModelMatrix = GROUND_translationMatrix * GROUND_rotationMatrix * GROUND_scaleMatrix;//world space
    GROUND_ModelMatrix = glm::rotate(GROUND_ModelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    RenderableMeshObject GROUND_planeObject(MeshType::PLANE, current_slices, GROUND_ModelMatrix);
    //LIGHTS
    float light_radius = 20.0f;
    float light_XY_Angle_Rad= 0.0f;
    float light_XY_Angle_Rad_increment = 0.0001f;
    float light_x = light_radius * glm::cos(light_XY_Angle_Rad);
    float light_z = light_radius * glm::sin(light_XY_Angle_Rad);

    glm::mat4 LIGHT_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(light_radius, 0.0f, 0.0f));
    glm::mat4 LIGHT_rotationMatrix = glm::rotate(glm::mat4(1.0f), light_XY_Angle_Rad, glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 LIGHT_scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    //model matrix
    glm::mat4 LIGHT_ModelMatrix = LIGHT_translationMatrix * LIGHT_rotationMatrix * LIGHT_scaleMatrix;//world space
    RenderableMeshObject LIGHT_sphereObject(MeshType::SPHERE, current_slices, LIGHT_ModelMatrix);

    /*******************************************************************************************************************************************/

    //view matrix
    glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //projection matrix
    float aspect = (float)WIDTH / HEIGHT;
    glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

    /*==========================================================================================================================================*/
    //HARD CODED for now
    glm::vec3 main_material_ambient(0.5);
    glm::vec3 main_light_ambient(1.0f);
    /*==========================================================================================================================================*/



    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    //1 = plane, 1: Plane, 2: Cube, 3 : Cone,  4 : Cylinder, 5 : Sphere       
    int current_mesh_to_display = 1;
    int Display_Normals = 0;
    bool Display_Wireframe = false;
    int ColoredBoxTextureOn = 1;
    int UsingFaceNormals = 1;
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
                    glm::mat4& mCube = cubeObject.GetModelRefference();
                    mCube = glm::rotate(cubeObject.GetModel(), glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mCylinder = cylinderObject.GetModelRefference();
                    mCylinder = glm::rotate(cylinderObject.GetModel(), glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mCone = coneObject.GetModelRefference();
                    mCone = glm::rotate(coneObject.GetModel(), glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mSphere = sphereObject.GetModelRefference();
                    mSphere = glm::rotate(sphereObject.GetModel(), glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_UP)
                {
                    glm::mat4& mPlane = planeObject.GetModelRefference();
                    mPlane = glm::rotate(planeObject.GetModel(), glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mCube = cubeObject.GetModelRefference();
                    mCube = glm::rotate(cubeObject.GetModel(), glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mCylinder = cylinderObject.GetModelRefference();
                    mCylinder = glm::rotate(cylinderObject.GetModel(), glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mCone = coneObject.GetModelRefference();
                    mCone = glm::rotate(coneObject.GetModel(), glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mSphere = sphereObject.GetModelRefference();
                    mSphere = glm::rotate(sphereObject.GetModel(), glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
                {
                    glm::mat4& mPlane = planeObject.GetModelRefference();
                    mPlane = glm::rotate(planeObject.GetModel(), glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mCube = cubeObject.GetModelRefference();
                    mCube = glm::rotate(cubeObject.GetModel(), glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mCylinder = cylinderObject.GetModelRefference();
                    mCylinder = glm::rotate(cylinderObject.GetModel(), glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mCone = coneObject.GetModelRefference();
                    mCone = glm::rotate(coneObject.GetModel(), glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mSphere = sphereObject.GetModelRefference();
                    mSphere = glm::rotate(sphereObject.GetModel(), glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
                {
                    glm::mat4& mPlane = planeObject.GetModelRefference();
                    mPlane = glm::rotate(planeObject.GetModel(), glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mCube = cubeObject.GetModelRefference();
                    mCube = glm::rotate(cubeObject.GetModel(), glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mCylinder = cylinderObject.GetModelRefference();
                    mCylinder = glm::rotate(cylinderObject.GetModel(), glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mCone = coneObject.GetModelRefference();
                    mCone = glm::rotate(coneObject.GetModel(), glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mSphere = sphereObject.GetModelRefference();
                    mSphere = glm::rotate(sphereObject.GetModel(), glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_1)
                {
                    current_mesh_to_display = 1;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_2)
                {
                    current_mesh_to_display = 2;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_3)
                {
                    current_mesh_to_display = 3;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_4)
                {
                    current_mesh_to_display = 4;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_5)
                {
                    current_mesh_to_display = 5;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_F)
                {
                    if (UsingFaceNormals == 1)
                    {
                        UsingFaceNormals = 0;
                    }
                    else
                    {
                        UsingFaceNormals = 1;
                    }
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_M)
                {
                    Display_Wireframe = !Display_Wireframe;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_N)
                {
                    if (Display_Normals == 1)
                    {
                        Display_Normals = 0;
                    }
                    else
                    {
                        Display_Normals = 1;
                    }
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_KP_PLUS)
                {
                    current_slices += 1;
                    MeshType cy = MeshType::CYLINDER;
                    MeshType cn = MeshType::CONE;
                    MeshType sp = MeshType::SPHERE;
                    cylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    coneObject.Renderable_ChangeSlices(current_slices, cn);
                    sphereObject.Renderable_ChangeSlices(current_slices, sp);
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_KP_MINUS)
                {
                    if (current_slices == 4)
                    {
                        break;
                    }
                    current_slices -= 1;
                    MeshType cy = MeshType::CYLINDER;
                    MeshType cn = MeshType::CONE;
                    MeshType sp = MeshType::SPHERE;
                    cylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    coneObject.Renderable_ChangeSlices(current_slices, cn);
                    sphereObject.Renderable_ChangeSlices(current_slices, sp);
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_T)
                {
                    if (ColoredBoxTextureOn == 1)
                    {
                        ColoredBoxTextureOn = 0;
                    }
                    else
                    {
                        ColoredBoxTextureOn = 1;
                    }
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_P)
                {
                //Debug
                    //move the lightsource
                    //rotate on xz plane
                    //translate back to origin,
                    light_XY_Angle_Rad += light_XY_Angle_Rad_increment;
                    float x = light_radius * glm::cos(light_XY_Angle_Rad);
                    float z = light_radius * glm::sin(light_XY_Angle_Rad);
                    glm::vec3 t(x, 0.0f, z);
                    glm::mat4 Light_translationMatrix = glm::translate(glm::mat4(1.0f), t);
                    LIGHT_ModelMatrix = LIGHT_ModelMatrix * Light_translationMatrix;
                    LIGHT_sphereObject.SetModel(LIGHT_ModelMatrix);
                }
                break;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////////////
        //change shader program to receive matrices as inputs
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        switch (current_mesh_to_display)
        {
            case 1:
            {
                planeObject.Renderable_displayMesh(ViewMatrix,ProjectionMatrix, shaderProgram, texture, Display_Wireframe, ColoredBoxTextureOn, main_light_ambient, main_material_ambient);
                if (Display_Normals)
                {
                    if (UsingFaceNormals)
                    {
                        planeObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, NormalshaderProgram);
                    }
                    else
                    {
                        planeObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, NormalshaderProgram);
                    }
                }
            }
            break;
            case 2:
            {
                cubeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, shaderProgram, texture, Display_Wireframe, ColoredBoxTextureOn, main_light_ambient, main_material_ambient);
                if (Display_Normals)
                {
                    if (UsingFaceNormals)
                    {
                        cubeObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, NormalshaderProgram);
                    }
                    else
                    {
                        cubeObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, NormalshaderProgram);
                    }
                }
            }
            break;
            case 3:
            {
                cylinderObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, shaderProgram, texture, Display_Wireframe, ColoredBoxTextureOn, main_light_ambient, main_material_ambient);
                if (Display_Normals)
                {
                    if (UsingFaceNormals)
                    {
                        cylinderObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, NormalshaderProgram);
                    }
                    else
                    {
                        cylinderObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, NormalshaderProgram);
                    }
                }
            }
            break;
            case 4:
            {
                coneObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, shaderProgram, texture, Display_Wireframe, ColoredBoxTextureOn, main_light_ambient, main_material_ambient);
                if (Display_Normals)
                {
                    if (UsingFaceNormals)
                    {
                        coneObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, NormalshaderProgram);
                    }
                    else
                    {
                        coneObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, NormalshaderProgram);
                    }
                }
            }
            break;
            case 5:
            {
                sphereObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, shaderProgram, texture, Display_Wireframe, ColoredBoxTextureOn, main_light_ambient, main_material_ambient);
                if (Display_Normals)
                {
                    if (UsingFaceNormals)
                    {
                        sphereObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, NormalshaderProgram);
                    }
                    else
                    {
                        sphereObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, NormalshaderProgram);
                    }
                }
            }
            break;
        }
        GROUND_planeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, shaderProgram, texture, Display_Wireframe, ColoredBoxTextureOn, main_light_ambient, main_material_ambient);
        LIGHT_sphereObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, NormalshaderProgram, texture, Display_Wireframe, ColoredBoxTextureOn, main_light_ambient, main_material_ambient);
        SDL_GL_SwapWindow(window);    
        
    }

    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}