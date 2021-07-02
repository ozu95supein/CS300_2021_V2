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
#include "LightSourceObject.h"

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
GLuint InitializeWhiteProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("White.vert", "White.frag");
    return theProgram;
}
GLuint InitializeRedProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("White.vert", "red.frag");
    return theProgram;
}
GLuint InitializeGreenProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("White.vert", "green.frag");
    return theProgram;
}
GLuint InitializeBlueProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("White.vert", "blue.frag");
    return theProgram;
}
GLuint InitializeNormalProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("NormalVertex.vert", "NormalFragment.frag");
    return theProgram;
}
GLuint InitializeSingleColorProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("White.vert", "SingleColor.frag");
    return theProgram;
}
GLuint InitializeBasicColorProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("BasicTextureShader.vert", "BasicTextureShader.frag");
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
    GLuint texture = makeTexture(texture);
    GLuint shaderProgram = InitializeProgram();
    GLuint NormalshaderProgram = InitializeNormalProgram();
    GLuint WhiteShaderProgram = InitializeWhiteProgram();
    GLuint SingleColorShaderProgram = InitializeSingleColorProgram();

    GLuint RedShaderProgram = InitializeRedProgram();
    GLuint GreenShaderProgram = InitializeGreenProgram();
    GLuint BlueShaderProgram = InitializeBlueProgram();

    GLuint BasicColorShaderProgram = InitializeBasicColorProgram();

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
    RenderableMeshObject cubeObject(MeshType::CUBE, current_slices, ModelMatrix);
    RenderableMeshObject cylinderObject(MeshType::CYLINDER, current_slices, ModelMatrix);
    RenderableMeshObject coneObject(MeshType::CONE, current_slices, ModelMatrix);
    RenderableMeshObject sphereObject(MeshType::SPHERE, current_slices, ModelMatrix);
    //GROUND
    glm::mat4 GROUND_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -15.0f, 0.0f));
    glm::mat4 GROUND_rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 GROUND_scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 1.0f, 50.0f));
    //model matrix
    glm::mat4 GROUND_ModelMatrix = GROUND_translationMatrix * GROUND_rotationMatrix * GROUND_scaleMatrix;//world space
    GROUND_ModelMatrix = glm::rotate(GROUND_ModelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    /*******************************************************************************************************************************************/
    RenderableMeshObject GROUND_planeObject(MeshType::PLANE, current_slices, GROUND_ModelMatrix);
    //LIGHTS
    float light_radius = 20.0f;
    float light_Theta_Angle_Rad = 0.0f;
    float light_Phi_Angle_Rad = 0.0f;
    float light_Phi_increment = 0.001f;
    float light_Amplitude = 14.0f;
    float light_Theta_increment = 0.0007f;  //MAGIC NUMBER
    float light_x = light_radius * glm::cos(light_Theta_Angle_Rad);
    float light_y = 0.0f;
    float light_z = light_radius * glm::sin(light_Theta_Angle_Rad);
    
    glm::mat4 LIGHT_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(light_x, light_y, light_z));
    glm::mat4 LIGHT_scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    //model matrix
    glm::mat4 LIGHT_ModelMatrix = LIGHT_translationMatrix * LIGHT_scaleMatrix;//world space
    /*==========================================================================================================================================*/
    //HARD CODED for now
    //AMBIENT
    glm::vec3 main_material_ambient(1.0f);
    glm::vec3 main_light_ambient(0.0f);
    //DIFFUSE
    glm::vec3 main_material_diffuse(1.0f);
    glm::vec3 main_light_diffuse(0.5f);
    glm::vec4 light_position = glm::vec4(glm::vec3(light_x, light_y, light_z), 1.0f);    //MAKE IT THE SAME AS THE SPHERE OBJECT FOR NOW
    //SPECULAR
    glm::vec3 main_light_specular = glm::vec3(1.0f);
    glm::vec3 main_material_specular = glm::vec3(1.0f);
    //ATTENUATION
    glm::vec3 main_light_attenuation = glm::vec3(0.0f, 0.0f, 0.005f);

    float main_ns = 10.0f;
    Light mLight;
    mLight.light_ambient = main_light_ambient;
    mLight.light_diffuse = main_light_diffuse;
    mLight.light_position = light_position;
    mLight.light_direction = glm::vec4(glm::vec3(0.0f), 1.0f) - mLight.light_position;
    mLight.light_specular = main_light_specular;
    mLight.light_attenuation = main_light_attenuation;
    mLight.light_type = 0;
    mLight.inner = 30.0f;
    mLight.outer = 35.0f;
    mLight.falloff = 1.0f;

    Material mMaterial;
    mMaterial.material_ambient = main_material_ambient;
    mMaterial.material_diffuse = main_light_diffuse;
    mMaterial.material_specular = main_material_specular;
    mMaterial.ns = main_ns;
    /*=========================================================================================================================================*/
    //MAKE LIGHTSOURCE OBJECT
    

    /*=========================================================================================================================================*/

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


    planeObject.SetMaterial(mMaterial);
    cubeObject.SetMaterial(mMaterial);
    cylinderObject.SetMaterial(mMaterial);
    coneObject.SetMaterial(mMaterial);
    sphereObject.SetMaterial(mMaterial);
    GROUND_planeObject.SetMaterial(mMaterial);
    sphereObject.SetMaterial(mMaterial);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    //1 = plane, 1: Plane, 2: Cube, 3 : Cone,  4 : Cylinder, 5 : Sphere       
    int current_mesh_to_display = 1;
    int Display_Normals = 0;
    bool Display_Wireframe = false;
    int RenderMode = 0;
    int UsingFaceNormals = 1;
    int PlayingLightAnimation = 1;
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
                else if (event.key.keysym.scancode == SDL_SCANCODE_7)
                {
                    //TO DO
                    // Turn all lights to point lights
                    mLight.light_type = 0;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_8)
                {
                    //TO DO
                    // turn all lights to spot lights
                    mLight.light_type = 1;

                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_9)
                {
                    //TO DO
                    // turn all lights to directional lights
                    mLight.light_type = 2;

                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_P)
                {
                    //TO DO
                    // Toggle to pause/start the light animation
                    if (PlayingLightAnimation == 1)
                    {
                        PlayingLightAnimation = 0;
                    }
                    else
                    {
                        PlayingLightAnimation = 1;
                    }
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
                    RenderMode += 1;
                    if (RenderMode > 3)
                    {
                        RenderMode = 0;
                    }
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
        switch (current_mesh_to_display)
        {
            case 1:
            {
                planeObject.Renderable_displayMesh(ViewMatrix,ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                if (Display_Normals)
                {
                    if (UsingFaceNormals)
                    {
                        planeObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                        planeObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                        planeObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                    }
                    else
                    {
                        planeObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, SingleColorShaderProgram);
                        planeObject.Renderable_displayAveragedTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                        planeObject.Renderable_displayAveragedBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                    }
                }
            }
            break;
            case 2:
            {
                cubeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                if (Display_Normals)
                {
                    if (UsingFaceNormals)
                    {
                        cubeObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                        cubeObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                        cubeObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                    }
                    else
                    {
                        cubeObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                        cubeObject.Renderable_displayAveragedTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                        cubeObject.Renderable_displayAveragedBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                    }
                }
            }
            break;
            case 3:
            {
                cylinderObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                if (Display_Normals)
                {
                    if (UsingFaceNormals)
                    {
                        cylinderObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                        cylinderObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                        cylinderObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                    }
                    else
                    {
                        cylinderObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                        cylinderObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                        cylinderObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                    }
                }
            }
            break;
            case 4:
            {
                coneObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                if (Display_Normals)
                {
                    if (UsingFaceNormals)
                    {
                        coneObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                        coneObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                        coneObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                    }
                    else
                    {
                        coneObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                        coneObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                        coneObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                    }
                }
            }
            break;
            case 5:
            {
                sphereObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                if (Display_Normals)
                {
                    if (UsingFaceNormals)
                    {
                        sphereObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                        sphereObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                        sphereObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                    }
                    else
                    {
                        sphereObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                        sphereObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                        sphereObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                    }
                }
            }
            break;
        }
        GROUND_planeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
        if (Display_Normals)
        {
            if (UsingFaceNormals)
            {
                GROUND_planeObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                GROUND_planeObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                GROUND_planeObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
            }
            else
            {
                GROUND_planeObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                GROUND_planeObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                GROUND_planeObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
            }
        }
        SDL_GL_SwapWindow(window);    
        
    }

    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}