#include <windows.h>
#include "oglrenderer.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <shlwapi.h>

#include "libpng/png.h"
#include "libjpeg/jpeglib.h"

extern HWND hWnd;
extern HDC hWindowDC;

static HGLRC hWindowRC;

void core::OGLRenderer::Initialize(void)
{
    // Binding OpenGL to the current window.
    PIXELFORMATDESCRIPTOR kPFD = { 0 };
    kPFD.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    kPFD.nVersion = 1;
    kPFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
    kPFD.dwLayerMask = PFD_MAIN_PLANE;
    kPFD.iPixelType = PFD_TYPE_RGBA;
    kPFD.cColorBits = 32;
    kPFD.cDepthBits = 32;

    int iPixelFormat = ChoosePixelFormat(hWindowDC, &kPFD);
    SetPixelFormat(hWindowDC, iPixelFormat, &kPFD);

    hWindowRC = wglCreateContext(hWindowDC);
    wglMakeCurrent(hWindowDC, hWindowRC);

    ShowCursor(false);

    // Shading model, depth test, back face culling, buffer clearing color.
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.3f, 0.3f, 0.3f, 1);

    // Enable material colors (should be set per mesh).
    glEnable(GL_COLOR_MATERIAL);

    // Enabling lighting, setting ambient color, etc...
    glEnable(GL_LIGHTING);
    float vec[4] = { 1.f, 1.f, 1.f, 1.f };//{0.5f, 0.5f, 0.5f, 0.2f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, vec);
    // Separating specular color update.
    //glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

    // Setting the texture blending mode.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Enabling vertex, normal and UV arrays.
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void core::OGLRenderer::UpdateViewportProperties(const core::Pipeline *pipeline)
{
    float x, y, width, height;
    pipeline->GetViewportInfo(x, y, width, height);
    glViewport(x, y, width, height);
}

void core::OGLRenderer::UpdateProjectionProperties(const core::Pipeline *pipeline)
{
    float left, right, bottom, top, _near, _far;
    pipeline->GetFrustumInfo(left, right, bottom, top, _near, _far);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (pipeline->GetProjectionType() == core::Pipeline::PERSPECTIVE)
        glFrustum(left, right, bottom, top, _near, _far);
    else
        glOrtho(left, right, bottom, top, _near, _far);
}

void core::OGLRenderer::PreUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void core::OGLRenderer::PostUpdate(int frametime)
{
    glFinish();
    SwapBuffers(hWindowDC);

    static char array[100] = { 0 };
    sprintf(array, "C++ Project: %ifps", frametime);
    SetWindowText(hWnd, array);
}

void core::OGLRenderer::Cleanup(void)
{
    std::map<std::string, unsigned int>::iterator iter = textures.begin();
    for (; iter != textures.end(); ++iter) {
        unsigned int n = (*iter).second;
        glDeleteTextures(1, &n);
    }
    textures.clear();

    glBindTexture(GL_TEXTURE_2D, 0);
    wglDeleteContext(hWindowRC);
    ReleaseDC(hWnd, hWindowDC);
}

bool core::OGLRenderer::LoadTextureMap(std::string path)
{
    std::string original_path = path;
    std::basic_string<char>::size_type index = 0;

    // If the file doesn't exist, we check in the default textures directory.
    if (!PathFileExists(path.c_str())) {
        std::string filename;
        std::string fullpath;
        std::basic_string<char>::size_type indicator = path.find_last_of("\\", path.size()) + 1;
        filename = path.substr(indicator, path.size() - indicator);
        char directory[1000];
        GetCurrentDirectory(1000, directory);
        fullpath = directory;
        fullpath += "\\media\\textures\\";
        fullpath += filename;

        // If the file still cannot be found return false.,
        if (!PathFileExists(fullpath.c_str()))
            return false;

        // Update path to point to the texture path.
        path = fullpath;
    }

    // Check if it already exists.
    std::map<std::string, unsigned int>::iterator iter = textures.find(original_path);
    if (iter != textures.end())
        return true;

    int width = 0, height = 0;
    int size = 0;
    unsigned char *buffer = NULL;
    GLenum format = GL_RGBA;
    int components = 4;

    if (path.substr(path.length() - 4, 4) == ".png") {
        png_image image;

        memset(&image, 0, (sizeof image));
        image.version = PNG_IMAGE_VERSION;

        if (png_image_begin_read_from_file(&image, path.c_str()) != 0)
        {
            image.format = PNG_FORMAT_RGBA;
            width = image.width;
            height = image.height;
            buffer = new unsigned char[PNG_IMAGE_SIZE(image)];

            if (buffer != NULL && png_image_finish_read(&image, NULL, (void *)buffer, 0, NULL) != 0)
            {
                ;
            }
            else
            {
                if (buffer == NULL)
                    png_image_free(&image);
                else
                    delete[] buffer;
            }
        }
    } else if (path.substr(path.length() - 4, 4) == ".jpg" || path.substr(path.length() - 5, 5) == ".jpeg") {
        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;

        FILE * infile;
        int row_stride;

        if ((infile = fopen(path.c_str(), "rb")) == NULL) {
            fprintf(stderr, "can't open %s\n", path.c_str());
            return 0;
        }

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);
        jpeg_stdio_src(&cinfo, infile);

        int value = jpeg_read_header(&cinfo, 0);

        components = 3;
        format = GL_RGB;
        cinfo.out_color_space = JCS_RGB;

        (void)jpeg_start_decompress(&cinfo);
        row_stride = cinfo.output_width * cinfo.output_components;
        row_stride += row_stride % 2;

        width = cinfo.output_width;
        height = cinfo.output_height;
        buffer = new BYTE[row_stride * cinfo.output_height];
        memset(buffer, 0, sizeof(buffer));

        BYTE* p1 = buffer + row_stride * (cinfo.output_height - 1);
        BYTE** p2 = &p1;
        int numlines = 0;

        while (cinfo.output_scanline < cinfo.output_height)
        {
            numlines = jpeg_read_scanlines(&cinfo, p2, 1);
            *p2 -= numlines * row_stride;
        }

        (void)jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);

        fclose(infile);
    }
    else
        return false;

    // Upload the texture, generate mipmaps and set wrapping modes.
    unsigned int n = 0;
    glGenTextures(1, &n);
    glBindTexture(GL_TEXTURE_2D, n);
    gluBuild2DMipmaps(GL_TEXTURE_2D, components, width, height, format, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Setting the magnification/minification filters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    delete[] buffer;
    textures[original_path] = n;

    return true;
}

void core::OGLRenderer::LoadTextureMaps(std::vector<std::string> paths)
{
    for (unsigned int i = 0; i < paths.size(); ++i)
        LoadTextureMap(paths[i]);
}

void core::OGLRenderer::DrawGrid(void) const
{
    const float area = 5000;
    const int amount = 100;

    core::Pipeline *current_pipeline = core::Pipeline::GetCurrentPipeline();
    // Push the current pipeline transformation.
    if (current_pipeline) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        current_pipeline->SetMatrixMode(core::Pipeline::MODELVIEW);
        math::Matrix4D output;
        current_pipeline->GetMatrix(output);
        float m[16];
        output.ToArrayColumnMajor(m);
        glMultMatrixf(m);
    }

    glDisable(GL_LIGHTING);
    glColor4f(0, 0, 0, 1);
    glBegin(GL_LINES);
    for (int i = 0; i <= amount; ++i) {
        // Horizontal.
        glVertex3f(-area / 2, 0, -area / 2 + area / amount * i);
        glVertex3f(area / 2, 0, -area / 2 + area / amount * i);

        // Vertical.
        glVertex3f(-area / 2 + area / amount * i, 0, -area / 2);
        glVertex3f(-area / 2 + area / amount * i, 0, area / 2);
    }
    glEnd();
    glEnable(GL_LIGHTING);

    // Pop it.
    if (current_pipeline)
        glPopMatrix();
}

void core::OGLRenderer::DrawModel(const Model &model) const
{
    core::Pipeline *current_pipeline = core::Pipeline::GetCurrentPipeline();
    // Push the current pipeline transformation.
    if (current_pipeline) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        current_pipeline->SetMatrixMode(core::Pipeline::MODELVIEW);
        math::Matrix4D output;
        current_pipeline->GetMatrix(output);
        float m[16];
        output.ToArrayColumnMajor(m);
        glMultMatrixf(m);
    }

    // Renderer all the meshes attached to the model first.
    for (unsigned int i = 0; i < model.meshes.size(); ++i)
        DrawMesh(*model.meshes[i]);

    // Go through its child models and render those.
    for (unsigned int i = 0; i < model.sub_models.size(); ++i)
        DrawModel(*model.sub_models[i]);

    // Pop it.
    if (current_pipeline)
        glPopMatrix();
}

void core::OGLRenderer::DrawMesh(const Mesh &mesh) const
{
    core::Color white;
    white.r = white.g = white.b = white.a = 1.f;

    // Setting ambient, diffuse and specular values.
    if (mesh.materials.size()) {
        glColorMaterial(GL_FRONT, GL_AMBIENT);
        glColor4f(mesh.materials[0].ambient.r, mesh.materials[0].ambient.g, mesh.materials[0].ambient.b, mesh.materials[0].ambient.a);
        glColorMaterial(GL_FRONT, GL_DIFFUSE);
        glColor4f(mesh.materials[0].diffuse.r, mesh.materials[0].diffuse.g, mesh.materials[0].diffuse.b, mesh.materials[0].diffuse.a);
        glColorMaterial(GL_FRONT, GL_SPECULAR);
        glColor4f(mesh.materials[0].specular.r, mesh.materials[0].specular.g, mesh.materials[0].specular.b, mesh.materials[0].specular.a);
        float shine = mesh.materials[0].shininess * 128;
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shine);

        // Texturing.
        std::map<std::string, unsigned int>::const_iterator search = textures.end();
        if (mesh.materials[0].textures.size() && mesh.materials[0].textures[0].name != "" && (search = textures.find(mesh.materials[0].textures[0].path)) != textures.end()) {
            glEnable(GL_TEXTURE_2D);
            unsigned int id = search->second;
            glBindTexture(GL_TEXTURE_2D, id);
        }
    }
    else {
        glColorMaterial(GL_FRONT, GL_AMBIENT);
        glColor4f(white.r, white.g, white.b, white.a);
        glColorMaterial(GL_FRONT, GL_DIFFUSE);
        glColor4f(white.r, white.g, white.b, white.a);
        glColorMaterial(GL_FRONT, GL_SPECULAR);
        glColor4f(white.r, white.g, white.b, white.a);
    }

    glVertexPointer(4, GL_FLOAT, 0, mesh.vertices);
    glTexCoordPointer(3, GL_FLOAT, 0, mesh.uv_coordinates[0]);
    glNormalPointer(GL_FLOAT, 0, mesh.normals);
    glDrawElements(GL_TRIANGLES, mesh.index_array_size, GL_UNSIGNED_SHORT, mesh.index_array);

    glDisable(GL_TEXTURE_2D);
}
