/**
 *	@file    rsaperf.c
 *	@version e6a3d9f (HEAD, tag: MATRIXSSL-3-7-1-OPEN, origin/master, origin/HEAD, master)
 *
 *	RSA performance testing	.
 */
/*
 *	Copyright (c) 2013-2014 INSIDE Secure Corporation
 *	Copyright (c) PeerSec Networks, 2002-2011
 *	All Rights Reserved
 *
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software 
 *	into proprietary programs.  If you are unable to comply with the GPL, a 
 *	commercial license for this software may be purchased from INSIDE at
 *	http://www.insidesecure.com/eng/Company/Locations
 *	
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	See the GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

/* OPERATIONS TO TEST */
#define SIGN_OP		/* Private encrypt operations */
#define VERIFY_OP	/* Public decrypt operations */

#define ENCRYPT_OP	/* Public encrypt operations */
#define DECRYPT_OP	/* Private decrypt operations */

/* KEY SIZES TO TEST */
//#define DO_512
#define DO_1024
#define DO_2048
#define DO_4096

/* NUMBER OF ENCRYPTION OPERATIONS TO AVERAGE FOR EACH KEY LEN */
#define ITER_512	50
#define ITER_1024	50
#define ITER_2048	50
#define ITER_4096	50

/* Another pass with different keys */
/* #define INCLUDE_SECOND_SET */

#include <unistd.h> /* sleep */
#include "crypto/cryptoApi.h"

typedef void pkaCmdInfo_t;

#define PS_OH sizeof(psPool_t)

/*
	Tuned to smallest K for each key size and optimization setting.
	Private key operations take about 1/2 the RAM when optimized for size
*/
#ifdef PS_PUBKEY_OPTIMIZE_FOR_SMALLER_RAM
#define POOL_SIGN_512		(1 * 1024) + PS_OH 
#define POOL_VERIFY_512		(2 * 1024) + PS_OH
#define POOL_ENCRYPT_512	(2 * 1024) + PS_OH
#define POOL_DECRYPT_512	(1 * 1024) + PS_OH
#define POOL_MISC_512		(3 * 1024) + PS_OH

#define POOL_SIGN_1024		(2 * 1024) + PS_OH
#define POOL_VERIFY_1024	(3 * 1024) + PS_OH
#define POOL_ENCRYPT_1024	(3 * 1024) + PS_OH
#define POOL_DECRYPT_1024	(2 * 1024) + PS_OH
#define POOL_MISC_1024		(4 * 1024) + PS_OH

#define POOL_SIGN_2048		(3 * 1024) + PS_OH 
#define POOL_VERIFY_2048	(5 * 1024) + PS_OH
#define POOL_ENCRYPT_2048	(5 * 1024) + PS_OH
#define POOL_DECRYPT_2048	(3 * 1024) + PS_OH
#define POOL_MISC_2048		(7 * 1024) + PS_OH

#define POOL_SIGN_4096		(6 * 1024) + PS_OH
#define POOL_VERIFY_4096	(8 * 1024) + PS_OH
#define POOL_ENCRYPT_4096	(8 * 1024) + PS_OH
#define POOL_DECRYPT_4096	(6 * 1024) + PS_OH
#define POOL_MISC_4096		(12 * 1024) + PS_OH

#else /* PS_PUBKEY_OPTIMIZE_FOR_FASTER_SPEED */

#define POOL_SIGN_512		(3 * 1024) + PS_OH 
#define POOL_VERIFY_512		(2 * 1024) + PS_OH
#define POOL_ENCRYPT_512	(2 * 1024) + PS_OH
#define POOL_DECRYPT_512	(3 * 1024) + PS_OH
#define POOL_MISC_512		(3 * 1024) + PS_OH

#define POOL_SIGN_1024		(4 * 1024) + PS_OH
#define POOL_VERIFY_1024	(3 * 1024) + PS_OH
#define POOL_ENCRYPT_1024	(3 * 1024) + PS_OH
#define POOL_DECRYPT_1024	(4 * 1024) + PS_OH
#define POOL_MISC_1024		(5 * 1024) + PS_OH

#define POOL_SIGN_2048		(7 * 1024) + PS_OH 
#define POOL_VERIFY_2048	(5 * 1024) + PS_OH
#define POOL_ENCRYPT_2048	(5 * 1024) + PS_OH
#define POOL_DECRYPT_2048	(7 * 1024) + PS_OH
#define POOL_MISC_2048		(9 * 1024) + PS_OH

#define POOL_SIGN_4096		(12 * 1024) + PS_OH
#define POOL_VERIFY_4096	(8 * 1024) + PS_OH
#define POOL_ENCRYPT_4096	(8 * 1024) + PS_OH
#define POOL_DECRYPT_4096	(12 * 1024) + PS_OH
#define POOL_MISC_4096		(16 * 1024) + PS_OH
#endif

#ifdef DO_512
#include "rsa3e512.h"
#include "rsa17e512.h"
#include "rsa257e512.h"
#include "rsa65537e512.h"
#endif /* DO_512 */

#ifdef DO_1024
#include "rsa3e1024.h"
#include "rsa17e1024.h"
#include "rsa257e1024.h"
#include "rsa65537e1024.h"
#endif /* DO_1024 */

#ifdef DO_2048
#include "rsa3e2048.h"
#include "rsa17e2048.h"
#include "rsa257e2048.h"
#include "rsa65537e2048.h"
#endif /* DO_2048 */

#ifdef DO_4096
#include "rsa3e4096.h"
#include "rsa17e4096.h"
#include "rsa257e4096.h"
#include "rsa65537e4096.h"
#endif /* DO_4096 */

#ifdef INCLUDE_SECOND_SET
#ifdef DO_512
#include "rsa3e512_1.h"
#include "rsa17e512_1.h"
#include "rsa257e512_1.h"
#include "rsa65537e512_1.h"
#endif /* DO_512 */

#ifdef DO_1024
#include "rsa3e1024_1.h"
#include "rsa17e1024_1.h"
#include "rsa257e1024_1.h"
#include "rsa65537e1024_1.h"
#endif /* DO_1024 */

#ifdef DO_2048
#include "rsa3e2048_1.h"
#include "rsa17e2048_1.h"
#include "rsa257e2048_1.h"
#include "rsa65537e2048_1.h"
#endif /* DO_2048 */

#ifdef DO_4096
#include "rsa3e4096_1.h"
#include "rsa17e4096_1.h"
#include "rsa257e4096_1.h"
#include "rsa65537e4096_1.h"
#endif /* DO_4096 */
#endif /* INCLUDE_SECOND_SET */

typedef struct {
	char				*name;	
	const unsigned char	*key;	
	uint32				len;
	int32				iter;
	int32				poolSign;
	int32				poolVerify;
	int32				poolEncrypt;
	int32				poolDecrypt;
	int32				poolMisc;
} keyList_t;

#ifdef USE_HIGHRES_TIME
  #define psDiffMsecs(A, B) psDiffUsecs(A, B)
  #define TIME_UNITS "    %lld usecs"
#else
  #define TIME_UNITS "    %d msecs"
#endif

/*
	Add an iteration count so we don't have to run the large keys so many times
*/
static keyList_t keys[] = {
#ifdef DO_512
	{"rsa3e512", rsa3e512, sizeof(rsa3e512), ITER_512, POOL_SIGN_512,
		POOL_VERIFY_512, POOL_ENCRYPT_512, POOL_DECRYPT_512, POOL_MISC_512},
	{"rsa17e512", rsa17e512, sizeof(rsa17e512), ITER_512,POOL_SIGN_512,
		POOL_VERIFY_512, POOL_ENCRYPT_512, POOL_DECRYPT_512, POOL_MISC_512},
	{"rsa257e512", rsa257e512, sizeof(rsa257e512), ITER_512, POOL_SIGN_512,
		POOL_VERIFY_512, POOL_ENCRYPT_512, POOL_DECRYPT_512, POOL_MISC_512},
	{"rsa65537e512", rsa65537e512, sizeof(rsa65537e512), ITER_512,
		POOL_SIGN_512, POOL_VERIFY_512, POOL_ENCRYPT_512, POOL_DECRYPT_512,
		POOL_MISC_512},
#endif
#ifdef DO_1024
	{"rsa3e1024", rsa3e1024, sizeof(rsa3e1024), ITER_1024, POOL_SIGN_1024,
		POOL_VERIFY_1024, POOL_ENCRYPT_1024, POOL_DECRYPT_1024,
		POOL_MISC_1024},
	{"rsa17e1024", rsa17e1024, sizeof(rsa17e1024), ITER_1024, POOL_SIGN_1024,
		POOL_VERIFY_1024, POOL_ENCRYPT_1024, POOL_DECRYPT_1024,
		POOL_MISC_1024},
	{"rsa257e1024", rsa257e1024, sizeof(rsa257e1024), ITER_1024, POOL_SIGN_1024,
		POOL_VERIFY_1024, POOL_ENCRYPT_1024, POOL_DECRYPT_1024,
		POOL_MISC_1024},
	{"rsa65537e1024", rsa65537e1024, sizeof(rsa65537e1024), ITER_1024,
		POOL_SIGN_1024,	POOL_VERIFY_1024, POOL_ENCRYPT_1024, POOL_DECRYPT_1024,
		POOL_MISC_1024},
#endif
#ifdef DO_2048
	{"rsa3e2048", rsa3e2048, sizeof(rsa3e2048), ITER_2048, POOL_SIGN_2048,
		POOL_VERIFY_2048, POOL_ENCRYPT_2048, POOL_DECRYPT_2048,
		POOL_MISC_2048},
	{"rsa17e2048", rsa17e2048, sizeof(rsa17e2048), ITER_2048, POOL_SIGN_2048,
		POOL_VERIFY_2048, POOL_ENCRYPT_2048, POOL_DECRYPT_2048,
		POOL_MISC_2048},
	{"rsa257e2048", rsa257e2048, sizeof(rsa257e2048), ITER_2048, POOL_SIGN_2048,
		POOL_VERIFY_2048, POOL_ENCRYPT_2048, POOL_DECRYPT_2048,
		POOL_MISC_2048},
	{"rsa65537e2048", rsa65537e2048, sizeof(rsa65537e2048), ITER_2048,
		POOL_SIGN_2048,	POOL_VERIFY_2048, POOL_ENCRYPT_2048, POOL_DECRYPT_2048,
		POOL_MISC_2048},
#endif
#ifdef DO_4096
	{"rsa3e4096", rsa3e4096, sizeof(rsa3e4096), ITER_4096, POOL_SIGN_4096,
		POOL_VERIFY_4096, POOL_ENCRYPT_4096, POOL_DECRYPT_4096,
		POOL_MISC_4096},
	{"rsa17e4096", rsa17e4096, sizeof(rsa17e4096), ITER_4096, POOL_SIGN_4096,
		POOL_VERIFY_4096, POOL_ENCRYPT_4096, POOL_DECRYPT_4096,
		POOL_MISC_4096},
	{"rsa257e4096", rsa257e4096, sizeof(rsa257e4096), ITER_4096, POOL_SIGN_4096,
		POOL_VERIFY_4096, POOL_ENCRYPT_4096, POOL_DECRYPT_4096,
		POOL_MISC_4096},
	{"rsa65537e4096", rsa65537e4096, sizeof(rsa65537e4096), ITER_4096,
		POOL_SIGN_4096,	POOL_VERIFY_4096, POOL_ENCRYPT_4096, POOL_DECRYPT_4096,
		POOL_MISC_4096},
#endif
#ifdef INCLUDE_SECOND_SET
#ifdef DO_512
	{"rsa3e512_1", rsa3e5121, sizeof(rsa3e5121), ITER_512, POOL_SIGN_512,
		POOL_VERIFY_512, POOL_ENCRYPT_512, POOL_DECRYPT_512, POOL_MISC_512},
	{"rsa17e512_1", rsa17e5121, sizeof(rsa17e5121), ITER_512, POOL_SIGN_512,
		POOL_VERIFY_512, POOL_ENCRYPT_512, POOL_DECRYPT_512, POOL_MISC_512},
	{"rsa257e512_1", rsa257e5121, sizeof(rsa257e5121), ITER_512, POOL_SIGN_512,
		POOL_VERIFY_512, POOL_ENCRYPT_512, POOL_DECRYPT_512, POOL_MISC_512},
	{"rsa65537e512_1", rsa65537e5121, sizeof(rsa65537e5121), ITER_512,
		POOL_SIGN_512, POOL_VERIFY_512, POOL_ENCRYPT_512, POOL_DECRYPT_512,
		POOL_MISC_512},
#endif
#ifdef DO_1024
	{"rsa3e1024_1", rsa3e10241, sizeof(rsa3e10241), ITER_1024, POOL_SIGN_1024,
		POOL_VERIFY_1024, POOL_ENCRYPT_1024, POOL_DECRYPT_1024,
		POOL_MISC_1024},
	{"rsa17e1024_1", rsa17e10241, sizeof(rsa17e10241), ITER_1024,
		POOL_SIGN_1024,	POOL_VERIFY_1024, POOL_ENCRYPT_1024, POOL_DECRYPT_1024,
		POOL_MISC_1024},
	{"rsa257e1024_1", rsa257e10241, sizeof(rsa257e10241), ITER_1024,
		POOL_SIGN_1024,	POOL_VERIFY_1024, POOL_ENCRYPT_1024, POOL_DECRYPT_1024,
		POOL_MISC_1024},
	{"rsa65537e1024_1", rsa65537e10241, sizeof(rsa65537e10241), ITER_1024,
		POOL_SIGN_1024,	POOL_VERIFY_1024, POOL_ENCRYPT_1024, POOL_DECRYPT_1024,
		POOL_MISC_1024},
#endif
#ifdef DO_2048
	{"rsa3e2048_1", rsa3e20481, sizeof(rsa3e20481), ITER_2048, POOL_SIGN_2048,
		POOL_VERIFY_2048, POOL_ENCRYPT_2048, POOL_DECRYPT_2048,
		POOL_MISC_2048},
	{"rsa17e2048_1", rsa17e20481, sizeof(rsa17e20481), ITER_2048,
		POOL_SIGN_2048,	POOL_VERIFY_2048, POOL_ENCRYPT_2048, POOL_DECRYPT_2048,
		POOL_MISC_2048},
	{"rsa257e2048_1", rsa257e20481, sizeof(rsa257e20481), ITER_2048,
		POOL_SIGN_2048,	POOL_VERIFY_2048, POOL_ENCRYPT_2048, POOL_DECRYPT_2048,
		POOL_MISC_2048},
	{"rsa65537e2048_1", rsa65537e20481, sizeof(rsa65537e20481), ITER_2048,
		POOL_SIGN_2048,	POOL_VERIFY_2048, POOL_ENCRYPT_2048, POOL_DECRYPT_2048,
		POOL_MISC_2048},
#endif
#ifdef DO_4096
	{"rsa3e4096_1", rsa3e40961, sizeof(rsa3e40961), ITER_4096, POOL_SIGN_4096,
		POOL_VERIFY_4096, POOL_ENCRYPT_4096, POOL_DECRYPT_4096,
		POOL_MISC_4096},
	{"rsa17e4096_1", rsa17e40961, sizeof(rsa17e40961), ITER_4096,
		POOL_SIGN_4096,	POOL_VERIFY_4096, POOL_ENCRYPT_4096, POOL_DECRYPT_4096,
		POOL_MISC_4096},
	{"rsa257e4096_1", rsa257e40961, sizeof(rsa257e40961), ITER_4096,
		POOL_SIGN_4096,	POOL_VERIFY_4096, POOL_ENCRYPT_4096, POOL_DECRYPT_4096,
		POOL_MISC_4096},
	{"rsa65537e4096_1", rsa65537e40961, sizeof(rsa65537e40961), ITER_4096,
		POOL_SIGN_4096,	POOL_VERIFY_4096, POOL_ENCRYPT_4096, POOL_DECRYPT_4096,
		POOL_MISC_4096},
#endif
#endif /* INCLUDE_SECOND_SET */
	{NULL, NULL, 0}
};

/******************************************************************************/
/*
	Main
*/


#ifdef STATS
    #include <fcntl.h>
#ifdef USE_HIGHRES_TIME
	#define TIME_STRING "\t%lld"
#else
	#define TIME_STRING "\t%d"
#endif
#endif 

int main(int argc, char **argv)
{
	psPool_t		*pool, *misc;
	psPubKey_t		*privkey;
	unsigned char	*in, *out, *savein, *saveout;
	psTime_t		start, end;
	uint32			iter, i = 0;
	int32	t;
#ifdef STATS
	uint32			*table, tsize;
    FILE			*sfd;
#endif
	pkaCmdInfo_t	*pkaInfo;

	pkaInfo = NULL;
	pool = misc = NULL;
	if(psCryptoOpen(PSCRYPTO_CONFIG) < PS_SUCCESS) {
		_psTrace("Failed to initialize library:  psCryptoOpen failed\n");
		return -1;
	}
	// TIME SANITY
	psGetTime(&start, NULL);
	sleep(1);
	psGetTime(&end, NULL);
	printf("Time sanity " TIME_UNITS "\n", psDiffMsecs(start, end, NULL));

	_psTraceStr("STARTING RSAPERF\n", NULL);
#ifdef STATS
	tsize = 0;
#ifdef DO_512
	tsize += 16; /* 4 key exponents and 4 time measurements */
#endif
#ifdef DO_1024
	tsize += 16; /* 4 key exponents and 4 time measurements */
#endif
#ifdef DO_2048
	tsize += 16; /* 4 key exponents and 4 time measurements */
#endif
#ifdef DO_4096
	tsize += 16; /* 4 key exponents and 4 time measurements */
#endif
#ifdef INCLUDE_SECOND_SET
	tsize *= 2; /* Double the table size */
#endif
	table = malloc(tsize * sizeof(uint32));	
	if ((sfd = fopen("perfstat.txt", "w")) == NULL) {
		return PS_FAILURE;
	}
#ifdef USE_HIGHRES_TIME
	fprintf(sfd, "Key\tSign(usec)\tVerify\tEncrypt\tDecrypt\n");
#else
	fprintf(sfd, "Key\tSign(msec)\tVerify\tEncrypt\tDecrypt\n");
#endif
#endif /* STATS */
	
	while (keys[i].key != NULL) {
		_psTraceStr("Test %s...\n", keys[i].name);
#ifdef STATS
		fprintf(sfd, "%s", keys[i].name);
#endif
		pkcs1ParsePrivBin(misc, (unsigned char*)keys[i].key, keys[i].len,
			&privkey);
		savein = in = psMalloc(misc, privkey->keysize);
		psGetEntropy(in, privkey->keysize, NULL);
		saveout = out = psMalloc(misc, privkey->keysize);
		
#ifdef SIGN_OP
		iter = 0;
		psGetTime(&start, NULL);
		while (iter < keys[i].iter) {
			if (psRsaEncryptPriv(pool, &privkey->key->rsa, in,
					privkey->keysize - 16, out, privkey->keysize, pkaInfo) < 0){
				_psTrace("	FAILED SIGNATURE OPERATION\n");
			}	
/*
			The idea here is that I wanted the encryption to happen over
			an ever-changing value without adding any timing overhead by
			going out and getting more random data (or whatever).  So just
			passing the output as the basis for the input each time. 
*/
			in = out;
/*			
			TODO: The reason the out pointer switches back and forth is because
			if the same addr is used for in and out, there is no change to
			the data (after the first time?) even though the encryption
			seems to happen.  WHY IS THIS?
*/			
			if (iter % 2) {
				out = saveout;
			} else {
				out = savein;
			}
			iter++;
		}
		psGetTime(&end, NULL);
		
		_psTraceInt(TIME_UNITS "/sig\n",
			t = psDiffMsecs(start, end, NULL)/keys[i].iter);
		
#ifdef STATS
		fprintf(sfd, TIME_STRING, t);
#endif
#endif /* SIGN_OP */

#ifdef VERIFY_OP
/*
		TODO: find a good way to time more than a single decrypt
*/
		memset(in, 0x0, privkey->keysize);
		memcpy(in, "hello", 5);
		if (psRsaEncryptPriv(misc, &privkey->key->rsa, in, 5, out,
				privkey->keysize, pkaInfo) < 0) {
			_psTrace("	FAILED VERIFY PREP\n");
		}
		memset(in, 0x0, privkey->keysize);
		
		psGetTime(&start, NULL);
		if (psRsaDecryptPub(pool, &privkey->key->rsa, out,
				privkey->keysize, in, 5, pkaInfo) < 0) {
			_psTrace("	FAILED VERIFY OPERATION\n");
		}
		psGetTime(&end, NULL);
		if (memcmp(in, "hello", 5) != 0) {
			_psTrace("	FAILED VERIFY VERIFY\n");
		}
		_psTraceInt(TIME_UNITS "/verify\n", t = psDiffMsecs(start, end, NULL));
#ifdef STATS
		fprintf(sfd, TIME_STRING, t);
#endif
#endif /* VERIFY_OP */			
			
#ifdef ENCRYPT_OP
		iter = 0;
		psGetEntropy(in, privkey->keysize, NULL);
		psGetTime(&start, NULL);
		while (iter < keys[i].iter) {
			if (psRsaEncryptPub(pool, &privkey->key->rsa, in,
					privkey->keysize - 16, out, privkey->keysize, pkaInfo) < 0) {
				_psTrace("	FAILED ENCRYPT OPERATION\n");
			}
/*
			The idea here is that I wanted the encryption to happen over
			an ever-changing value without adding any timing overhead by
			going out and getting more random data (or whatever).  So just
			passing the output as the basis for the input each time. 
*/
			in = out;
/*			
			TODO: The reason the out pointer switches back and forth is because
			if the same addr is used for in and out, there is no change to
			the data (after the first time?) even though the encryption
			seems to happen.  WHY IS THIS?
*/			
			if (iter % 2) {
				out = saveout;
			} else {
				out = savein;
			}
			iter++;
		}
		psGetTime(&end, NULL);
		_psTraceInt(TIME_UNITS "/encrypt\n",
			t = psDiffMsecs(start, end, NULL)/keys[i].iter);
#ifdef STATS
		fprintf(sfd, TIME_STRING, t);
#endif
#endif /* ENCRYPT_OP */
			
#ifdef DECRYPT_OP
/*
		TODO: find a good way to time more than a single decrypt
*/
		if (in == out) {
			out = saveout;
		}
		memset(in, 0x0, privkey->keysize);
		memcpy(in, "hello", 5);
		if (psRsaEncryptPub(misc, &privkey->key->rsa, in, 5, out,
				privkey->keysize, pkaInfo) < 0) {
			_psTrace("	FAILED VERIFY PREP\n");
		}
		memset(in, 0x0, privkey->keysize);
		
		psGetTime(&start, NULL);
		if (psRsaDecryptPriv(pool, &privkey->key->rsa, out,
				privkey->keysize, in, 5, pkaInfo) < 0) {
			_psTrace("	FAILED DECRYPT OPERATION\n");
		}
		psGetTime(&end, NULL);
		if (memcmp(in, "hello", 5) != 0) {
			_psTrace("	FAILED DECRYPT VERIFY\n");
		}
		_psTraceInt(TIME_UNITS "/decrypt\n", t = psDiffMsecs(start, end, NULL));
#ifdef STATS
		fprintf(sfd, TIME_STRING "\n", t);
#endif
#endif /* DECRYPT_OP */	

		psFree(savein, misc);
		psFree(saveout, misc);
		psFreePubKey(privkey);
		i++;
	}
	
#ifdef STATS
	fclose(sfd);
	free(table);
#endif
#ifdef WIN32
	_psTrace("Press any key to close");
	getchar();
#endif
	_psTraceStr("FINISHED RSAPERF\n", NULL);
	psCryptoClose();
	return 0;
}
