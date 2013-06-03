#ifndef _ACCOUNT_STRUCTURE_DEFINED
#define _ACCOUNT_STRUCTURE_DEFINED

#include <controls.h>
#include <filemon.h>

#define VER_CODE(major,minor) (((major)<<16)|(minor))

#define ACCOUNT_STRUCTURES_DEFINED

#ifndef O_BINARY
#define O_BINARY 0
#endif

typedef struct file_info_tag
{
	TEXTSTR full_name;
	CTEXTSTR name;
	struct {
		BIT_FIELD bDirectory : 1;
		BIT_FIELD bScanned : 1; // ignore the first scan from monitor files.  If not scanned, just set scanned, otherwise, update flie info.
		BIT_FIELD bDeleted : 1;
		BIT_FIELD bResequenced : 1;
	} flags;
	INDEX PathID;
	INDEX ID;  // ID in our lists
	INDEX Source_ID; // ID the server referred to this file as...
	DWORD dwSize;
	FILETIME lastmodified;
	FILETIME lastcreate;
	FILETIME lastaccess;
	_32 *crc;
	size_t crclen;
	PMONITOR pDirMon;
	PCHANGEHANDLER pHandler;
} FILE_INFO, *PFILE_INFO;

typedef struct filechange_tag
{
	size_t start;
	size_t size;
   LOGICAL last_block; /*FOLD00*/
	PFILE_INFO pFileInfo;
} FILECHANGE, *PFILECHANGE;

typedef struct netbuffer_tag {
    SOCKADDR *sa;
    char *buffer;
    int size;
    int valid;   // if we ever got a real game state....
    int present;
	 _32 LastMessage;
    DeclareLink( struct netbuffer_tag );
} NETBUFFER, *PNETBUFFER;

typedef struct address_tag {
	_32 start; // non net order (host order)
	_32 end;   // host order max (inclusive)
	struct address_tag *next;
} ADDRESS,*PADDRESS;



typedef struct accounts_tag ACCOUNT, *PACCOUNT;

typedef struct directory_tag {
	struct {
		BIT_FIELD bIncoming : 1;
		BIT_FIELD bStaticData : 1;
		BIT_FIELD bVerify : 1;
	} flags;
	char *name;  //optional name (to be done)
	_32 ID;  // optional ID (to be done)
	CTEXTSTR mask;
	PACCOUNT account;
	PLIST files;   // list of PFILE_INFO
	// for new manifest protocol...
	PMONITOR pDirMon;
	PCHANGEHANDLER pHandler;
	PLIST keep_files;
	TEXTCHAR path[];
} *PDIRECTORY, DIRECTORY;

typedef struct client_connection
{
	struct
	{
		BIT_FIELD bUpdated : 1; // set if server sends any FDAT commands (or hears a SEND from the client)
		BIT_FIELD failed : 1;
	} flags;
	_32 timer;
	int lastupdate;
	INDEX file;
	char *buffer;
	char LastFile[320];
	PCLIENT pc;
	PLIST Monitors; // one monitor for each in/out directory.
	_32 version;  // active version of client logged in...
	PLINKQUEUE segments;  // used on client side to request file info
	int segment_total;
	size_t segment_total_size;
} CLIENT_CONNECTION, *PCLIENT_CONNECTION;

struct accounts_tag {
	TEXTCHAR unique_name[128];
	struct {
		BIT_FIELD client : 1;   // is a client account, else is a server account
		BIT_FIELD bLoggedIn : 1;  // used on client side when connecting to server
		BIT_FIELD sent_login : 1;  // used on client side when connecting to server
		BIT_FIELD opened_status : 1;
		BIT_FIELD manifest_process : 1;
		BIT_FIELD bClean : 1;
		BIT_FIELD bRequestedUpdates : 1;
	} flags;
	int logincount;
	PADDRESS allowed;
	PLIST Directories;  // list of PDIRECTORY
	INDEX defined_directories; // used to count where we should be (from reading accounts) for the path ID, manifest has no name for pathIDs
	INDEX current_file;
	PNETBUFFER netbuffer;  // this is the server state.  (unused, only set when loading config)

	// in case someone else has updated,
	// just assume that we do need to send also without
	// recomputing differential.
	
	_32 SendResponce;
	_32 NextResponce;
	_32 WhatResponce;
	_32 DoScanTime;
	DeclareLink( struct accounts_tag );

	PLIST connections; // list of PCLIENT_CONNECTION

	struct {
		PCLIENT TCPClient;
		PSOCKADDR server;
		TEXTCHAR server_name[128];
		PSI_CONTROL frame;
		//_32 version;  // version received?
	} client;

	// have to synchronize this, need to set environment variables for things like the client's IP
	PLIST update_commands;  // list of strings that are executed if the client udpates (these will be launchpad commands)
	PLIST update_failure_commands;  // list of strings that are executed if the client udpates (these will be launchpad commands)
	PLIST verify_commands;  // list of strings that are executed if the client verify fails (these will be launchpad commands)

	struct file_totals
	{
		_32 count;
		size_t size;
		_32 start;   // set on client side when overall is received (used for status)
	} files;
	struct finished_file_totals
	{
		_32 count;
		size_t size;
	} finished_files;
	struct manifest_file_totals
	{
		_32 count;
		size_t size;
	} manifest_files;

	CRITICALSECTION cs;
	POINTER manifest;
	size_t manifest_len;
	size_t manifest_cursor; // as we 'PrescanFile's this is stepped through the manifest, so only files that look different are checked.
};

typedef struct monitored_directory_tag
{
	struct {
		BIT_FIELD bIncoming : 1;
		BIT_FIELD bStaticData : 1;
	} flags;
	INDEX PathID;
	PACCOUNT account;
	PCHANGEHANDLER pHandler;
	PMONITOR monitor;
	PDIRECTORY pDirectory;
	POINTER data;
	PCLIENT_CONNECTION pcc;
	INDEX current_file;
} MONDIR, *PMONDIR;

typedef struct connection_tag {
	PCLIENT pc;
	_32     LastCommunication;
	//_32     Version;
} CONNECTION, *PCONNECTION;

struct network_state
{
	POINTER buffer;
	PACCOUNT account;
	PACCOUNT datamirror;
	PCONNECTION connection;
	_32 last_message;
	_32 last_message_time;
	_32 pings_sent;
	_32 login_fails;
	//_32 version;
	_32 filesize; // total size of the file being checked
	_32 filestart; // start position of block received for file
	_32 filepath; // integer to indicate which path this came from and should go into
	_32 file_block_size; // a block of data may not be the whole file.
	FILETIME filetime_create;
	FILETIME filetime_modify;
	FILETIME filetime_access;
	PCLIENT_CONNECTION client_connection;
	_32 path_id;
	_32 file_id; // this is a unique identifier to server to get back this info (used in FDAT)
   POINTER longbuffer;
	_32 version;  // active version of client logged in...
	struct {
		BIT_FIELD bInUse : 1;  // if in use, do not delete
		BIT_FIELD bDeleted : 1; // if was in use, set deleted, when not in use, release
		BIT_FIELD bWantClose : 1;
		BIT_FIELD last_block : 1;
		BIT_FIELD want_update_commands : 1; // else wants verify commands
	} flags;
};
typedef struct network_state NETWORK_STATE;
typedef struct network_state *PNETWORK_STATE;

#endif
