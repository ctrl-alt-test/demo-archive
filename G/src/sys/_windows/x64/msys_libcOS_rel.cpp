//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

void msys_memset( void *dst, int val, int amount )
{
    for( int i=0; i<amount; i++ ) ((char*)dst)[i] = val;
}

void msys_memcpy( void *dst, const void *ori, int amount )
{
    for( int i=0; i<amount; i++ ) ((char*)dst)[i] = ((char*)ori)[i];
}

size_t msys_strlen( const char *str )
{
    size_t n; for( n=0; str[n]; n++ ); return n;
}

char* msys_strcpy(char *dst, const char *src)
{
    size_t i; for( i=0; src[i]; dst[i] = src[i++] ); dst[i] = '\0'; return dst;
}

char* msys_strncpy(char *dst, const char *src, size_t n)
{
    size_t i; for( i=0; i<n && src[n]; dst[n] = src[n++]); dst[i] = '\0'; return dst;
}

int msys_strcmp(const char *s1, const char *s2)
{
	while(*s1 && (*s1==*s2))
		s1++,s2++;
	return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

int msys_strncmp(const char *s1, const char *s2, size_t n)
{
	while(n--)
		if(*s1++!=*s2++)
			return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
	return 0;
}
