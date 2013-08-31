// don't define exit() apache has a definition for it.
#define NO_SACK_EXIT_OVERRIDE

#include <stdhdrs.h>

#include <httpd.h>  // apache header files.
#include <http_config.h>
#include <http_protocol.h>

//HTTPD_CALL_CONVENTION

// to avoid having to build different target libraries; and there's no telling what
struct apache_interface_cdecl
{

	void (CPROC *ap_set_content_type)(request_rec *r, const char *ct);
	int  (CPROC *ap_rprintf)(request_rec *r, const char *fmt,...) __attribute__((format(printf,2,3)));
	int  (CPROC *ap_hook_handler)(ap_HOOK_handler_t, const char * const *aszPre, const char * const *aszSucc, int nOrder);
	int  (CPROC *ap_hook_post_read_request)(ap_HOOK_handler_t, const char * const *aszPre, const char * const *aszSucc, int nOrder);
	void *ap_set_string_slot;
};

struct apache_interface_stdcall
{

	void (WINAPI *ap_set_content_type)(request_rec *r, const char *ct);
	int  (WINAPI *ap_rprintf)(request_rec *r, const char *fmt,...) __attribute__((format(printf,2,3)));
	int  (WINAPI *ap_hook_handler)(ap_HOOK_handler_t, const char * const *aszPre, const char * const *aszSucc, int nOrder);
	int  (WINAPI *ap_hook_post_read_request)(ap_HOOK_handler_t, const char * const *aszPre, const char * const *aszSucc, int nOrder);
	void *ap_set_string_slot;
};

struct dir_data {
   CTEXTSTR name;
};

static struct {
	struct apache_interface_cdecl __a_c_interface;
	struct apache_interface_stdcall __a_stdcall_interface;
	struct apache_interface_cdecl *a_c_interface;
	struct apache_interface_stdcall *a_stdcall_interface;
	TEXTCHAR server_core[256];

	PLIST root_path;
#define l apache_shell_local
} l;


#define ALIAS(n,...)  ( (l.a_c_interface)?(l.a_c_interface)->n(__VA_ARGS__):((l.a_stdcall_interface)?(l.a_stdcall_interface)->n(__VA_ARGS__):0))
#define ALIAS_F(n)  ( (l.a_c_interface)?(l.a_c_interface)->n:((l.a_stdcall_interface)?(l.a_stdcall_interface)->n:0))

#define ap_set_content_type(...)        ALIAS(ap_set_content_type,__VA_ARGS__)
#define ap_rprintf(...)                 ALIAS(ap_rprintf,__VA_ARGS__)
#define ap_hook_handler(...)            ALIAS(ap_hook_handler,__VA_ARGS__)
#define ap_hook_post_read_request(...)  ALIAS(ap_hook_post_read_request,__VA_ARGS__)
//#define ap_set_string_slot(...)     ALIAS(ap_set_string_slot,__VA_ARGS__)

#define ap_set_string_slot     ALIAS(ap_set_string_slot)

PRELOAD( InitApacheModule )
{
	l.__a_c_interface.ap_set_content_type       = LoadFunction( l.server_core, "ap_set_content_type" );
	l.__a_c_interface.ap_rprintf                = LoadFunction( l.server_core, "ap_rprintf" );
	l.__a_c_interface.ap_hook_handler           = LoadFunction( l.server_core, "ap_hook_handler" );
	l.__a_c_interface.ap_hook_post_read_request = LoadFunction( l.server_core, "ap_hook_post_read_request" );

	l.__a_stdcall_interface.ap_set_content_type        = (void (__stdcall *)(request_rec *,const char *))LoadFunction( l.server_core, "ap_set_content_type" );
	l.__a_stdcall_interface.ap_rprintf                 = (int (__cdecl *)(request_rec *,const char *,...))LoadFunction( l.server_core, "ap_rprintf" );
	l.__a_stdcall_interface.ap_hook_handler            = (int (__stdcall *)(ap_HOOK_handler_t (__cdecl *),const char *const *,const char *const *,int))LoadFunction( l.server_core, "ap_hook_handler" );
	l.__a_stdcall_interface.ap_hook_post_read_request  = (int (__stdcall *)(ap_HOOK_handler_t (__cdecl *),const char *const *,const char *const *,int))LoadFunction( l.server_core, "ap_hook_post_read_request" );

   // setup one of the interfaces... 50-50 chance to get it right?
   //l.a_c_interface = &l.__a_c_interface;
   l.a_stdcall_interface = &l.__a_stdcall_interface;

}

#if 0
module AP_MODULE_DECLARE_DATA foo_module =
{
    STANDARD20_MODULE_STUFF,
    NULL, //create_dir_conf, /* Per-directory configuration handler */
    NULL, //merge_dir_conf,  /* Merge handler for per-directory configurations */
    NULL, //create_svr_conf, /* Per-server configuration handler */
    NULL, //merge_svr_conf,  /* Merge handler for per-server configurations */
    NULL, //directives,      /* Any directives we may have for httpd */
    register_hooks   /* Our hook registering function */
};
#endif



static int example_handler(request_rec *r)
{
    /* First off, we need to check if this is a call for the "example-handler" handler.
     * If it is, we accept it and do our things, if not, we simply return DECLINED,
     * and the server will try somewhere else.
	  */
   lprintf( "r_handler is %s", r->handler );
    if (!r->handler || strcmp(r->handler, "org.d3x0r.sack.apache.module")) return (DECLINED);
    
    /* Now that we are handling this request, we'll write out "Hello, world!" to the client.
     * To do so, we must first set the appropriate content type, followed by our output.
     */
    ap_set_content_type(r, "text/html");
    ap_rprintf(r, "Hello, world!");
    
    /* Lastly, we must tell the server that we took care of this request and everything went fine.
     * We do so by simply returning the value OK to the server.
     */
    return OK;
}

static void*  register_dir_handler( apr_pool_t *pool, char *x )
{
	struct dir_data *dir = New( struct dir_data );
	lprintf( "Adding to %s", x?x:"<global>" );
	dir->name = x;
	return dir;
}


//static const command_rec mosquitto_cmds[] 
/*= 
{
  AP_INIT_TAKE1("MosBroker", ap_set_string_slot,
	(void*) APR_OFFSETOF(struct dir_data, name), ACCESS_CONF,
	"Broker") ,
  AP_INIT_TAKE1("MosPort", ap_set_string_slot,
	(void*) APR_OFFSETOF(struct dir_data, name), ACCESS_CONF,
	"Port") ,
 	{NULL}	
}	*/
;

static void first_post( apr_pool_t *pool )
{
   lprintf( "here" );

}

static void register_hooks(apr_pool_t *pool)
{
	/* Create a hook in the request handler, so we get called when a request arrives */
   InvokeDeadstart();
   lprintf( "here" );
	ap_hook_handler(example_handler, NULL, NULL, APR_HOOK_FIRST);
   ap_hook_post_read_request( first_post, NULL, NULL, APR_HOOK_FIRST );
									  // ap_hook_fixups() ; // last chance hook before content generation
                             // ap_hook_log_transaction
}

module AP_MODULE_DECLARE_DATA org_d3x0r_sack_apache_module =
{
    STANDARD20_MODULE_STUFF,
    register_dir_handler, //create_dir_conf, /* Per-directory configuration handler */
    NULL, //merge_dir_conf,  /* Merge handler for per-directory configurations */
    NULL, //create_svr_conf, /* Per-server configuration handler */
    NULL, //merge_svr_conf,  /* Merge handler for per-server configurations */
    NULL, //directives,      /* Any directives we may have for httpd */
    register_hooks   /* Our hook registering function */
};


