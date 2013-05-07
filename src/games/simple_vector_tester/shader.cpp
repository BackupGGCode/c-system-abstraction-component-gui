
// Source mostly from
//  http://www.opengl.org/wiki/Tutorial:_OpenGL_3.1_The_First_Triangle_(C%2B%2B/Win)
//   ported as a SACK plugin module.
//  Updated to support modelview projection...

#define MAKE_RCOORD_SINGLE
#define NO_FILEOP_ALIAS
#include <stdhdrs.h>
#define USE_RENDER_INTERFACE l.pri
#define USE_IMAGE_INTERFACE l.pii
#define NEED_VECTLIB_COMPARE

// define local instance.
#define TERRAIN_MAIN_SOURCE  
#include <vectlib.h>
#include <render.h>
#include <render3d.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "local.h"




void InitPerspective( void )
{
	RCOORD n = 1;  // near
	RCOORD f = 30000;  // far	
	RCOORD lt = -1;
	RCOORD r = 1;
	RCOORD b = -1;
	RCOORD t = 1;

	l.projection[0] = ( 2*n / (r-lt));
	l.projection[4] = 0;
	l.projection[8] = (r+lt)/(r-lt);
	l.projection[12] = 0;

	l.projection[1] = 0;
	l.projection[5] = ( 2*n )/(t-b);
	l.projection[9] =  (t+b)/(t-b);
	l.projection[13] = 0;

	l.projection[2] = 0;
	l.projection[6] = 0;
	l.projection[10] = -(f+n)/(f-n);
	l.projection[14] = -(2*f*n)/(f-n);

	l.projection[3] = 0;
	l.projection[7] = 0;
	l.projection[11] = -1;
	l.projection[15] = 0;

}

void InitShader( void )
{
	GLint result;
	const char *simple_color_vertex_source = "#version 140\n"
 
	                                         "uniform mat4 worldView;\n"
	                                         "uniform mat4 Projection;\n"
	                                         "in  vec3 in_Position;\n"
	                                         "in  vec3 in_Color;\n"
	                                         "out vec3 ex_Color;\n"
 
	                                         "void main(void)\n"
	                                         "{\n"
											 "	gl_Position = Projection * worldView * vec4(in_Position, 1.0);\n"
	                                         "	ex_Color = in_Color;\n"
	                                         "}\n";

	const char *simple_color_pixel_source = "#version 140\n"
 
	                                         //"precision highp float; // needed only for version 1.30\n"
 
	                                         "in  vec3 ex_Color;\n"
	                                         "out vec4 out_Color;\n"
 
	                                         "void main(void)\n"
	                                         "{\n"
	                                         "	out_Color = vec4(ex_Color,1.0);\n"
	                                         "}\n"
											 "\n";

	{
		const char **codeblocks;

		if (GLEW_OK != glewInit() )
		{
			// okay let's just init glew.
			return;
		}
		glEnable(GL_FRAGMENT_PROGRAM_ARB);
		glEnable(GL_VERTEX_PROGRAM_ARB);

		l.shader.simple_shader.shader = glCreateProgram();

		//Obtain a valid handle to a vertex shader object.
		l.shader.simple_shader.vert_shader = glCreateShader(GL_VERTEX_SHADER);

		codeblocks = &simple_color_vertex_source;
		//Now, compile the shader source. 
		//Note that glShaderSource takes an array of chars. This is so that one can load multiple vertex shader files at once.
		//This is similar in function to linking multiple C++ files together. Note also that there can only be one "void main" definition
		//In all of the linked source files that are compiling with this funciton.
		glShaderSource(
			l.shader.simple_shader.vert_shader, //The handle to our shader
			1, //The number of files.
			codeblocks, //An array of const char * data, which represents the source code of theshaders
			NULL); //An array of string leng7ths. For have null terminated strings, pass NULL.
	 
		//Attempt to compile the shader.
		glCompileShader(l.shader.simple_shader.vert_shader);
		{
			//Error checking.
			glGetObjectParameterivARB(l.shader.simple_shader.vert_shader, GL_OBJECT_COMPILE_STATUS_ARB, &result);
			if (!result)
			{
				GLint length;
				GLsizei final;
				char *buffer;
				//We failed to compile.
				lprintf("Vertex shader 'program A' failed compilation.\n");
				//Attempt to get the length of our error log.
				glGetObjectParameterivARB(l.shader.simple_shader.vert_shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
				buffer = NewArray( char, length );
				//Create a buffer.
					
				//Used to get the final length of the log.
				glGetInfoLogARB(l.shader.simple_shader.vert_shader, length, &final, buffer);
				//Convert our buffer into a string.
				lprintf( "message: %s", buffer );


				if (final > length)
				{
					//The buffer does not contain all the shader log information.
					printf("Shader Log contained more information!\n");
				}
		
			}
		}

		l.shader.simple_shader.frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
		codeblocks[0] = simple_color_pixel_source;
		glShaderSource(
			l.shader.simple_shader.frag_shader, //The handle to our shader
			1, //The number of files.
			codeblocks, //An array of const char * data, which represents the source code of theshaders
			NULL); //An array of string lengths. For have null terminated strings, pass NULL.
	 
		//Attempt to compile the shader.
		glCompileShader(l.shader.simple_shader.frag_shader);

		{
			//Error checking.
			glGetObjectParameterivARB(l.shader.simple_shader.frag_shader, GL_OBJECT_COMPILE_STATUS_ARB, &result);
			if (!result)
			{
				GLint length;
				GLsizei final;
				char *buffer;
				//We failed to compile.
				lprintf("Vertex shader 'program B' failed compilation.\n");
				//Attempt to get the length of our error log.
				glGetObjectParameterivARB(l.shader.simple_shader.frag_shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
				buffer = NewArray( char, length );
				//Create a buffer.
					
				//Used to get the final length of the log.
				glGetInfoLogARB(l.shader.simple_shader.frag_shader, length, &final, buffer);
				//Convert our buffer into a string.
				lprintf( "message: %s", buffer );


				if (final > length)
				{
					//The buffer does not contain all the shader log information.
					printf("Shader Log contained more information!\n");
				}
		
			}
		}

		glAttachObjectARB(l.shader.simple_shader.shader, l.shader.simple_shader.vert_shader );
		glAttachObjectARB(l.shader.simple_shader.shader, l.shader.simple_shader.frag_shader );
		
		glBindAttribLocation(l.shader.simple_shader.shader, 0, "in_Position");
		glBindAttribLocation(l.shader.simple_shader.shader, 1, "in_Color");

		glLinkProgram(l.shader.simple_shader.shader);

	}

	glUseProgram( l.shader.simple_shader.shader);
	result = glGetError();
	if( result )
	{
				GLint length;
				GLsizei final;
				char *buffer;
				//We failed to compile.
				lprintf("something something" );
				//Attempt to get the length of our error log.
				glGetObjectParameterivARB(l.shader.simple_shader.shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
				buffer = NewArray( char, length );
				glGetInfoLogARB(l.shader.simple_shader.frag_shader, length, &final, buffer);

				//Convert our buffer into a string.
				lprintf( "message: %s", buffer );
	}

	glUseProgram( 0 );
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
	glDisable(GL_VERTEX_PROGRAM_ARB);

}


static void OnDraw3d( WIDE("Simple Shader Array") )( PTRSZVAL psvView )
//static int OnDrawCommon( WIDE("Terrain View") )( PSI_CONTROL pc )
{
	int result;
	//if( 0 )
	{
		glEnable(GL_FRAGMENT_PROGRAM_ARB);
		glEnable(GL_VERTEX_PROGRAM_ARB);
		glUseProgram( l.shader.simple_shader.shader);
		{
			MATRIX out;
			int n;

			GetGLCameraMatrix( (PTRANSFORM)psvView, out );
			for( n = 0; n < 16; n++ )
				l.modelview[n] = out[0][n];

			n = glGetUniformLocation(l.shader.simple_shader.shader, "worldView");
			glUniformMatrix4fv(n, 1, GL_FALSE, l.modelview);
			n = glGetError();
			n = glGetUniformLocation(l.shader.simple_shader.shader, "Projection");
			glUniformMatrix4fv(n, 1, GL_FALSE, l.projection);
			n = glGetError();
		}
		{
			// First simple object
			float* vert = new float[9];	// vertex array
			float* col  = new float[9];	// color array
			
			vert[0] =-0.3; vert[1] = 0.5; vert[2] =-1.0;
			vert[3] =-0.8; vert[4] =-0.5; vert[5] =-1.0;
			vert[6] = 0.2; vert[7] =-0.5; vert[8]= -1.0;

			col[0] = 1.0; col[1] = 0.0; col[2] = 0.0;
			col[3] = 0.0; col[4] = 1.0; col[5] = 0.0;
			col[6] = 0.0; col[7] = 0.0; col[8] = 1.0;

			// Second simple object
			float* vert2 = new float[9];	// vertex array

			vert2[0] =-0.2; vert2[1] = 0.5; vert2[2] =-1.0;
			vert2[3] = 0.3; vert2[4] =-0.5; vert2[5] =-1.0;
			vert2[6] = 0.8; vert2[7] = 0.5; vert2[8]= -1.0;

			glEnableVertexAttribArray( 0 );
			glEnableVertexAttribArray( 1 );
			glVertexAttribPointer( 0, 3, GL_FLOAT, FALSE, 0, vert );
			result = glGetError();
			if( result )
            lprintf( "vp err %d", result );
			glVertexAttribPointer( 1, 3, GL_FLOAT, FALSE, 0, col );
			result = glGetError();
			if( result )
            lprintf( "vp err %d", result );
			//glColorPointer( 3, GL_FLOAT, 0, col );
			glDrawArrays(GL_TRIANGLES, 0, 3);	// draw first object
			glDisableVertexAttribArray( 1 );


			glVertexAttribPointer( 0, 3, GL_FLOAT, FALSE, 0, vert2 );
			glVertexAttrib3f((GLuint)1, 1.0, 0.0, 0.0); // set constant color attribute
			glDrawArrays(GL_TRIANGLES, 0, 3);	// draw second object

		}

		glDisableVertexAttribArray( 0 );
		glUseProgram( 0 );

		glDisable(GL_FRAGMENT_PROGRAM_ARB);
		glDisable(GL_VERTEX_PROGRAM_ARB);

	}
}

static void OnBeginDraw3d( WIDE( "Simple Shader Array" ) )( PTRSZVAL psv,PTRANSFORM camera )
{
}

static void OnFirstDraw3d( WIDE( "Simple Shader Array" ) )( PTRSZVAL psvInit )
{
	// and really if initshader fails, it sets up in local flags and 
	// states to make sure we just fall back to the old way.
	// so should load the classic image along with any new images.
	if (GLEW_OK != glewInit() )
	{
		// okay let's just init glew.
		return;
	}

	InitPerspective();
	InitShader();

}

static PTRSZVAL OnInit3d( WIDE( "Simple Shader Array" ) )( PTRANSFORM camera, RCOORD *identity_depth, RCOORD *aspect )
{
	// keep the camera as a 
	return (PTRSZVAL)camera;
}

