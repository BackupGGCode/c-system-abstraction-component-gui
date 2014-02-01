
#include <imglib/imagestruct.h>

#include <render.h>
#include <render3d.h>
#include <image3d.h>
#include <sqlgetoption.h>
#include <html5.websocket.h>
#include <json_emitter.h>
#include "server_local.h"

IMAGE_NAMESPACE
#ifdef __cplusplus
namespace loader {
#endif
extern LOGICAL PngImageFile ( Image pImage, _8 ** buf, size_t *size);
#ifdef __cplusplus
};
#endif
IMAGE_NAMESPACE_END
#ifdef __cplusplus
using namespace sack::image::loader;
#endif


static IMAGE_INTERFACE ProxyImageInterface;


static void FormatColor( PVARTEXT pvt, CPOINTER data )
{
	vtprintf( pvt, "\"rgba(%u,%u,%u,%g)\""
		, ((*(PCDATA)data) >> 16) & 0xFF
		, ((*(PCDATA)data) >> 8) & 0xFF
		, ((*(PCDATA)data) >> 0) & 0xFF 
		, (((*(PCDATA)data) >> 24) & 0xFF)/255.0
		);
}

static struct json_context_object *WebSockInitReplyJson( enum proxy_message_id message )
{
	struct json_context_object *cto;
	struct json_context_object *cto_data;
	int ofs = 4;  // first thing is length, but that is not encoded..
	if( !l.json_reply_context )
		l.json_reply_context = json_create_context();
	cto = json_create_object( l.json_reply_context, 0 );
	SetLink( &l.messages, (int)message, cto );
	json_add_object_member( cto, WIDE("MsgID"), 0, JSON_Element_Unsigned_Integer_8, 0 );
	cto_data = json_add_object_member( cto, WIDE( "data" ), 1, JSON_Element_Object, 0 );

	ofs = 0;
	switch( message )
	{
	case PMID_Reply_OpenDisplayAboveUnderSizedAt:
		json_add_object_member( cto_data, WIDE("server_render_id"), ofs = 0, JSON_Element_PTRSZVAL, 0 );
		json_add_object_member( cto_data, WIDE("client_render_id"), ofs = ofs + sizeof(PTRSZVAL), JSON_Element_PTRSZVAL, 0 );
		break;
	case PMID_Event_Mouse:
		json_add_object_member( cto_data, WIDE("server_render_id"), ofs = 0, JSON_Element_PTRSZVAL, 0 );
		json_add_object_member( cto_data, WIDE("x"), ofs = ofs + sizeof(PTRSZVAL), JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("y"), ofs = ofs + sizeof(S_32), JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("b"), ofs = ofs + sizeof(S_32), JSON_Element_Unsigned_Integer_32, 0 );

		break;
	}
	return cto;
}

static struct json_context_object *WebSockInitJson( enum proxy_message_id message )
{
	struct json_context_object *cto;
	struct json_context_object *cto_data;
	int ofs = 4;  // first thing is length, but that is not encoded..
	if( !l.json_context )
		l.json_context = json_create_context();
	cto = json_create_object( l.json_context, 0 );
	SetLink( &l.messages, (int)message, cto );
	json_add_object_member( cto, WIDE("MsgID"), 0, JSON_Element_Unsigned_Integer_8, 0 );
	cto_data = json_add_object_member( cto, WIDE( "data" ), 1, JSON_Element_Object, 0 );

	ofs = 0;
	switch( message )
	{
	case PMID_Version:
		json_add_object_member( cto_data, WIDE("version"), 0, JSON_Element_Unsigned_Integer_32, 0 );
		break;
	case PMID_SetApplicationTitle:
		json_add_object_member( cto_data, WIDE("title"), 0, JSON_Element_CharArray, 0 );
		break;
	case PMID_OpenDisplayAboveUnderSizedAt:
		json_add_object_member( cto_data, WIDE("x"), ofs = 0, JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("y"), ofs = ofs + sizeof(S_32), JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("width"), ofs = ofs + sizeof(S_32), JSON_Element_Unsigned_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("height"), ofs = ofs + sizeof(_32), JSON_Element_Unsigned_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("attrib"), ofs = ofs + sizeof(_32), JSON_Element_Unsigned_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("server_render_id"), ofs = ofs + sizeof(_32), JSON_Element_PTRSZVAL, 0 );
		json_add_object_member( cto_data, WIDE("over_render_id"), ofs = ofs + sizeof(PTRSZVAL), JSON_Element_PTRSZVAL_BLANK_0, 0 );
		json_add_object_member( cto_data, WIDE("under_render_id"), ofs = ofs + sizeof(PTRSZVAL), JSON_Element_PTRSZVAL_BLANK_0, 0 );
		break;
	case PMID_CloseDisplay:
		json_add_object_member( cto_data, WIDE("client_render_id"), 0, JSON_Element_PTRSZVAL, 0 );
		break;
	case PMID_MakeImage:
		json_add_object_member( cto_data, WIDE("width"), ofs = 0, JSON_Element_Unsigned_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("height"), ofs = ofs + sizeof(_32), JSON_Element_Unsigned_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("server_image_id"), ofs = ofs + sizeof(_32), JSON_Element_PTRSZVAL, 0 );
		json_add_object_member( cto_data, WIDE("server_render_id"), ofs = ofs + sizeof(PTRSZVAL), JSON_Element_PTRSZVAL, 0 );
		break;
	case PMID_MakeSubImage:
		json_add_object_member( cto_data, WIDE("x"), ofs = 0, JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("y"), ofs = ofs + sizeof(S_32), JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("width"), ofs = ofs + sizeof(S_32), JSON_Element_Unsigned_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("height"), ofs = ofs + sizeof(_32), JSON_Element_Unsigned_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("server_parent_image_id"), ofs = ofs + sizeof(_32), JSON_Element_PTRSZVAL, 0 );
		json_add_object_member( cto_data, WIDE("server_image_id"), ofs = ofs + sizeof(PTRSZVAL), JSON_Element_PTRSZVAL, 0 );
		break;
	case PMID_BlatColor:
	case PMID_BlatColorAlpha:
		json_add_object_member( cto_data, WIDE("server_image_id"), ofs = 0, JSON_Element_PTRSZVAL, 0 );
		json_add_object_member( cto_data, WIDE("x"), ofs = ofs + sizeof(PTRSZVAL), JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("y"), ofs = ofs + sizeof(S_32), JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("width"), ofs = ofs + sizeof(S_32), JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("height"), ofs = ofs + sizeof(_32), JSON_Element_Unsigned_Integer_32, 0 );
		json_add_object_member_user_routine( cto_data, WIDE("color"), ofs = ofs + sizeof(_32), JSON_Element_Unsigned_Integer_32, 0, FormatColor );
		break;
	case PMID_BlotScaledImageSizedTo:
	case PMID_BlotImageSizedTo:
		json_add_object_member( cto_data, WIDE("server_image_id"), ofs, JSON_Element_PTRSZVAL, 0 );
		json_add_object_member( cto_data, WIDE("x"), ofs = ofs + sizeof(PTRSZVAL), JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("y"), ofs = ofs + sizeof(S_32), JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("width"), ofs = ofs + sizeof(S_32), JSON_Element_Unsigned_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("height"), ofs = ofs + sizeof(_32), JSON_Element_Unsigned_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("xs"), ofs = ofs + sizeof(_32), JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("ys"), ofs = ofs + sizeof(S_32), JSON_Element_Integer_32, 0 );
		if( message == PMID_BlotScaledImageSizedTo )
		{
			json_add_object_member( cto_data, WIDE("ws"), ofs = ofs + sizeof(S_32), JSON_Element_Unsigned_Integer_32, 0 );
			json_add_object_member( cto_data, WIDE("hs"), ofs = ofs + sizeof(_32), JSON_Element_Unsigned_Integer_32, 0 );
		}
		json_add_object_member( cto_data, WIDE("image_id"), ofs = ofs + sizeof(_32), JSON_Element_PTRSZVAL, 0 );
		break;
	case PMID_ImageData:
		json_add_object_member( cto_data, WIDE("server_image_id"), ofs = 0, JSON_Element_PTRSZVAL, 0 );
		json_add_object_member( cto_data, WIDE("data"), ofs = ofs + sizeof(PTRSZVAL), JSON_Element_CharArray, 0 );
		break;
	case PMID_DrawLine:
		json_add_object_member( cto_data, WIDE("server_image_id"), ofs, JSON_Element_PTRSZVAL, 0 );
		json_add_object_member( cto_data, WIDE("x1"), ofs = ofs + sizeof(PTRSZVAL), JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("y1"), ofs = ofs + sizeof(S_32), JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("x2"), ofs = ofs + sizeof(S_32), JSON_Element_Integer_32, 0 );
		json_add_object_member( cto_data, WIDE("y2"), ofs = ofs + sizeof(S_32), JSON_Element_Integer_32, 0 );
		json_add_object_member_user_routine( cto_data, WIDE("color"), ofs = ofs + sizeof(S_32), JSON_Element_Unsigned_Integer_32, 0, FormatColor );
		break;
	}
	return cto;
}

static const char *base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
static void encodeblock( unsigned char in[3], char out[4], int len )
{
	out[0] = base64[ in[0] >> 2 ];
	out[1] = base64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
	out[2] = (len > 1 ? base64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : 0);
	out[3] = (len > 2 ? base64[ in[2] & 0x3f ] : 0);
}



static TEXTSTR EncodeImage( Image image, size_t *outsize )
{
	TEXTSTR real_output;
	size_t length;
	{
		_8 *buf;
		PngImageFile( image, &buf, &length );
		if( 0 ) 
		{
			TEXTCHAR tmpname[32];
			static int n;
			FILE *out;
			snprintf( tmpname, 32, "blah%d.png", n++ );
			out = fopen( tmpname, "wt" );
			fwrite( buf, 1, length, out );
			fclose( out );
		}
		real_output = NewArray( char, 22 + ( length * 4 / 3 ) + 1 );
		StrCpy( real_output, "data:image/png;base64," );
		{
			size_t n;
			for( n = 0; n < (length)/3; n++ )
			{
				int blocklen;
				blocklen = length - n*3;
				if( blocklen > 3 )
				blocklen = 3;
				encodeblock( ((P_8)buf) + n * 3, real_output + 22 + n*4, blocklen );
			}
			(*outsize) = 22 + n*4;
			real_output[22 + n*4] = 0;
		}
		Release( buf );
	}

	if( 0 )
	{
		BITMAPFILEHEADER *header;
		BITMAPV5HEADER *output;
		header = (BITMAPFILEHEADER*)NewArray( _8, length = ( ( image->width * image->height * sizeof( CDATA ) ) + sizeof( BITMAPV5HEADER ) + sizeof( BITMAPFILEHEADER ) ) );
		MemSet( header, 0, sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPV5HEADER ) );
		header->bfType = 'MB';
		header->bfSize = (DWORD)length;
		header->bfOffBits = sizeof( BITMAPV5HEADER ) + sizeof( BITMAPFILEHEADER );
		output = (BITMAPV5HEADER*)(header + 1);

		output->bV5Size = sizeof( BITMAPV5HEADER );
		output->bV5Width = image->width;
		output->bV5Height = image->height;
		output->bV5Planes = 1;
		output->bV5BitCount = 32;
		output->bV5XPelsPerMeter = 120;
		output->bV5YPelsPerMeter = 120;
		//output->bV5Intent = LCS_CALIBRATED_RGB;   // 0
		output->bV5CSType = LCS_sRGB;
		{
			PCDATA color_out = (PCDATA)(output + 1);
			int n;
			for( n = 0; n < image->height; n++ )
				MemCpy( color_out + image->width * n, image->image + image->pwidth * n, sizeof( CDATA ) * image->width );
		}

		{
			TEXTCHAR tmpname[32];
			static int n;
			FILE *out;
			snprintf( tmpname, 32, "blah%d.bmp", n++ );
			out = fopen( tmpname, "wt" );
			fwrite( header, 1, length, out );
			fclose( out );
		}

		real_output = NewArray( char, 22 + ( length * 4 / 3 ) + 1 );
		StrCpy( real_output, "data:image/bmp;base64," );
		{
			size_t n;
			for( n = 0; n < (length)/3; n++ )
			{
				int blocklen;
				blocklen = length - n*3;
				if( blocklen > 3 )
					blocklen = 3;
				encodeblock( ((P_8)header) + n * 3, real_output + 22 + n*4, blocklen );
			}
			(*outsize) = 22 + n*4;
			real_output[22 + n*4] = 0;
		}
		Release( header );
	}

	return real_output;
}

static void ClearDirtyFlag( PVPImage image )
{
	for( ; image; image = image->next )
	{
		image->image->flags &= ~IF_FLAG_UPDATED;
		ClearDirtyFlag( image->child );
	}
}



static void SendTCPMessageV( PCLIENT pc, INDEX idx, LOGICAL websock, enum proxy_message_id message, ... );
static void SendTCPMessage( PCLIENT pc, INDEX idx, LOGICAL websock, enum proxy_message_id message, va_list args )
{
	TEXTSTR json_msg;
	struct json_context_object *cto;
	size_t sendlen;
	struct common_message *outmsg;
	// often used; sometimes unused...
	PVPRENDER render;
	PVPImage image;
	_8 *msg;
	if( websock )
	{
		cto = (struct json_context_object *)GetLink( &l.messages, message );
		if( !cto )
			cto = WebSockInitJson( message );
	}
	else
		cto = NULL;
	switch( message )
	{
	case PMID_Version:
		{
			msg = NewArray( _8, sendlen = ( 4 + 1 + StrLen( l.application_title ) + 1 ) );
			StrCpy( msg + 1, l.application_title );
			((_32*)msg)[0] = (_32)(sendlen - 4);
			msg[4] = message;
			if( websock )
			{
				json_msg = json_build_message( cto, msg );
				WebSocketSendText( pc, json_msg, StrLen( json_msg ) );
				Release( json_msg );
			}
			else
				SendTCP( pc, msg, sendlen );
			Release( msg );
		}
		break;
	case PMID_SetApplicationTitle:
		{
			msg = NewArray( _8, sendlen = ( 4 + 1 + StrLen( l.application_title ) + 1 ) );
			StrCpy( msg + 1, l.application_title );
			((_32*)msg)[0] = (_32)(sendlen - 4);
			msg[4] = message;
			if( websock )
			{
				json_msg = json_build_message( cto, msg );
				WebSocketSendText( pc, json_msg, StrLen( json_msg ) );
				Release( json_msg );
			}
			else
				SendTCP( pc, msg, sendlen );
			Release( msg );
		}
		break;
	case PMID_OpenDisplayAboveUnderSizedAt:
		{
			render = va_arg( args, PVPRENDER );
			msg = NewArray( _8, sendlen = ( 4 + 1 + sizeof( struct opendisplay_data ) ) );
			((_32*)msg)[0] = (_32)(sendlen - 4);
			msg[4] = message;

			((struct opendisplay_data*)(msg+5))->x = render->x;
			((struct opendisplay_data*)(msg+5))->y = render->y;
			((struct opendisplay_data*)(msg+5))->w = render->w;
			((struct opendisplay_data*)(msg+5))->h = render->h;
			((struct opendisplay_data*)(msg+5))->attr = render->attributes;
			((struct opendisplay_data*)(msg+5))->server_display_id = (PTRSZVAL)FindLink( &l.renderers, render );

			if( render->above )
				((struct opendisplay_data*)(msg+5))->over = render->above->id;
			else
				((struct opendisplay_data*)(msg+5))->over = 0;
			if( render->under )
				((struct opendisplay_data*)(msg+5))->under = render->under->id;
			else
				((struct opendisplay_data*)(msg+5))->under = 0;

			if( websock )
			{
				json_msg = json_build_message( cto, msg + 4 );
				WebSocketSendText( pc, json_msg, StrLen( json_msg ) );
				Release( json_msg );
			}
			else
				SendTCP( pc, msg, sendlen );
			Release( msg );
		}
		break;
	case PMID_CloseDisplay:
		{
			msg = NewArray( _8, sendlen = ( 4 + 1 + sizeof( POINTER ) ) );
			render = va_arg( args, PVPRENDER );
			((_32*)msg)[0] = (_32)(sendlen - 4);
			msg[4] = message;
			if( ((POINTER*)(msg+5))[0] = GetLink( &render->remote_render_id, idx ) )
				if( websock )
				{
					json_msg = json_build_message( cto, msg + 4 );
					WebSocketSendText( pc, json_msg, StrLen( json_msg ) );
					Release( json_msg );
				}
				else
					SendTCP( pc, msg, sendlen );
			Release( msg );
		}
		break;
	case PMID_MakeImage:
		{
			msg = NewArray( _8, sendlen = ( 4 + 1 + sizeof( struct make_image_data ) ) );
			image = va_arg( args, PVPImage );
			render = va_arg( args, PVPRENDER );
			((_32*)msg)[0] = (_32)(sendlen - 4);
			outmsg = (struct common_message*)(msg + 4);
			outmsg->message_id = message;
			outmsg->data.make_image.w = image->w;
			outmsg->data.make_image.h = image->h;
			outmsg->data.make_image.server_image_id = image->id;
			outmsg->data.make_image.server_display_id = image->render_id;

			if( websock )
			{
				json_msg = json_build_message( cto, outmsg );
				WebSocketSendText( pc, json_msg, StrLen( json_msg ) );
				Release( json_msg );
			}
			else
				SendTCP( pc, msg, sendlen );
			Release( msg );
			if( image->render_id == INVALID_INDEX )
				SendTCPMessageV( pc, idx, websock, PMID_ImageData, image );
		}
		break;
	case PMID_MakeSubImage:
		{
			msg = NewArray( _8, sendlen = ( 4 + 1 + sizeof( struct make_subimage_data ) ) );
			image = va_arg( args, PVPImage );
			((_32*)msg)[0] = (_32)(sendlen - 4);
			outmsg = (struct common_message*)(msg + 4);
			outmsg->message_id = message;
			outmsg->data.make_subimage.x = image->x;
			outmsg->data.make_subimage.y = image->y;
			outmsg->data.make_subimage.w = image->w;
			outmsg->data.make_subimage.h = image->h;
			outmsg->data.make_subimage.server_image_id = image->id;
			outmsg->data.make_subimage.server_parent_image_id = image->parent?image->parent->id:INVALID_INDEX;

			if( websock )
			{
				json_msg = json_build_message( cto, outmsg );
				WebSocketSendText( pc, json_msg, StrLen( json_msg ) );
				Release( json_msg );
			}
			else
				SendTCP( pc, msg, sendlen );
			Release( msg );
		}
		break;
	case PMID_ImageData:
		{
			TEXTSTR encoded_image;
			size_t outlen;
			image = va_arg( args, PVPImage );
			while( image && image->parent ) 
				image = image->parent;
			ClearDirtyFlag( image );
			encoded_image = EncodeImage( image->image, &outlen );
			msg = NewArray( _8, sendlen = ( 4 + 1 + sizeof( struct image_data_data ) + outlen ) );
			((_32*)msg)[0] = (_32)(sendlen - 4);
			outmsg = (struct common_message*)(msg + 4);
			outmsg->message_id = message;
			outmsg->data.image_data.server_image_id = image->id;
			// include nul in copy
			MemCpy( outmsg->data.image_data.data, encoded_image, outlen + 1 );

			if( websock )
			{
				json_msg = json_build_message( cto, outmsg );
				WebSocketSendText( pc, json_msg, StrLen( json_msg ) );
				Release( json_msg );
			}
			else
				SendTCP( pc, msg, sendlen );
			Release( msg );


		}
		break;
	}
}

void SendTCPMessageV( PCLIENT pc, INDEX idx, LOGICAL websock, enum proxy_message_id message, ... )
{
	va_list args;
	va_start( args, message );
	SendTCPMessage( pc, idx, websock, message, args );
}

static void SendClientMessage( enum proxy_message_id message, ... )
{
	INDEX idx;
	struct server_proxy_client *client;
	LIST_FORALL( l.clients, idx, struct server_proxy_client*, client )
	{
		va_list args;
		va_start( args, message );
		SendTCPMessage( client->pc, idx, client->websock, message, args );
	}
}


static void CPROC SocketRead( PCLIENT pc, POINTER buffer, int size )
{
	if( !buffer )
	{
		buffer = NewArray( _8, 1024 );
		size = 1024;
	}
	else
	{

	}
	ReadTCP( pc, buffer, size );
}

static void CPROC Connected( PCLIENT pcServer, PCLIENT pcNew )
{
	struct server_proxy_client *client = New( struct server_proxy_client );
	INDEX idx;
	client->pc = pcNew;
	client->websock = TRUE;
	AddLink( &l.clients, client );
	idx = FindLink ( &l.clients, client );
	if( l.application_title )
		SendTCPMessageV( pcNew, idx, FALSE, PMID_SetApplicationTitle );
	{
		INDEX idx;
		PVPRENDER render;
		LIST_FORALL( l.renderers, idx, PVPRENDER, render )
		{
			SendTCPMessageV( pcNew, idx, FALSE, PMID_OpenDisplayAboveUnderSizedAt, render );
		}
	}
}

static void SendInitialImage( PCLIENT pc, PLIST *sent, PVPImage image )
{
	if( image->parent )
	{
		if( FindLink( sent, image->parent ) == INVALID_INDEX )
		{
			SendInitialImage( pc, sent, image->parent );
		}
		SendTCPMessageV( pc, 0, TRUE, PMID_MakeSubImage, image );
	}
	else
		SendTCPMessageV( pc, 0, TRUE, PMID_MakeImage, image, image->render_id );

	AddLink( sent,  image );
}

static PTRSZVAL WebSockOpen( PCLIENT pc, PTRSZVAL psv )
{
	struct server_proxy_client *client = New( struct server_proxy_client );
	INDEX idx;
	client->pc = pc;
	client->websock = TRUE;
	AddLink( &l.clients, client );
	idx = FindLink( &l.clients, client );
	if( l.application_title )
		SendTCPMessageV( pc, idx, TRUE, PMID_SetApplicationTitle );
	{
		INDEX idx;
		PVPRENDER render;
		LIST_FORALL( l.renderers, idx, PVPRENDER, render )
		{
			SendTCPMessageV( pc, idx, TRUE, PMID_OpenDisplayAboveUnderSizedAt, render );
		}

		{
			PLIST sent = NULL;
			INDEX idx;
			PVPImage image;
			LIST_FORALL( l.images, idx, PVPImage, image )
			{
				SendInitialImage( pc, &sent, image );
			}
			LIST_FORALL( sent, idx, PVPImage, image )
			{
				if( image->websock_buffer && image->websock_sendlen )
				{
					image->websock_buffer[image->websock_sendlen] = ']';
					WebSocketSendText( pc, image->websock_buffer, image->websock_sendlen + 1 );
				}
			}
			DeleteList( &sent );
		}

	}
	return (PTRSZVAL)client;
}

static void WebSockClose( PCLIENT pc, PTRSZVAL psv )
{
}

static void WebSockError( PCLIENT pc, PTRSZVAL psv, int error )
{
}

static void WebSockEvent( PCLIENT pc, PTRSZVAL psv, POINTER buffer, int msglen )
{
	POINTER msg = NULL;
	struct server_proxy_client *client= (struct server_proxy_client *)psv;
	struct json_context_object *json_object;
	lprintf( "Received:%s", buffer );
	if( json_parse_message( l.json_reply_context, buffer, msglen, &json_object, &msg ) )
	{
		struct common_message *message = (struct common_message *)msg;
		switch( message->message_id )
		{
		case PMID_Reply_OpenDisplayAboveUnderSizedAt:  // depricated; server does not keep client IDs
			{
				PVPRENDER render = GetLink( &l.renderers, message->data.open_display_reply.server_display_id );
				SetLink( &render->remote_render_id, FindLink( &l.clients, client ), message->data.open_display_reply.client_display_id );
			}
			break;
		case PMID_Event_Mouse:
			{
				PVPRENDER render = GetLink( &l.renderers, message->data.mouse_event.server_render_id );
				if( render && render->mouse_callback )
					render->mouse_callback( render->psv_mouse_callback, message->data.mouse_event.x, message->data.mouse_event.y, message->data.mouse_event.b );
			}
			break;
		}
		lprintf( "Success" );
		json_dispose_message( json_object,  msg );
	}
}


static void InitService( void )
{
	if( !l.listener )
	{
		NetworkStart();
		l.listener = OpenTCPListenerAddrEx( CreateSockAddress( "0.0.0.0", 4241 ), Connected );
		l.web_listener = WebSocketCreate( "ws://0.0.0.0:4240/Sack/Vidlib/Proxy"
													, WebSockOpen
													, WebSockEvent
													, WebSockClose
													, WebSockError
													, 0 );
	}
}

static int CPROC VidlibProxy_InitDisplay( void )
{
	return TRUE;
}

static void CPROC VidlibProxy_SetApplicationIcon( Image icon )
{
	// no support
}

static LOGICAL CPROC VidlibProxy_RequiresDrawAll( void )
{
	// force application to mostly draw itself...
	return FALSE;
}

static void VidlibProxy_SetApplicationTitle( CTEXTSTR title )
{
	if( l.application_title )
		Release( l.application_title );
	l.application_title = StrDup( title );
	SendClientMessage( PMID_SetApplicationTitle );
}

static void VidlibProxy_GetDisplaySize( _32 *width, _32 *height )
{
	if( width )
		(*width) = SACK_GetProfileInt( "SACK/Vidlib", "Default Display Width", 1024 );
	if( height )
		(*height) = SACK_GetProfileInt( "SACK/Vidlib", "Default Display Height", 768 );
}

static void VidlibProxy_SetDisplaySize		( _32 width, _32 height )
{
	SACK_WriteProfileInt( "SACK/Vidlib", "Default Display Width", width );
	SACK_WriteProfileInt( "SACK/Vidlib", "Default Display Height", height );
}


static PVPImage Internal_MakeImageFileEx ( INDEX iRender, _32 Width, _32 Height DBG_PASS)
{
	PVPImage image = New( struct vidlib_proxy_image );
	MemSet( image, 0, sizeof( struct vidlib_proxy_image ) );
	image->w = Width;
	image->h = Height;
	image->render_id = iRender;
	image->image = l.real_interface->_MakeImageFileEx( Width, Height DBG_RELAY );
	image->image->reverse_interface = &ProxyImageInterface;
	image->image->reverse_interface_instance = image;
	if( iRender != INVALID_INDEX )
		image->image->flags |= IF_FLAG_FINAL_RENDER;
	SendClientMessage( PMID_MakeImage, image, iRender );
	AddLink( &l.images, image );
	image->id = FindLink( &l.images, image );
	return image;
}

static Image CPROC VidlibProxy_MakeImageFileEx (_32 Width, _32 Height DBG_PASS)
{
	return (Image)Internal_MakeImageFileEx( INVALID_INDEX, Width, Height DBG_RELAY );
}


static PRENDERER VidlibProxy_OpenDisplayAboveUnderSizedAt( _32 attributes, _32 width, _32 height, S_32 x, S_32 y, PRENDERER above, PRENDERER under )
{

	PVPRENDER Renderer = New( struct vidlib_proxy_renderer );
	MemSet( Renderer, 0, sizeof( struct vidlib_proxy_renderer ) );
	AddLink( &l.renderers, Renderer );
	Renderer->id = FindLink( &l.renderers, Renderer );
	Renderer->x = x;
	Renderer->y = y;
	Renderer->w = width;
	Renderer->h = height;
	Renderer->attributes = attributes;
	Renderer->above = (PVPRENDER)above;
	Renderer->under = (PVPRENDER)under;
	Renderer->image = Internal_MakeImageFileEx( Renderer->id, width, height DBG_SRC );
	
	SendClientMessage( PMID_OpenDisplayAboveUnderSizedAt, Renderer );
	return (PRENDERER)Renderer;
}

static PRENDERER VidlibProxy_OpenDisplayAboveSizedAt( _32 attributes, _32 width, _32 height, S_32 x, S_32 y, PRENDERER above )
{
	return VidlibProxy_OpenDisplayAboveUnderSizedAt( attributes, width, height, x, y, above, NULL );
}

static PRENDERER VidlibProxy_OpenDisplaySizedAt	  ( _32 attributes, _32 width, _32 height, S_32 x, S_32 y )
{
	return VidlibProxy_OpenDisplayAboveUnderSizedAt( attributes, width, height, x, y, NULL, NULL );
}

static void  VidlibProxy_CloseDisplay ( PRENDERER Renderer )
{
	SendClientMessage( PMID_CloseDisplay, Renderer );
	DeleteLink( &l.renderers, Renderer );
	Release( Renderer );
}

static void VidlibProxy_UpdateDisplayPortionEx( PRENDERER r, S_32 x, S_32 y, _32 width, _32 height DBG_PASS )
{
	// no-op; it will ahve already displayed(?)
}

static void VidlibProxy_UpdateDisplayEx( PRENDERER r DBG_PASS)
{
	// no-op; it will ahve already displayed(?)
}

static void VidlibProxy_GetDisplayPosition ( PRENDERER r, S_32 *x, S_32 *y, _32 *width, _32 *height )
{
	PVPRENDER pRender = (PVPRENDER)r;
	if( x )
		(*x) = pRender->x;
	if( y )
		(*y) = pRender->y;
	if( width )
		(*width) = pRender->w;
	if( height ) 
		(*height) = pRender->h;
}

static void CPROC VidlibProxy_MoveDisplay		  ( PRENDERER r, S_32 x, S_32 y )
{
	PVPRENDER pRender = (PVPRENDER)r;
	pRender->x = x;
	pRender->y = y;
}

static void CPROC VidlibProxy_MoveDisplayRel( PRENDERER r, S_32 delx, S_32 dely )
{
	PVPRENDER pRender = (PVPRENDER)r;
	pRender->x += delx;
	pRender->y += dely;
}

static void CPROC VidlibProxy_SizeDisplay( PRENDERER r, _32 w, _32 h )
{
	PVPRENDER pRender = (PVPRENDER)r;
	pRender->w = w;
	pRender->h = h;
}

static void CPROC VidlibProxy_SizeDisplayRel( PRENDERER r, S_32 delw, S_32 delh )
{
	PVPRENDER pRender = (PVPRENDER)r;
	pRender->w += delw;
	pRender->h += delh;
}

static void CPROC VidlibProxy_MoveSizeDisplayRel( PRENDERER r
																 , S_32 delx, S_32 dely
																 , S_32 delw, S_32 delh )
{
	PVPRENDER pRender = (PVPRENDER)r;
	pRender->w += delw;
	pRender->h += delh;
	pRender->x += delx;
	pRender->y += dely;
}

static void CPROC VidlibProxy_MoveSizeDisplay( PRENDERER r
													 , S_32 x, S_32 y
													 , S_32 w, S_32 h )
{
	PVPRENDER pRender = (PVPRENDER)r;
	pRender->x = x;
	pRender->y = y;
	pRender->w = w;
	pRender->h = h;
}

static void CPROC VidlibProxy_PutDisplayAbove		( PRENDERER r, PRENDERER above )
{
	lprintf( "window ordering is not implemented" );
}

static Image CPROC VidlibProxy_GetDisplayImage( PRENDERER r )
{
	PVPRENDER pRender = (PVPRENDER)r;
	return (Image)pRender->image;
}

static void CPROC VidlibProxy_SetCloseHandler	 ( PRENDERER r, CloseCallback c, PTRSZVAL p )
{
}

static void CPROC VidlibProxy_SetMouseHandler  ( PRENDERER r, MouseCallback c, PTRSZVAL p )
{
	PVPRENDER render = (PVPRENDER)r;
	render->mouse_callback = c;
	render->psv_mouse_callback = p;
}

static void CPROC VidlibProxy_SetRedrawHandler  ( PRENDERER r, RedrawCallback c, PTRSZVAL p )
{
	PVPRENDER render = (PVPRENDER)r;
	render->redraw = c;
	render->psv_redraw = p;
}

static void CPROC VidlibProxy_SetKeyboardHandler	( PRENDERER r, KeyProc c, PTRSZVAL p )
{
}

static void CPROC VidlibProxy_SetLoseFocusHandler  ( PRENDERER r, LoseFocusCallback c, PTRSZVAL p )
{
}

static void CPROC VidlibProxy_GetMousePosition	( S_32 *x, S_32 *y )
{
}

static void CPROC VidlibProxy_SetMousePosition  ( PRENDERER r, S_32 x, S_32 y )
{
}

static LOGICAL CPROC VidlibProxy_HasFocus		 ( PRENDERER  r )
{
	return TRUE;
}

static TEXTCHAR CPROC VidlibProxy_GetKeyText		 ( int key )
{ 
	return 0;
}

static _32 CPROC VidlibProxy_IsKeyDown		  ( PRENDERER r, int key )
{
	return 0;
}

static _32 CPROC VidlibProxy_KeyDown		  ( PRENDERER r, int key )
{
	return 0;
}

static LOGICAL CPROC VidlibProxy_DisplayIsValid ( PRENDERER r )
{
	return (r != NULL);
}

static void CPROC VidlibProxy_OwnMouseEx ( PRENDERER r, _32 Own DBG_PASS)
{

}

static int CPROC VidlibProxy_BeginCalibration ( _32 points )
{
	return 0;
}

static void CPROC VidlibProxy_SyncRender( PRENDERER pDisplay )
{
}

static void CPROC VidlibProxy_MakeTopmost  ( PRENDERER r )
{
}

static void CPROC VidlibProxy_HideDisplay	 ( PRENDERER r )
{
}

static void CPROC VidlibProxy_RestoreDisplay  ( PRENDERER r )
{
}

static void CPROC VidlibProxy_ForceDisplayFocus ( PRENDERER r )
{
}

static void CPROC VidlibProxy_ForceDisplayFront( PRENDERER r )
{
}

static void CPROC VidlibProxy_ForceDisplayBack( PRENDERER r )
{
}

static int CPROC  VidlibProxy_BindEventToKey( PRENDERER pRenderer, _32 scancode, _32 modifier, KeyTriggerHandler trigger, PTRSZVAL psv )
{
	return 0;
}

static int CPROC VidlibProxy_UnbindKey( PRENDERER pRenderer, _32 scancode, _32 modifier )
{
	return 0;
}

static int CPROC VidlibProxy_IsTopmost( PRENDERER r )
{
	return 0;
}

static void CPROC VidlibProxy_OkaySyncRender( void )
{
	// redundant thing?
}

static int CPROC VidlibProxy_IsTouchDisplay( void )
{
	return 0;
}

static void CPROC VidlibProxy_GetMouseState( S_32 *x, S_32 *y, _32 *b )
{
}

static PSPRITE_METHOD CPROC VidlibProxy_EnableSpriteMethod(PRENDERER render, void(CPROC*RenderSprites)(PTRSZVAL psv, PRENDERER renderer, S_32 x, S_32 y, _32 w, _32 h ), PTRSZVAL psv )
{
	return NULL;
}

static void CPROC VidlibProxy_WinShell_AcceptDroppedFiles( PRENDERER renderer, dropped_file_acceptor f, PTRSZVAL psvUser )
{
}

static void CPROC VidlibProxy_PutDisplayIn(PRENDERER r, PRENDERER hContainer)
{
}

static void CPROC VidlibProxy_SetRendererTitle( PRENDERER render, const TEXTCHAR *title )
{
}

static void CPROC VidlibProxy_DisableMouseOnIdle(PRENDERER r, LOGICAL bEnable )
{
}

static void CPROC VidlibProxy_SetDisplayNoMouse( PRENDERER r, int bNoMouse )
{
}

static void CPROC VidlibProxy_Redraw( PRENDERER r )
{
	PVPRENDER render = (PVPRENDER)r;
	if( render->redraw )
		render->redraw( render->psv_redraw, (PRENDERER)render );
}

static void CPROC VidlibProxy_MakeAbsoluteTopmost(PRENDERER r)
{
}

static void CPROC VidlibProxy_SetDisplayFade( PRENDERER r, int level )
{
}

static LOGICAL CPROC VidlibProxy_IsDisplayHidden( PRENDERER r )
{
	PVPRENDER pRender = (PVPRENDER)r;
	return pRender->flags.hidden;
}

#ifdef WIN32
static HWND CPROC VidlibProxy_GetNativeHandle( PRENDERER r )
{
}
#endif

static void CPROC VidlibProxy_GetDisplaySizeEx( int nDisplay
														  , S_32 *x, S_32 *y
														  , _32 *width, _32 *height)
{
}

static void CPROC VidlibProxy_LockRenderer( PRENDERER render )
{
}

static void CPROC VidlibProxy_UnlockRenderer( PRENDERER render )
{
}

static void CPROC VidlibProxy_IssueUpdateLayeredEx( PRENDERER r, LOGICAL bContent, S_32 x, S_32 y, _32 w, _32 h DBG_PASS )
{
}


#ifndef NO_TOUCH
		/* <combine sack::image::render::SetTouchHandler@PRENDERER@fte inc asdfl;kj
		 fteTouchCallback@PTRSZVAL>
		 
		 \ \																									  */
static void CPROC VidlibProxy_SetTouchHandler  ( PRENDERER r, TouchCallback c, PTRSZVAL p )
{
}
#endif

static void CPROC VidlibProxy_MarkDisplayUpdated( PRENDERER r  )
{
}

static void CPROC VidlibProxy_SetHideHandler		( PRENDERER r, HideAndRestoreCallback c, PTRSZVAL p )
{
}

static void CPROC VidlibProxy_SetRestoreHandler  ( PRENDERER r, HideAndRestoreCallback c, PTRSZVAL p )
{
}

static void CPROC VidlibProxy_RestoreDisplayEx ( PRENDERER r DBG_PASS )
{
}

// android extension
PUBLIC( void, SACK_Vidlib_ShowInputDevice )( void )
{
}

PUBLIC( void, SACK_Vidlib_HideInputDevice )( void )
{
}


static RENDER_INTERFACE ProxyInterface = {
	VidlibProxy_InitDisplay
													  , VidlibProxy_SetApplicationTitle
													  , VidlibProxy_SetApplicationIcon
													  , VidlibProxy_GetDisplaySize
													  , VidlibProxy_SetDisplaySize
													  , VidlibProxy_OpenDisplaySizedAt
													  , VidlibProxy_OpenDisplayAboveSizedAt
													  , VidlibProxy_CloseDisplay
													  , VidlibProxy_UpdateDisplayPortionEx
													  , VidlibProxy_UpdateDisplayEx
													  , VidlibProxy_GetDisplayPosition
													  , VidlibProxy_MoveDisplay
													  , VidlibProxy_MoveDisplayRel
													  , VidlibProxy_SizeDisplay
													  , VidlibProxy_SizeDisplayRel
													  , VidlibProxy_MoveSizeDisplayRel
													  , VidlibProxy_PutDisplayAbove
													  , VidlibProxy_GetDisplayImage
													  , VidlibProxy_SetCloseHandler
													  , VidlibProxy_SetMouseHandler
													  , VidlibProxy_SetRedrawHandler
													  , VidlibProxy_SetKeyboardHandler
	 /* <combine sack::image::render::SetLoseFocusHandler@PRENDERER@LoseFocusCallback@PTRSZVAL>
		 
		 \ \																												 */
													  , VidlibProxy_SetLoseFocusHandler
			 ,  0  //POINTER junk1;
													  , VidlibProxy_GetMousePosition
													  , VidlibProxy_SetMousePosition
													  , VidlibProxy_HasFocus

													  , VidlibProxy_GetKeyText
													  , VidlibProxy_IsKeyDown
													  , VidlibProxy_KeyDown
													  , VidlibProxy_DisplayIsValid
													  , VidlibProxy_OwnMouseEx
													  , VidlibProxy_BeginCalibration
													  , VidlibProxy_SyncRender

													  , VidlibProxy_MoveSizeDisplay
													  , VidlibProxy_MakeTopmost
													  , VidlibProxy_HideDisplay
													  , VidlibProxy_RestoreDisplay
													  , VidlibProxy_ForceDisplayFocus
													  , VidlibProxy_ForceDisplayFront
													  , VidlibProxy_ForceDisplayBack
													  , VidlibProxy_BindEventToKey
													  , VidlibProxy_UnbindKey
													  , VidlibProxy_IsTopmost
													  , VidlibProxy_OkaySyncRender
													  , VidlibProxy_IsTouchDisplay
													  , VidlibProxy_GetMouseState
													  , VidlibProxy_EnableSpriteMethod
													  , VidlibProxy_WinShell_AcceptDroppedFiles
													  , VidlibProxy_PutDisplayIn
#ifdef WIN32
													  , NULL // make renderer from native handle
#endif
													  , VidlibProxy_SetRendererTitle
													  , VidlibProxy_DisableMouseOnIdle
													  , VidlibProxy_OpenDisplayAboveUnderSizedAt
													  , VidlibProxy_SetDisplayNoMouse
													  , VidlibProxy_Redraw
													  , VidlibProxy_MakeAbsoluteTopmost
													  , VidlibProxy_SetDisplayFade
													  , VidlibProxy_IsDisplayHidden
#ifdef WIN32
													, NULL // get native handle from renderer
#endif
													  , VidlibProxy_GetDisplaySizeEx

													  , VidlibProxy_LockRenderer
													  , VidlibProxy_UnlockRenderer
													  , VidlibProxy_IssueUpdateLayeredEx
													  , VidlibProxy_RequiresDrawAll
#ifndef NO_TOUCH
													  , VidlibProxy_SetTouchHandler
#endif
													  , VidlibProxy_MarkDisplayUpdated
													  , VidlibProxy_SetHideHandler
													  , VidlibProxy_SetRestoreHandler
													  , VidlibProxy_RestoreDisplayEx
												, SACK_Vidlib_ShowInputDevice
												, SACK_Vidlib_HideInputDevice
};

static void InitProxyInterface( void )
{
	ProxyInterface._RequiresDrawAll = VidlibProxy_RequiresDrawAll;
}

static RENDER3D_INTERFACE Proxy3dInterface = {
	NULL
};

static void CPROC VidlibProxy_SetStringBehavior( Image pImage, _32 behavior )
{

}
static void CPROC VidlibProxy_SetBlotMethod	  ( _32 method )
{

}

static Image CPROC VidlibProxy_BuildImageFileEx ( PCOLOR pc, _32 width, _32 height DBG_PASS)
{
	PVPImage image = New( struct vidlib_proxy_image );
	MemSet( image, 0, sizeof( struct vidlib_proxy_image ) );
	lprintf( "CRITICAL; BuildImageFile is not possible" );
	image->w = width;
	image->h = height;
	image->image = l.real_interface->_BuildImageFileEx( pc, width, height DBG_RELAY );
	// don't really need to make this; if it needs to be updated to the client it will be handled later
	//SendClientMessage( PMID_MakeImageFile, image );
	AddLink( &l.images, image );
	image->id = FindLink( &l.images, image );
	return (Image)image;
}

static Image CPROC VidlibProxy_MakeSubImageEx  ( Image pImage, S_32 x, S_32 y, _32 width, _32 height DBG_PASS )
{
	PVPImage image = New( struct vidlib_proxy_image );
	MemSet( image, 0, sizeof( struct vidlib_proxy_image ) );
	image->x = x;
	image->y = y;
	image->w = width;
	image->h = height;
	image->render_id = ((PVPImage)pImage)->render_id;
	image->image = l.real_interface->_MakeSubImageEx( ((PVPImage)pImage)->image, x, y, width, height DBG_RELAY );
	image->image->reverse_interface = &ProxyImageInterface;
	image->image->reverse_interface_instance = image;
	image->parent = (PVPImage)pImage;
	if( image->next = ((PVPImage)pImage)->child )
		image->next->prior = image;
	((PVPImage)pImage)->child = image;

	AddLink( &l.images, image );
	image->id = FindLink( &l.images, image );
	// don't really need to make this; if it needs to be updated to the client it will be handled later
	SendClientMessage( PMID_MakeSubImage, image );

	return (Image)image;
}

static Image CPROC VidlibProxy_RemakeImageEx	 ( Image pImage, PCOLOR pc, _32 width, _32 height DBG_PASS)
{
	PVPImage image;
	if( !(image = (PVPImage)pImage ) )
	{
		image = New( struct vidlib_proxy_image );
		MemSet( image, 0, sizeof( struct vidlib_proxy_image ) );
		image->render_id = INVALID_INDEX;
	}
	lprintf( "CRITICAL; RemakeImageFile is not possible" );
	image->w = width;
	image->h = height;
	image->image = l.real_interface->_RemakeImageEx( image->image, pc, width, height DBG_RELAY );
	image->image->reverse_interface = &ProxyImageInterface;
	image->image->reverse_interface_instance = image;
	AddLink( &l.images, image );
	image->id = FindLink( &l.images, image );
	return (Image)image;
}

static Image CPROC VidlibProxy_LoadImageFileFromGroupEx( INDEX group, CTEXTSTR filename DBG_PASS )
{
	PVPImage image = New( struct vidlib_proxy_image );
	MemSet( image, 0, sizeof( struct vidlib_proxy_image ) );
	image->filegroup = group;
	image->filename = StrDup( filename );
	image->image = l.real_interface->_LoadImageFileFromGroupEx( group, filename DBG_RELAY );
	image->w = image->image->actual_width;
	image->h = image->image->actual_height;
	image->render_id = INVALID_INDEX;
	// don't really need to make this; if it needs to be updated to the client it will be handled later
	SendClientMessage( PMID_MakeImage, image );
	SendClientMessage( PMID_ImageData, image );
	AddLink( &l.images, image );
	image->id = FindLink( &l.images, image );
	return (Image)image;
}

static Image CPROC VidlibProxy_LoadImageFileEx( CTEXTSTR filename DBG_PASS )
{
	return VidlibProxy_LoadImageFileFromGroupEx( 0, filename DBG_RELAY );
}



/* <combine sack::image::LoadImageFileEx@CTEXTSTR name>
	
	Internal
	Interface index 10																	*/  IMAGE_PROC_PTR( Image,VidlibProxy_LoadImageFileEx)  ( CTEXTSTR name DBG_PASS );
static  void CPROC VidlibProxy_UnmakeImageFileEx( Image pif DBG_PASS )
{
	if( pif )
	{
		SendClientMessage( PMID_UnmakeImage, pif );
		SetLink( &l.images, ((PVPImage)pif)->id, NULL );
		Release( pif );
	}
}

static void CPROC VidlibProxy_ResizeImageEx	  ( Image pImage, S_32 width, S_32 height DBG_PASS)
{
	PVPImage image = (PVPImage)pImage;
	image->w = width;
	image->h = height;
	l.real_interface->_ResizeImageEx( image->image, width, height DBG_RELAY );
}

static void CPROC VidlibProxy_MoveImage			( Image pImage, S_32 x, S_32 y )
{
	PVPImage image = (PVPImage)pImage;
	if( image->x != x || image->y != y )
	{
		image->x = x;
		image->y = y;
		l.real_interface->_MoveImage( image->image, x, y );
	}
}

P_8 GetMessageBuf( PVPImage image, size_t size )
{
	P_8 resultbuf;
	if( ( image->buf_avail - image->sendlen ) < size )
	{
		P_8 newbuf;
		image->buf_avail += size + 256;
		newbuf = NewArray( _8, image->buf_avail );
		if( image->buffer )
		{
			MemCpy( newbuf, image->buffer, image->sendlen );
			Release( image->buffer );
		}
		image->buffer = newbuf;
	}
	resultbuf = image->buffer + image->sendlen;
	image->sendlen += size;

	return resultbuf + 4;
}

static void AppendJSON( PVPImage image, CTEXTSTR msg )
{
	size_t size = StrLen( msg );
	if( (image->websock_buf_avail - image->websock_sendlen) < size )
	{
		P_8 newbuf;
		image->websock_buf_avail += size + 256;
		newbuf = NewArray( _8, image->websock_buf_avail );
		if( image->websock_buffer )
		{
			MemCpy( newbuf, image->websock_buffer, image->websock_sendlen );
			Release( image->websock_buffer );
		}
		image->websock_buffer = newbuf;
	}
	if( image->websock_sendlen == 0 )
	{
		image->websock_buffer[0] = '[';
		image->websock_sendlen++;
	}
	else
	{
		image->websock_buffer[image->websock_sendlen] = ',';
		image->websock_sendlen++;
	}
	MemCpy( image->websock_buffer + image->websock_sendlen, msg, size );
	image->websock_sendlen += size;
}

static void CPROC VidlibProxy_BlatColor	  ( Image pifDest, S_32 x, S_32 y, _32 w, _32 h, CDATA color )
{
	if( ((PVPImage)pifDest)->render_id != INVALID_INDEX )
	{
		struct json_context_object *cto;
		PVPImage image = (PVPImage)pifDest;
		struct common_message *outmsg;
		cto = (struct json_context_object *)GetLink( &l.messages, PMID_BlatColor );
		if( !cto )
			cto = WebSockInitJson( PMID_BlatColor );

		outmsg = (struct common_message*)GetMessageBuf( image, ( 4 + 1 + sizeof( struct blatcolor_data ) ) );
		outmsg->message_id = PMID_BlatColor;
		outmsg->data.blatcolor.x = x;
		outmsg->data.blatcolor.y = y;
		outmsg->data.blatcolor.w = w;
		outmsg->data.blatcolor.h = h;
		outmsg->data.blatcolor.color = color;
		outmsg->data.blatcolor.server_image_id = image->id;
		{
			TEXTSTR json_msg = json_build_message( cto, outmsg );
			AppendJSON( image, json_msg );
			Release( json_msg );
		}
	}
	else
	{
		l.real_interface->_BlatColor( ((PVPImage)pifDest)->image, x, y, w, h, color );
	}

}

static void CPROC VidlibProxy_BlatColorAlpha( Image pifDest, S_32 x, S_32 y, _32 w, _32 h, CDATA color )
{
	if( ((PVPImage)pifDest)->render_id != INVALID_INDEX )
	{
		struct json_context_object *cto;
		PVPImage image = (PVPImage)pifDest;
		struct common_message *outmsg;
		cto = (struct json_context_object *)GetLink( &l.messages, PMID_BlatColorAlpha );
		if( !cto )
			cto = WebSockInitJson( PMID_BlatColor );

		outmsg = (struct common_message*)GetMessageBuf( image, ( 4 + 1 + sizeof( struct blatcolor_data ) ) );
		outmsg->message_id = PMID_BlatColorAlpha;
		outmsg->data.blatcolor.x = x;
		outmsg->data.blatcolor.y = y;
		outmsg->data.blatcolor.w = w;
		outmsg->data.blatcolor.h = h;
		outmsg->data.blatcolor.color = color;
		outmsg->data.blatcolor.server_image_id = image->id;
		{
			TEXTSTR json_msg = json_build_message( cto, outmsg );
			AppendJSON( image, json_msg );
			Release( json_msg );
		}
	}
	else
	{
		l.real_interface->_BlatColorAlpha( ((PVPImage)pifDest)->image, x, y, w, h, color );
	}
}

static void CPROC VidlibProxy_BlotImageEx	  ( Image pDest, Image pIF, S_32 x, S_32 y, _32 nTransparent, _32 method, ... )
{
	if( ((PVPImage)pDest)->render_id != INVALID_INDEX )
	{
		struct json_context_object *cto;
		PVPImage image = (PVPImage)pDest;
		struct common_message *outmsg;
		cto = (struct json_context_object *)GetLink( &l.messages, PMID_BlotImageSizedTo );
		if( !cto )
			cto = WebSockInitJson( PMID_BlotImageSizedTo );

		// sending this clears the flag.
		if( ((PVPImage)pIF)->image->flags & IF_FLAG_UPDATED )
			SendClientMessage( PMID_ImageData, pIF );

		outmsg = (struct common_message*)GetMessageBuf( image, ( 4 + 1 + sizeof( struct blot_image_data ) ) );
		outmsg->message_id = PMID_BlotImageSizedTo;
		outmsg->data.blot_image.x = x;
		outmsg->data.blot_image.y = y;
		outmsg->data.blot_image.w = ((PVPImage)pIF)->w;
		outmsg->data.blot_image.h = ((PVPImage)pIF)->h;
		outmsg->data.blot_image.xs = 0;
		outmsg->data.blot_image.ys = 0;
		outmsg->data.blot_image.image_id = ((PVPImage)pIF)->id;
		outmsg->data.blot_image.server_image_id = image->id;
		{
			TEXTSTR json_msg = json_build_message( cto, outmsg );
			AppendJSON( image, json_msg );
			Release( json_msg );
		}
	}
	else
	{
		va_list args;
		va_start( args, method );
		l.real_interface->_BlotImageEx( ((PVPImage)pDest)->image, ((PVPImage)pIF)->image, x, y, nTransparent
					, method
					, va_arg( args, CDATA ), va_arg( args, CDATA ), va_arg( args, CDATA ) );
	}
}

static void CPROC VidlibProxy_BlotImageSizedEx( Image pDest, Image pIF, S_32 x, S_32 y, S_32 xs, S_32 ys, _32 wd, _32 ht, _32 nTransparent, _32 method, ... )
{
	if( ((PVPImage)pDest)->render_id != INVALID_INDEX )
	{
		struct json_context_object *cto;
		PVPImage image = (PVPImage)pDest;
		struct common_message *outmsg;
		cto = (struct json_context_object *)GetLink( &l.messages, PMID_BlotImageSizedTo );
		if( !cto )
			cto = WebSockInitJson( PMID_BlotImageSizedTo );

		// sending this clears the flag.
		if( ((PVPImage)pIF)->image->flags & IF_FLAG_UPDATED )
			SendClientMessage( PMID_ImageData, pIF );

		outmsg = (struct common_message*)GetMessageBuf( image, ( 4 + 1 + sizeof( struct blot_image_data ) ) );
		outmsg->message_id = PMID_BlotImageSizedTo;
		outmsg->data.blot_image.x = x;
		outmsg->data.blot_image.y = y;
		outmsg->data.blot_image.w = wd;
		outmsg->data.blot_image.h = ht;
		outmsg->data.blot_image.xs = xs;
		outmsg->data.blot_image.ys = ys;
		outmsg->data.blot_image.image_id = ((PVPImage)pIF)->id;
		outmsg->data.blot_image.server_image_id = image->id;
		{
			TEXTSTR json_msg = json_build_message( cto, outmsg );
			AppendJSON( image, json_msg );
			Release( json_msg );
		}
	}
	else
	{
		va_list args;
		va_start( args, method );
		l.real_interface->_BlotImageEx( ((PVPImage)pDest)->image, ((PVPImage)pIF)->image, x, y, xs, ys, wd, ht
					, nTransparent
					, method
					, va_arg( args, CDATA ), va_arg( args, CDATA ), va_arg( args, CDATA ) );
	}
}

static void CPROC VidlibProxy_BlotScaledImageSizedEx( Image pifDest, Image pifSrc
											  , S_32 xd, S_32 yd
											  , _32 wd, _32 hd
											  , S_32 xs, S_32 ys
											  , _32 ws, _32 hs
											  , _32 nTransparent
											  , _32 method, ... )
{
	struct json_context_object *cto;
	PVPImage image = (PVPImage)pifDest;
	struct common_message *outmsg;
	cto = (struct json_context_object *)GetLink( &l.messages, PMID_BlotScaledImageSizedTo );
	if( !cto )
		cto = WebSockInitJson( PMID_BlotScaledImageSizedTo );

	// sending this clears the flag.
	if( ((PVPImage)pifSrc)->image->flags & IF_FLAG_UPDATED )
		SendClientMessage( PMID_ImageData, pifSrc );

	outmsg = (struct common_message*)GetMessageBuf( image, ( 4 + 1 + sizeof( struct blot_scaled_image_data ) ) );
	outmsg->message_id = PMID_BlotScaledImageSizedTo;
	outmsg->data.blot_scaled_image.x = xd;
	outmsg->data.blot_scaled_image.y = yd;
	outmsg->data.blot_scaled_image.w = wd;
	outmsg->data.blot_scaled_image.h = hd;
	outmsg->data.blot_scaled_image.xs = xs;
	outmsg->data.blot_scaled_image.ys = ys;
	outmsg->data.blot_scaled_image.ws = ws;
	outmsg->data.blot_scaled_image.hs = hs;
	outmsg->data.blot_scaled_image.image_id = ((PVPImage)pifSrc)->id;
	outmsg->data.blot_scaled_image.server_image_id = image->id;
	{
		TEXTSTR json_msg = json_build_message( cto, outmsg );
		AppendJSON( image, json_msg );
		lprintf( "json: %p", json_msg );
		Release( json_msg );
	}
}


#define DIMAGE_DATA_PROC(type,name,args)  static type (CPROC VidlibProxy2_##name)args;static type (CPROC* VidlibProxy_##name)args = VidlibProxy2_##name; type (CPROC VidlibProxy2_##name)args

DIMAGE_DATA_PROC( void,plot,		( Image pi, S_32 x, S_32 y, CDATA c ))
{
	if( ((PVPImage)pi)->render_id != INVALID_INDEX )
	{
	}
	else
	{
		l.real_interface->_plot[0]( ((PVPImage)pi)->image, x, y, c );
	}
}

DIMAGE_DATA_PROC( void,plotalpha, ( Image pi, S_32 x, S_32 y, CDATA c ))
{
	if( ((PVPImage)pi)->render_id != INVALID_INDEX )
	{
	}
	else
	{
		l.real_interface->_plot[0]( ((PVPImage)pi)->image, x, y, c );
	}
}

DIMAGE_DATA_PROC( CDATA,getpixel, ( Image pi, S_32 x, S_32 y ))
{
	if( ((PVPImage)pi)->render_id != INVALID_INDEX )
	{
	}
	else
	{
		PVPImage my_image = (PVPImage)pi;
		if( my_image )
		{
			return (*l.real_interface->_getpixel)( my_image->image, x, y );
		}
	}
	return 0;
}

DIMAGE_DATA_PROC( void,do_line,	  ( Image pifDest, S_32 x, S_32 y, S_32 xto, S_32 yto, CDATA color ))
{
	struct json_context_object *cto;
	PVPImage image = (PVPImage)pifDest;
	struct common_message *outmsg;
	cto = (struct json_context_object *)GetLink( &l.messages, PMID_DrawLine );
	if( !cto )
		cto = WebSockInitJson( PMID_DrawLine );

	outmsg = (struct common_message*)GetMessageBuf( image, ( 4 + 1 + sizeof( struct blot_scaled_image_data ) ) );
	outmsg->message_id = PMID_DrawLine;
	outmsg->data.line.server_image_id = image->id;
	outmsg->data.line.x1 = x;
	outmsg->data.line.y1 = y;
	outmsg->data.line.x2 = xto;
	outmsg->data.line.y2 = yto;
	outmsg->data.line.color = color;
	{
		TEXTSTR json_msg = json_build_message( cto, outmsg );
		AppendJSON( image, json_msg );
		lprintf( "json: %p", json_msg );
		Release( json_msg );
	}

}

DIMAGE_DATA_PROC( void,do_lineAlpha,( Image pBuffer, S_32 x, S_32 y, S_32 xto, S_32 yto, CDATA color))
{
	VidlibProxy2_do_line( pBuffer, x, y, xto, yto, color );
}


DIMAGE_DATA_PROC( void,do_hline,	  ( Image pImage, S_32 y, S_32 xfrom, S_32 xto, CDATA color ))
{
	VidlibProxy2_do_line( pImage, xfrom, y, xto, y, color );
}

DIMAGE_DATA_PROC( void,do_vline,	  ( Image pImage, S_32 x, S_32 yfrom, S_32 yto, CDATA color ))
{
	VidlibProxy2_do_line( pImage, x, yfrom, x, yto, color );
}

DIMAGE_DATA_PROC( void,do_hlineAlpha,( Image pImage, S_32 y, S_32 xfrom, S_32 xto, CDATA color ))
{
	VidlibProxy2_do_line( pImage, xfrom, y, xto, y, color );
}

DIMAGE_DATA_PROC( void,do_vlineAlpha,( Image pImage, S_32 x, S_32 yfrom, S_32 yto, CDATA color ))
{
	VidlibProxy2_do_line( pImage, x, yfrom, x, yto, color );
}

static SFTFont CPROC VidlibProxy_GetDefaultFont ( void )
{
	return l.real_interface->_GetDefaultFont( );
}

static _32 CPROC VidlibProxy_GetFontHeight  ( SFTFont font )
{
	return l.real_interface->_GetFontHeight( font );
}

static _32 CPROC VidlibProxy_GetStringSizeFontEx( CTEXTSTR pString, size_t len, _32 *width, _32 *height, SFTFont UseFont )
{
	return l.real_interface->_GetStringSizeFontEx( pString, len, width, height, UseFont );
}

static void CPROC VidlibProxy_PutCharacterFont		  ( Image pImage, S_32 x, S_32 y, CDATA color, CDATA background, TEXTCHAR c, SFTFont font )
{
	l.real_interface->_PutCharacterFont( ((PVPImage)pImage)->image, x, y, color, background, c, font );
}

static void CPROC VidlibProxy_PutCharacterVerticalFont( Image pImage, S_32 x, S_32 y, CDATA color, CDATA background, TEXTCHAR c, SFTFont font )
{
	l.real_interface->_PutCharacterVerticalFont( ((PVPImage)pImage)->image, x, y, color, background, c, font );
}

static void CPROC VidlibProxy_PutCharacterInvertFont  ( Image pImage, S_32 x, S_32 y, CDATA color, CDATA background, TEXTCHAR c, SFTFont font )
{
	l.real_interface->_PutCharacterInvertFont( ((PVPImage)pImage)->image, x, y, color, background, c, font );
}

static void CPROC VidlibProxy_PutCharacterVerticalInvertFont( Image pImage, S_32 x, S_32 y, CDATA color, CDATA background, TEXTCHAR c, SFTFont font )
{
	l.real_interface->_PutCharacterVerticalInvertFont( ((PVPImage)pImage)->image, x, y, color, background, c, font );
}

static void CPROC VidlibProxy_PutStringFontEx  ( Image pImage, S_32 x, S_32 y, CDATA color, CDATA background
												, CTEXTSTR pc, size_t nLen, SFTFont font )
{
	l.real_interface->_PutStringFontEx( ((PVPImage)pImage)->image, x, y, color, background, pc, nLen, font );
}

static void CPROC VidlibProxy_PutStringVerticalFontEx		( Image pImage, S_32 x, S_32 y, CDATA color, CDATA background, CTEXTSTR pc, size_t nLen, SFTFont font )
{
	l.real_interface->_PutStringVerticalFontEx( ((PVPImage)pImage)->image, x, y, color, background, pc, nLen, font );
}

static void CPROC VidlibProxy_PutStringInvertFontEx		  ( Image pImage, S_32 x, S_32 y, CDATA color, CDATA background, CTEXTSTR pc, size_t nLen, SFTFont font )
{
	l.real_interface->_PutStringInvertFontEx( ((PVPImage)pImage)->image, x, y, color, background, pc, nLen, font );
}

static void CPROC VidlibProxy_PutStringInvertVerticalFontEx( Image pImage, S_32 x, S_32 y, CDATA color, CDATA background, CTEXTSTR pc, size_t nLen, SFTFont font )
{
	l.real_interface->_PutStringInvertVerticalFontEx( ((PVPImage)pImage)->image, x, y, color, background, pc, nLen, font );
}

static _32 CPROC VidlibProxy_GetMaxStringLengthFont( _32 width, SFTFont UseFont )
{
	return l.real_interface->_GetMaxStringLengthFont( width, UseFont );
}

static void CPROC VidlibProxy_GetImageSize ( Image pImage, _32 *width, _32 *height )
{
	if( width )
		(*width) = ((PVPImage)pImage)->w;
	if( height )
		(*height) = ((PVPImage)pImage)->h;
}

static SFTFont CPROC VidlibProxy_LoadFont ( SFTFont font )
{
}
			/* <combine sack::image::UnloadFont@SFTFont>
				
				\ \												*/
			IMAGE_PROC_PTR( void, UnloadFont )					  ( SFTFont font );

/* Internal
	Interface index 44
	
	This is used by internal methods to transfer image and font
	data to the render agent.											  */	IMAGE_PROC_PTR( DataState, BeginTransferData )	 ( _32 total_size, _32 segsize, CDATA data );
/* Internal
	Interface index 45
	
	Used internally to transfer data to render agent. */	IMAGE_PROC_PTR( void, ContinueTransferData )		( DataState state, _32 segsize, CDATA data );
/* Internal
	Interface index 46
	
	Command issues at end of data transfer to decode the data
	into an image.														  */	IMAGE_PROC_PTR( Image, DecodeTransferredImage )	 ( DataState state );
/* After a data transfer decode the information as a font.
	Internal
	Interface index 47												  */	IMAGE_PROC_PTR( SFTFont, AcceptTransferredFont )	  ( DataState state );

DIMAGE_DATA_PROC( CDATA, ColorAverage,( CDATA c1, CDATA c2
													, int d, int max ))
{
	return c1;
}
/* <combine sack::image::SyncImage>
	
	Internal
	Interface index 49					*/	IMAGE_PROC_PTR( void, SyncImage )					  ( void );

static PCDATA CPROC VidlibProxy_GetImageSurface 		 ( Image pImage )
{
	if( pImage )
	{
		if( ((PVPImage)pImage)->render_id == INVALID_INDEX )
			return l.real_interface->_GetImageSurface( ((PVPImage)pImage)->image );
	}
	return NULL;
}

/* <combine sack::image::IntersectRectangle@IMAGE_RECTANGLE *@IMAGE_RECTANGLE *@IMAGE_RECTANGLE *>
				
				\ \																															*/
			IMAGE_PROC_PTR( int, IntersectRectangle )		( IMAGE_RECTANGLE *r, IMAGE_RECTANGLE *r1, IMAGE_RECTANGLE *r2 );
	/* <combine sack::image::MergeRectangle@IMAGE_RECTANGLE *@IMAGE_RECTANGLE *@IMAGE_RECTANGLE *>
		
		\ \																													  */
	IMAGE_PROC_PTR( int, MergeRectangle )( IMAGE_RECTANGLE *r, IMAGE_RECTANGLE *r1, IMAGE_RECTANGLE *r2 );
	/* <combine sack::image::GetImageAuxRect@Image@P_IMAGE_RECTANGLE>
		
		\ \																				*/
	IMAGE_PROC_PTR( void, GetImageAuxRect )	( Image pImage, P_IMAGE_RECTANGLE pRect );
	/* <combine sack::image::SetImageAuxRect@Image@P_IMAGE_RECTANGLE>
		
		\ \																				*/
	IMAGE_PROC_PTR( void, SetImageAuxRect )	( Image pImage, P_IMAGE_RECTANGLE pRect );

static void CPROC VidlibProxy_OrphanSubImage ( Image pImage )
{
	PVPImage image = (PVPImage)pImage;
	if( image )
	{
		//if( !image->parent
		//	|| ( pImage->flags & IF_FLAG_OWN_DATA ) )
		//	return;
		if( image->prior )
			image->prior->next = image->next;
		else
			image->parent->child = image->next;

		if( image->next )
			image->next->prior = image->prior;

		image->parent = NULL;
		image->next = NULL; 
		image->prior = NULL; 
		
		if( image->image )
			l.real_interface->_OrphanSubImage( image->image );
	}
}

static void SmearRenderFlag( PVPImage image )
{
	for( ; image; image = image->next )
	{
		if( image->image && ( ( image->render_id = image->parent->render_id ) != INVALID_INDEX ) )
			image->image->flags |= IF_FLAG_FINAL_RENDER;
		image->image->reverse_interface = &ProxyImageInterface;
		image->image->reverse_interface_instance = image;
		SmearRenderFlag( image->child );
	}
}


static void CPROC VidlibProxy_AdoptSubImage ( Image pFoster, Image pOrphan )
{
	PVPImage foster = (PVPImage)pFoster;
	PVPImage orphan = (PVPImage)pOrphan;
	if( foster && orphan )
	{
		if( ( orphan->next = foster->child ) )
			orphan->next->prior = orphan;
		orphan->parent = foster;
		foster->child = orphan;
		orphan->prior = NULL; // otherwise would be undefined
		SmearRenderFlag( orphan );

		if( foster->image && orphan->image )
			l.real_interface->_AdoptSubImage( foster->image, orphan->image );
	}
}

static void CPROC VidlibProxy_TransferSubImages( Image pImageTo, Image pImageFrom )
{
	PVPImage tmp;
	while( tmp = ((PVPImage)pImageFrom)->child )
	{
		// moving a child allows it to keep all of it's children too?
		// I think this is broken in that case; Orphan removes from the family entirely?
		VidlibProxy_OrphanSubImage( (Image)tmp );
		VidlibProxy_AdoptSubImage( (Image)pImageTo, (Image)tmp );
	}
}

	/* <combine sack::image::MakeSpriteImageFileEx@CTEXTSTR fname>
		
		\ \																			*/
	IMAGE_PROC_PTR( PSPRITE, MakeSpriteImageFileEx )( CTEXTSTR fname DBG_PASS );
	/* <combine sack::image::MakeSpriteImageEx@Image image>
		
		\ \																  */
	IMAGE_PROC_PTR( PSPRITE, MakeSpriteImageEx )( Image image DBG_PASS );
	/* <combine sack::image::rotate_scaled_sprite@Image@PSPRITE@fixed@fixed@fixed>
		
		\ \																								 */
	IMAGE_PROC_PTR( void	, rotate_scaled_sprite )(Image bmp, PSPRITE sprite, fixed angle, fixed scale_width, fixed scale_height);
	/* <combine sack::image::rotate_sprite@Image@PSPRITE@fixed>
		
		\ \																		*/
	IMAGE_PROC_PTR( void	, rotate_sprite )(Image bmp, PSPRITE sprite, fixed angle);
 /* <combine sack::image::BlotSprite@Image@PSPRITE>
																	  
	 Internal
	Interface index 61															 */
		IMAGE_PROC_PTR( void	, BlotSprite )( Image pdest, PSPRITE ps );
	 /* <combine sack::image::DecodeMemoryToImage@P_8@_32>
		 
		 \ \																*/
	 IMAGE_PROC_PTR( Image, DecodeMemoryToImage )( P_8 buf, _32 size );

/* <combine sack::image::GetFontRenderData@SFTFont@POINTER *@_32 *>
	
	\ \																			  */
IMAGE_PROC_PTR( PSPRITE, SetSpriteHotspot )( PSPRITE sprite, S_32 x, S_32 y );
/* <combine sack::image::SetSpritePosition@PSPRITE@S_32@S_32>
	
	\ \																		  */
IMAGE_PROC_PTR( PSPRITE, SetSpritePosition )( PSPRITE sprite, S_32 x, S_32 y );
	/* <combine sack::image::UnmakeImageFileEx@Image pif>
		
		\ \																*/
	IMAGE_PROC_PTR( void, UnmakeSprite )( PSPRITE sprite, int bForceImageAlso );
/* <combine sack::image::GetGlobalFonts>
	
	\ \											  */

/* <combinewith sack::image::GetStringRenderSizeFontEx@CTEXTSTR@_32@_32 *@_32 *@_32 *@SFTFont, sack::image::GetStringRenderSizeFontEx@CTEXTSTR@size_t@_32 *@_32 *@_32 *@SFTFont>
	
	\ \																																																							*/
IMAGE_PROC_PTR( _32, GetStringRenderSizeFontEx )( CTEXTSTR pString, size_t nLen, _32 *width, _32 *height, _32 *charheight, SFTFont UseFont );

IMAGE_PROC_PTR( SFTFont, RenderScaledFont )( CTEXTSTR name, _32 width, _32 height, PFRACTION width_scale, PFRACTION height_scale, _32 flags );
IMAGE_PROC_PTR( SFTFont, RenderScaledFontEx )( CTEXTSTR name, _32 width, _32 height, PFRACTION width_scale, PFRACTION height_scale, _32 flags, size_t *pnFontDataSize, POINTER *pFontData );

IMAGE_PROC_PTR( CDATA, MakeColor )( COLOR_CHANNEL r, COLOR_CHANNEL green, COLOR_CHANNEL b );
IMAGE_PROC_PTR( CDATA, MakeAlphaColor )( COLOR_CHANNEL r, COLOR_CHANNEL green, COLOR_CHANNEL b, COLOR_CHANNEL a );


IMAGE_PROC_PTR( PTRANSFORM, GetImageTransformation )( Image pImage );
IMAGE_PROC_PTR( void, SetImageRotation )( Image pImage, int edge_flag, RCOORD offset_x, RCOORD offset_y, RCOORD rx, RCOORD ry, RCOORD rz );
IMAGE_PROC_PTR( void, RotateImageAbout )( Image pImage, int edge_flag, RCOORD offset_x, RCOORD offset_y, PVECTOR vAxis, RCOORD angle );

static void CPROC VidlibProxy_MarkImageDirty ( Image pImage )
{
	l.real_interface->_MarkImageDirty( ((PVPImage)pImage)->image );
	if( 0 )
	{
		size_t outlen;
		TEXTSTR encoded_image;
	
		if( ((PVPImage)pImage)->parent )
			encoded_image = EncodeImage( ((PVPImage)pImage)->parent->image, &outlen );
		else
			encoded_image = EncodeImage( ((PVPImage)pImage)->image, &outlen );
		Release( encoded_image );
	}
}

static Image CPROC VidlibProxy_GetNativeImage( Image pImage )
{
	return ((PVPImage)pImage)->image;
}

IMAGE_PROC_PTR( void, DumpFontCache )( void );
IMAGE_PROC_PTR( void, RerenderFont )( SFTFont font, S_32 width, S_32 height, PFRACTION width_scale, PFRACTION height_scale );
// option(1) == use GL_RGBA_EXT; option(2)==clamp; option(4)==repeat
IMAGE_PROC_PTR( int, ReloadTexture )( Image child_image, int option );
// option(1) == use GL_RGBA_EXT; option(2)==clamp; option(4)==repeat
IMAGE_PROC_PTR( int, ReloadShadedTexture )( Image child_image, int option, CDATA color );
// option(1) == use GL_RGBA_EXT; option(2)==clamp; option(4)==repeat
IMAGE_PROC_PTR( int, ReloadMultiShadedTexture )( Image child_image, int option, CDATA red, CDATA green, CDATA blue );

IMAGE_PROC_PTR( void, SetImageTransformRelation )( Image pImage, enum image_translation_relation relation, PRCOORD aux );
IMAGE_PROC_PTR( void, Render3dImage )( Image pImage, PCVECTOR o, LOGICAL render_pixel_scaled );
IMAGE_PROC_PTR( void, DumpFontFile )( CTEXTSTR name, SFTFont font_to_dump );
IMAGE_PROC_PTR( void, Render3dText )( CTEXTSTR string, int characters, CDATA color, SFTFont font, VECTOR o, LOGICAL render_pixel_scaled );


 IMAGE_INTERFACE ProxyImageInterface = {
	VidlibProxy_SetStringBehavior,
		VidlibProxy_SetBlotMethod,
		VidlibProxy_BuildImageFileEx,
		VidlibProxy_MakeImageFileEx,
		VidlibProxy_MakeSubImageEx,
		VidlibProxy_RemakeImageEx,
		VidlibProxy_LoadImageFileEx,
		VidlibProxy_UnmakeImageFileEx,
		VidlibProxy_ResizeImageEx,
		VidlibProxy_MoveImage,
		VidlibProxy_BlatColor
		, VidlibProxy_BlatColorAlpha
		, VidlibProxy_BlotImageEx
		, VidlibProxy_BlotImageSizedEx
		, VidlibProxy_BlotScaledImageSizedEx
		, &VidlibProxy_plot
		, &VidlibProxy_plotalpha
		, &VidlibProxy_getpixel
		, &VidlibProxy_do_line
		, &VidlibProxy_do_lineAlpha
		, &VidlibProxy_do_hline
		, &VidlibProxy_do_vline
		, &VidlibProxy_do_hlineAlpha
		, &VidlibProxy_do_vlineAlpha
		, VidlibProxy_GetDefaultFont
		, VidlibProxy_GetFontHeight
		, VidlibProxy_GetStringSizeFontEx
		, VidlibProxy_PutCharacterFont
		, VidlibProxy_PutCharacterVerticalFont
		, VidlibProxy_PutCharacterInvertFont
		, VidlibProxy_PutCharacterVerticalInvertFont
		, VidlibProxy_PutStringFontEx
		, VidlibProxy_PutStringVerticalFontEx
		, VidlibProxy_PutStringInvertFontEx
		, VidlibProxy_PutStringInvertVerticalFontEx
		, VidlibProxy_GetMaxStringLengthFont
		, VidlibProxy_GetImageSize

		, NULL//VidlibProxy_LoadFont
		, NULL//VidlibProxy_UnloadFont
		, NULL//VidlibProxy_BeginTransferData
		, NULL//VidlibProxy_ContinueTransferData
		, NULL//VidlibProxy_DecodeTransferredImage
		, NULL//VidlibProxy_AcceptTransferredFont
		, &VidlibProxy_ColorAverage
		, NULL//VidlibProxy_SyncImage
		, VidlibProxy_GetImageSurface
		, NULL//VidlibProxy_IntersectRectangle
		, NULL//VidlibProxy_MergeRectangle
		, NULL// ** VidlibProxy_GetImageAuxRect
		, NULL// ** VidlibProxy_SetImageAuxRect
		, VidlibProxy_OrphanSubImage
		, VidlibProxy_AdoptSubImage
		, NULL // *****   VidlibProxy_MakeSpriteImageFileEx
		, NULL // *****   VidlibProxy_MakeSpriteImageEx
		, NULL // *****   VidlibProxy_rotate_scaled_sprite
		, NULL // *****   VidlibProxy_rotate_sprite
		, NULL // *****   VidlibProxy_BlotSprite
		, NULL // *****   VidlibProxy_DecodeMemoryToImage
		, NULL//VidlibProxy_InternalRenderFontFile
		, NULL//VidlibProxy_InternalRenderFont
		, NULL//VidlibProxy_RenderScaledFontData
		, NULL//VidlibProxy_RenderFontFileScaledEx
		, NULL//VidlibProxy_DestroyFont
		, NULL
		, NULL//VidlibProxy_GetFontRenderData
		, NULL//VidlibProxy_SetFontRendererData
		, NULL //VidlibProxy_SetSpriteHotspot
		, NULL //VidlibProxy_SetSpritePosition
		, NULL //VidlibProxy_UnmakeSprite
		, NULL //VidlibProxy_struct font_global_tag *, GetGlobalFonts)( void );

, NULL //IMAGE_PROC_PTR( _32, GetStringRenderSizeFontEx )( CTEXTSTR pString, size_t nLen, _32 *width, _32 *height, _32 *charheight, SFTFont UseFont );

, NULL //IMAGE_PROC_PTR( Image, LoadImageFileFromGroupEx )( INDEX group, CTEXTSTR filename DBG_PASS );

, NULL //IMAGE_PROC_PTR( SFTFont, RenderScaledFont )( CTEXTSTR name, _32 width, _32 height, PFRACTION width_scale, PFRACTION height_scale, _32 flags );
, NULL //IMAGE_PROC_PTR( SFTFont, RenderScaledFontEx )( CTEXTSTR name, _32 width, _32 height, PFRACTION width_scale, PFRACTION height_scale, _32 flags, size_t *pnFontDataSize, POINTER *pFontData );

, NULL //IMAGE_PROC_PTR( COLOR_CHANNEL, GetRedValue )( CDATA color ) ;
, NULL //IMAGE_PROC_PTR( COLOR_CHANNEL, GetGreenValue )( CDATA color );
, NULL //IMAGE_PROC_PTR( COLOR_CHANNEL, GetBlueValue )( CDATA color );
, NULL //IMAGE_PROC_PTR( COLOR_CHANNEL, GetAlphaValue )( CDATA color );
, NULL //IMAGE_PROC_PTR( CDATA, SetRedValue )( CDATA color, COLOR_CHANNEL r ) ;
, NULL //IMAGE_PROC_PTR( CDATA, SetGreenValue )( CDATA color, COLOR_CHANNEL green );
, NULL //IMAGE_PROC_PTR( CDATA, SetBlueValue )( CDATA color, COLOR_CHANNEL b );
, NULL //IMAGE_PROC_PTR( CDATA, SetAlphaValue )( CDATA color, COLOR_CHANNEL a );
, NULL //IMAGE_PROC_PTR( CDATA, MakeColor )( COLOR_CHANNEL r, COLOR_CHANNEL green, COLOR_CHANNEL b );
, NULL //IMAGE_PROC_PTR( CDATA, MakeAlphaColor )( COLOR_CHANNEL r, COLOR_CHANNEL green, COLOR_CHANNEL b, COLOR_CHANNEL a );

, NULL //IMAGE_PROC_PTR( PTRANSFORM, GetImageTransformation )( Image pImage );
, NULL //IMAGE_PROC_PTR( void, SetImageRotation )( Image pImage, int edge_flag, RCOORD offset_x, RCOORD offset_y, RCOORD rx, RCOORD ry, RCOORD rz );
, NULL //IMAGE_PROC_PTR( void, RotateImageAbout )( Image pImage, int edge_flag, RCOORD offset_x, RCOORD offset_y, PVECTOR vAxis, RCOORD angle );
, NULL //IMAGE_PROC_PTR( void, MarkImageDirty )( Image pImage );

, NULL //IMAGE_PROC_PTR( void, DumpFontCache )( void );
, NULL //IMAGE_PROC_PTR( void, RerenderFont )( SFTFont font, S_32 width, S_32 height, PFRACTION width_scale, PFRACTION height_scale );
// option(1) == use GL_RGBA_EXT; option(2)==clamp; option(4)==repeat
, NULL //IMAGE_PROC_PTR( int, ReloadTexture )( Image child_image, int option );
// option(1) == use GL_RGBA_EXT; option(2)==clamp; option(4)==repeat
, NULL //IMAGE_PROC_PTR( int, ReloadShadedTexture )( Image child_image, int option, CDATA color );
// option(1) == use GL_RGBA_EXT; option(2)==clamp; option(4)==repeat
, NULL //IMAGE_PROC_PTR( int, ReloadMultiShadedTexture )( Image child_image, int option, CDATA red, CDATA green, CDATA blue );

, NULL //IMAGE_PROC_PTR( void, SetImageTransformRelation )( Image pImage, enum image_translation_relation relation, PRCOORD aux );
, NULL //IMAGE_PROC_PTR( void, Render3dImage )( Image pImage, PCVECTOR o, LOGICAL render_pixel_scaled );
, NULL // IMAGE_PROC_PTR( void, DumpFontFile )( CTEXTSTR name, SFTFont font_to_dump );
, NULL //IMAGE_PROC_PTR( void, Render3dText )( CTEXTSTR string, int characters, CDATA color, SFTFont font, VECTOR o, LOGICAL render_pixel_scaled );
};

static CDATA CPROC VidlibProxy_SetRedValue( CDATA color, COLOR_CHANNEL r )
{
	return ( ((color)&0xFFFFFF00) | ( ((r)&0xFF)<<0 ) );
}
static CDATA CPROC VidlibProxy_SetGreenValue( CDATA color, COLOR_CHANNEL green )
{
	return ( ((color)&0xFFFF00FF) | ( ((green)&0xFF)<<8 ) );
}
static CDATA CPROC VidlibProxy_SetBlueValue( CDATA color, COLOR_CHANNEL b )
{
	return ( ((color)&0xFF00FFFF) | ( ((b)&0xFF)<<16 ) );
}
static CDATA CPROC VidlibProxy_SetAlphaValue( CDATA color, COLOR_CHANNEL a )
{
	return ( ((color)&0xFFFFFF) | ( (a)<<24 ) );
}

static COLOR_CHANNEL CPROC VidlibProxy_GetRedValue( CDATA color )
{
	return (color & 0xFF ) >> 0;
}

static COLOR_CHANNEL CPROC VidlibProxy_GetGreenValue( CDATA color )
{
	return (COLOR_CHANNEL)((color & 0xFF00 ) >> 8);
}

static COLOR_CHANNEL CPROC VidlibProxy_GetBlueValue( CDATA color )
{
	return (COLOR_CHANNEL)((color & 0x00FF0000 ) >> 16);
}

static COLOR_CHANNEL CPROC VidlibProxy_GetAlphaValue( CDATA color )
{
	return (COLOR_CHANNEL)((color & 0xFF000000 ) >> 24);
}

static CDATA CPROC VidlibProxy_MakeAlphaColor( COLOR_CHANNEL r, COLOR_CHANNEL grn, COLOR_CHANNEL b, COLOR_CHANNEL alpha )
{
#  ifdef _WIN64
#	 define _AND_FF &0xFF
#  else
/* This is a macro to cure a 64bit warning in visual studio. */
#	 define _AND_FF
#  endif
#define _AColor( r,g,b,a ) (((_32)( ((_8)((b)_AND_FF))|((_16)((_8)((g))_AND_FF)<<8))|(((_32)((_8)((r))_AND_FF)<<16)))|(((a)_AND_FF)<<24))
	return _AColor( r, grn, b, alpha );
}

static CDATA CPROC VidlibProxy_MakeColor( COLOR_CHANNEL r, COLOR_CHANNEL grn, COLOR_CHANNEL b )
{
	return VidlibProxy_MakeAlphaColor( r,grn,b, 0xFF );
}

static void InitImageInterface( void )
{
	ProxyImageInterface._GetRedValue = VidlibProxy_GetRedValue;
	ProxyImageInterface._GetGreenValue = VidlibProxy_GetGreenValue;
	ProxyImageInterface._GetBlueValue = VidlibProxy_GetBlueValue;
	ProxyImageInterface._GetAlphaValue = VidlibProxy_GetAlphaValue;
	ProxyImageInterface._SetRedValue = VidlibProxy_SetRedValue;
	ProxyImageInterface._SetGreenValue = VidlibProxy_SetGreenValue;
	ProxyImageInterface._SetBlueValue = VidlibProxy_SetBlueValue;
	ProxyImageInterface._SetAlphaValue = VidlibProxy_SetAlphaValue;
	ProxyImageInterface._MakeColor = VidlibProxy_MakeColor;
	ProxyImageInterface._MakeAlphaColor = VidlibProxy_MakeAlphaColor;
	ProxyImageInterface._LoadImageFileFromGroupEx = VidlibProxy_LoadImageFileFromGroupEx;
	ProxyImageInterface._GetStringSizeFontEx = VidlibProxy_GetStringSizeFontEx;
	ProxyImageInterface._GetFontHeight = VidlibProxy_GetFontHeight;
	ProxyImageInterface._OrphanSubImage = VidlibProxy_OrphanSubImage;
	ProxyImageInterface._AdoptSubImage = VidlibProxy_AdoptSubImage;
	ProxyImageInterface._TransferSubImages = VidlibProxy_TransferSubImages;
	ProxyImageInterface._MarkImageDirty = VidlibProxy_MarkImageDirty;
	ProxyImageInterface._GetNativeImage = VidlibProxy_GetNativeImage;

	// ============= FONT Support ============================
	// these should go through real_interface
	ProxyImageInterface._RenderFontFileScaledEx = l.real_interface->_RenderFontFileScaledEx;
	ProxyImageInterface._RenderScaledFont = l.real_interface->_RenderScaledFont;
	ProxyImageInterface._RenderScaledFontData = l.real_interface->_RenderScaledFontData;
	ProxyImageInterface._RerenderFont = l.real_interface->_RerenderFont;
	ProxyImageInterface._RenderScaledFontEx = l.real_interface->_RenderScaledFontEx;
	ProxyImageInterface._DumpFontCache = l.real_interface->_DumpFontCache;
	ProxyImageInterface._DumpFontFile = l.real_interface->_DumpFontFile;

	ProxyImageInterface._GetFontHeight = l.real_interface->_GetFontHeight;

	ProxyImageInterface._GetFontRenderData = l.real_interface->_GetFontRenderData;
	ProxyImageInterface._GetStringRenderSizeFontEx = l.real_interface->_GetStringRenderSizeFontEx;
	ProxyImageInterface._GetStringSizeFontEx = l.real_interface->_GetStringSizeFontEx;

	// this is part of the old interface; and isn't used anymore
	//ProxyImageInterface._UnloadFont = l.real_interface->_UnloadFont;
	ProxyImageInterface._DestroyFont = l.real_interface->_DestroyFont;
	ProxyImageInterface._global_font_data = l.real_interface->_global_font_data;
	ProxyImageInterface._GetGlobalFonts = l.real_interface->_GetGlobalFonts;

}

static IMAGE_3D_INTERFACE Proxy3dImageInterface = {
	NULL
};

static POINTER CPROC GetProxyDisplayInterface( void )
{
	// open server socket
	return &ProxyInterface;
}
static void CPROC DropProxyDisplayInterface( POINTER i )
{
	// close connections
}
static POINTER CPROC Get3dProxyDisplayInterface( void )
{
	// open server socket
	return &Proxy3dInterface;
}
static void CPROC Drop3dProxyDisplayInterface( POINTER i )
{
	// close connections
}

static POINTER CPROC GetProxyImageInterface( void )
{
	// open server socket
	return &ProxyImageInterface;
}
static void CPROC DropProxyImageInterface( POINTER i )
{
	// close connections
}
static POINTER CPROC Get3dProxyImageInterface( void )
{
	// open server socket
	return &Proxy3dImageInterface;
}
static void CPROC Drop3dProxyImageInterface( POINTER i )
{
	// close connections
}

PRIORITY_PRELOAD( RegisterProxyInterface, VIDLIB_PRELOAD_PRIORITY )
{
	LoadFunction( "bag.image.dll", NULL );
	l.real_interface = (PIMAGE_INTERFACE)GetInterface( WIDE( "sack.image" ) );

	InitProxyInterface();
	// needs sack.image loaded before; fonts are passed to this
	InitImageInterface();
	RegisterInterface( WIDE( "sack.image.proxy.server" ), GetProxyImageInterface, DropProxyImageInterface );
	RegisterInterface( WIDE( "sack.image.3d.proxy.server" ), Get3dProxyImageInterface, Drop3dProxyImageInterface );
	RegisterInterface( WIDE( "sack.render.proxy.server" ), GetProxyDisplayInterface, DropProxyDisplayInterface );
	RegisterInterface( WIDE( "sack.render.3d.proxy.server" ), Get3dProxyDisplayInterface, Drop3dProxyDisplayInterface );

	// wanted to delay-init; until a renderer is actually open..
	InitService();

	// have to init all of the reply message formats;
	// sends will be initialized on-demand
	//WebSockInitReplyJson( PMID_Reply_OpenDisplayAboveUnderSizedAt );
	WebSockInitReplyJson( PMID_Event_Mouse );
}


