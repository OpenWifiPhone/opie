%{

/***************************************************************************
(C) Copyright 1996 Apple Computer, Inc., AT&T Corp., International
Business Machines Corporation and Siemens Rolm Communications Inc.

For purposes of this license notice, the term Licensors shall mean,
collectively, Apple Computer, Inc., AT&T Corp., International
Business Machines Corporation and Siemens Rolm Communications Inc.
The term Licensor shall mean any of the Licensors.

Subject to acceptance of the following conditions, permission is hereby
granted by Licensors without the need for written agreement and without
license or royalty fees, to use, copy, modify and distribute this
software for any purpose.

The above copyright notice and the following four paragraphs must be
reproduced in all copies of this software and any software including
this software.

THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS AND NO LICENSOR SHALL HAVE
ANY OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS OR
MODIFICATIONS.

IN NO EVENT SHALL ANY LICENSOR BE LIABLE TO ANY PARTY FOR DIRECT,
INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOST PROFITS ARISING OUT
OF THE USE OF THIS SOFTWARE EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.

EACH LICENSOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO ANY WARRANTY OF NONINFRINGEMENT OR THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.

The software is provided with RESTRICTED RIGHTS.  Use, duplication, or
disclosure by the government are subject to restrictions set forth in
DFARS 252.227-7013 or 48 CFR 52.227-19, as applicable.

***************************************************************************/

/*
 * src: vcc.c
 * doc: Parser for vCard and vCalendar. Note that this code is
 * generated by a yacc parser generator. Generally it should not
 * be edited by hand. The real source is vcc.y. The #line directives
 * can be commented out here to make it easier to trace through
 * in a debugger. However, if a bug is found it should
 * be fixed in vcc.y and this file regenerated.
 */


/* debugging utilities */
#if __DEBUG
#define DBG_(x) printf x
#else
#define DBG_(x)
#endif

/****  External Functions  ****/

/* assign local name to parser variables and functions so that
   we can use more than one yacc based parser.
*/

#if 0
#define yyparse mime_parse
#define yylex mime_lex
#define yyerror mime_error
#define yychar mime_char
/* #define p_yyval p_mime_val */
#undef yyval
#define yyval mime_yyval
/* #define p_yylval p_mime_lval */
#undef yylval
#define yylval mime_yylval
#define yydebug mime_debug
#define yynerrs mime_nerrs
#define yyerrflag mime_errflag
#define yyss mime_ss
#define yyssp mime_ssp
#define yyvs mime_vs
#define yyvsp mime_vsp
#define yylhs mime_lhs
#define yylen mime_len
#define yydefred mime_defred
#define yydgoto mime_dgoto
#define yysindex mime_sindex
#define yyrindex mime_rindex
#define yygindex mime_gindex
#define yytable mime_table
#define yycheck mime_check
#define yyname mime_name
#define yyrule mime_rule
#ifdef YYPREFIX
#undef YYPREFIX
#endif
#define YYPREFIX "mime_"
#endif


#ifndef _NO_LINE_FOLDING
#define _SUPPORT_LINE_FOLDING 1
#endif

/* undef below if compile with MFC */
/* #define INCLUDEMFC 1 */

#if defined(WIN32) || defined(_WIN32)
#ifdef INCLUDEMFC
#include <afx.h>
#endif
#endif

#include <string.h>
#ifndef __MWERKS__
#include <stdlib.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//#ifdef PALMTOPCENTER
//#include <qpe/vobject_p.h>
//#else
#include "vobject_p.h"
//#endif

/****  Types, Constants  ****/

#define YYDEBUG		0	/* 1 to compile in some debugging code */
#define MAXTOKEN	256	/* maximum token (line) length */
#define YYSTACKSIZE 	100	// ~unref ?
#define MAXLEVEL	10	/* max # of nested objects parseable */
				/* (includes outermost) */


/****  Global Variables  ****/
int mime_lineNum, mime_numErrors; /* yyerror() can use these */
static VObject* vObjList;
static VObject *curProp;
static VObject *curObj;
static VObject* ObjStack[MAXLEVEL];
static int ObjStackTop;


/* A helpful utility for the rest of the app. */
#if __CPLUSPLUS__
extern "C" {
#endif

    extern void yyerror(char *s);

#if __CPLUSPLUS__
    };
#endif

int yyparse();

enum LexMode {
	L_NORMAL,
	L_VCARD,
	L_VCAL,
	L_VEVENT,
	L_VTODO,
	L_VALUES,
	L_BASE64,
	L_QUOTED_PRINTABLE
	};

/****  Private Forward Declarations  ****/
static int pushVObject(const char *prop);
static VObject* popVObject();
static void lexPopMode(int top);
static int lexWithinMode(enum LexMode mode);
static void lexPushMode(enum LexMode mode);
static void enterProps(const char *s);
static void enterAttr(const char *s1, const char *s2);
static void enterValues(const char *value);
#define mime_error yyerror
void mime_error(char *s);
void mime_error_(char *s);

%}

/***************************************************************************/
/***                           The grammar                              ****/
/***************************************************************************/

%union {
    char *str;
    VObject *vobj;
    }

%token
	EQ COLON DOT SEMICOLON SPACE HTAB LINESEP NEWLINE
	BEGIN_VCARD END_VCARD BEGIN_VCAL END_VCAL
	BEGIN_VEVENT END_VEVENT BEGIN_VTODO END_VTODO
	ID

/*
 * NEWLINE is the token that would occur outside a vCard,
 * while LINESEP is the token that would occur inside a vCard.
 */

%token <str>
	STRING ID

%type <str> name value

%type <vobj> vcard vcal vobject

%start mime

%%


mime: vobjects
	;

vobjects: vobjects vobject
	{ addList(&vObjList, $2); curObj = 0; }
	| vobject
		{ addList(&vObjList, $1); curObj = 0; }
	;

vobject: vcard
	| vcal
	;

vcard:
	BEGIN_VCARD
	{
	lexPushMode(L_VCARD);
	if (!pushVObject(VCCardProp)) YYERROR;
	}
	items END_VCARD
	{
	lexPopMode(0);
	$$ = popVObject();
	}
	| BEGIN_VCARD
	{
	lexPushMode(L_VCARD);
	if (!pushVObject(VCCardProp)) YYERROR;
	}
	END_VCARD
	{
	lexPopMode(0);
	$$ = popVObject();
	}
	;

items: items item
	| item
	;

item: prop COLON
	{
	lexPushMode(L_VALUES);
	}
	values LINESEP
	{
	if (lexWithinMode(L_BASE64) || lexWithinMode(L_QUOTED_PRINTABLE))
	   lexPopMode(0);
	lexPopMode(0);
	}
	| error
	;

prop: name
	{
	enterProps($1);
	}
	attr_params
	| name
	{
	enterProps($1);
	}
	;

attr_params: attr_params attr_param
	| attr_param
	;

attr_param: SEMICOLON attr
	;

attr: name
	{
	enterAttr($1,0);
	}
	| name EQ name
	{
	enterAttr($1,$3);

	}
	;
	
name: ID
	;

values: value SEMICOLON { enterValues($1); } values
	| value
	{ enterValues($1); }
	;

value: STRING
	|
	{ $$ = 0; }
	;

vcal:
	BEGIN_VCAL
	{ if (!pushVObject(VCCalProp)) YYERROR; }
	calitems
	END_VCAL
	{ $$ = popVObject(); }
	| BEGIN_VCAL
	{ if (!pushVObject(VCCalProp)) YYERROR; }
	END_VCAL
	{ $$ = popVObject(); }
	;

calitems: calitems calitem
	| calitem
	;

calitem:
	eventitem
	| todoitem
	| items
	;

eventitem:
	BEGIN_VEVENT
	{
	lexPushMode(L_VEVENT);
	if (!pushVObject(VCEventProp)) YYERROR;
	}
	items
	END_VEVENT
	{
	lexPopMode(0);
	popVObject();
	}
	| BEGIN_VEVENT
	{
	lexPushMode(L_VEVENT);
	if (!pushVObject(VCEventProp)) YYERROR;
	}
	END_VEVENT
	{
	lexPopMode(0);
	popVObject();
	}
	;

todoitem:
	BEGIN_VTODO
	{
	lexPushMode(L_VTODO);
	if (!pushVObject(VCTodoProp)) YYERROR;
	}
	items
	END_VTODO
	{
	lexPopMode(0);
	popVObject();
	}
	| BEGIN_VTODO
	{
	lexPushMode(L_VTODO);
	if (!pushVObject(VCTodoProp)) YYERROR;
	}
	END_VTODO
	{
	lexPopMode(0);
	popVObject();
	}
	;

%%
/*------------------------------------*/
static int pushVObject(const char *prop)
    {
    VObject *newObj;
    if (ObjStackTop == MAXLEVEL)
	return FALSE;

    ObjStack[++ObjStackTop] = curObj;

    if (curObj) {
        newObj = addProp(curObj,prop);
        curObj = newObj;
	}
    else
	curObj = newVObject(prop);

    return TRUE;
    }


/*---------------------------------------*/
/* This pops the recently built vCard off the stack and returns it. */
static VObject* popVObject()
    {
    VObject *oldObj;
    if (ObjStackTop < 0) {
	yyerror("pop on empty Object Stack\n");
	return 0;
	}
    oldObj = curObj;
    curObj = ObjStack[ObjStackTop--];

    return oldObj;
    }


static void enterValues(const char *value)
    {
    if (fieldedProp && *fieldedProp) {
	if (value) {
	    addPropValue(curProp,*fieldedProp,value);
	    }
	/* else this field is empty, advance to next field */
	fieldedProp++;
	}
    else {
	if (value) {
	    setVObjectStringZValue_(curProp,strdup( value ));
	    }
	}
    deleteStr(value);
    }

static void enterProps(const char *s)
    {
    curProp = addGroup(curObj,s);
    deleteStr(s);
    }

static void enterAttr(const char *s1, const char *s2)
    {
    const char *p1, *p2;
    p1 = lookupProp_(s1);
    if (s2) {
	VObject *a;
	p2 = lookupProp_(s2);
	a = addProp(curProp,p1);
	setVObjectStringZValue(a,p2);
	}
    else
	addProp(curProp,p1);
    if (qstricmp(p1,VCBase64Prop) == 0 || (s2 && qstricmp(p2,VCBase64Prop)==0))
	lexPushMode(L_BASE64);
    else if (qstricmp(p1,VCQuotedPrintableProp) == 0
	    || (s2 && qstricmp(p2,VCQuotedPrintableProp)==0))
	lexPushMode(L_QUOTED_PRINTABLE);
    deleteStr(s1); deleteStr(s2);
    }


#define MAX_LEX_LOOKAHEAD_0 32
#define MAX_LEX_LOOKAHEAD 64
#define MAX_LEX_MODE_STACK_SIZE 10
#define LEXMODE() (lexBuf.lexModeStack[lexBuf.lexModeStackTop])

struct LexBuf {
	/* input */
#ifdef INCLUDEMFC
    CFile *inputFile;
#else
    FILE *inputFile;
#endif
    char *inputString;
    unsigned long curPos;
    unsigned long inputLen;
	/* lookahead buffer */
	/*   -- lookahead buffer is short instead of char so that EOF
	 /      can be represented correctly.
	*/
    unsigned long len;
    short buf[MAX_LEX_LOOKAHEAD];
    unsigned long getPtr;
	/* context stack */
    unsigned long lexModeStackTop;
    enum LexMode lexModeStack[MAX_LEX_MODE_STACK_SIZE];
	/* token buffer */
    unsigned long maxToken;
    char *strs;
    unsigned long strsLen;
    } lexBuf;

static void lexPushMode(enum LexMode mode)
    {
    if (lexBuf.lexModeStackTop == (MAX_LEX_MODE_STACK_SIZE-1))
	yyerror("lexical context stack overflow");
    else {
	lexBuf.lexModeStack[++lexBuf.lexModeStackTop] = mode;
	}
    }

static void lexPopMode(int top)
    {
    /* special case of pop for ease of error recovery -- this
	version will never underflow */
    if (top)
	lexBuf.lexModeStackTop = 0;
    else
	if (lexBuf.lexModeStackTop > 0) lexBuf.lexModeStackTop--;
    }

static int lexWithinMode(enum LexMode mode) {
    unsigned long i;
    for (i=0;i<lexBuf.lexModeStackTop;i++)
	if (mode == lexBuf.lexModeStack[i]) return 1;
    return 0;
    }

static char lexGetc_()
    {
    /* get next char from input, no buffering. */
    if (lexBuf.curPos == lexBuf.inputLen)
	return EOF;
    else if (lexBuf.inputString)
	return *(lexBuf.inputString + lexBuf.curPos++);
    else {
#ifdef INCLUDEMFC
	char result;
	return lexBuf.inputFile->Read(&result, 1) == 1 ? result : EOF;
#else
	return fgetc(lexBuf.inputFile);
#endif
	}
    }

static int lexGeta()
    {
    ++lexBuf.len;
    return (lexBuf.buf[lexBuf.getPtr] = lexGetc_());
    }

static int lexGeta_(int i)
    {
    ++lexBuf.len;
    return (lexBuf.buf[(lexBuf.getPtr+i)%MAX_LEX_LOOKAHEAD] = lexGetc_());
    }

static void lexSkipLookahead() {
    if (lexBuf.len > 0 && lexBuf.buf[lexBuf.getPtr]!=EOF) {
	/* don't skip EOF. */
        lexBuf.getPtr = (lexBuf.getPtr + 1) % MAX_LEX_LOOKAHEAD;
	lexBuf.len--;
        }
    }

static int lexLookahead() {
    int c = (lexBuf.len)?
	lexBuf.buf[lexBuf.getPtr]:
	lexGeta();
    /* do the \r\n -> \n or \r -> \n translation here */
    if (c == '\r') {
	int a = (lexBuf.len>1)?
	    lexBuf.buf[(lexBuf.getPtr+1)%MAX_LEX_LOOKAHEAD]:
	    lexGeta_(1);
	if (a == '\n') {
	    lexSkipLookahead();
	    }
	lexBuf.buf[lexBuf.getPtr] = c = '\n';
	}
    else if (c == '\n') {
	int a = (lexBuf.len>1)?
	    lexBuf.buf[lexBuf.getPtr+1]:
	    lexGeta_(1);
	if (a == '\r') {
	    lexSkipLookahead();
	    }
	lexBuf.buf[lexBuf.getPtr] = '\n';
	}
    return c;
    }

static int lexGetc() {
    int c = lexLookahead();
    if (lexBuf.len > 0 && lexBuf.buf[lexBuf.getPtr]!=EOF) {
	/* EOF will remain in lookahead buffer */
        lexBuf.getPtr = (lexBuf.getPtr + 1) % MAX_LEX_LOOKAHEAD;
	lexBuf.len--;
        }
    return c;
    }

static void lexSkipLookaheadWord() {
    if (lexBuf.strsLen <= lexBuf.len) {
	lexBuf.len -= lexBuf.strsLen;
	lexBuf.getPtr = (lexBuf.getPtr + lexBuf.strsLen) % MAX_LEX_LOOKAHEAD;
	}
    }

static void lexClearToken()
    {
    lexBuf.strsLen = 0;
    }

static void lexAppendc(int c)
    {
    lexBuf.strs[lexBuf.strsLen] = c;
    /* append up to zero termination */
    if (c == 0) return;
    lexBuf.strsLen++;
    if (lexBuf.strsLen > lexBuf.maxToken) {
	/* double the token string size */
	lexBuf.maxToken <<= 1;
	lexBuf.strs = (char*) realloc(lexBuf.strs,(size_t)lexBuf.maxToken);
	}
    }

static char* lexStr() {
    return dupStr(lexBuf.strs,(size_t)lexBuf.strsLen+1);
    }

static void lexSkipWhite() {
    int c = lexLookahead();
    while (c == ' ' || c == '\t') {
	lexSkipLookahead();
	c = lexLookahead();
	}
    }

static char* lexGetWord() {
    int c;
    lexSkipWhite();
    lexClearToken();
    c = lexLookahead();
    while (c != EOF && !strchr("\t\n ;:=",c)) {
	lexAppendc(c);
	lexSkipLookahead();
	c = lexLookahead();
	}
    lexAppendc(0);
    return lexStr();
    }

static void lexPushLookaheadc(int c) {
    int putptr;
    /* can't putback EOF, because it never leaves lookahead buffer */
    if (c == EOF) return;
    putptr = (int)lexBuf.getPtr - 1;
    if (putptr < 0) putptr += MAX_LEX_LOOKAHEAD;
    lexBuf.getPtr = putptr;
    lexBuf.buf[putptr] = c;
    lexBuf.len += 1;
    }

static char* lexLookaheadWord() {
    /* this function can lookahead word with max size of MAX_LEX_LOOKAHEAD_0
     /  and thing bigger than that will stop the lookahead and return 0;
     / leading white spaces are not recoverable.
     */
    int c;
    int len = 0;
    int curgetptr = 0;
    lexSkipWhite();
    lexClearToken();
    curgetptr = (int)lexBuf.getPtr;	// remember!
    while (len < (MAX_LEX_LOOKAHEAD_0)) {
	c = lexGetc();
	len++;
	if (c == EOF || strchr("\t\n ;:=", c)) {
	    lexAppendc(0);
	    /* restore lookahead buf. */
	    lexBuf.len += len;
	    lexBuf.getPtr = curgetptr;
	    return lexStr();
	    }
        else
	    lexAppendc(c);
	}
    lexBuf.len += len;	/* char that has been moved to lookahead buffer */
    lexBuf.getPtr = curgetptr;
    return 0;
    }

#ifdef _SUPPORT_LINE_FOLDING
static void handleMoreRFC822LineBreak(int c) {
    /* suport RFC 822 line break in cases like
     *	ADR: foo;
     *    morefoo;
     *    more foo;
     */
    if (c == ';') {
	int a;
	lexSkipLookahead();
	/* skip white spaces */
	a = lexLookahead();
	while (a == ' ' || a == '\t') {
	    lexSkipLookahead();
	    a = lexLookahead();
	    }
	if (a == '\n') {
	    lexSkipLookahead();
	    a = lexLookahead();
	    if (a == ' ' || a == '\t') {
		/* continuation, throw away all the \n and spaces read so
		 * far
		 */
		lexSkipWhite();
		lexPushLookaheadc(';');
		}
	    else {
		lexPushLookaheadc('\n');
		lexPushLookaheadc(';');
		}
	    }
	else {
	    lexPushLookaheadc(';');
	    }
	}
    }

static char* lexGet1Value() {
    int c;
    lexSkipWhite();
    c = lexLookahead();
    lexClearToken();
    while (c != EOF && c != ';') {
	if (c == '\\' ) {
	    int a;
	    lexSkipLookahead();
	    a = lexLookahead();
	    if ( a != ';' ) {
		lexAppendc('\\');
	    } else {
		lexAppendc( ';' );
		lexSkipLookahead();
	    }
	} else if (c == '\n') {
	    int a;
	    lexSkipLookahead();
	    a  = lexLookahead();
	    if (a == ' ' || a == '\t') {
		lexAppendc(' ');
		lexSkipLookahead();
		}
	    else {
		lexPushLookaheadc('\n');
		break;
		}
	    }
	else {
	    lexAppendc(c);
	    lexSkipLookahead();
	    }
	c = lexLookahead();
	}
    lexAppendc(0);
    handleMoreRFC822LineBreak(c);
    return c==EOF?0:lexStr();
    }
#endif

static int match_begin_name(int end) {
    char *n = lexLookaheadWord();
    int token = ID;
    if (n) {
	if (!qstricmp(n,"vcard")) token = end?END_VCARD:BEGIN_VCARD;
	else if (!qstricmp(n,"vcalendar")) token = end?END_VCAL:BEGIN_VCAL;
	else if (!qstricmp(n,"vevent")) token = end?END_VEVENT:BEGIN_VEVENT;
	else if (!qstricmp(n,"vtodo")) token = end?END_VTODO:BEGIN_VTODO;
	deleteStr(n);
	return token;
	}
    return 0;
    }


#ifdef INCLUDEMFC
void initLex(const char *inputstring, unsigned long inputlen, CFile *inputfile)
#else
void initLex(const char *inputstring, unsigned long inputlen, FILE *inputfile)
#endif
    {
    // initialize lex mode stack
    lexBuf.lexModeStack[lexBuf.lexModeStackTop=0] = L_NORMAL;

    // iniatialize lex buffer.
    lexBuf.inputString = (char*) inputstring;
    lexBuf.inputLen = inputlen;
    lexBuf.curPos = 0;
    lexBuf.inputFile = inputfile;

    lexBuf.len = 0;
    lexBuf.getPtr = 0;

    lexBuf.maxToken = MAXTOKEN;
    lexBuf.strs = (char*)malloc(MAXTOKEN);
    lexBuf.strsLen = 0;

    }

static void finiLex() {
    free(lexBuf.strs);
    }


/*-----------------------------------*/
/* This parses and converts the base64 format for binary encoding into
 * a decoded buffer (allocated with new).  See RFC 1521.
 */
static char * lexGetDataFromBase64()
    {
    unsigned long bytesLen = 0, bytesMax = 0;
    int quadIx = 0, pad = 0;
    unsigned long trip = 0;
    unsigned char b;
    int c;
    unsigned char *bytes = NULL;
    unsigned char *oldBytes = NULL;

    DBG_(("db: lexGetDataFromBase64\n"));
    while (1) {
	c = lexGetc();
	if (c == '\n') {
	    ++mime_lineNum;
	    if (lexLookahead() == '\n') {
		/* a '\n' character by itself means end of data */
		break;
		}
	    else continue; /* ignore '\n' */
	    }
	else {
	    if ((c >= 'A') && (c <= 'Z'))
		b = (unsigned char)(c - 'A');
	    else if ((c >= 'a') && (c <= 'z'))
		b = (unsigned char)(c - 'a') + 26;
	    else if ((c >= '0') && (c <= '9'))
		b = (unsigned char)(c - '0') + 52;
	    else if (c == '+')
		b = 62;
	    else if (c == '/')
		b = 63;
	    else if (c == '=') {
		b = 0;
		pad++;
	    } else if ((c == ' ') || (c == '\t')) {
		continue;
	    } else { /* error condition */
		if (bytes) free(bytes);
		else if (oldBytes) free(oldBytes);
		// error recovery: skip until 2 adjacent newlines.
		DBG_(("db: invalid character 0x%x '%c'\n", c,c));
		if (c != EOF)  {
		    c = lexGetc();
		    while (c != EOF) {
			if (c == '\n' && lexLookahead() == '\n') {
			    ++mime_lineNum;
			    break;
			    }
			c = lexGetc();
			}
		    }
		return NULL;
		}
	    trip = (trip << 6) | b;
	    if (++quadIx == 4) {
		unsigned char outBytes[3];
		int numOut;
		int i;
		for (i = 0; i < 3; i++) {
		    outBytes[2-i] = (unsigned char)(trip & 0xFF);
		    trip >>= 8;
		    }
		numOut = 3 - pad;
		if (bytesLen + numOut > bytesMax) {
		    if (!bytes) {
			bytesMax = 1024;
			bytes = (unsigned char*)malloc((size_t)bytesMax);
			}
		    else {
			bytesMax <<= 2;
			oldBytes = bytes;
			bytes = (unsigned char*)realloc(bytes,(size_t)bytesMax);
			}
		    if (bytes == 0) {
			mime_error("out of memory while processing BASE64 data\n");
			}
		    }
		if (bytes) {
		    memcpy(bytes + bytesLen, outBytes, numOut);
		    bytesLen += numOut;
		    }
		trip = 0;
		quadIx = 0;
		}
	    }
	} /* while */
    DBG_(("db: bytesLen = %d\n",  bytesLen));
    /* kludge: all this won't be necessary if we have tree form
	representation */
    if (bytes) {
	setValueWithSize(curProp,bytes,(unsigned int)bytesLen);
	free(bytes);
	}
    else if (oldBytes) {
	setValueWithSize(curProp,oldBytes,(unsigned int)bytesLen);
	free(oldBytes);
	}
    return 0;
    }

static int match_begin_end_name(int end) {
    int token;
    lexSkipWhite();
    if (lexLookahead() != ':') return ID;
    lexSkipLookahead();
    lexSkipWhite();
    token = match_begin_name(end);
    if (token == ID) {
	lexPushLookaheadc(':');
	DBG_(("db: ID '%s'\n", yylval.str));
	return ID;
	}
    else if (token != 0) {
	lexSkipLookaheadWord();
	deleteStr(yylval.str);
	DBG_(("db: begin/end %d\n", token));
	return token;
	}
    return 0;
    }

static char* lexGetQuotedPrintable()
    {
    char cur;

    lexClearToken();
    do {
	cur = lexGetc();
	switch (cur) {
	    case '=': {
		int c = 0;
		int next[2];
		int i;
		for (i = 0; i < 2; i++) {
		    next[i] = lexGetc();
		    if (next[i] >= '0' && next[i] <= '9')
			c = c * 16 + next[i] - '0';
		    else if (next[i] >= 'A' && next[i] <= 'F')
			c = c * 16 + next[i] - 'A' + 10;
		    else
			break;
		    }
		if (i == 0) {
		    /* single '=' follow by LINESEP is continuation sign? */
		    if (next[0] == '\n') {
			++mime_lineNum;
			}
		    else {
			lexPushLookaheadc('=');
			goto EndString;
			}
		    }
		else if (i == 1) {
		    lexPushLookaheadc(next[1]);
		    lexPushLookaheadc(next[0]);
		    lexAppendc('=');
		} else {
		    lexAppendc(c);
		    }
		break;
		} /* '=' */
	    case '\n': {
		lexPushLookaheadc('\n');
		goto EndString;
		}
	    case (char)EOF:
		break;
	    default:
		lexAppendc(cur);
		break;
	    } /* switch */
	} while (cur != (char)EOF);

EndString:
    lexAppendc(0);
    return lexStr();
    } /* LexQuotedPrintable */

static int yylex() {

    int lexmode = LEXMODE();
    if (lexmode == L_VALUES) {
	int c = lexGetc();
	if (c == ';') {
	    DBG_(("db: SEMICOLON\n"));
	    lexPushLookaheadc(c);
	    handleMoreRFC822LineBreak(c);
	    lexSkipLookahead();
	    return SEMICOLON;
	    }
	else if (strchr("\n",c)) {
	    ++mime_lineNum;
	    /* consume all line separator(s) adjacent to each other */
	    c = lexLookahead();
	    while (strchr("\n",c)) {
		lexSkipLookahead();
		c = lexLookahead();
		++mime_lineNum;
		}
	    DBG_(("db: LINESEP\n"));
	    return LINESEP;
	    }
	else {
	    char *p = 0;
	    lexPushLookaheadc(c);
	    if (lexWithinMode(L_BASE64)) {
		/* get each char and convert to bin on the fly... */
		p = lexGetDataFromBase64();
		yylval.str = p;
		return STRING;
		}
	    else if (lexWithinMode(L_QUOTED_PRINTABLE)) {
		p = lexGetQuotedPrintable();
		}
	    else {
#ifdef _SUPPORT_LINE_FOLDING
		p = lexGet1Value();
#else
		p = lexGetStrUntil(";\n");
#endif
		}
	    if (p) {
		DBG_(("db: STRING: '%s'\n", p));
		yylval.str = p;
		return STRING;
		}
	    else return 0;
	    }
	}
    else {
	/* normal mode */
	while (1) {
	    int c = lexGetc();
	    switch(c) {
		case ':': {
		    /* consume all line separator(s) adjacent to each other */
		    /* ignoring linesep immediately after colon. */
		    c = lexLookahead();
		    while (strchr("\n",c)) {
			lexSkipLookahead();
			c = lexLookahead();
			++mime_lineNum;
			}
		    DBG_(("db: COLON\n"));
		    return COLON;
		    }
		case ';':
		    DBG_(("db: SEMICOLON\n"));
		    return SEMICOLON;
		case '=':
		    DBG_(("db: EQ\n"));
		    return EQ;
		/* ignore whitespace in this mode */
		case '\t':
		case ' ': continue;
		case '\n': {
		    ++mime_lineNum;
		    continue;
		    }
		case EOF: return 0;
		    break;
		default: {
		    lexPushLookaheadc(c);
		    if (isalnum(c)) {
			char *t = lexGetWord();
			yylval.str = t;
			if (!qstricmp(t, "begin")) {
			    return match_begin_end_name(0);
			    }
			else if (!qstricmp(t,"end")) {
			    return match_begin_end_name(1);
			    }
		        else {
			    DBG_(("db: ID '%s'\n", t));
			    return ID;
			    }
			}
		    else {
			/* unknow token */
			return 0;
			}
		    break;
		    }
		}
	    }
	}
    return 0;
    }


/***************************************************************************/
/***							Public Functions						****/
/***************************************************************************/

static VObject* Parse_MIMEHelper()
    {
    ObjStackTop = -1;
    mime_numErrors = 0;
    mime_lineNum = 1;
    vObjList = 0;
    curObj = 0;

    if (yyparse() != 0)
	return 0;

    finiLex();
    return vObjList;
    }

/*--------------------------------------------*/
DLLEXPORT(VObject*) Parse_MIME(const char *input, unsigned long len)
    {
    initLex(input, len, 0);
    return Parse_MIMEHelper();
    }


#if INCLUDEMFC

DLLEXPORT(VObject*) Parse_MIME_FromFile(CFile *file)
    {
    unsigned long startPos;
    VObject *result;	

    initLex(0,-1,file);
    startPos = file->GetPosition();
    if (!(result = Parse_MIMEHelper()))
	file->Seek(startPos, CFile::begin);
    return result;
    }

#else

VObject* Parse_MIME_FromFile(FILE *file)
    {
    VObject *result;	
    long startPos;

    initLex(0,(unsigned long)-1,file);
    startPos = ftell(file);
    if (!(result = Parse_MIMEHelper())) {
	fseek(file,startPos,SEEK_SET);
	}
    return result;
    }

DLLEXPORT(VObject*) Parse_MIME_FromFileName(char *fname)
    {
    FILE *fp = fopen(fname,"r");
    if (fp) {
	VObject* o = Parse_MIME_FromFile(fp);
	fclose(fp);
	return o;
	}
    else {
	char msg[80];
	sprintf(msg, "can't open file '%s' for reading\n", fname);
	mime_error_(msg);
	return 0;
	}
    }

#endif

/*-------------------------------------*/

static MimeErrorHandler mimeErrorHandler;

DLLEXPORT(void) registerMimeErrorHandler(MimeErrorHandler me)
    {
    mimeErrorHandler = me;
    }

void mime_error(char *s)
    {
    char msg[256];
    if (mimeErrorHandler) {
	sprintf(msg,"%s at line %d", s, mime_lineNum);
	mimeErrorHandler(msg);
	}
    }

void mime_error_(char *s)
    {
    if (mimeErrorHandler) {
	mimeErrorHandler(s);
	}
    }

