#if defined(WIN32)
#  pragma warning (disable : 4786)
#endif

// Global includes
#include <ScriptLoader/ScriptManager.h>
#include <ScriptLoader/ScriptFunctionInstance.h>
#include <PropertyManager/PropertyManager.h>

#include <PropertyManager/CVariableSource.h>
#include <PropertyManager/CFunctionSource.h>
#include <PropertyManager/CArrayFunctionSource.h>

#include <PropertyManager/CBasicProperty.h>
#include <PropertyManager/CBasicArrayProperty.h>

#include <PropertyManager/CgGLEnumMatrixProperty.h>
#include <PropertyManager/VaryingProperty.h>
#include <ScriptLoaderGeneral/TextureManager.h>
#include <GpuManager/GpuManager.h>
#include <GpuManager/GpuProgram.h>
#include <GpuManager/GpuProgramFactory.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <Cg/cgGL.h>
#include <glh/glh_glut.h>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/util/PlatformUtils.hpp>

// Local includes
#include "../nv_shared/data_path.h"
#include "../nv_shared/objload.h"
#include "GpuOrenNayarFP.h"

// Namespace
using namespace glh;
XERCES_CPP_NAMESPACE_USE

// Globals
int numLights = 2;
float color[4] = { 1.0, 1.0, 0.9, 1.0 };

struct light {
	float vector[4];
	float color[4];
	bool specular;
};

std::vector<light> lights;

class program {
public:
	GpuProgram* prog;
	PropertyMap map;
};

int currentVertexProgram = 0;
int currentFragmentProgram = 0;
int numVertexPrograms = 0;
int numFragmentPrograms = 0;
std::vector<program> vPrograms;
std::vector<program> fPrograms;

const char* modelFile = "data/horse.obj";
const char* envFile = "data/nvlobby_cube_mipmap.dds";
const char* vProgramsFile = "data/vertex_programs.xml";
const char* fProgramsFile = "data/fragment_programs.xml";
data_path data_loc;

PropertyMap engine;
PropertyMap model;

// GL Globals
const float PI = 3.141592654;
glut_simple_mouse_interactor camera, object;
glut_callbacks cb;
glut_perspective_reshaper reshaper;
bool b[256];
float eyePos[4];

// data for obj-format load
unsigned int nverts = 0;
unsigned int nindices = 0;
unsigned int *indices = NULL;
float *vertexdata = NULL;
float *normaldata = NULL;
float *tangentdata = NULL;
float *binormdata = NULL;
float *texcoords = NULL;

void drawTextAt(char *p, int x, int y);
void drawInfo();
void drawSkyBox();
void display();
void idle();
void resize(int w, int h);
void key(unsigned char k, int x, int y);
void special(int k, int x, int y);
void initOpenGL();

void init();
void initProperties();
void initPrograms();
void initDataLocation();
void initLights();

void addVertexProgram(DOMNode* node);
void addFragmentProgram(DOMNode* node);

float* getLightColor(int light);
float* getLightVector(int light);
bool* getLightSpecular(int light);
float* getEyePosition();
float* getWorldToCube();

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
	glutCreateWindow("Shader System Prototype Test #1");
	glutReshapeWindow(700, 500);

	init();

	glut_helpers_initialize();

	object.configure_buttons(1);
	object.trackball.legacy_mode = true;
	object.dolly.dolly[2] = -3;
	object.trackball.r.set_value(vec3f(0, 1, 0), -PI/4);
//	quaternionf q;
//	q.set_value(vec3f(0, 0, 1), PI);
//	object.trackball.r *= q;

	object.enable();

    cb.keyboard_function = key;
    cb.special_function = special;

    glut_add_interactor(&cb);
    glut_add_interactor(&object);
	glut_add_interactor(&reshaper);
    
	glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutIdleFunc(idle);
    
//	glutReshapeFunc(resize);

    b[' '] = true;

    glutMainLoop();

    return 0;
}

void init() {
	initLights();
	initDataLocation();
	initOpenGL();
	initPrograms();
	initProperties();
}

void initProperties() {
	// Register properties to maps
	engine.registerProperty(new CBasicProperty<int>(new CVariableSource<int>(&numLights, 1), "ENGINE.NumberOfLights"));
	engine.registerProperty(new CgGLEnumMatrixProperty(CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY, "ENGINE.ModelViewProjection"));
	engine.registerProperty(new CgGLEnumMatrixProperty(CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE, "ENGINE.ModelViewIT"));
	engine.registerProperty(new CgGLEnumMatrixProperty(CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY, "ENGINE.ModelView"));
	engine.registerProperty(new CgGLEnumMatrixProperty(CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE, "ENGINE.ModelViewI"));
   	engine.registerProperty(new CBasicArrayProperty<float>(new CArrayFunctionSource<float>(&getLightVector, 4), "ENGINE.LightVector"));
   	engine.registerProperty(new CBasicArrayProperty<float>(new CArrayFunctionSource<float>(&getLightColor, 3), "ENGINE.LightColor"));
	engine.registerProperty(new CBasicArrayProperty<bool>(new CArrayFunctionSource<bool>(&getLightSpecular), "ENGINE.LightSpecular"));
	engine.registerProperty(new CBasicProperty<float>(new CFunctionSource<float>(&getEyePosition, 4), "ENGINE.EyePosition"));
	engine.registerProperty(new CgGLEnumMatrixProperty(CG_GL_TEXTURE_MATRIX, CG_GL_MATRIX_IDENTITY, "ENGINE.TextureMatrix"));

	PropertyManager::instance().setPropertyMap("ENGINE", &engine);

	model.registerProperty(new VaryingProperty(3, GL_FLOAT, 0, tangentdata, "MODEL.Tangent"));
	model.registerProperty(new VaryingProperty(3, GL_FLOAT, 0, binormdata, "MODEL.Binormal"));

	PropertyManager::instance().setPropertyMap("MODEL", &model);
}


void initPrograms() {
	GpuProgramFactory::instance().registerProgram("OrenNayarFP", GpuOrenNayarFP::create);

	try {
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		std::cout << "Error during initialization! :\n"
			<< message << "\n";
		XMLString::release(&message);
		exit(-1);
	}

	static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);
	DOMBuilder* parser = ((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = NULL;

	try {
		doc = parser->parseURI(vProgramsFile);
	} catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		std::cout << "Exception message is: \n"
			<< message << "\n";
		XMLString::release(&message);
		exit(-1);
	} catch (const DOMException& toCatch) {
		char* message = XMLString::transcode(toCatch.msg);
		std::cout << "Exception message is: \n"
			<< message << "\n";
		XMLString::release(&message);
		exit(-1);
	} catch (...) {
		std::cout << "Unexpected Exception \n" ;
		exit(-1);
	}

	DOMNode* node = (DOMNode*)doc->getDocumentElement();

	if(!node)
		exit(-1);
	if(node->getNodeType() != DOMNode::ELEMENT_NODE)
		exit(-1);

	char *name = XMLString::transcode(node->getNodeName());
	if(strcmp(name, "Programs") != 0)
		exit(-1);

	// Add programs
	vPrograms.resize(node->getChildNodes()->getLength());
	for(DOMNode* child = node->getFirstChild(); child != 0; child=child->getNextSibling()) {
		if(child->getNodeType() != DOMNode::ELEMENT_NODE)
			continue;
		name = XMLString::transcode(child->getNodeName());
		if(strcmp(name, "Program") == 0) {
			addVertexProgram(child);
		}
	}
	vPrograms.resize(numVertexPrograms);
	parser->release();

	parser = ((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    
	try {
		doc = parser->parseURI(fProgramsFile);
	} catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		std::cout << "Exception message is: \n"
			<< message << "\n";
		XMLString::release(&message);
		exit(-1);
	} catch (const DOMException& toCatch) {
		char* message = XMLString::transcode(toCatch.msg);
		std::cout << "Exception message is: \n"
			<< message << "\n";
		XMLString::release(&message);
		exit(-1);
	} catch (...) {
		std::cout << "Unexpected Exception \n" ;
		exit(-1);
	}

	node = (DOMNode*)doc->getDocumentElement();

	if(!node)
		exit(-1);
	if(node->getNodeType() != DOMNode::ELEMENT_NODE)
		exit(-1);

	name = XMLString::transcode(node->getNodeName());
	if(strcmp(name, "Programs") != 0)
		exit(-1);

	// Add properties
	fPrograms.resize(node->getChildNodes()->getLength());
	for(DOMNode* child = node->getFirstChild(); child != 0; child=child->getNextSibling()) {
		if(child->getNodeType() != DOMNode::ELEMENT_NODE)
			continue;
		name = XMLString::transcode(child->getNodeName());
		if(strcmp(name, "Program") == 0) {
			addFragmentProgram(child);
		}
	}
	fPrograms.resize(numFragmentPrograms);
	parser->release();

	XMLPlatformUtils::Terminate();
}

void addVertexProgram(DOMNode* node) {
	DOMNamedNodeMap* attributes = node->getAttributes();
	DOMAttr* attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("program"));
	const char* programFile = XMLString::transcode(attr->getValue());
	attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("parameters"));
	const char* parameterFile = XMLString::transcode(attr->getValue());
	attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("type"));
	std::string type("default");
	if(attr)
		type = XMLString::transcode(attr->getValue());
	std::string programFileLoc = data_loc.get_file(programFile);
	if(programFileLoc == "") {
		printf("Program %s not found!\n", programFile);
		return;
	}
	std::string parameterFileLoc = data_loc.get_file(parameterFile);
	if(parameterFileLoc == "") {
		printf("Property map %s not found!\n", parameterFile);
	}
	vPrograms[numVertexPrograms].prog = GpuManager::instance().loadProgram(programFileLoc, type);
	vPrograms[numVertexPrograms].map.loadXML(parameterFileLoc);
	numVertexPrograms++;
}

void addFragmentProgram(DOMNode* node) {
	DOMNamedNodeMap* attributes = node->getAttributes();
	DOMAttr* attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("program"));
	const char* programFile = XMLString::transcode(attr->getValue());
	attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("parameters"));
	const char* parameterFile = XMLString::transcode(attr->getValue());
	attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("type"));
	std::string type = "default";
	if(attr)
		type = XMLString::transcode(attr->getValue());
	std::string programFileLoc = data_loc.get_file(programFile);
	if(programFileLoc == "") {
		printf("Program %s not found!\n", programFile);
		return;
	}
	std::string parameterFileLoc = data_loc.get_file(parameterFile);
	if(parameterFileLoc == "") {
		printf("Property map %s not found!\n", parameterFile);
	}
	fPrograms[numFragmentPrograms].prog = GpuManager::instance().loadProgram(programFileLoc, type);
	fPrograms[numFragmentPrograms].map.loadXML(parameterFileLoc);
	numFragmentPrograms++;
}

void initDataLocation() {
	data_loc.path.push_back(".");
	data_loc.path.push_back("..");
	data_loc.path.push_back("../..");
}

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.8);
	glClearColor(0.2, 0.2, 0.2, 1.0);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, getLightColor(0));
	glLightfv(GL_LIGHT0, GL_POSITION, getLightVector(0));

	// import external geometry
	std::string filename = data_loc.get_file(modelFile);
	if (filename == "") {
		printf("Unable to locate %s, exiting...\n", modelFile);
		exit(0);
	}

	if (!LoadObjModel(filename.c_str(),nverts,nindices,indices,vertexdata,normaldata,tangentdata,binormdata,texcoords)) {
		printf("Unable to load %s, exiting...\n", filename.c_str());
		exit(0);
	}


}

void initLights() {
	light light1 = {{-4.0, 6.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, true};
	lights.push_back(light1);
	light light2 = {{2.0, 4.0,-5.0, 1.0}, {1.0, 0.2, 0.0, 1.0}, false};
	lights.push_back(light2);
	light light3 = {{2.0,-5.0, 1.0, 1.0}, {0.1, 0.4, 0.6, 1.0}, false};
	lights.push_back(light3);
	light light4 = {{-10.0,6.0, 1.0, 1.0}, {0.88, 0.68, 0.08, 1.0}, false};
	lights.push_back(light4);
}

float* getLightColor(int light) {
	return &lights[light].color[0];
}

float* getLightVector(int light) {
	return &lights[light].vector[0];
}

bool* getLightSpecular(int light) {
	return &lights[light].specular;
}

float* getEyePosition() {
	matrix4f mat = object.get_inverse_transform();
	vec4f pos = mat.get_column(3);
	pos.get_value(eyePos[0], eyePos[1], eyePos[2], eyePos[3]);
	return &eyePos[0];
}

void drawTextAt(char *p, int x, int y) {
	glRasterPos2i( x, y);
    for ( ; *p; p++ ) {
		glutBitmapCharacter( GLUT_BITMAP_8_BY_13, *p );
	}
}

void drawInfo() {
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( 0, reshaper.width-1, 0, reshaper.height-1 );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();
	glDisable(GL_LIGHTING);
	glColor3f(0.83f, 0.7f, 0.05f);
	char string[256];

	sprintf(string, "Vertex program: %s", (numVertexPrograms>0?vPrograms[currentVertexProgram].prog->getName().c_str():""));
	drawTextAt(string, 5, reshaper.height - 30);
	sprintf(string, "Fragment program: %s", (numFragmentPrograms>0?fPrograms[currentFragmentProgram].prog->getName().c_str():""));
	drawTextAt(string, 5, reshaper.height - 45);

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
}

void drawSkyBox(void) {
    glDisable(GL_DEPTH_TEST);

	GLuint env = TextureManager::instance().loadTextureCube(envFile);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, env);
	glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);

    // initialize object linear texgen
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    GLfloat s_plane[] = { 1.0, 0.0, 0.0, 0.0 };
    GLfloat t_plane[] = { 0.0, 1.0, 0.0, 0.0 };
    GLfloat r_plane[] = { 0.0, 0.0, 1.0, 0.0 };
    glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane);
    glTexGenfv(GL_R, GL_OBJECT_PLANE, r_plane);
    glPopMatrix();
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glScalef(10.0, 10.0, 10.0);
    glutSolidCube(1.0);
    glPopMatrix();

    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);

    glEnable(GL_DEPTH_TEST);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	if(b['s'])
		drawSkyBox();

	glEnable(GL_TEXTURE_2D);

    object.apply_transform();

	glColor3fv(color);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

    glPushMatrix();
    glVertexPointer(3, GL_FLOAT, 0, vertexdata);
    glNormalPointer(GL_FLOAT, 0, normaldata);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	if(b['g']) {
		if(numVertexPrograms > 0) {
			PropertyManager::instance().setPropertyMap("USER", &(vPrograms[currentVertexProgram].map));
			vPrograms[currentVertexProgram].prog->activate();
		}
		if(numFragmentPrograms > 0) {
			PropertyManager::instance().setPropertyMap("USER", &(fPrograms[currentFragmentProgram].map));
			fPrograms[currentFragmentProgram].prog->activate();
		}
	}
    glDrawElements(GL_TRIANGLES, nindices, GL_UNSIGNED_INT, indices);
	if(b['g']) {
		if(numVertexPrograms > 0) {
			vPrograms[currentVertexProgram].prog->deactivate();
		}
		if(numFragmentPrograms > 0) {
			fPrograms[currentFragmentProgram].prog->deactivate();
		}
	}
    glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	drawInfo();

	glutSwapBuffers();
}

void key(unsigned char k, int x, int y) {
    b[k] = ! b[k];
    if(k==27 || k=='q') exit(0);

    if (k == 'w' || k == 'W')
    {
        if (b[k])
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} else if(k == 'l') {
		numLights--;
		if(numLights < 0) numLights = 0;
	} else if(k == 'L') {
		numLights++;
		if(numLights > 4) numLights = 4;
	} else if(k == 'v') {
		currentVertexProgram++;
		if(currentVertexProgram >= numVertexPrograms)
			currentVertexProgram = 0;
	} else if(k == 'V') {
		currentVertexProgram--;
		if(currentVertexProgram < 0)
			currentVertexProgram = numVertexPrograms-1;
	} else if(k == 'f') {
		currentFragmentProgram++;
		if(currentFragmentProgram >= numFragmentPrograms)
			currentFragmentProgram = 0;
	} else if(k == 'F') {
		currentFragmentProgram--;
		if(currentFragmentProgram < 0)
			currentFragmentProgram = numFragmentPrograms-1;
	}

    glutPostRedisplay();
}

void special(int k, int x, int y) {
    glutPostRedisplay();
}

void idle() {
    if (b[' '])
        object.trackball.increment_rotation();

    glutPostRedisplay();
}

void resize(int w, int h) {
    if (h == 0) h = 1;
    
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 0.1, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}