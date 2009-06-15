/* Copyright, 2003 Neverborn Entertainment.

 * This file is part of our XSI Plugins.

 * The XSI Plugins are free software, you can redistribute them and/or modify
 * them under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2, or (at your option)
 * any later version.

 * XSI Plugins are distributed WITHOUT ANY WARRANTY or implied warranty of
 * MERCHANTABILITY. See the GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with the XSI Plugins; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.

 */


#ifndef NEL_TOOLS_HELPER_H
#define NEL_TOOLS_HELPER_H

//
// Includes
//



//
// Functions
//

inline void XSIA2WHelper( wchar_t* out_wcs, const char* in_sz, int in_cch = -1 )
{
	if ( out_wcs != NULL && 0 != in_cch )
	{
		out_wcs[0] = L'\0';

		if ( in_sz != NULL )
		{
			size_t l_iLen = 0;
			l_iLen = ::mbstowcs( out_wcs, in_sz, ( in_cch < 0 ) ? ::strlen(in_sz) + 1 : (size_t) in_cch );
			nlassert( (int)l_iLen != -1 );

        	if ( in_cch > 0 )
			{
				out_wcs[ in_cch - 1 ] = L'\0';
			}
		}
	}
}

inline void XSIW2AHelper( char* out_sz, const wchar_t* in_wcs, int in_cch = -1 )
{
	if ( out_sz != NULL && 0 != in_cch )
	{
		out_sz[0] = '\0' ;
		if ( in_wcs != NULL )
		{
			size_t l_iLen = 0;
			l_iLen = ::wcstombs( out_sz, in_wcs, ( in_cch < 0 ) ? ::wcslen(in_wcs) + 1 : (size_t) in_cch );
			nlassert( (int)l_iLen != -1 );
        	if ( in_cch > 0 )
			{
				out_sz[ in_cch - 1 ] = '\0';
			}
		}
	}
}

#define XSIA2W(out_wcs, in_sz) \
	if (NULL == (const char *)(in_sz)) \
		*(out_wcs) = NULL; \
	else \
	{ \
		*(out_wcs) = (wchar_t*)alloca((strlen((in_sz)) + 1) * sizeof(wchar_t)); \
		XSIA2WHelper(*(out_wcs), (in_sz)); \
	}

#define XSIW2A(out_sz, in_wcs) \
	if (NULL == (const wchar_t *)(in_wcs)) \
		*(out_sz) = NULL; \
	else \
	{ \
		*(out_sz) = (char*)alloca(wcslen((in_wcs)) + 1); \
		XSIW2AHelper(*(out_sz), (in_wcs)); \
	}

#endif
