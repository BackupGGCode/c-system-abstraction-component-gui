#ifndef IMAGE_SHADER_EXTENSION_INCLUDED
#define IMAGE_SHADER_EXTENSION_INCLUDED

#define __need___va_list
#include <stdarg.h>

#ifdef USE_GLES2
#include <GLES2/gl2.h>
#endif

#ifdef _MSC_VER
#include <GL/GLU.h>
#define CheckErr()  				{    \
					GLenum err = glGetError();  \
					if( err )                   \
						lprintf( WIDE("err=%d (%") _cstring_f WIDE(")"),err, gluErrorString( err ) ); \
				}                               
#define CheckErrf(f,...)  				{    \
					GLenum err = glGetError();  \
					if( err )                   \
					lprintf( WIDE("err=%d ")f,err,##__VA_ARGS__ ); \
				}                               
#else
#define CheckErr()  				{    \
					GLenum err = glGetError();  \
					if( err )                   \
						lprintf( WIDE("err=%d "),err ); \
				}                               
#define CheckErrf(f,...)  				{    \
					GLenum err = glGetError();  \
					if( err )                   \
					lprintf( WIDE("err=%d ")f,err,##__VA_ARGS__ ); \
				}                               
#endif

#ifdef __ANDROID__
#define va_list __va_list
#else
#define va_list va_list
#endif

#include <image3d.h>

struct shader_buffer {
	float *data;
	int dimensions;
	int used;
	int avail;
	int expand_by;
};

IMAGE_NAMESPACE
typedef struct image_shader_tracker ImageShaderTracker;

struct image_shader_tracker
{
	struct image_shader_flags
	{
		BIT_FIELD set_matrix : 1; // flag indicating we have set the matrix; this is cleared at the beginning of each enable of a context
		BIT_FIELD failed : 1; // shader compilation failed, abort enable; and don't reinitialize
		BIT_FIELD set_modelview : 1;
	} flags;
	CTEXTSTR name;
	int glProgramId;
	int glVertexProgramId;
	int glFragProgramId;

	int eye_point;
	int position_attrib;
	int color_attrib;
	int projection;
	int worldview;
	int modelview;
	void (CPROC*Init)( PImageShaderTracker, PTRSZVAL psv );
	PTRSZVAL (CPROC*Setup)( void );
	PTRSZVAL psv_userdata;
	void (CPROC*Enable)( PImageShaderTracker,PTRSZVAL,va_list);
	void (CPROC*AppendTristrip )( struct image_shader_op *,int triangles,PTRSZVAL,va_list);
	void (CPROC*Flush)( PImageShaderTracker tracker, PTRSZVAL, PTRSZVAL, int from, int to );

	PTRSZVAL (CPROC*InitShaderOp)( PImageShaderTracker tracker, PTRSZVAL psvShader, va_list args );
};

struct image_shader_op
{
	struct image_shader_tracker *tracker;
	PTRSZVAL psvKey;
	int from;
	int to;
	int depth_enabled;
};

struct image_shader_image_buffer
{
	struct image_shader_tracker *tracker;
	Image target;
	PLIST output;
};

struct image_shader_image_buffer_op
{
	struct image_shader_image_buffer *image_shader_op;
	struct image_shader_op *last_op;
	PTRSZVAL psvKey;
};

PImageShaderTracker CPROC GetShaderInit( CTEXTSTR name, PTRSZVAL (CPROC*)(void), void(CPROC*)(PImageShaderTracker,PTRSZVAL) );
#define GetShader(name) GetShaderInit( name, NULL, NULL )
void CPROC SetShaderModelView( PImageShaderTracker tracker, RCOORD *matrix );

int CPROC CompileShaderEx( PImageShaderTracker shader, char const *const *vertex_code, int vert_blocks, char const*const*frag_code, int frag_blocks, struct image_shader_attribute_order *, int nAttribs );
int CPROC CompileShader( PImageShaderTracker shader, char const *const*vertex_code, int vert_blocks, char const*const*frag_code, int frag_blocks );
void CPROC ClearShaders( void );

void CPROC EnableShader( PImageShaderTracker shader, ... );


// verts and a single color
PTRSZVAL CPROC SetupSuperSimpleShader( void );
void CPROC InitSuperSimpleShader( PImageShaderTracker tracker, PTRSZVAL psv );

// verts, texture verts and a single texture
PTRSZVAL CPROC SetupSimpleTextureShader( void );
void CPROC InitSimpleTextureShader( PImageShaderTracker tracker, PTRSZVAL psv );

// verts, texture_verts, texture and a single texture
PTRSZVAL CPROC SetupSimpleShadedTextureShader( void );
void CPROC InitSimpleShadedTextureShader( PImageShaderTracker tracker, PTRSZVAL psv );

//
PTRSZVAL CPROC SetupSimpleMultiShadedTextureShader( void );
void CPROC InitSimpleMultiShadedTextureShader( PImageShaderTracker tracker, PTRSZVAL psv );

void DumpAttribs( PImageShaderTracker tracker, int program );

void CloseShaders( struct glSurfaceData *glSurface );
void FlushShaders( struct glSurfaceData *glSurface );

struct shader_buffer *CreateShaderBuffer( int dimensions, int start_size, int expand_by );
void CPROC AppendShaderData( struct image_shader_op *op, struct shader_buffer *buffer, float *data );

void CPROC  SetShaderEnable( PImageShaderTracker tracker, void (CPROC*EnableShader)( PImageShaderTracker tracker, PTRSZVAL, va_list args ), PTRSZVAL psv );
void CPROC  SetShaderOpInit( PImageShaderTracker tracker, PTRSZVAL (CPROC*InitOp)( PImageShaderTracker tracker, PTRSZVAL, va_list args ) );
void CPROC  SetShaderFlush( PImageShaderTracker tracker, void (CPROC*FlushShader)( PImageShaderTracker tracker, PTRSZVAL, PTRSZVAL, int from, int to ) );
void CPROC  SetShaderAppendTristrip( PImageShaderTracker tracker, void (CPROC*AppendTriStrip)(  struct image_shader_op *op, int triangles, PTRSZVAL,va_list args ) );
void CPROC  AppendShaderTristripQuad( struct image_shader_op * op, ... );
void CPROC  AppendShaderTristrip( struct image_shader_op * op, int triangles, ... );

size_t AppendShaderBufferData( struct shader_buffer *buffer, float *data );


struct image_shader_op * BeginImageShaderOp(PImageShaderTracker tracker, Image target, ... );
void AppendImageShaderOpTristrip( struct image_shader_op *op, int triangles, ... );


IMAGE_NAMESPACE_END
#endif
