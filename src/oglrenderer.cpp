#include <windows.h>
#include <gdiplus.h>
#include "oglrenderer.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <shlwapi.h>

extern HWND hWnd;
extern HINSTANCE hInst;
extern HDC hWindowDC;

static HGLRC hWindowRC;

void core::OGLRenderer::Initialize(void)
{
    // Binding OpenGL to the current window.
	PIXELFORMATDESCRIPTOR kPFD = {0};
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
	float vec[4] = {1.f, 1.f, 1.f, 1.f};//{0.5f, 0.5f, 0.5f, 0.2f};
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

    static char array[100] = {0};
    sprintf(array, "C++ Project: %ifps", frametime);
	SetWindowText(hWnd, array);
}

void core::OGLRenderer::Cleanup(void)
{
    std::map<std::string, unsigned int>::iterator iter = textures.begin();
	for(; iter != textures.end(); ++iter) {
		unsigned int n = (*iter).second;
		glDeleteTextures(1, &n);
	}
	textures.clear();

    glBindTexture(GL_TEXTURE_2D, 0);
    wglDeleteContext(hWindowRC);
    ReleaseDC(hWnd, hWindowDC);
}

/**
 * @brief Given a GDI plus bitmap class, it returns the content as an RGBA buffer.
 * @param bitmap The bitmap to get the data for.
 * @param [out] width Will be filled with the width of the bitmap.
 * @param [out] height Will be filled with the height of the bitmap.
 * @todo Replace GDI plus code with native C libraries.
 * @return NULL in case of failure, or a void pointer to an RGBA buffer (each component is an
 * unsigned char).
 */
static unsigned char *GetBitmapBuffer(Gdiplus::Bitmap &bitmap, int &width, int &height)
{
	width = bitmap.GetWidth();
	height = bitmap.GetHeight();
	Gdiplus::Rect *rect = new Gdiplus::Rect(0, 0, width, height);
	Gdiplus::BitmapData *bitmapdata = new Gdiplus::BitmapData;

	if (bitmap.LockBits(rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapdata) == 0) {
        // Buffer that has the image data.
        unsigned char *src = (unsigned char *)bitmapdata->Scan0;
		int stride = bitmapdata->Stride;
		// Buffer that will contain the copied image data.
        unsigned char *dst = new unsigned char[width * height * 4];

		// Copying the data to the destination buffer.
		for (int j = 0; j < height; ++j) {
			for (int i = 0; i < width; ++i) {
				dst[(height - 1 - j) * (width * 4) + (i * 4) + 0] = src[j * stride + i * 4 + 2];
				dst[(height - 1 - j) * (width * 4) + (i * 4) + 1] = src[j * stride + i * 4 + 1];
				dst[(height - 1 - j) * (width * 4) + (i * 4) + 2] = src[j * stride + i * 4 + 0];
				dst[(height - 1 - j) * (width * 4) + (i * 4) + 3] = src[j * stride + i * 4 + 3];
			}
		}

		bitmap.UnlockBits(bitmapdata);

		// Return the point to the buffer.
		return dst;
	}

	return NULL;
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
	std::map<std::string, unsigned int>::iterator iter = textures.find(path);
	if (iter != textures.end())
        return true;

    // GDI plus works with Unicode/multi-byte strings.
	WCHAR widepathbuffer[1000];
	MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1, widepathbuffer, 1000);
	Gdiplus::Bitmap bitmap(widepathbuffer);

	// Get the texture color data, in case of failure return false.
	int width = 0, height = 0;
	unsigned char *buffer = GetBitmapBuffer(bitmap, width, height);
	if (!buffer)
        return false;

    // Upload the texture, generate mipmaps and set wrapping modes.
	unsigned int n = 0;
	glGenTextures(1, &n);
	glBindTexture(GL_TEXTURE_2D, n);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Setting the magnification/minification filters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    delete [] buffer;
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
        glVertex3f(-area/2, 0, -area/2 + area/amount * i);
        glVertex3f(area/2, 0, -area/2 + area/amount * i);

        // Vertical.
        glVertex3f(-area/2 + area/amount * i, 0, -area/2);
        glVertex3f(-area/2 + area/amount * i, 0, area/2);
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
        if (mesh.materials[0].textures.size() && mesh.materials[0].textures[0].name != "") {
            glEnable(GL_TEXTURE_2D);
            unsigned int id = textures.find(mesh.materials[0].textures[0].path)->second;
            glBindTexture(GL_TEXTURE_2D, id);
        }
    } else {
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
