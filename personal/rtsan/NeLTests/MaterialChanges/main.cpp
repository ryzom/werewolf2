#include <nel/misc/types_nl.h>

#ifdef NL_OS_WINDOWS
#	include <windows.h>
#	undef min
#	undef max
#endif

#include <cstdio>

#include <nel/misc/time_nl.h>
#include <nel/misc/path.h>
#include <nel/misc/config_file.h>
#include <nel/misc/vectord.h>
#include <3d/event_mouse_listener.h>
#include <3d/mesh_instance.h>
#include <nel/3d/u_point_light.h>

#include <nel/3d/u_driver.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_instance.h>
#include <nel/misc/value_smoother.h>
#include <nel/misc/file.h>
#include <3d/texture_file.h>

#include <vector>

using namespace std;
using namespace NLMISC;
using namespace NL3D;

CConfigFile			 ConfigFile;

// The 3d driver
UDriver					*Driver = NULL;

// This is the main scene
UScene					*Scene = NULL;

// The previous and current frame dates
TTime					LastTime, NewTime;

// This variable is used to display text on the screen
UTextContext			*TextContext = NULL;

UInstance				Instance;
UPointLight				pl;

UCamera					Camera;

// true if you want to exit the main loop
static bool				NeedExit = false;

CEvent3dMouseListener	MouseListener;

NLMISC::CValueSmootherTemplate<double> m_DeltaTimeSmooth;

const char* matString = "data/mat%u.material";
unsigned int numMaterials = 0;
unsigned int currentMaterial = 0;
std::vector<CMaterial> materials;

bool showTextureInfo = false;
CConfigFile::CVar textureVar;
std::vector<CSmartPtr<ITexture> > textures;
unsigned int numTextures = 0;
unsigned int currentTexture = 0;
unsigned int currentStage = 0;
CMaterial::TTexOperator currentOpRGB = CMaterial::Modulate;
CMaterial::TTexOperator currentOpAlpha = CMaterial::Modulate;
CMaterial::TTexSource currentRGBSource0 = CMaterial::Previous;
CMaterial::TTexSource currentRGBSource1 = CMaterial::Texture;
CMaterial::TTexOperand currentRGBOperand0 = CMaterial::SrcColor;
CMaterial::TTexOperand currentRGBOperand1 = CMaterial::SrcColor;
CMaterial::TTexSource currentAlphaSource0 = CMaterial::Previous;
CMaterial::TTexSource currentAlphaSource1 = CMaterial::Texture;
CMaterial::TTexOperand currentAlphaOperand0 = CMaterial::SrcAlpha;
CMaterial::TTexOperand currentAlphaOperand1 = CMaterial::SrcAlpha;
const char* operators[] = {"Replace", "Modulate", "Add", "AddSigned", 
	"InterpolateTexture", "InterpolatePrevious", "InterpolateDiffuse", "InterpolateConstant", "EMBM", "Mad" };
const char* sources[] = {"Texture", "Previous", "Diffuse", "Constant"};
const char* operands[] = {"SrcColor", "InvSrcColor", "SrcAlpha", "InvSrcAlpha"};
bool enableTextureCoordGen = false;
CMaterial::TTexCoordGenMode currentCoordGen = CMaterial::TexCoordGenReflect;
const char* coordGenModes[] = {"TexCoordGenReflect", "TexCoordGenObjectSpace", "TexCoordGenEyeSpace"};

void createMaterials();
void init();
void mainLoop();
void preloadMaterials();
void preloadTextures();
void nextMaterial();
void nextTexture();
void nextStage();
void nextOperator(CMaterial::TTexOperator& op);
void nextOperand(CMaterial::TTexOperand& op);
void nextSource(CMaterial::TTexSource& src);
void nextCoordGen();
void assignTexture();

int main(int argc, char **argv) {
	init();
	createMaterials();
	preloadTextures();
	preloadMaterials();
	mainLoop();
	// Release the 3d driver
	MouseListener.removeFromServer(Driver->EventServer);
	Scene->deleteInstance(Instance);

	delete Driver;

	// Exit
	return EXIT_SUCCESS;
}

void init() {
	m_DeltaTimeSmooth.init(1000);

	ConfigFile.load ("material_changes.cfg");
	CPath::addSearchPath(ConfigFile.getVar("DataPath").asString (), true, false);
	Driver = UDriver::createDriver();

	// Create the window with config file values
	Driver->setDisplay (UDriver::CMode(ConfigFile.getVar("ScreenWidth").asInt(), 
									   ConfigFile.getVar("ScreenHeight").asInt(),
									   ConfigFile.getVar("ScreenDepth").asInt(),
									   ConfigFile.getVar("ScreenFull").asInt()==0));
	Driver->setSwapVBLInterval(0);

	// Set the cache size for the font manager (in bytes)
	Driver->setFontManagerMaxMemory (2000000);

	MouseListener.addToServer(Driver->EventServer);

	// Create a Text context for later text rendering
	TextContext = Driver->createTextContext (CPath::lookup(ConfigFile.getVar("FontName").asString ()));
	
	// Create a scene
	Scene = Driver->createScene(false);

	nlinfo ("Testing material changes in NeL");
	nlinfo ("");
	nlinfo ("Press ESC to exit...");
	// Get the current time

	Instance = Scene->createInstance (ConfigFile.getVar("Model").asString());
	CConfigFile::CVar cv;

	textureVar = ConfigFile.getVar("Textures");
	numTextures = textureVar.size();

	pl = Scene->createPointLight();
	cv = ConfigFile.getVar("LightPosition");
	pl.setPos(cv.asFloat(0), cv.asFloat(1), cv.asFloat(2));
	cv = ConfigFile.getVar("LightColor");
	pl.setDiffuse(CRGBA(cv.asInt(0), cv.asInt(1), cv.asInt(2)));

	cv=ConfigFile.getVar("SunAmbientColor");
	NLMISC::CRGBA landscapeSunAmbient(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	cv=ConfigFile.getVar("SunDiffuseColor");
	NLMISC::CRGBA landscapeSunDiffuse(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	cv=ConfigFile.getVar("SunSpecularColor");
	NLMISC::CRGBA landscapeSunSpecular(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	cv=ConfigFile.getVar("SunDirection");
	NLMISC::CVector sunDirection(cv.asFloat(0), cv.asFloat(1), cv.asFloat(2));

	Scene->setSunDirection(sunDirection);
	Scene->setSunAmbient(landscapeSunAmbient);
	Scene->setSunDiffuse(landscapeSunDiffuse);
	Scene->setSunSpecular(landscapeSunSpecular);
	Scene->enableLightingSystem(true);

	Camera = Scene->getCam();
	Camera.setTransformMode (UTransformable::DirectMatrix);
	Camera.setPerspective ((float)Pi/2.f, 1.33f, 0.1f, 1000);

	CAABBox bbox;
	Instance.getShapeAABBox(bbox);
	Camera.lookAt (CVector(0.7f, -0.7f, 0.7f) * bbox.getRadius(), CVector(0, 0, 0));

	MouseListener.setMatrix (Camera.getMatrix());
	MouseListener.setFrustrum (Camera.getFrustum());
	MouseListener.setViewport (Driver->getViewport());
	MouseListener.setHotSpot (CVector(0,0,0));
	MouseListener.setMouseMode (CEvent3dMouseListener::edit3d);
	MouseListener.enableModelMatrixEdition(false);
	MouseListener.enableTranslateXYInWorld(false);

	NewTime = CTime::getLocalTime();
}

void mainLoop() {
	while ((!NeedExit) && Driver->isActive()) {
		// Clear all buffers
		Driver->clearBuffers (CRGBA (0, 0, 0));

		// Update the time counters
		LastTime = NewTime;
		NewTime = CTime::getLocalTime();

		// Set new animation date
		Scene->animate (float(NewTime)/1000);

		// Render
		Scene->render ();

		float lineHeight = ConfigFile.getVar("LineHeight").asFloat();

		// Display the frame rate
		double dt = (double)(NewTime - LastTime);
		m_DeltaTimeSmooth.addValue(dt/1000);
		float fps = m_DeltaTimeSmooth.getSmoothValue() ? 1.0 / m_DeltaTimeSmooth.getSmoothValue() : 0.0;
		TextContext->setHotSpot (UTextContext::BottomRight);
		TextContext->setColor (CRGBA(255, 255, 255, 255));
		TextContext->setFontSize (14);
		TextContext->printfAt (ConfigFile.getVar("FpsX").asFloat(),
			ConfigFile.getVar("FpsY").asFloat(), "%.0ffps", fps);

		TextContext->setHotSpot (UTextContext::BottomLeft);
		TextContext->printfAt (ConfigFile.getVar("MaterialX").asFloat(),
			ConfigFile.getVar("MaterialY").asFloat(), "(m) Material: %2u", currentMaterial);

		if(showTextureInfo) {
			TextContext->setHotSpot (UTextContext::TopLeft);
			float x = ConfigFile.getVar("TextureX").asFloat();
			float y = ConfigFile.getVar("TextureY").asFloat();
			TextContext->printfAt (x, y,                "(t) Assign");
			TextContext->printfAt (x, y-(1*lineHeight), "(t) Texture        : %s", textureVar.asString(currentTexture).c_str());
			TextContext->printfAt (x, y-(2*lineHeight), "(s) Stage          : %u", currentStage);
			TextContext->printfAt (x, y-(3*lineHeight), "(1) RGB Operator   : %s", operators[currentOpRGB]);
			TextContext->printfAt (x, y-(4*lineHeight), "(2) Alpha Operator : %s", operators[currentOpAlpha]);
			TextContext->printfAt (x, y-(5*lineHeight), "(3) RGB Source 0   : %s", sources[currentRGBSource0]);
			TextContext->printfAt (x, y-(6*lineHeight), "(4) RGB Source 1   : %s", sources[currentRGBSource1]);
			TextContext->printfAt (x, y-(7*lineHeight), "(5) RGB Operand 0  : %s", operands[currentRGBOperand0]);
			TextContext->printfAt (x, y-(8*lineHeight), "(6) RGB Operand 1  : %s", operands[currentRGBOperand1]);
			TextContext->printfAt (x, y-(9*lineHeight), "(7) Alpha Source 0 : %s", sources[currentAlphaSource0]);
			TextContext->printfAt (x, y-(10*lineHeight), "(8) Alpha Source 1 : %s", sources[currentAlphaSource1]);
			TextContext->printfAt (x, y-(11*lineHeight), "(9) Alpha Operand 0: %s", operands[currentAlphaOperand0]);
			TextContext->printfAt (x, y-(12*lineHeight), "(0) Alpha Operand 1: %s", operands[currentAlphaOperand1]);
			TextContext->printfAt (x, y-(13*lineHeight), "(e) Coord Gen      : %s", enableTextureCoordGen?"true":"false");
			TextContext->printfAt (x, y-(14*lineHeight), "(g) Coord Gen Mode : %s", coordGenModes[currentCoordGen]);
		}

//		TextContext->setHotSpot (UTextContext::MiddleTop);
//		TextContext->printfAt (0.5f, 0.99f, "THE TEST APPLICATION NO. 01!!!");
		Driver->swapBuffers ();

		// Pump user input messages
		Driver->EventServer.pump();

		// Manage the keyboard

		if (Driver->AsyncListener.isKeyDown (KeyESCAPE))
		{
			NeedExit = true;
		}
		if (Driver->AsyncListener.isKeyPushed(KeyM)) {
			nextMaterial();
		}
		if (Driver->AsyncListener.isKeyPushed(KeyT)) {
			nextTexture();
		}
		if (Driver->AsyncListener.isKeyPushed(KeyS)) {
			nextStage();
		}
		if (Driver->AsyncListener.isKeyPushed(KeyA)) {
			assignTexture();
		}
		if (Driver->AsyncListener.isKeyPushed(KeyD)) {
			showTextureInfo = !showTextureInfo;
		}
		if (Driver->AsyncListener.isKeyPushed(Key1)) {
			nextOperator(currentOpRGB);
		}
		if (Driver->AsyncListener.isKeyPushed(Key2)) {
			nextOperator(currentOpAlpha);
		}
		if (Driver->AsyncListener.isKeyPushed(Key3)) {
			nextSource(currentRGBSource0);
		}
		if (Driver->AsyncListener.isKeyPushed(Key4)) {
			nextSource(currentRGBSource1);
		}
		if (Driver->AsyncListener.isKeyPushed(Key5)) {
			nextOperand(currentRGBOperand0);
		}
		if (Driver->AsyncListener.isKeyPushed(Key6)) {
			nextOperand(currentRGBOperand1);
		}
		if (Driver->AsyncListener.isKeyPushed(Key7)) {
			nextSource(currentAlphaSource0);
		}
		if (Driver->AsyncListener.isKeyPushed(Key8)) {
			nextSource(currentAlphaSource1);
		}
		if (Driver->AsyncListener.isKeyPushed(Key9)) {
			nextOperand(currentAlphaOperand0);
		}
		if (Driver->AsyncListener.isKeyPushed(Key0)) {
			nextOperand(currentAlphaOperand1);
		}
		if (Driver->AsyncListener.isKeyPushed(KeyE)) {
			enableTextureCoordGen = !enableTextureCoordGen;
		}
		if (Driver->AsyncListener.isKeyPushed(KeyG)) {
			nextCoordGen();
		}
		Camera.setMatrix (MouseListener.getViewMatrix());
	}
	nlinfo("Exiting...");
}

void createMaterials() {
	COFile file;
	char buffer[256];

	CMaterial *mat0 = Instance.getMaterial(0).getObjectPtr();

	sprintf(buffer, matString, numMaterials);
	if(file.open(buffer)) {
		try {
			mat0->serial(file);
			numMaterials++;
		} catch(...) {
			nlwarning("Creating material 0 failed");
		}
		file.close();
	}

	CMaterial mat1;
	mat1.initLighted();
	mat1.setColor(CRGBA::White);
	mat1.setLighting(true, CRGBA::Black, CRGBA::Black, CRGBA::White, CRGBA::Black, 0);
	mat1.setOpacity(255);

	sprintf(buffer, matString, numMaterials);
	if(file.open(buffer)) {
		try {
			mat1.serial(file);
			numMaterials++;
		} catch(...) {
			nlwarning("Creating material 1 failed");
		}
		file.close();
	}

	mat1.initLighted();
	mat1.setColor(CRGBA::Green);
	mat1.setLighting(true, CRGBA::Black, CRGBA::Black, CRGBA::Green, CRGBA::Yellow, 50);
	mat1.setOpacity(255);

	sprintf(buffer, matString, numMaterials);
	if(file.open(buffer)) {
		try {
			mat1.serial(file);
			numMaterials++;
		} catch(...) {
			nlwarning("Creating material 1 failed");
		}
		file.close();
	}

	mat1.initLighted();
	mat1.setColor(CRGBA::White);
	mat1.setLighting(true, CRGBA::Black, CRGBA::Black, CRGBA::White, CRGBA::Yellow, 60);
	mat1.setOpacity(255);

	ITexture* tex;
	tex = new CTextureFile(ConfigFile.getVar("Textures").asString(0));
	mat1.setTexture(0, tex);
	mat1.setTexCoordGen (0, true);
	mat1.setTexCoordGenMode(0, NL3D::CMaterial::TexCoordGenReflect);
	mat1.texEnvOpRGB(0, NL3D::CMaterial::Modulate);
	mat1.texEnvArg0RGB(0, NL3D::CMaterial::Diffuse, NL3D::CMaterial::SrcColor);
	mat1.texEnvArg1RGB(0, NL3D::CMaterial::Texture, NL3D::CMaterial::SrcColor);
	mat1.texEnvOpAlpha(0, NL3D::CMaterial::Modulate);
	mat1.texEnvArg0Alpha(0, NL3D::CMaterial::Diffuse, NL3D::CMaterial::SrcAlpha);
	mat1.texEnvArg1Alpha(0, NL3D::CMaterial::Texture, NL3D::CMaterial::SrcAlpha);

	sprintf(buffer, matString, numMaterials);
	if(file.open(buffer)) {
		try {
			mat1.serial(file);
			numMaterials++;
		} catch(...) {
			nlwarning("Creating material 1 failed");
		}
		file.close();
	}
}

void preloadMaterials() {
	for(unsigned int i = 0; i < numMaterials; ++i) {
		char buffer[256];
		sprintf(buffer, matString, i);
		CIFile file;
		CMaterial mat;
		if(file.open(buffer)) {
			try {
				mat.serial(file);
				materials.push_back(mat);
			} catch(...) {
				nlwarning("Failed to load material %s", buffer);
			}
			file.close();
		}
	}
	numMaterials = materials.size();
}

void preloadTextures() {
	for(unsigned int i = 0; i < numTextures; ++i) {
		CSmartPtr<ITexture> tex = (textureVar.asString(i) == "")?NULL:new CTextureFile(textureVar.asString(i));
		textures.push_back(tex);
	}
}

void nextMaterial() {
	if(numMaterials == 0)
		return;
	currentMaterial++;
	if(currentMaterial >= numMaterials)
		currentMaterial = 0;
	CMaterial* mat = Instance.getMaterial(0).getObjectPtr();
	(*mat) = materials[currentMaterial];
}

void nextTexture() {
	if(numTextures == 0)
		return;
	currentTexture++;
	if(currentTexture >= numTextures)
		currentTexture = 0;
}

void nextStage() {
	currentStage++;
	if(currentStage >= IDRV_MAT_MAXTEXTURES)
		currentStage = 0;
}

void assignTexture() {
	if(numTextures == 0)
		return;
	CMaterial* mat = Instance.getMaterial(0).getObjectPtr();
	mat->setTexture(currentStage, textures[currentTexture]);
	mat->setTexCoordGen (currentStage, enableTextureCoordGen);
	mat->setTexCoordGenMode(currentStage, currentCoordGen);
	mat->texEnvOpRGB(currentStage, currentOpRGB);
	mat->texEnvArg0RGB(currentStage, currentRGBSource0, currentRGBOperand0);
	mat->texEnvArg1RGB(currentStage, currentRGBSource1, currentRGBOperand1);
	mat->texEnvOpAlpha(currentStage, currentOpAlpha);
	mat->texEnvArg0Alpha(currentStage, currentAlphaSource0, currentAlphaOperand0);
	mat->texEnvArg1Alpha(currentStage, currentAlphaSource1, currentAlphaOperand1);
}

void nextOperator(CMaterial::TTexOperator& op) {
	op = (CMaterial::TTexOperator)(op + 1);
	if(op >= CMaterial::TexOperatorCount)
		op = CMaterial::Replace;
}

void nextOperand(CMaterial::TTexOperand& op) {
	op = (CMaterial::TTexOperand)(op + 1);
	if(op >= CMaterial::TexOperandCount)
		op = CMaterial::SrcColor;
}

void nextSource(CMaterial::TTexSource& src) {
	src = (CMaterial::TTexSource)(src + 1);
	if(src >= CMaterial::TexSourceCount)
		src = CMaterial::Texture;
}

void nextCoordGen() {
	currentCoordGen = (CMaterial::TTexCoordGenMode)(currentCoordGen + 1);
	if(currentCoordGen >= CMaterial::numTexCoordGenMode)
		currentCoordGen = CMaterial::TexCoordGenReflect;
}