#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LOG_TAG		"libumbrellademo"
#define LOGI(...)	__android_log_print(ANDROID_LOG_INFO, LOG_TAG,__VA_ARGS__)
#define LOGE(...)	__android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__)
#define PI 3.14159

static void printGLString(const char *name, GLenum s)
{
	const char *v = (const char *)glGetString(s);
	LOGI("GL %s = %s\n", name, v);
}

static void checkGLError(const char* op)
{
	for (GLint error = glGetError(); error; error = glGetError())
	{
		LOGI("after %s() glError (0x%x\n", op, error);
	}
}

// ------------------------------------------------------------------------------
// SHADERIT
static const char gVertexShader[] =
"attribute vec4 vPosition;\n"
"void main() {\n"
"  gl_Position = vPosition;\n"
"}\n";

// PUNAINEN
static const char gFragmentShader1[] =
"precision mediump float;\n"
"void main() {\n"
"  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n";
// VALKOINEN
static const char gFragmentShader2[] =
"precision mediump float;\n"
"void main() {\n"
"  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
"}\n";

// Ladataan shaderit
GLuint loadShader(GLenum shaderType, const char* pSource)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader) {
		glShaderSource(shader, 1, &pSource, NULL);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				char* buf = (char*)malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					LOGE("Could not compile shader %d:\n%s\n",
						shaderType, buf);
					free(buf);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource)
{
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader)
	{
		return 0;
	}

	GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader)
	{
		return 0;
	}

	GLuint program = glCreateProgram();
	if (program)
	{
		glAttachShader(program, vertexShader);
		checkGLError("glAttachShader");
		glAttachShader(program, pixelShader);
		checkGLError("glAttachShader");
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE)
		{
			GLint bufLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength)
			{
				char* buf = (char*)malloc(bufLength);
				if (buf)
				{
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					LOGE("Could not link program:\n%s\n", buf);
					free(buf);
				}
			}
			glDeleteProgram(program);
			program = 0;
		}
	}
	return program;
}

GLuint gProgramRed;
GLuint gProgramWhite;
GLuint gvPositionHandleRed;
GLuint gvPositionHandleWhite;


// Luodaan grafiikka, programRed ja programWhite
bool setupGraphics(int w, int h)
{
	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLString("Extensions", GL_EXTENSIONS);

	LOGI("setupGraphics(%d, %d)", w, h);
	gProgramRed = createProgram(gVertexShader, gFragmentShader1);
	if (!gProgramRed) {
		LOGE("Could not create program.");
		return false;
	}

	gProgramWhite = createProgram(gVertexShader, gFragmentShader2);
	if (!gProgramWhite) {
		LOGE("Could not create program.");
		return false;
	}

	gvPositionHandleRed = glGetAttribLocation(gProgramRed, "vPosition");
	checkGLError("glGetAttribLocation");
	LOGI("glGetAttribLocation(\"vPosition\") = %d\n",
		gvPositionHandleRed);

	gvPositionHandleWhite = glGetAttribLocation(gProgramWhite, "vPosition");
	checkGLError("glGetAttribLocation");
	LOGI("glGetAttribLocation(\"vPosition\") = %d\n",
		gvPositionHandleWhite);

	glViewport(0, 0, w, h);
	checkGLError("glViewport");
	return true;
}

void renderFrame() {
	static float grey;
	static float angle;
	angle += 0.01f;
	/* grey += 0.01f;			// vilkkuminen
	if (grey > 1.0f) {
	grey = 0.0f;
	}*/
	//glClearColor(grey, grey, grey, 1.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	checkGLError("glClearColor");
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	checkGLError("glClear");

	{
		// red triangles

		glUseProgram(gProgramRed);
		checkGLError("glUseProgram");

		GLfloat vertexBuffer[3 * 2 * 4];

		for (int i = 0; i < 4; i++)
		{
			float angle2 = angle + (float)i*PI / 2;
			float a = PI / 8;
			const float gTriangleVertices[] =
			{
				cos(angle2)*0.05, sin(angle2)*0.05,
				cos(angle2 + a - 0.025)*0.9, sin(angle2 + a - 0.025)*0.9,
				cos(angle2 - a + 0.025)*0.9, sin(angle2 - a + 0.025)*0.9,
			};
			for (int j = 0; j < 6; j++)
			{
				vertexBuffer[j + i * 6] = gTriangleVertices[j];
			}
		}
		glVertexAttribPointer(gvPositionHandleRed, 2, GL_FLOAT, GL_FALSE, 0, vertexBuffer);
		checkGLError("glVertexAttribPointer");
		glEnableVertexAttribArray(gvPositionHandleRed);
		checkGLError("glEnableVertexAttribArray");
		glDrawArrays(GL_TRIANGLES, 0, 3 * 4);
		checkGLError("glDrawArrays");
	};

	{
		// white triangles

		glUseProgram(gProgramWhite);
		checkGLError("glUseProgram");

		GLfloat vertexBuffer[3 * 2 * 4];

		for (int i = 0; i < 4; i++)
		{
			float angle2 = angle + PI / 4 + (float)i * PI / 2;
			float a = PI / 8;
			const float gTriangleVertices[] =
			{
				cos(angle2)*0.05, sin(angle2)* 0.05,
				cos(angle2 + a - 0.025)*0.9, sin(angle2 + a - 0.025)*0.9,
				cos(angle2 - a + 0.025)*0.9, sin(angle2 - a + 0.025)*0.9,
			};
			for (int j = 0; j < 6; j++)
			{
				vertexBuffer[j + i * 6] = gTriangleVertices[j];
			}
		}
		glVertexAttribPointer(gvPositionHandleWhite, 2, GL_FLOAT, GL_FALSE, 0, vertexBuffer);
		checkGLError("glVertexAttribPointer");
		glEnableVertexAttribArray(gvPositionHandleWhite);
		checkGLError("glEnableVertexAttribArray");
		glDrawArrays(GL_TRIANGLES, 0, 3 * 4);
		checkGLError("glDrawArrays");


	};
}

extern "C" {
	JNIEXPORT void JNICALL Java_fi_umbrellademo_UmbrellaDemoLib_init(JNIEnv * env, jobject obj, jint width, jint height);
	JNIEXPORT void JNICALL Java_fi_umbrellademo_UmbrellaDemoLib_step(JNIEnv * env, jobject obj);
};

JNIEXPORT void JNICALL Java_fi_umbrellademo_UmbrellaDemoLib_init(JNIEnv * env, jobject obj, jint width, jint height)
{
	setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_fi_umbrellademo_UmbrellaDemoLib_step(JNIEnv * env, jobject obj)
{
	renderFrame();
}