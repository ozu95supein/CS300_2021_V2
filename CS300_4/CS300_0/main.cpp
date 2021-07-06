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
GLuint InitializeCubeMapProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("CubeMap.vert", "CubeMap.frag");
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

std::vector<std::string> CotonSkyboxFaces
{
    "./Textures/CottonCandy_Right.png",
    "./Textures/CottonCandy_Left.png",
    "./Textures/CottonCandy_Top.png",
    "./Textures/CottonCandy_Bottom.png",
    "./Textures/CottonCandy_Front.png",
    "./Textures/CottonCandy_Back.png"
};
std::vector<std::string> CubeMapSkyboxFaces
{
    "./Textures/CubeMap_Right.png",
    "./Textures/CubeMap_Left.png",
    "./Textures/CubeMap_Top.png",
    "./Textures/CubeMap_Bottom.png",
    "./Textures/CubeMap_Front.png",
    "./Textures/CubeMap_Back.png"
};
GLuint LoadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        SDL_Surface* data = IMG_Load(faces[i].c_str());
        if (data)
        {
            int width = data->w;
            int height = data->h;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data->pixels);
            SDL_FreeSurface(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            SDL_FreeSurface(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
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
    GLuint CubemapShaderProgram = InitializeCubeMapProgram();

    //Make a normal map for the height maps
    GLuint mNormalMap = makeNormalMapTexture("./Textures/normal_map.png");
    GLuint mCottonMapSkybox = LoadCubemap(CotonSkyboxFaces);
    GLuint mCubeMapSkybox = LoadCubemap(CubeMapSkyboxFaces);
    /*******************************************************************************************************************************************/
    //create matrices
    glm::mat4 Center_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 MobileLeft_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, 0.0f, 0.0f));
    glm::mat4 MobileRight_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 0.0f, 0.0f));
    glm::mat4 StaticLeft_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-40.0f, 0.0f, 0.0f));
    glm::mat4 StaticRight_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 0.0f, 0.0f));
    
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
   
    glm::mat4 Center_scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 20.0f, 20.0f));
    glm::mat4 Mobile_scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
    glm::mat4 Static_scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));

    //model matrix
    glm::mat4 Center_ModelMatrix = Center_translationMatrix * rotationMatrix * Center_scaleMatrix;
    glm::mat4 MobileLeft_ModelMatrix = MobileLeft_translationMatrix * rotationMatrix * Mobile_scaleMatrix;
    glm::mat4 MobileRight_ModelMatrix = MobileRight_translationMatrix * rotationMatrix * Mobile_scaleMatrix;  
    glm::mat4 StaticLeft_ModelMatrix = StaticLeft_translationMatrix * rotationMatrix * Static_scaleMatrix;
    glm::mat4 StaticRight_ModelMatrix = StaticRight_translationMatrix * rotationMatrix * Static_scaleMatrix;

    int current_slices = 10;
    //create objects to swap when pressing buttons
    RenderableMeshObject Center_planeObject(MeshType::PLANE, current_slices, Center_ModelMatrix);
    RenderableMeshObject Center_cubeObject(MeshType::CUBE, current_slices, Center_ModelMatrix);
    RenderableMeshObject Center_cylinderObject(MeshType::CYLINDER, current_slices, Center_ModelMatrix);
    RenderableMeshObject Center_coneObject(MeshType::CONE, current_slices, Center_ModelMatrix);
    RenderableMeshObject Center_sphereObject(MeshType::SPHERE, current_slices, Center_ModelMatrix);

    RenderableMeshObject LeftMobile_planeObject(MeshType::PLANE, current_slices, MobileLeft_ModelMatrix);
    RenderableMeshObject LeftMobile_cubeObject(MeshType::CUBE, current_slices, MobileLeft_ModelMatrix);
    RenderableMeshObject LeftMobile_cylinderObject(MeshType::CYLINDER, current_slices, MobileLeft_ModelMatrix);
    RenderableMeshObject LeftMobile_coneObject(MeshType::CONE, current_slices, MobileLeft_ModelMatrix);
    RenderableMeshObject LeftMobile_sphereObject(MeshType::SPHERE, current_slices, MobileLeft_ModelMatrix);

    RenderableMeshObject RightMobile_planeObject(MeshType::PLANE, current_slices, MobileRight_ModelMatrix);
    RenderableMeshObject RightMobile_cubeObject(MeshType::CUBE, current_slices, MobileRight_ModelMatrix);
    RenderableMeshObject RightMobile_cylinderObject(MeshType::CYLINDER, current_slices, MobileRight_ModelMatrix);
    RenderableMeshObject RightMobile_coneObject(MeshType::CONE, current_slices, MobileRight_ModelMatrix);
    RenderableMeshObject RightMobile_sphereObject(MeshType::SPHERE, current_slices, MobileRight_ModelMatrix);

    RenderableMeshObject LeftStatic_planeObject(MeshType::PLANE, current_slices, StaticLeft_ModelMatrix);
    RenderableMeshObject LeftStatic_cubeObject(MeshType::CUBE, current_slices, StaticLeft_ModelMatrix);
    RenderableMeshObject LeftStatic_cylinderObject(MeshType::CYLINDER, current_slices, StaticLeft_ModelMatrix);
    RenderableMeshObject LeftStatic_coneObject(MeshType::CONE, current_slices, StaticLeft_ModelMatrix);
    RenderableMeshObject LeftStatic_sphereObject(MeshType::SPHERE, current_slices, StaticLeft_ModelMatrix);

    RenderableMeshObject RightStatic_planeObject(MeshType::PLANE, current_slices, StaticRight_ModelMatrix);
    RenderableMeshObject RightStatic_cubeObject(MeshType::CUBE, current_slices, StaticRight_ModelMatrix);
    RenderableMeshObject RightStatic_cylinderObject(MeshType::CYLINDER, current_slices, StaticRight_ModelMatrix);
    RenderableMeshObject RightStatic_coneObject(MeshType::CONE, current_slices, StaticRight_ModelMatrix);
    RenderableMeshObject RightStatic_sphereObject(MeshType::SPHERE, current_slices, StaticRight_ModelMatrix);
    
    
    float SideObjectAngle = 0.0f;
    float SideObjectAngleIncrement = 0.001f;

    Light mLight = Light();
    Material mMaterial = Material();
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
    Center_planeObject.SetMaterial(mMaterial);
    Center_cubeObject.SetMaterial(mMaterial);
    Center_cylinderObject.SetMaterial(mMaterial);
    Center_coneObject.SetMaterial(mMaterial);
    Center_sphereObject.SetMaterial(mMaterial);

    LeftMobile_planeObject.SetMaterial(mMaterial);
    LeftMobile_cubeObject.SetMaterial(mMaterial);
    LeftMobile_cylinderObject.SetMaterial(mMaterial);
    LeftMobile_coneObject.SetMaterial(mMaterial);
    LeftMobile_sphereObject.SetMaterial(mMaterial);

    RightMobile_planeObject.SetMaterial(mMaterial);
    RightMobile_cubeObject.SetMaterial(mMaterial);
    RightMobile_cylinderObject.SetMaterial(mMaterial);
    RightMobile_coneObject.SetMaterial(mMaterial);
    RightMobile_sphereObject.SetMaterial(mMaterial);

    LeftStatic_planeObject.SetMaterial(mMaterial);
    LeftStatic_cubeObject.SetMaterial(mMaterial);
    LeftStatic_cylinderObject.SetMaterial(mMaterial);
    LeftStatic_coneObject.SetMaterial(mMaterial);
    LeftStatic_sphereObject.SetMaterial(mMaterial);

    RightStatic_planeObject.SetMaterial(mMaterial);
    RightStatic_cubeObject.SetMaterial(mMaterial);
    RightStatic_cylinderObject.SetMaterial(mMaterial);
    RightStatic_coneObject.SetMaterial(mMaterial);
    RightStatic_sphereObject.SetMaterial(mMaterial);

    glm::mat4 Skybox_translationMatrix = glm::translate(glm::mat4(1.0f), cam_pos);
    glm::mat4 Skybox_scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    glm::mat4 Skybox_ModelMatrix = Skybox_translationMatrix * rotationMatrix * Skybox_scaleMatrix;
    RenderableMeshObject Skybox_cubeObject(MeshType::CUBE, current_slices, Skybox_ModelMatrix);
    
    //1 = plane, 1: Plane, 2: Cube, 3 : Cone,  4 : Cylinder, 5 : Sphere       
    int current_mesh_to_display = 1;
    int Display_Normals = 0;
    bool Display_Wireframe = false;
    int RenderMode = 1;
    int UsingFaceNormals = 1;
    int PlayingLightAnimation = 1;
    int MovingSideObjects = 1;
    int CurrentSkybox = 1;  //1 is cubemap, 0 is cotton

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
                    glm::mat4& mPlane = Center_planeObject.GetModelRefference();
                    mPlane = glm::rotate(Center_planeObject.GetModel(), glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mCube = Center_cubeObject.GetModelRefference();
                    mCube = glm::rotate(Center_cubeObject.GetModel(), glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mCylinder = Center_cylinderObject.GetModelRefference();
                    mCylinder = glm::rotate(Center_cylinderObject.GetModel(), glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mCone = Center_coneObject.GetModelRefference();
                    mCone = glm::rotate(Center_coneObject.GetModel(), glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mSphere = Center_sphereObject.GetModelRefference();
                    mSphere = glm::rotate(Center_sphereObject.GetModel(), glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_UP)
                {
                    glm::mat4& mPlane = Center_planeObject.GetModelRefference();
                    mPlane = glm::rotate(Center_planeObject.GetModel(), glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mCube = Center_cubeObject.GetModelRefference();
                    mCube = glm::rotate(Center_cubeObject.GetModel(), glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mCylinder = Center_cylinderObject.GetModelRefference();
                    mCylinder = glm::rotate(Center_cylinderObject.GetModel(), glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mCone = Center_coneObject.GetModelRefference();
                    mCone = glm::rotate(Center_coneObject.GetModel(), glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    glm::mat4& mSphere = Center_sphereObject.GetModelRefference();
                    mSphere = glm::rotate(Center_sphereObject.GetModel(), glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
                {
                    glm::mat4& mPlane = Center_planeObject.GetModelRefference();
                    mPlane = glm::rotate(Center_planeObject.GetModel(), glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mCube = Center_cubeObject.GetModelRefference();
                    mCube = glm::rotate(Center_cubeObject.GetModel(), glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mCylinder = Center_cylinderObject.GetModelRefference();
                    mCylinder = glm::rotate(Center_cylinderObject.GetModel(), glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mCone = Center_coneObject.GetModelRefference();
                    mCone = glm::rotate(Center_coneObject.GetModel(), glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mSphere = Center_sphereObject.GetModelRefference();
                    mSphere = glm::rotate(Center_sphereObject.GetModel(), glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
                {
                    glm::mat4& mPlane = Center_planeObject.GetModelRefference();
                    mPlane = glm::rotate(Center_planeObject.GetModel(), glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mCube = Center_cubeObject.GetModelRefference();
                    mCube = glm::rotate(Center_cubeObject.GetModel(), glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mCylinder = Center_cylinderObject.GetModelRefference();
                    mCylinder = glm::rotate(Center_cylinderObject.GetModel(), glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mCone = Center_coneObject.GetModelRefference();
                    mCone = glm::rotate(Center_coneObject.GetModel(), glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    glm::mat4& mSphere = Center_sphereObject.GetModelRefference();
                    mSphere = glm::rotate(Center_sphereObject.GetModel(), glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
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
                else if (event.key.keysym.scancode == SDL_SCANCODE_O)
                {
                if (MovingSideObjects == 1)
                {
                    MovingSideObjects = 0;
                }
                else
                {
                    MovingSideObjects = 1;
                }
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_KP_PLUS)
                {
                    current_slices += 1;
                    MeshType cy = MeshType::CYLINDER;
                    MeshType cn = MeshType::CONE;
                    MeshType sp = MeshType::SPHERE;
                    Center_cylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    Center_coneObject.Renderable_ChangeSlices(current_slices, cn);
                    Center_sphereObject.Renderable_ChangeSlices(current_slices, sp);
                    LeftMobile_cylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    LeftMobile_coneObject.Renderable_ChangeSlices(current_slices, cn);
                    LeftMobile_sphereObject.Renderable_ChangeSlices(current_slices, sp);
                    RightMobile_cylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    RightMobile_coneObject.Renderable_ChangeSlices(current_slices, cn);
                    RightMobile_sphereObject.Renderable_ChangeSlices(current_slices, sp);
                    LeftStatic_cylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    LeftStatic_coneObject.Renderable_ChangeSlices(current_slices, cn);
                    LeftStatic_sphereObject.Renderable_ChangeSlices(current_slices, sp);
                    RightStatic_cylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    RightStatic_coneObject.Renderable_ChangeSlices(current_slices, cn);
                    RightStatic_sphereObject.Renderable_ChangeSlices(current_slices, sp);
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
                    Center_cylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    Center_coneObject.Renderable_ChangeSlices(current_slices, cn);
                    Center_sphereObject.Renderable_ChangeSlices(current_slices, sp);
                    LeftMobile_cylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    LeftMobile_coneObject.Renderable_ChangeSlices(current_slices, cn);
                    LeftMobile_sphereObject.Renderable_ChangeSlices(current_slices, sp);
                    RightMobile_cylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    RightMobile_coneObject.Renderable_ChangeSlices(current_slices, cn);
                    RightMobile_sphereObject.Renderable_ChangeSlices(current_slices, sp);
                    LeftStatic_cylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    LeftStatic_coneObject.Renderable_ChangeSlices(current_slices, cn);
                    LeftStatic_sphereObject.Renderable_ChangeSlices(current_slices, sp);
                    RightStatic_cylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    RightStatic_coneObject.Renderable_ChangeSlices(current_slices, cn);
                    RightStatic_sphereObject.Renderable_ChangeSlices(current_slices, sp);
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
                else if (event.key.keysym.scancode == SDL_SCANCODE_Z)
                {
                    CurrentSkybox = 1;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_X)
                {
                    CurrentSkybox = 0;
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
        //updateSkybox
        Skybox_translationMatrix = glm::translate(glm::mat4(1.0f), cam_pos);
        Skybox_ModelMatrix = Skybox_translationMatrix * rotationMatrix * Skybox_scaleMatrix;
        Skybox_cubeObject.SetModel(Skybox_ModelMatrix);
        //ASK IF I NEED TO SET MODEL MATRIX EVERY FRAME
        if (MovingSideObjects)
        {
            SideObjectAngle += SideObjectAngleIncrement;
            float left_y = 10.0f * glm::sin(SideObjectAngle);

            MobileLeft_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, left_y, 0.0f));
            MobileRight_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, left_y, 0.0f));

            MobileLeft_ModelMatrix = MobileLeft_translationMatrix * rotationMatrix * Mobile_scaleMatrix;
            MobileRight_ModelMatrix = MobileRight_translationMatrix * rotationMatrix * Mobile_scaleMatrix;  

            LeftMobile_planeObject.SetModel(MobileLeft_ModelMatrix);
            LeftMobile_cubeObject.SetModel(MobileLeft_ModelMatrix);
            LeftMobile_cylinderObject.SetModel(MobileLeft_ModelMatrix);
            LeftMobile_coneObject.SetModel(MobileLeft_ModelMatrix);
            LeftMobile_sphereObject.SetModel(MobileLeft_ModelMatrix);

            RightMobile_planeObject.SetModel(MobileRight_ModelMatrix);
            RightMobile_cubeObject.SetModel(MobileRight_ModelMatrix);
            RightMobile_cylinderObject.SetModel(MobileRight_ModelMatrix);
            RightMobile_coneObject.SetModel(MobileRight_ModelMatrix);
            RightMobile_sphereObject.SetModel(MobileRight_ModelMatrix);
        }
        ////////////////////////////////////////////////////////////////////////////////
        //change shader program to receive matrices as inputs
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //render skybox
        //disable depth test

        mCottonMapSkybox;
        mCubeMapSkybox;


        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        //render Cubemap
        glm::mat4 SkyViewMatrix = ViewMatrix;
        SkyViewMatrix[3].x = 0.0f;
        SkyViewMatrix[3].y = 0.0f;
        SkyViewMatrix[3].z = 0.0f;
        //SkyViewMatrix
        if (CurrentSkybox)
        {
            Skybox_cubeObject.Renderable_displayCubeMap(SkyViewMatrix, ProjectionMatrix, CubemapShaderProgram, mCubeMapSkybox);
        }
        else
        {
            Skybox_cubeObject.Renderable_displayCubeMap(SkyViewMatrix, ProjectionMatrix, CubemapShaderProgram, mCottonMapSkybox);
        }
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        ////////////////////////////////////////////////////////////////////////////////
        // Renderable_DisplayBasicTexture(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, GLuint& shader, GLuint& texture, int texture_toggle, int display_wiremesh)
        switch (current_mesh_to_display)
        {
            case 1:
            {
                //Renderable_DisplayBasicTexture(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, RenderMode, Display_Wireframe);
                Center_planeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                LeftMobile_planeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);               
                RightMobile_planeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                LeftStatic_planeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                RightStatic_planeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                //if (Display_Normals)
                //{
                //    if (UsingFaceNormals)
                //    {
                //        planeObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                //        planeObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                //        planeObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                //    }
                //    else
                //    {
                //        planeObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, SingleColorShaderProgram);
                //        planeObject.Renderable_displayAveragedTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                //        planeObject.Renderable_displayAveragedBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                //    }
                //}
            }
            break;
            case 2:
            {
                Center_cubeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                LeftMobile_cubeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                RightMobile_cubeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                LeftStatic_cubeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                RightStatic_cubeObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                //if (Display_Normals)
                //{
                //    if (UsingFaceNormals)
                //    {
                //        cubeObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                //        cubeObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                //        cubeObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                //    }
                //    else
                //    {
                //        cubeObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                //        cubeObject.Renderable_displayAveragedTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                //        cubeObject.Renderable_displayAveragedBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                //    }
                //}
            }
            break;
            case 3:
            {
                Center_cylinderObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                LeftMobile_cylinderObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                RightMobile_cylinderObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                LeftStatic_cylinderObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                RightStatic_cylinderObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                //if (Display_Normals)
                //{
                //    if (UsingFaceNormals)
                //    {
                //        cylinderObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                //        cylinderObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                //        cylinderObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                //    }
                //    else
                //    {
                //        cylinderObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                //        cylinderObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                //        cylinderObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                //    }
                //}
            }
            break;
            case 4:
            {
                Center_coneObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                LeftMobile_coneObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                RightMobile_coneObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                LeftStatic_coneObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                RightStatic_coneObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                //if (Display_Normals)
                //{
                //    if (UsingFaceNormals)
                //    {
                //        coneObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                //        coneObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                //        coneObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                //    }
                //    else
                //    {
                //        coneObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                //        coneObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                //        coneObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                //    }
                //}
            }
            break;
            case 5:
            {
                Center_sphereObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                LeftMobile_sphereObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                RightMobile_sphereObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                LeftStatic_sphereObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                RightStatic_sphereObject.Renderable_displayMesh(ViewMatrix, ProjectionMatrix, BasicColorShaderProgram, texture, Display_Wireframe, RenderMode, mLight, mNormalMap, UsingFaceNormals);
                //if (Display_Normals)
                //{
                //    if (UsingFaceNormals)
                //    {
                //        sphereObject.Renderable_displayNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                //        sphereObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                //        sphereObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                //    }
                //    else
                //    {
                //        sphereObject.Renderable_displayAveragedNormals(ViewMatrix, ProjectionMatrix, BlueShaderProgram);
                //        sphereObject.Renderable_displayTangents(ViewMatrix, ProjectionMatrix, RedShaderProgram);
                //        sphereObject.Renderable_displayBiTangents(ViewMatrix, ProjectionMatrix, GreenShaderProgram);
                //    }
                //}
            }
            break;
        }
        SDL_GL_SwapWindow(window);           
    }
    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}