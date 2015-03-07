/*
 * SNMP Variable Binding.  Complies with:
 *
 * RFC 1905: Protocol Operations for SNMPv2
 *
 */

/**********************************************************************
 *
 *           Copyright 1998 by Carnegie Mellon University
 * 
 *                       All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of CMU not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 * 
 * CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * 
 * Author: Ryan Troll <ryan+@andrew.cmu.edu>
 * 
 **********************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#ifdef WIN32
#include <winsock2.h>
#else  /* WIN32 */
#include <netinet/in.h>
#include <sys/param.h>
#endif /* WIN32 */

#ifdef HAVE_STRINGS_H
# include <strings.h>
#else /* HAVE_STRINGS_H */
# include <string.h>
#endif /* HAVE_STRINGS_H */

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif /* HAVE_MALLOC_H */

#include <snmp/libsnmp.h>
#if 0
#include "asn1.h"
#include "snmp_vars.h"
#include "mibii.h"
#include "snmp_api_error.h"
#include "snmp_pdu.h"
#include "snmp_msg.h"
#endif
SNMP_NAMESPACE

//static char rcsid[] = 
//"$Id: snmp_vars.c,v 1.1.1.1 2001/11/15 01:29:34 panther Exp $";

/* #define DEBUG_VARS 1 */
/* #define DEBUG_VARS_MALLOC 1 */
/* #define DEBUG_VARS_DECODE 1 */
/* #define DEBUG_VARS_ENCODE 1 */

#define ASN_PARSE_ERROR(x) { snmpInASNParseErrs_Add(1); return(x); }

/* Create a new variable_list structure representing oid Name of length Len.
 *
 * Returns NULL upon error.
 */

struct variable_list *snmp_var_new(oid *Name, int Len)
{
  struct variable_list *New;

#ifdef DEBUG_VARS
  printf("VARS: Creating.\n");
#endif

  New = (struct variable_list *)malloc(sizeof(struct variable_list));
  if (New == NULL) {
    snmp_set_api_error(SNMPERR_OS_ERR);
    return(NULL);
  }
  memset(New, '\0', sizeof(struct variable_list));
  /*  New->next_variable = NULL; */

  New->type = ASN_NULL;
  New->name_length = Len;

  if (New->name_length == 0) {
    New->name = NULL;
    return(New);
  }

  New->name = (oid *)malloc(Len * sizeof(oid));
  if (New->name == NULL) {
    free(New);
    snmp_set_api_error(SNMPERR_OS_ERR);
    return(NULL);
  }

#ifdef DEBUG_VARS
  printf("VARS: Copying name, size (%d)\n", Len);
#endif

  /* Only copy a name if it was specified. */
  if (Name)
    memcpy((char *)New->name, (char *)Name, Len * sizeof(oid));
  
  /*  New->val.string = NULL; */
  /*  New->val_len = 0; */

#ifdef DEBUG_VARS
  printf("VARS: Created %x.\n", (unsigned int)New);
#endif

#ifdef DEBUG_VARS_MALLOC
  printf("VARS: Created (%x)\n", (unsigned int)New);
  printf("VARS: Name is (%x)\n", (unsigned int)New->name);
#endif
  return(New);
}

/* Clone a variable list.
 *
 * Returns NULL upon error.
 */

struct variable_list *snmp_var_clone(struct variable_list *Src)
{
  struct variable_list *Dest;

#ifdef DEBUG_VARS
  printf("VARS: Cloning.\n");
#endif

  Dest = (struct variable_list *)malloc(sizeof(struct variable_list));
  if (Dest == NULL) {
    snmp_set_api_error(SNMPERR_OS_ERR);
    return(NULL);
  }

#ifdef DEBUG_VARS
  printf("VARS: Copying entire variable list.  (Size %d)\n",
	 sizeof(struct variable_list));
#endif

  memcpy((char *)Dest, (char *)Src, sizeof(struct variable_list));

  if (Src->name != NULL){
    Dest->name = (oid *)malloc(Src->name_length * sizeof(oid));
    if (Dest->name == NULL) {
      snmp_set_api_error(SNMPERR_OS_ERR);
      free(Dest);
      return(NULL);
    }
#ifdef DEBUG_VARS
    printf("VARS: Copying name OID. (Size %d)\n", Src->name_length);
#endif
    memcpy((char *)Dest->name, (char *)Src->name,
    	   Src->name_length * sizeof(oid));
  }

  /* CISCO Catalyst 2900 returns NULL strings as data of length 0. */
  if ((Src->val.string != NULL) &&
      (Src->val_len)) {
    Dest->val.string = (u_char *)malloc(Src->val_len);
    if (Dest->val.string == NULL) {
      snmp_set_api_error(SNMPERR_OS_ERR);
      free(Dest->name);
      free(Dest);
      return(NULL);
    }

#ifdef DEBUG_VARS
    printf("VARS: Copying value (Size %d)\n", Src->val_len);
#endif
    memcpy((char *)Dest->val.string, (char *)Src->val.string, Src->val_len);
  }

#ifdef DEBUG_VARS
  printf("VARS: Cloned %x.\n", (unsigned int)Dest);
#endif
#ifdef DEBUG_VARS_MALLOC
  printf("VARS: Cloned  (%x)\n", (unsigned int)Dest);
  printf("VARS: Name is (%x)\n", (unsigned int)Dest->name);
#endif

  return(Dest);
}

/* Free a variable_list.
 */
void snmp_var_free(struct variable_list *Ptr)
{
#ifdef DEBUG_VARS_MALLOC
  printf("VARS: Free'd  (%x)\n", (unsigned int)Ptr);
  printf("VARS: Name was(%x)\n", (unsigned int)Ptr->name);
#endif

  /* CISCO Catalyst 2900 returns NULL strings as data of length 0. */
  if (Ptr->name)
    free((char *) Ptr->name);

  if (Ptr->val.string)
    free((char *) Ptr->val.string);

  free(Ptr);
}

/**********************************************************************/

/* Build a variable binding.
 *
 * RFC 1905: Protocol Operations for SNMPv2
 *
 * VarBind ::= 
 *   SEQUENCE {
 *     name ObjectName
 *     CHOICE {
 *       value ObjectSyntax
 *       unSpecified NULL
 *       noSuchObject[0] NULL
 *       noSuchInstance[1] NULL
 *       endOfMibView[2] NULL
 *     }
 *   }
 */
u_char *snmp_var_EncodeVarBind(u_char *Buffer, int *BufLenP,
			       struct variable_list *VarList,
			       int Version)
{
  struct variable_list *Vars;
  u_char *bufp;
  u_char *HeaderStart;
  u_char *HeaderEnd;
  int FakeArg = *BufLenP;
#ifdef DEBUG_VARS_ENCODE
  int StartLen = *BufLenP;
  int Counter = 1;
#endif

  bufp = Buffer;

#ifdef DEBUG_VARS_ENCODE
  printf("VARS: Encoding Variable list into buffer at 0x%x.\n", Buffer);
#endif

  for (Vars=VarList; Vars; Vars=Vars->next_variable) {

#ifdef DEBUG_VARS_ENCODE
    printf("VARS %d: Encoding Variable 0x%x.\n", Counter, Vars);
    printf("VARS %d: Starting at 0x%x (%d bytes left)\n", 
	   Counter, bufp, *BufLenP);
#endif

    /* Build the header for this variable
     *
     * Use Maximum size.
     */
    HeaderStart = bufp;
    HeaderEnd = asn_build_header(HeaderStart, BufLenP,
				 (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 
				 FakeArg);
    if (HeaderEnd == NULL)
      return(NULL);

#ifdef DEBUG_VARS_ENCODE
    printf("VARS %d: Encoding Object Identifier 0x%x (%d bytes) at 0x%x (%d bytes left)\n", 
	   Counter, Vars,
	   Vars->name_length, HeaderEnd, *BufLenP);
    print_oid(Vars->name, Vars->name_length),
#endif
    /* Now, let's put the Object Identifier into the buffer */
    bufp = asn_build_objid(HeaderEnd, BufLenP,
			   (u_char)(ASN_UNIVERSAL | 
				    ASN_PRIMITIVE | 
				    ASN_OBJECT_ID),
			   Vars->name, Vars->name_length);
    if (bufp == NULL)
      return(NULL);

    /* Now put the data in */
    switch(Vars->type) {

    case ASN_INTEGER:
#ifdef DEBUG_VARS_ENCODE
      printf("VARS %d: Encoding Integer %d at 0x%x\n", Counter,
	     *(Vars->val.integer), bufp);
#endif

      bufp = asn_build_int(bufp, 
			   BufLenP, Vars->type,
			   (int *)Vars->val.integer, Vars->val_len);
      break;

    case SMI_COUNTER32:
    case SMI_GAUGE32:
      /*  case SMI_UNSIGNED32: */
    case SMI_TIMETICKS:
#ifdef DEBUG_VARS_ENCODE
      printf("VARS %d: Encoding Timeticks %d at 0x%x\n", Counter,
	     *(Vars->val.integer), bufp);
#endif
      bufp = asn_build_unsigned_int(bufp, BufLenP, 
				    Vars->type,
				    (u_int *)Vars->val.integer, Vars->val_len);
      break;

    case ASN_OCTET_STR:
    case SMI_IPADDRESS:
    case SMI_OPAQUE:
#ifdef DEBUG_VARS_ENCODE
      printf("VARS %d: Encoding String %s (%d bytes) at 0x%x\n", Counter,
	     (Vars->val.string), Vars->val_len, bufp);
#endif
      bufp = asn_build_string(bufp, BufLenP, Vars->type,
			      Vars->val.string, Vars->val_len);
      break;

    case ASN_OBJECT_ID:
#ifdef DEBUG_VARS_ENCODE
      printf("VARS %d: Encoding Object Identifier (%d bytes) at 0x%x\n",
	     Counter,
	     Vars->val_len, bufp);
#endif
      bufp = asn_build_objid(bufp, BufLenP, Vars->type,
			     (oid *)Vars->val.objid, Vars->val_len / sizeof(oid));
      break;

    case SMI_NOSUCHINSTANCE:
    case SMI_NOSUCHOBJECT:
    case SMI_ENDOFMIBVIEW:

#ifdef DEBUG_VARS_ENCODE
      printf("VARS %d: Encoding NULL at 0x%x\n", Counter, bufp);
#endif
      if (Version == SNMP_VERSION_1) {
        /* SNMP Version 1 does not support these error codes. */
	bufp = asn_build_null(bufp, BufLenP, SMI_NOSUCHOBJECT);
      } else {
	bufp = asn_build_exception(bufp, BufLenP, Vars->type);
      }
      break;

    case ASN_NULL:
#ifdef DEBUG_VARS_ENCODE
      printf("VARS %d: Encoding NULL at 0x%x\n", Counter, bufp);
#endif
      bufp = asn_build_null(bufp, BufLenP, Vars->type);
      break;

    case SMI_COUNTER64:
#ifdef STDERR_OUTPUT
      fprintf(stderr, WIDE("Unable to encode type SMI_COUNTER64!\n"));
#endif
      /* Fall through */

    default:
      snmp_set_api_error(SNMPERR_UNSUPPORTED_TYPE);
      return(NULL);
    }

    /* ASSERT:  bufp should now point to the next valid byte. */
    if (bufp == NULL)
      return(NULL);

    /* Rebuild the header with the appropriate length */
#ifdef DEBUG_VARS_ENCODE
    printf("VARS %d: Resetting length to %d at 0x%x (%d bytes left)\n",
	   Counter,
	   (bufp - HeaderEnd), HeaderStart, *BufLenP);
#endif
    HeaderEnd = asn_build_header(HeaderStart, &FakeArg,
				 (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 
				 (bufp - HeaderEnd));

    /* Returns NULL */
    if (HeaderEnd == NULL)
      return(NULL);

#ifdef DEBUG_VARS_ENCODE
    Counter++;
#endif
  }

#ifdef DEBUG_VARS_ENCODE
  printf("VARS: Variable list of %d vars takes up %d bytes.\n",
	 --Counter, StartLen - *BufLenP);
#endif

  /* or the end of the entire thing */
  return(bufp);
}

			      


/* Parse all Vars from the buffer */
u_char *snmp_var_DecodeVarBind(u_char *Buffer, int *BufLen,
			       struct variable_list **VarP,
			       int Version)
{
  struct variable_list *Var, **VarLastP;
  u_char *bufp, *tmp;
  u_char  VarBindType;
  u_char *DataPtr;
  int     DataLen;
  oid TmpBuf[MAX_NAME_LEN];

  int AllVarLen = *BufLen;
  int ThisVarLen = 0;

  VarLastP = VarP;
#ifdef DEBUG_VARS_DECODE
  printf("VARS: Decoding buffer of length %d\n", *BufLen);
#endif

  /* Now parse the variables */
  bufp = asn_parse_header(Buffer, &AllVarLen, &VarBindType);
  if (bufp == NULL)
    ASN_PARSE_ERROR(NULL);

  if (VarBindType != (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR)) {
      snmp_set_api_error(SNMPERR_PDU_PARSE);
      ASN_PARSE_ERROR(NULL);
  }

#ifdef DEBUG_VARS_DECODE
  printf("VARS: All Variable length %d\n", AllVarLen);
#endif

  /* We know how long the variable list is.  Parse it. */
  while ((int)AllVarLen > 0) {

    /* Create a new variable */
    Var = snmp_var_new(NULL, MAX_NAME_LEN);
    if (Var == NULL)
      return(NULL);
    
    /* Parse the header to find out the length of this variable. */
    ThisVarLen = AllVarLen;
    tmp = asn_parse_header(bufp, &ThisVarLen, &VarBindType);
    if (tmp == NULL)
      ASN_PARSE_ERROR(NULL);

    /* Now that we know the length , figure out how it relates to 
     * the entire variable list
     */
    AllVarLen = AllVarLen - (ThisVarLen + (tmp - bufp));
    bufp = tmp;

    /* Is it valid? */
    if (VarBindType != (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR)) {
      snmp_set_api_error(SNMPERR_PDU_PARSE);
      ASN_PARSE_ERROR(NULL);
    }

#ifdef DEBUG_VARS_DECODE
    printf("VARS: Header type 0x%x (%d bytes left)\n", VarBindType, ThisVarLen);
#endif

    /* Parse the OBJID */
    bufp = asn_parse_objid(bufp, &ThisVarLen, &VarBindType, 
			   Var->name, &(Var->name_length));
    if (bufp == NULL)
      ASN_PARSE_ERROR(NULL);

    if (VarBindType != (u_char)(ASN_UNIVERSAL | 
				ASN_PRIMITIVE | 
				ASN_OBJECT_ID)) {
      snmp_set_api_error(SNMPERR_PDU_PARSE);
      ASN_PARSE_ERROR(NULL);
    }

#ifdef DEBUG_VARS_DECODE
    printf("VARS: Decoded OBJID (%d bytes). (%d bytes left)\n", 
	   Var->name_length, ThisVarLen);
#endif

    /* Keep a pointer to this object */
    DataPtr = bufp;
    DataLen = ThisVarLen;

    /* find out type of object */
    bufp = asn_parse_header(bufp, &ThisVarLen, &(Var->type));
    if (bufp == NULL)
      ASN_PARSE_ERROR(NULL);
    ThisVarLen = DataLen;

#ifdef DEBUG_VARS_DECODE
    printf("VARS: Data type %d\n", Var->type);
#endif

    /* Parse the type */
    
    switch((short)Var->type){

    case ASN_INTEGER:
      Var->val.integer = (int *)malloc(sizeof(int));
      if (Var->val.integer == NULL) {
	snmp_set_api_error(SNMPERR_OS_ERR);
	return(NULL);
      }
      Var->val_len = sizeof(int);
      bufp = asn_parse_int(DataPtr, &ThisVarLen, 
			   &Var->type, (int *)Var->val.integer, 
			   Var->val_len);
#ifdef DEBUG_VARS_DECODE
      printf("VARS: Decoded integer '%d' (%d bytes left)\n",
	     *(Var->val.integer), ThisVarLen);
#endif
      break;

    case SMI_COUNTER32:
    case SMI_GAUGE32:
      /*  case SMI_UNSIGNED32: */
    case SMI_TIMETICKS:
      Var->val.integer = (int *)malloc(sizeof(u_int));
      if (Var->val.integer == NULL) {
	snmp_set_api_error(SNMPERR_OS_ERR);
	return(NULL);
      }
      Var->val_len = sizeof(u_int);
      bufp = asn_parse_unsigned_int(DataPtr, &ThisVarLen, 
				    &Var->type, (u_int *)Var->val.integer, 
				    Var->val_len);
#ifdef DEBUG_VARS_DECODE
      printf("VARS: Decoded timeticks '%d' (%d bytes left)\n",
	     *(Var->val.integer), ThisVarLen);
#endif
      break;

    case ASN_OCTET_STR:
    case SMI_IPADDRESS:
    case SMI_OPAQUE:
      Var->val_len = *&ThisVarLen; /* String is this at most */
      Var->val.string = (u_char *)malloc((unsigned)Var->val_len);
      if (Var->val.string == NULL) {
	snmp_set_api_error(SNMPERR_OS_ERR);
	return(NULL);
      }
      bufp = asn_parse_string(DataPtr, &ThisVarLen, 
			      &Var->type, Var->val.string, 
			      &Var->val_len);
#ifdef DEBUG_VARS_DECODE
      printf("VARS: Decoded string '%s' (length %d) (%d bytes left)\n",
	     (Var->val.string), Var->val_len, ThisVarLen);
#endif
      break;
         
    case ASN_OBJECT_ID:
      Var->val_len = MAX_NAME_LEN;
      bufp = asn_parse_objid(DataPtr, &ThisVarLen, 
			     &Var->type, TmpBuf, &Var->val_len);
      Var->val_len *= sizeof(oid);
      Var->val.objid = (oid *)malloc((unsigned)Var->val_len);
      if (Var->val.integer == NULL) {
	snmp_set_api_error(SNMPERR_OS_ERR);
	return(NULL);
      }
      /* Only copy if we successfully decoded something */
      if (bufp) {
	memcpy((char *)Var->val.objid, (char *)TmpBuf, Var->val_len);
      }
#ifdef DEBUG_VARS_DECODE
      printf("VARS: Decoded OBJID (length %d) (%d bytes left)\n",
	      Var->val_len, ThisVarLen);
#endif
      break;

    case ASN_NULL:
    case SMI_NOSUCHINSTANCE:
    case SMI_NOSUCHOBJECT:
    case SMI_ENDOFMIBVIEW:
      Var->val_len   = 0;
      Var->val.objid = NULL;
      bufp = asn_parse_null(DataPtr, &ThisVarLen, &Var->type);
#ifdef DEBUG_VARS_DECODE
      printf("VARS: Decoded ASN_NULL (length %d) (%d bytes left)\n",
	     Var->val_len, ThisVarLen);
#endif
      break;

    case SMI_COUNTER64:
#ifdef STDERR_OUTPUT
      fprintf(stderr, WIDE("Unable to parse type SMI_COUNTER64!\n"));
#endif
      snmp_set_api_error(SNMPERR_UNSUPPORTED_TYPE);
      return(NULL);
      break;

    default:
#ifdef STDERR_OUTPUT
      fprintf(stderr, WIDE("bad type returned (%x)\n"), Var->type);
#endif
      snmp_set_api_error(SNMPERR_PDU_PARSE);
      return(NULL);
      break;
    } /* End of var type switch */

    /* Why is this here? XXXXX */
    if (bufp == NULL)
      return(NULL);

#ifdef DEBUG_VARS_DECODE
    printf("VARS: Adding to list of decoded variables.  (%d bytes remain.)\n", AllVarLen);
#endif
    /* Add variable to the list */
    *VarLastP = Var;
    VarLastP = &(Var->next_variable);
  }

  return(bufp);
}

void *LibraryAllocMem( int nSize )
{
   return malloc( nSize );
}
SNMP_NAMESPACE_END
// $Log: $
