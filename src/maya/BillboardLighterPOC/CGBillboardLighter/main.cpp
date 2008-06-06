#include <windows.h>

#undef min
#undef max

#define GLH_EXT_SINGLE_FILE

#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>
#include <nel/misc/quat.h>
#include <Cg/cgGL.h>
#include <glh/glh_extensions.h>
#include <glh/glh_glut.h>
#include "data_path.h"
#include "nv_dds.h"
#include <stdio.h>
#include <vector>

using namespace glh;

void display();
void idle();
void key(unsigned char k, int x, int y);
void init_opengl();
static void create_sphere_dl();
void DrawQuad(NLMISC::CVector pos, NLMISC::CVector x, NLMISC::CVector y);

glut_perspective_reshaper reshaper;

CGcontext shaderContext;
CGprogram vProgram;
CGprofile vProfile;
CGprofile fProfile;
CGprogram fProgram;

CGparameter Light;
CGparameter ModelViewProj;
CGparameter ModelViewIT;
CGparameter translucencyParam;
CGparameter useFaceNormal;


// #define _BL_PROGRESS_

float size = 3; 
int dim = 5;
int numSpheres = 4;
float spheresRadius = max(1.0, sqrt((float)numSpheres-1.0));;
float sphereRadius = 1;
float light_radius = 3.0;
float theta = 0.;
float billboard_radius = size / dim;
float translucency = 0.5;
float translucency_step = 0.05;
float uFaceNormal = 0.0;
float uFaceNormalUser = 4.0;
bool noShadow = true;
GLuint leaftexture;
int fps = 0;
int frameCount = 0;
DWORD lastTime;
DWORD timing = 0;
enum TBillboardAlignment { eViewAlign, eScreenAlign, eAutoAlign, eNumAlign };
enum TShadowMode { eNoShadows, eBBShadows, eNumShadow };
TBillboardAlignment align = eAutoAlign;
TShadowMode shadowMode = eNoShadows;

const float PI = 3.141592654;
const float theta_step = .02;
const float light_radius_step = .1;
const float min_light_radius = .5;

glut_simple_mouse_interactor camera, object;
glut_callbacks cb;
bool b[256];

NLMISC::CVector *sphereArray;
NLMISC::CVector *positionArray;
float *shadowArray;

GLuint draw_solid_sphere;

void fillVector(int seed, float radius, NLMISC::CVector spherePos, int sphereIndex) {
#ifdef _BL_PROGRESS_
	printf("Executing fillVector()\n");
#endif
	unsigned int amount = dim*dim;
	srand(seed);
	float radiusSq = radius*radius;
	for(int i = 0; i < amount; i++) {
		NLMISC::CVector pos;
		do {
			pos.x = ((float)rand()/(float)RAND_MAX)*2*radius - radius;
			pos.y = ((float)rand()/(float)RAND_MAX)*2*radius - radius;
			pos.z = ((float)rand()/(float)RAND_MAX)*2*radius - radius;
		} while(pos.sqrnorm() > radiusSq);
		positionArray[amount*sphereIndex + i] = pos + spherePos;
	}
#ifdef _BL_PROGRESS_
	printf("Done executing fillVector()\n");
#endif
}

void fillSpheresVector( bool noChange = false) {
#ifdef _BL_PROGRESS_
	printf("Executing fillSpheresVector()\n");
#endif
	if(!noChange) {
		if(sphereArray) {
			delete sphereArray;
		}
		sphereArray = new NLMISC::CVector[numSpheres];
		spheresRadius = max(1.0, sqrt((float)numSpheres-1.0));
		srand(10);
	}
	if(positionArray) {
		delete positionArray;
	}
	positionArray = new NLMISC::CVector[numSpheres * dim * dim];
	shadowArray = new float[numSpheres*dim*dim];
	noShadow = true;
	float radiusSquared = spheresRadius*spheresRadius;
	for(int i = 0; i < numSpheres; i++) {
		NLMISC::CVector pos;
		if(!noChange) {
			do {
				pos.x = ((float)rand()/(float)RAND_MAX)*2*spheresRadius - spheresRadius;
				pos.y = ((float)rand()/(float)RAND_MAX)*2*spheresRadius - spheresRadius;
				pos.z = ((float)rand()/(float)RAND_MAX)*2*spheresRadius - spheresRadius;
			} while(pos.sqrnorm() > radiusSquared);
			sphereArray[i] = pos;
		} else {
			pos = sphereArray[i];
		}
		fillVector((int)(pos.x*100), sphereRadius, pos, i);
	}
#ifdef _BL_PROGRESS_
	printf("Done executing fillSpheresVector()\n");
#endif
}

static void create_sphere_dl()
{
    draw_solid_sphere = glGenLists(1);
    glNewList(draw_solid_sphere, GL_COMPILE);
    glutSolidSphere(0.1, 15, 15);
    glEndList();
}

void cleanExit(int exitval)
{
	if(sphereArray) {
		delete sphereArray;
	}
	if(positionArray) {
		delete positionArray;
	}
    if (vProgram) cgDestroyProgram(vProgram);
	if (fProgram) cgDestroyProgram(fProgram);
    if (shaderContext) cgDestroyContext(shaderContext);
	
    exit(exitval);
}

void cgErrorCallback(void)
{
    CGerror LastError = cgGetError();
	
    if(LastError)
    {
        const char *Listing = cgGetLastListing(shaderContext);
        printf("\n---------------------------------------------------\n");
        printf("%s\n\n", cgGetErrorString(LastError));
        printf("%s\n", Listing);
        printf("---------------------------------------------------\n");
        printf("Cg error, exiting...\n");
        cleanExit(0);
    }
}

void init_opengl()
{
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.8);
    glClearColor(0.2, 0.2, 0.2, 1.0);
	reshaper.zFar = 1000;
    if (!glh_init_extensions("GL_EXT_secondary_color "))
    {
        printf("Necessary extensions unsupported: %s\n", glh_get_unsupported_extensions());
        cleanExit(0);
    }

	// check profile support
    if (cgGLIsProfileSupported(CG_PROFILE_ARBVP1)) {
        vProfile = CG_PROFILE_ARBVP1;
		printf("Using GL_ARB_vertex_program\n");
    } else if (cgGLIsProfileSupported(CG_PROFILE_VP20)) {
        vProfile = CG_PROFILE_VP20;
		printf("Using GL_NV_vertex_program\n");
    } else {
        printf("Vertex programming extensions (GL_ARB_vertex_program or "
			"GL_NV_vertex_program) not supported, exiting...\n");
        cleanExit(0);
    }
	
#ifdef _BL_PROGRESS_
	printf("Creating sphere display list\n");
#endif
	create_sphere_dl();
#ifdef _BL_PROGRESS_
	printf("Done creating sphere display list\n");
	printf("Filling spheres array\n");
#endif
	fillSpheresVector();
#ifdef _BL_PROGRESS_
	printf("Done filling spheres array\n");
#endif
	
#ifdef _BL_PROGRESS_
	printf("Adding media paths\n");
#endif
    data_path media;
    media.path.push_back(".");
	media.path.push_back("..");
	media.path.push_back("../Binary");
	media.path.push_back("../Sample Data");
	media.path.push_back("../Sample Data/sources");
#ifdef _BL_PROGRESS_
	printf("Done adding media paths\n");
#endif
	
	// Make texture
#ifdef _BL_PROGRESS_
	printf("Making texture\n");
#endif
	glGenTextures(1, &leaftexture);
    glBindTexture(GL_TEXTURE_2D, leaftexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
#ifdef _BL_PROGRESS_
	printf("Loading DDS image\n");
#endif
	nv_dds::CDDSImage image;
    if (image.load(media.get_file("DefaultLeaves.dds")))
        image.upload_texture2D();
	else
		printf("Unable to load texture\n");
#ifdef _BL_PROGRESS_
	printf("Done making texture\n");
#endif
	
	// do some cg initialization
#ifdef _BL_PROGRESS_
	printf("Initializing CG\n");
#endif
    cgSetErrorCallback(cgErrorCallback);
    shaderContext = cgCreateContext();
#ifdef _BL_PROGRESS_
	printf("Done initializing CG\n");
#endif
	
	// load vertex program
#ifdef _BL_PROGRESS_
	printf("Creating vertex program\n");
#endif
    string vpFilename = media.get_file("billboard_vertex_program.cg");
    if (vpFilename == "")
    {
        printf("Unable to load VP, exiting...\n");
        cleanExit(0);
    }
	
    // create the vertex program
    vProgram = cgCreateProgramFromFile(shaderContext, 
		CG_SOURCE, vpFilename.data(),
		vProfile, NULL, NULL);
	
#ifdef _BL_PROGRESS_
	printf("Loading vertex program\n");
#endif
    cgGLLoadProgram(vProgram);
	// get VP params
#ifdef _BL_PROGRESS_
	printf("Getting vertex program parameters\n");
#endif
    Light = cgGetNamedParameter(vProgram, "LightVec");
    ModelViewProj = cgGetNamedParameter(vProgram, "ModelViewProj");
	ModelViewIT = cgGetNamedParameter(vProgram, "ModelViewIT");
	translucencyParam = cgGetNamedParameter(vProgram, "translucency");
	useFaceNormal = cgGetNamedParameter(vProgram, "useFaceNormal");
	
	// check if we got them
    if (!Light || !ModelViewProj || !ModelViewIT || !translucencyParam || !useFaceNormal)
    {
        printf("Unable to retrieve vertex program parameters, exiting...\n");
        cleanExit(0);
    }
#ifdef _BL_PROGRESS_
	printf("Done creating vertex program\n");
#endif
   
}


// ray and sphere intersection test
bool raySphereIntersect(NLMISC::CVector start, NLMISC::CVector end, NLMISC::CVector center, float radius, float &disc) {
	end = end - start;
	end.normalize();
	start = center - start;
	disc = end * start;
	if(disc < 0.0f)
		return false;
	disc = radius*radius - (start.sqrnorm() - disc*disc);
	if(disc < 0.0f)
		return false;
	return true;
}

// sets the shadow of a billboard in a leaf cluster based on ray intersection tests with all the other leaf clusters
float setShadow(NLMISC::CVector pos, NLMISC::CVector lightPos, int currentSphere) {
	float shadow = 1;
	for(int i = 0; i < numSpheres; i++) {
		if(i == currentSphere)
			continue;
		float disc = 1;
		if(raySphereIntersect(pos, lightPos, sphereArray[i], sphereRadius, disc)) {
			disc = 1 - disc;
			shadow *= disc;
		}
	}
	shadow = shadow * (1 - translucency) + translucency;
	return shadow;
}
// draw a quad given the x and y vectors
void DrawQuad(NLMISC::CVector pos, NLMISC::CVector x, NLMISC::CVector y) {
	NLMISC::CVector left  = pos;  left   -= y; 
	NLMISC::CVector right = left; right  += x; 
	left  -= x;
	glNormal3f(pos.x, pos.y, pos.z);
	glTexCoord2f(0, 0); glVertex3f(left.x, left.y, left.z);
	glTexCoord2f(1, 0); glVertex3f(right.x, right.y, right.z);
	left  += y;  left  += y;
	right += y;  right += y;
	glTexCoord2f(1, 1); glVertex3f(right.x, right.y, right.z);
	glTexCoord2f(0, 1); glVertex3f(left.x, left.y, left.z);
}

void DrawTextAt(char *p, int x, int y) {
#ifdef _BL_PROGRESS_
	printf("Executing DrawTextAt(%s)\n", p);
#endif
	glRasterPos2i( x, y);
    for ( ; *p; p++ ) {
		glutBitmapCharacter( GLUT_BITMAP_8_BY_13, *p );
	}
#ifdef _BL_PROGRESS_
	printf("Done executing DrawTextAt(%s)\n", p);
#endif
}

void display()
{
#ifdef _BL_PROGRESS_
	printf("Executing display()\n");
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

#ifdef _BL_PROGRESS_
	printf("Applying camera transforms\n");
#endif
    camera.apply_inverse_transform();
    object.apply_transform();
	
#ifdef _BL_PROGRESS_
	printf("Binding vertex program\n");
#endif
    cgGLBindProgram(vProgram);

#ifdef _BL_PROGRESS_
	printf("Moving light\n");
#endif
    NLMISC::CVector light_position;
    float phi = theta / (2. * PI) * theta_step;
    light_position.x = light_radius * sin(theta);
    light_position.y = light_radius * cos(theta) * cos(phi);
    light_position.z = light_radius * cos(theta) * sin(phi);
	billboard_radius = size / (float)dim;
	
#ifdef _BL_PROGRESS_
	printf("Setting CG parameters\n");
#endif
    cgGLSetParameter1f(translucencyParam, translucency);
	uFaceNormal = uFaceNormalUser/(dim*dim);
	NLMISC::clamp(uFaceNormal, 0.0f, 1.0f);
   cgGLSetParameter1f(useFaceNormal, uFaceNormal);
	
    cgGLSetParameter4f(Light, light_position.x, light_position.y, light_position.z, 1.0);
    cgGLSetStateMatrixParameter(ModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(ModelViewIT, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	
#ifdef _BL_PROGRESS_
	printf("Disabling GL lighting\n");
#endif
    glDisable(GL_LIGHTING);
	
#ifdef _BL_PROGRESS_
	printf("Getting camera matrix info\n");
#endif
	// used to calculate billboard vectors
	matrix4f mo = object.get_inverse_transform();
	NLMISC::CMatrix mat;
	mat.set(mo.m);
	NLMISC::CVector cam = mat.getPos();
	NLMISC::CVector up = mat.getJ().normed();
	NLMISC::CVector normal = mat.getK().normed();
	NLMISC::CVector right = -mat.getI().normed();
	NLMISC::CVector x, y, z;
	NLMISC::CQuat rotQ;
	NLMISC::CQuat rotQconj;


	bool autoAlign = false;

	if(align == eScreenAlign) {
		z = normal;
		x = right;
		x *= billboard_radius;
		y = up;
		y *= billboard_radius;
	}

#ifdef _BL_PROGRESS_
	printf("Executing billboards loop\n");
#endif
	for(int j = 0; j < numSpheres; j++) {
#ifdef _BL_PROGRESS_
	printf("Drawing sphere %i\n", j);
#endif
		if(b['m'])
			glEnable(GL_TEXTURE_2D);
		else
			glDisable(GL_TEXTURE_2D);

		bool localAutoAlign = false;
		DWORD test = GetTickCount();
		NLMISC::CVector spherePos = sphereArray[j];
		// parameter to the VP - WARNING! DO NOT OVERWRITE THE SECONDARY OR PRIMARY COLOR!
		glSecondaryColor3fEXT(spherePos.x, spherePos.y, spherePos.z);
		
#ifdef _BL_PROGRESS_
	printf("Enabling vertex profile\n");
#endif
	cgGLEnableProfile(vProfile);
#ifdef _BL_PROGRESS_
	printf("Starting to draw quads\n");
#endif
		glBegin(GL_QUADS);
		int numBillboards = dim*dim;
		if(align == eAutoAlign) {
			NLMISC::CVector camDistance = cam - spherePos;
			if(abs(camDistance.sqrnorm()) > ((sphereRadius*sphereRadius)+ billboard_radius)) {
				z = normal;
				x = right;
				x *= billboard_radius;
				y = up;
				y *= billboard_radius;
			} else {
				autoAlign = localAutoAlign = true;
			}
		}

		for(int i = 0; i < numBillboards; i++) {
#ifdef _BL_PROGRESS_
	printf("Drawing billboards %i\n", i);
#endif
			int index = j*numBillboards + i;
			NLMISC::CVector pos = positionArray[index];
			if(shadowMode == eBBShadows) {
				if((noShadow || (frameCount%2 == 0)) &&  b[' ']) {
					shadowArray[index] = setShadow(pos, light_position, j);
				}
				glColor4f(shadowArray[index], shadowArray[index], shadowArray[index], 1);
			} else {
				glColor4f(1, 1, 1, 1);
			}

			// calculate billboard vectors and draw it
			if(align == eViewAlign || localAutoAlign) {
				z = cam;
				z -= pos;
				z.normalize();
				x = z^up;
				x.normalize();
				x *= billboard_radius;
				y = x^z;
				y.normalize();
				y *= billboard_radius;
			}
			if(b['g']) {
				NLMISC::CQuat xQ(x.x, x.y, x.z, 0);
				NLMISC::CQuat yQ(y.x, y.y, y.z, 0);
				rotQ.setAngleAxis(z, pos.y+pos.x+pos.z);
				rotQconj = rotQ.conjugate();
				xQ = rotQ*xQ*rotQconj;
				yQ = rotQ*yQ*rotQconj;
				DrawQuad(pos, NLMISC::CVector(xQ.x, xQ.y, xQ.z), NLMISC::CVector(yQ.x, yQ.y, yQ.z));
			} else {
				DrawQuad(pos, x, y);
			}
		}
		glEnd();
#ifdef _BL_PROGRESS_
	printf("Disabling vertex profile\n");
#endif

		cgGLDisableProfile(vProfile);
		glDisable(GL_TEXTURE_2D);
		// draw bounding spheres if needed
		if(b['s']) {
#ifdef _BL_PROGRESS_
	printf("Drawing bounding sphere\n");
#endif
			glPushMatrix();
			glTranslatef(spherePos.x, spherePos.y, spherePos.z);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glColor3f(0.42f, 0.75f, 0.2f);
			glutSolidSphere(sphereRadius, 10, 10);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glPopMatrix();
		}
		timing += GetTickCount() - test;
	}
	timing /= numSpheres;
	
#ifdef _BL_PROGRESS_
	printf("Drawing light\n");
#endif
	// draw light as a sphere
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(light_position.x, light_position.y, light_position.z);
	glCallList(draw_solid_sphere);
	
#ifdef _BL_PROGRESS_
	printf("Calculating FPS\n");
#endif
	// fps stuff
	frameCount++;
	if(frameCount > 50) {
		DWORD currentTime = GetTickCount();
		fps = ((float)frameCount/(currentTime - lastTime)) * 1000;
		lastTime = currentTime;
		frameCount = 0;
	}
	
#ifdef _BL_PROGRESS_
	printf("Setting up GL to draw text\n");
#endif
	// draw some text
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( 0, reshaper.width-1, 0, reshaper.height-1 );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();
	glColor3f(0.83f, 0.7f, 0.05f);
#ifdef _BL_PROGRESS_
	printf("Drawing text\n");
#endif
	char string[256];
	switch(align) {
		case eViewAlign:
	        sprintf( string, "View aligned billboards" ); break;
		case eScreenAlign:
	        sprintf( string, "Screen aligned billboards" ); break;
		case eAutoAlign:
			sprintf( string, "Auto aligned billboards (%s)", autoAlign ? "ON" : "OFF"); break;
		default:
	        sprintf( string, "Unknown alignment" ); break;
	}
	DrawTextAt( string, 5, 25);
	if ( b['g'] )
        sprintf( string, "Billboard rotation ENABLED" );
    else
        sprintf( string, "Billboard rotation DISABLED" );
	DrawTextAt( string, 5, 10);
	switch(shadowMode) {
		case eNoShadows:
	        sprintf( string, "No shadows" ); break;
		case eBBShadows:
	        sprintf( string, "Billboard based shadows" ); break;
		default:
	        sprintf( string, "Unknows shadow mode" ); break;
	}
	DrawTextAt( string, 5, 40);

	sprintf( string, "               FPS: %4i", fps );
	DrawTextAt( string, reshaper.width - 210, reshaper.height - 15 );
	sprintf( string, "          #spheres: %4i",  numSpheres);
	DrawTextAt( string, reshaper.width - 210, reshaper.height - 30 );
	sprintf( string, "#billboards/sphere: %4i", dim*dim);
	DrawTextAt( string, reshaper.width - 210, reshaper.height - 45 );
	sprintf( string, " total #billboards: %4i", dim*dim*numSpheres);
	DrawTextAt( string, reshaper.width - 210, reshaper.height - 60 );
	sprintf( string, "     polygon count: %4i", dim*dim*numSpheres*2);
	DrawTextAt( string, reshaper.width - 210, reshaper.height - 75 );
	sprintf( string, "      translucency: %1.2f", translucency);
	DrawTextAt( string, reshaper.width - 210, reshaper.height - 90 );
	sprintf( string, "  face normal infl: %1.2f", uFaceNormal);
	DrawTextAt( string, reshaper.width - 210, reshaper.height - 105 );
	sprintf( string, "  face normal ctrl: %4.0f", uFaceNormalUser);
	DrawTextAt( string, reshaper.width - 210, reshaper.height - 120 );
	sprintf( string, "      timing value: %8d ms", timing);
	DrawTextAt( string, reshaper.width - 250, 10 );
	
	
	// help
	sprintf( string, "Press 'h' for help." );
	DrawTextAt( string, 5, reshaper.height - 15 );
	if(b['h']) {
		sprintf( string, "Keys: " );
		DrawTextAt( string, 5, reshaper.height - 30 );
		sprintf( string, "  'l'    -  toggle billboard lighting");
		DrawTextAt( string, 5, reshaper.height - 45 );
		sprintf( string, "  's'    -  toggle bounding spheres");
		DrawTextAt( string, 5, reshaper.height - 60 );
		sprintf( string, "  'w'    -  toggle wireframe");
		DrawTextAt( string, 5, reshaper.height - 75 );
		sprintf( string, "  'a'    -  toggle screen aligned billboards");
		DrawTextAt( string, 5, reshaper.height - 90 );
		sprintf( string, "  'd'/'D'-  increase/decrease number of billboards per sphere");
		DrawTextAt( string, 5, reshaper.height - 105 );
		sprintf( string, "  'n'/'N'-  increase/decrease number of spheres");
		DrawTextAt( string, 5, reshaper.height - 120 );
		sprintf( string, "  '+'/'-'-  increase/decrease size of billboards");
		DrawTextAt( string, 5, reshaper.height - 135 );
		sprintf( string, "  't'/'T'-  increase/decrease translucency");
		DrawTextAt( string, 5, reshaper.height - 150 );
		sprintf( string, "  'r'/'R'-  increase/decrease light circling radius");
		DrawTextAt( string, 5, reshaper.height - 165 );
		sprintf( string, "spacebar -  pause light movement");
		DrawTextAt( string, 5, reshaper.height - 180 );
		sprintf( string, "  'g'    -  toggle per billboard rotation");
		DrawTextAt( string, 5, reshaper.height - 195 );
		sprintf( string, "  'm'    -  toggle textures");
		DrawTextAt( string, 5, reshaper.height - 210 );
		sprintf( string, "  'z'/'Z'-  increase/decrease face normal influence");
		DrawTextAt( string, 5, reshaper.height - 225 );
		sprintf( string, "  'x'    -  toggle shadow mode");
		DrawTextAt( string, 5, reshaper.height - 240 );
	}
#ifdef _BL_PROGRESS_
	printf("Popping GL state\n");
#endif
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
	glutSwapBuffers();

#ifdef _BL_PROGRESS_
	printf("Unbinding CG programs\n");
#endif
	cgGLUnbindProgram(vProfile);

	noShadow = false;

#ifdef _BL_PROGRESS_
	printf("Done executing display()\n");
#endif
}

void key(unsigned char k, int x, int y)
{
#ifdef _BL_PROGRESS_
	printf("Executing key()\n");
#endif
    b[k] = ! b[k];
    if(k==27 || k=='q') cleanExit(0);
	
    if (k == 'w' || k == 'W')
    {
        if (b[k])
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
	
	if ('d' == k) {
        dim++;
		fillSpheresVector(true);
	} else if ('D' == k && dim > 1) {
		dim--;
		fillSpheresVector(true);
	}
	
	if ('+' == k) {
		size += 0.5;
	} else if ('-' == k) {
		size -= 0.5;
	}
	
	if ('n' == k) {
        numSpheres++;
		fillSpheresVector();
	} else if ('N' == k && numSpheres > 1) {
		numSpheres--;
		fillSpheresVector();
	}

	if('a' == k) {
		align = (TBillboardAlignment)(((int)align + 1) % eNumAlign);
	}

	if('x' == k) {
		shadowMode = (TShadowMode)(((int)shadowMode + 1) % eNumShadow);
	}

	if ('t' == k && translucency < 1.0)
        translucency += translucency_step;
	else if ('T' == k && translucency > 0.0)
		translucency -= translucency_step;

	if ('r' == k)
        light_radius++;
	else if ('R' == k)
		light_radius--;

	if ('z' == k)
        uFaceNormalUser++;
	else if ('Z' == k && uFaceNormalUser > 0.0)
		uFaceNormalUser--;

	glutPostRedisplay();
#ifdef _BL_PROGRESS_
	printf("Done executing key()\n");
#endif
}

void idle()
{
#ifdef _BL_PROGRESS_
	printf("Executing idle()\n");
#endif
    if (b[' '])
		theta += theta_step;
	
    glutPostRedisplay();
#ifdef _BL_PROGRESS_
	printf("Done executing idle()\n");
#endif
}

void resize(int w, int h)
{
#ifdef _BL_PROGRESS_
	printf("Executing resize()\n");
#endif
    if (h == 0) h = 1;
    
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 0.1, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#ifdef _BL_PROGRESS_
	printf("Done executing resize()\n");
#endif
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
	glutCreateWindow("Cg Billboard Lighter - Author: RTSan aka Henri Kuuste");
	glutReshapeWindow(700, 500);
	
#ifdef _BL_PROGRESS_
	printf("Initializing OpenGL\n");
#endif
	init_opengl();
#ifdef _BL_PROGRESS_
	printf("Done initializing OpenGL\n");
#endif

	lastTime = GetTickCount();

#ifdef _BL_PROGRESS_
	printf("Initializing glut helpers\n");
#endif
    glut_helpers_initialize();
#ifdef _BL_PROGRESS_
	printf("Done initializing glut helpers\n");
#endif
	
#ifdef _BL_PROGRESS_
	printf("Initializing camera\n");
#endif
    camera.configure_buttons(1);
    camera.set_camera_mode(true);
    object.configure_buttons(1);
	
    object.dolly.dolly[2] = -8;
    object.trackball.r.set_value(vec3f(1, 0, 0), -PI/4.);
#ifdef _BL_PROGRESS_
	printf("Done initializing camera\n");
#endif
	
#ifdef _BL_PROGRESS_
	printf("Initializing keyboard\n");
#endif
    cb.keyboard_function = key;
#ifdef _BL_PROGRESS_
	printf("Done initializing keyboard\n");
#endif

#ifdef _BL_PROGRESS_
	printf("Initializing interactors\n");
#endif
    glut_add_interactor(&cb);
    glut_add_interactor(&object);
    glut_add_interactor(&reshaper);
#ifdef _BL_PROGRESS_
	printf("Done initializing interactors\n");
#endif
	
#ifdef _BL_PROGRESS_
	printf("Initializing glut functions\n");
#endif
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
#ifdef _BL_PROGRESS_
	printf("Done initializing glut functions\n");
#endif
	
    b[' '] = true;
	b['g'] = true;
	b['m'] = true;
	
#ifdef _BL_PROGRESS_
	printf("Starting main loop\n");
#endif
    glutMainLoop();
#ifdef _BL_PROGRESS_
	printf("End main loop\n");
#endif

	if(sphereArray) {
		delete sphereArray;
	}
	if(positionArray) {
		delete positionArray;
	}
    if (vProgram) cgDestroyProgram(vProgram);
    if (shaderContext) cgDestroyContext(shaderContext);
	
    return 0;
}
