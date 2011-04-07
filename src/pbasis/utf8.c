/*
  Basic UTF-8 manipulation routines
  by Jeff Bezanson
  placed in the public domain Fall 2005

  This code is designed to provide the utilities you need to manipulate
  UTF-8 as an internal string encoding. These functions do not perform the
  error checking normally needed when handling UTF-8 data, so if you happen
  to be from the Unicode Consortium you will want to flay me alive.
  I do this because error checking can be performed at the boundaries (I/O),
  with these routines reserved for higher performance on data known to be
  valid.
*/

#include "pbasis.h"

#if _WIN32
#define snprintf _snprintf
#endif

static const wchar offsetsFromUTF8[6] = {
    0x00000000UL, 0x00003080UL, 0x000E2080UL,
    0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

static const uchar trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/* -FUNCTION--------------------------------------------------------------------
	Function:		u8_seqlen()

	Author:			Jeff Bezanson

	Usage:			returns length of next utf-8 sequence

	Parameters:		uchar*		s					Pointer to begin of
													UTF-8 sequence.

	Returns:		int								Unicode character code

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int u8_seqlen(uchar *s)
{
    return trailingBytesForUTF8[(wchar)s[0]] + 1;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		u8_char()

	Author:			Jan Max Meyer

	Usage:			Return single character.

	Parameters:		uchar*		str					Pointer to character begin

	Returns:		wchar							Returns the converted

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
wchar u8_char( uchar* str )
{
	int 	nb;
	wchar 	ch = 0;

	switch( ( nb = trailingBytesForUTF8[*str] ) )
	{
        case 3:
			ch += *str++;
			ch <<= 6;
        case 2:
			ch += *str++;
			ch <<= 6;
        case 1:
			ch += *str++;
			ch <<= 6;
        case 0:
			ch += *str++;
	}
	
	ch -= offsetsFromUTF8[ nb ];

	return ch;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		u8_move()

	Author:			Jan Max Meyer

	Usage:			Moves x characters ahead.

	Parameters:		uchar*		str					Pointer to UTF-8 string
					int			count				Number of characters to
													move left.
					

	Returns:		uchar*							Address of new string
													begin

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* u8_move( uchar* str, int count )
{
	for( ; count > 0; count-- )
		str += u8_seqlen( str );

	return str;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		u8_parse_char()
	
	Author:			Jan Max Meyer
	
	Usage:			Read one character from an UTF-8 input stream.
					This character can be escaped, a UTF-8 character or an
					ordinary ASCII-char.
					
	Parameters:		uchar**		chr				Input-/Output-Pointer
	
	Returns:		wchar						Returns the character code
												of the parsed character.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
wchar u8_parse_char( uchar** ch )
{
	wchar	ret;
	PROC( "ccl_u8_to_char" );
	PARMS( "ch", "%p", ch );

	if( u8_char( *ch ) == (wchar)'\\' )
	{
		MSG( "Escape sequence detected" );
		(*ch)++;

		VARS( "*ch", "%s", *ch );
		(*ch) += u8_read_escape_sequence( *ch, &ret );
		VARS( "*ch", "%s", *ch );
	}
	else
	{
		MSG( "No escape sequence, normal UTF-8 character sequence processing" );
		ret = u8_char( *ch );
		(*ch) += u8_seqlen( *ch );
	}

	VARS( "ret", "%d", ret );
	RETURN( ret );
}


/* conversions without error checking
   only works for valid UTF-8, i.e. no 5- or 6-byte sequences
   srcsz = source size in bytes, or -1 if 0-terminated
   sz = dest size in # of wide characters

   returns # characters converted
   dest will always be L'\0'-terminated, even if there isn't enough room
   for all the characters.
   if sz = srcsz+1 (i.e. 4*srcsz+4 bytes), there will always be enough space.
*/
int u8_toucs(wchar *dest, int sz, uchar *src, int srcsz)
{
    wchar ch;
    uchar *src_end = src + srcsz;
    int nb;
    int i=0;

    while (i < sz-1) {
        nb = trailingBytesForUTF8[*src];
        if (srcsz == -1) {
            if (*src == 0)
                goto done_toucs;
        }
        else {
            if (src + nb >= src_end)
                goto done_toucs;
        }
        ch = 0;
        switch (nb) {
            /* these fall through deliberately */
        case 3: ch += *src++; ch <<= 6;
        case 2: ch += *src++; ch <<= 6;
        case 1: ch += *src++; ch <<= 6;
        case 0: ch += *src++;
        }
        ch -= offsetsFromUTF8[nb];
        dest[i++] = ch;
    }
 done_toucs:
    dest[i] = 0;
    return i;
}

/* srcsz = number of source characters, or -1 if 0-terminated
   sz = size of dest buffer in bytes

   returns # characters converted
   dest will only be '\0'-terminated if there is enough space. this is
   for consistency; imagine there are 2 bytes of space left, but the next
   character requires 3 bytes. in this case we could NUL-terminate, but in
   general we can't when there's insufficient space. therefore this function
   only NUL-terminates if all the characters fit, and there's space for
   the NUL as well.
   the destination string will never be bigger than the source string.
*/
int u8_toutf8(uchar *dest, int sz, wchar *src, int srcsz)
{
    wchar ch;
    int i = 0;
    uchar *dest_end = dest + sz;

    while (srcsz<0 ? src[i]!=0 : i < srcsz) {
        ch = src[i];
        if (ch < 0x80) {
            if (dest >= dest_end)
                return i;
            *dest++ = (uchar)ch;
        }
        else if (ch < 0x800) {
            if (dest >= dest_end-1)
                return i;
            *dest++ = (ch>>6) | 0xC0;
            *dest++ = (ch & 0x3F) | 0x80;
        }
        else if (ch < 0x10000) {
            if (dest >= dest_end-2)
                return i;
            *dest++ = (ch>>12) | 0xE0;
            *dest++ = ((ch>>6) & 0x3F) | 0x80;
            *dest++ = (ch & 0x3F) | 0x80;
        }
        else if (ch < 0x110000) {
            if (dest >= dest_end-3)
                return i;
            *dest++ = (ch>>18) | 0xF0;
            *dest++ = ((ch>>12) & 0x3F) | 0x80;
            *dest++ = ((ch>>6) & 0x3F) | 0x80;
            *dest++ = (ch & 0x3F) | 0x80;
        }
        i++;
    }
    if (dest < dest_end)
        *dest = '\0';
    return i;
}

int u8_wc_toutf8(uchar *dest, wchar ch)
{
    if (ch < 0x80) {
        dest[0] = (uchar)ch;
        return 1;
    }
    if (ch < 0x800) {
        dest[0] = (ch>>6) | 0xC0;
        dest[1] = (ch & 0x3F) | 0x80;
        return 2;
    }
    if (ch < 0x10000) {
        dest[0] = (ch>>12) | 0xE0;
        dest[1] = ((ch>>6) & 0x3F) | 0x80;
        dest[2] = (ch & 0x3F) | 0x80;
        return 3;
    }
    if (ch < 0x110000) {
        dest[0] = (ch>>18) | 0xF0;
        dest[1] = ((ch>>12) & 0x3F) | 0x80;
        dest[2] = ((ch>>6) & 0x3F) | 0x80;
        dest[3] = (ch & 0x3F) | 0x80;
        return 4;
    }
    return 0;
}

/* charnum => byte offset */
int u8_offset(uchar *str, int charnum)
{
    int offs=0;

    while (charnum > 0 && str[offs]) {
        (void)(u8_isutf(str[++offs]) || u8_isutf(str[++offs]) ||
               u8_isutf(str[++offs]) || ++offs);
        charnum--;
    }
    return offs;
}

/* byte offset => charnum */
int u8_charnum(uchar *s, int offset)
{
    int charnum = 0, offs=0;

    while (offs < offset && s[offs]) {
        (void)(u8_isutf(s[++offs]) || u8_isutf(s[++offs]) ||
               u8_isutf(s[++offs]) || ++offs);
        charnum++;
    }
    return charnum;
}

/* number of characters */
int u8_strlen(uchar *s)
{
    int count = 0;
    int i = 0;

    while (u8_nextchar(s, &i) != 0)
        count++;

    return count;
}

/* reads the next utf-8 sequence out of a string, updating an index */
wchar u8_nextchar(uchar *s, int *i)
{
    wchar ch = 0;
    int sz = 0;

    do {
        ch <<= 6;
        ch += s[(*i)++];
        sz++;
    } while (s[*i] && !u8_isutf(s[*i]));
    ch -= offsetsFromUTF8[sz-1];

    return ch;
}

void u8_inc(uchar *s, int *i)
{
    (void)(u8_isutf(s[++(*i)]) || u8_isutf(s[++(*i)]) ||
           u8_isutf(s[++(*i)]) || ++(*i));
}

void u8_dec(uchar *s, int *i)
{
    (void)(u8_isutf(s[--(*i)]) || u8_isutf(s[--(*i)]) ||
           u8_isutf(s[--(*i)]) || --(*i));
}

int octal_digit(uchar c)
{
    return (c >= '0' && c <= '7');
}

int hex_digit(uchar c)
{
    return ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'F') ||
            (c >= 'a' && c <= 'f'));
}

/* assumes that src points to the character after a backslash
   returns number of input characters processed */
int u8_read_escape_sequence(uchar *str, wchar *dest)
{
    wchar ch;
    uchar digs[9]="\0\0\0\0\0\0\0\0";
    int dno=0, i=1;

    ch = (wchar)str[0];    /* take literal character */
    if (str[0] == 'n')
        ch = L'\n';
    else if (str[0] == 't')
        ch = L'\t';
    else if (str[0] == 'r')
        ch = L'\r';
    else if (str[0] == 'b')
        ch = L'\b';
    else if (str[0] == 'f')
        ch = L'\f';
    else if (str[0] == 'v')
        ch = L'\v';
    else if (str[0] == 'a')
        ch = L'\a';
    else if (octal_digit(str[0])) {
        i = 0;
        do {
            digs[dno++] = str[i++];
        } while (octal_digit(str[i]) && dno < 3);
        ch = strtol(digs, NULL, 8);
    }
    else if (str[0] == 'x') {
        while (hex_digit(str[i]) && dno < 2) {
            digs[dno++] = str[i++];
        }
        if (dno > 0)
            ch = strtol(digs, NULL, 16);
    }
    else if (str[0] == 'u') {
        while (hex_digit(str[i]) && dno < 4) {
            digs[dno++] = str[i++];
        }
        if (dno > 0)
            ch = strtol(digs, NULL, 16);
    }
    else if (str[0] == 'U') {
        while (hex_digit(str[i]) && dno < 8) {
            digs[dno++] = str[i++];
        }
        if (dno > 0)
            ch = strtol(digs, NULL, 16);
    }
    *dest = ch;

    return i;
}

/* convert a string with literal \uxxxx or \Uxxxxxxxx characters to UTF-8
   example: u8_unescape(mybuf, 256, "hello\\u220e")
   note the double backslash is needed if called on a C string literal */
int u8_unescape(uchar *buf, int sz, uchar *src)
{
    int c=0, amt;
    wchar ch;
    uchar temp[4];

    while (*src && c < sz) {
        if (*src == '\\') {
            src++;
            amt = u8_read_escape_sequence(src, &ch);
        }
        else {
            ch = (wchar)*src;
            amt = 1;
        }
        src += amt;
        amt = u8_wc_toutf8(temp, ch);
        if (amt > sz-c)
            break;
        memcpy(&buf[c], temp, amt);
        c += amt;
    }
    if (c < sz)
        buf[c] = '\0';
    return c;
}

int u8_escape_wchar(uchar *buf, int sz, wchar ch)
{
    if (ch == L'\n')
        return snprintf(buf, sz, "\\n");
    else if (ch == L'\t')
        return snprintf(buf, sz, "\\t");
    else if (ch == L'\r')
        return snprintf(buf, sz, "\\r");
    else if (ch == L'\b')
        return snprintf(buf, sz, "\\b");
    else if (ch == L'\f')
        return snprintf(buf, sz, "\\f");
    else if (ch == L'\v')
        return snprintf(buf, sz, "\\v");
    else if (ch == L'\a')
        return snprintf(buf, sz, "\\a");
    else if (ch == L'\\')
        return snprintf(buf, sz, "\\\\");
    else if (ch < 32 || ch == 0x7f)
        return snprintf(buf, sz, "\\x%hhX", ch);
    else if (ch > 0xFFFF)
        return snprintf(buf, sz, "\\U%.8X", (wchar)ch);
    else if (ch >= 0x80 && ch <= 0xFFFF)
        return snprintf(buf, sz, "\\u%.4hX", (unsigned short)ch);

    return snprintf(buf, sz, "%c", (uchar)ch);
}

int u8_escape(uchar *buf, int sz, uchar *src, int escape_quotes)
{
    int c=0, i=0, amt;

    while (src[i] && c < sz) {
        if (escape_quotes && src[i] == '"') {
            amt = snprintf(buf, sz - c, "\\\"");
            i++;
        }
        else {
            amt = u8_escape_wchar(buf, sz - c, u8_nextchar(src, &i));
        }
        c += amt;
        buf += amt;
    }
    if (c < sz)
        *buf = '\0';
    return c;
}

uchar *u8_strchr(uchar *s, wchar ch, int *charn)
{
    int i = 0, lasti=0;
    wchar c;

    *charn = 0;
    while (s[i]) {
        c = u8_nextchar(s, &i);
        if (c == ch) {
            return &s[lasti];
        }
        lasti = i;
        (*charn)++;
    }
    return NULL;
}

uchar *u8_memchr(uchar *s, wchar ch, size_t sz, int *charn)
{
    int i = 0, lasti=0;
    wchar c;
    int csz;

    *charn = 0;
    while (i < sz) {
        c = csz = 0;
        do {
            c <<= 6;
            c += s[i++];
            csz++;
        } while (i < sz && !u8_isutf(s[i]));
        c -= offsetsFromUTF8[csz-1];

        if (c == ch) {
            return &s[lasti];
        }
        lasti = i;
        (*charn)++;
    }
    return NULL;
}

int u8_is_locale_utf8(uchar *locale)
{
    /* this code based on libutf8 */
    const uchar* cp = locale;

    for (; *cp != '\0' && *cp != '@' && *cp != '+' && *cp != ','; cp++) {
        if (*cp == '.') {
            const uchar* encoding = ++cp;
            for (; *cp != '\0' && *cp != '@' && *cp != '+' && *cp != ','; cp++)
                ;
            if ((cp-encoding == 5 && !strncmp(encoding, "UTF-8", 5))
                || (cp-encoding == 4 && !strncmp(encoding, "utf8", 4)))
                return 1; /* it's UTF-8 */
            break;
        }
    }
    return 0;
}


#if 0
int u8_vprintf(uchar *fmt, va_list ap)
{
    int cnt, sz=0;
    uchar *buf;
    wchar *wcs;

    sz = 512;
    buf = (uchar*)alloca(sz);
 try_print:
    cnt = vsnprintf(buf, sz, fmt, ap);
    if (cnt >= sz) {
        buf = (uchar*)alloca(cnt - sz + 1);
        sz = cnt + 1;
        goto try_print;
    }
    wcs = (wchar*)alloca((cnt+1) * sizeof(wchar));
    cnt = u8_toucs(wcs, cnt+1, buf, cnt);
    printf("%ls", (wchar_t*)wcs);
    return cnt;
}

int u8_printf(uchar *fmt, ...)
{
    int cnt;
    va_list args;

    va_start(args, fmt);

    cnt = u8_vprintf(fmt, args);

    va_end(args);
    return cnt;
}


/* is c the start of a utf8 sequence? */
#define u8_isutf(c) (((c)&0xC0)!=0x80)

/* convert UTF-8 data to wide character */
int u8_toucs(wchar *dest, int sz, char *src, int srcsz);

/* the opposite conversion */
int u8_toutf8(char *dest, int sz, wchar *src, int srcsz);

/* single character to UTF-8 */
int u8_wc_toutf8(char *dest, wchar ch);

/* character number to byte offset */
int u8_offset(char *str, int charnum);

/* byte offset to character number */
int u8_charnum(char *s, int offset);

/* return next character, updating an index variable */
wchar u8_nextchar(char *s, int *i);

/* move to next character */
void u8_inc(char *s, int *i);

/* move to previous character */
void u8_dec(char *s, int *i);

/* returns length of next utf-8 sequence */
int u8_seqlen(char *s);

/* assuming src points to the character after a backslash, read an
   escape sequence, storing the result in dest and returning the number of
   input characters processed */
int u8_read_escape_sequence(char *src, wchar *dest);

/* given a wide character, convert it to an ASCII escape sequence stored in
   buf, where buf is "sz" bytes. returns the number of characters output. */
int u8_escape_wchar(char *buf, int sz, wchar ch);

/* convert a string "src" containing escape sequences to UTF-8 */
int u8_unescape(char *buf, int sz, char *src);

/* convert UTF-8 "src" to ASCII with escape sequences.
   if escape_quotes is nonzero, quote characters will be preceded by
   backslashes as well. */
int u8_escape(char *buf, int sz, char *src, int escape_quotes);

/* utility predicates used by the above */
int octal_digit(char c);
int hex_digit(char c);

/* return a pointer to the first occurrence of ch in s, or NULL if not
   found. character index of found character returned in *charn. */
char *u8_strchr(char *s, wchar ch, int *charn);

/* same as the above, but searches a buffer of a given size instead of
   a NUL-terminated string. */
char *u8_memchr(char *s, wchar ch, size_t sz, int *charn);

/* count the number of characters in a UTF-8 string */
int u8_strlen(char *s);

int u8_is_locale_utf8(char *locale);

/* printf where the format string and arguments may be in UTF-8.
   you can avoid this function and just use ordinary printf() if the current
   locale is UTF-8. */
int u8_vprintf(char *fmt, va_list ap);
int u8_printf(char *fmt, ...);

#endif

