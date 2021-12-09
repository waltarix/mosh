/*
    Mosh: the mobile shell
    Copyright 2012 Keith Winstein

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    In addition, as a special exception, the copyright holders give
    permission to link the code of portions of this program with the
    OpenSSL library under certain conditions as described in each
    individual source file, and distribute linked combinations including
    the two.

    You must obey the GNU General Public License in all respects for all
    of the code used other than OpenSSL. If you modify file(s) with this
    exception, you may extend this exception to your version of the
    file(s), but you are not obligated to do so. If you do not wish to do
    so, delete this exception statement from your version. If you delete
    this exception statement from all source files in the program, then
    also delete it here.
*/

#include <new>
#include <stdint.h>
#include <stdlib.h>
#include <zstd.h>

#include "compressor.h"
#include "src/util/dos_assert.h"

#define MINCLEVEL 1
#define MAXCLEVEL ZSTD_maxCLevel()
#define DEFCLEVEL ZSTD_defaultCLevel()

using namespace Network;

static uint8_t get_zstd_level()
{
  const uint8_t default_level = static_cast<uint8_t>( DEFCLEVEL );

  const char *level_envar = getenv( "MOSH_COMPRESSION_LEVEL" );
  if (!level_envar) {
    return default_level;
  }

  char *endptr;
  const uint8_t level = static_cast<uint8_t>( strtol( level_envar, &endptr, 10 ));
  if ( MINCLEVEL <= level && level <= MAXCLEVEL ) {
    return level;
  } else {
          fprintf( stderr, "Warning: %d is invalid value for MOSH_COMPRESSION_LEVEL [%d-%d], use default (%d).\n",
                 level, MINCLEVEL, MAXCLEVEL, default_level );
  }

  return default_level;
}

std::string Compressor::compress_str( const std::string& input )
{
  size_t const compressedSize = ZSTD_compressCCtx(
      cctx, buffer, bufferSize, reinterpret_cast<const unsigned char*>( input.data() ), input.size(), level );
  dos_assert( !ZSTD_isError(compressedSize) );
  return std::string( reinterpret_cast<char*>( buffer ), compressedSize );
}

std::string Compressor::uncompress_str( const std::string& input )
{
  size_t const decompressedSize = ZSTD_decompressDCtx(
      dctx, buffer, bufferSize, reinterpret_cast<const unsigned char*>( input.data() ), input.size() );
  dos_assert( !ZSTD_isError(decompressedSize) );
  return std::string( reinterpret_cast<char*>( buffer ), decompressedSize );
}

/* construct on first use */
Compressor& Network::get_compressor( void )
{
  static Compressor the_compressor = Compressor( ZSTD_compressBound( Compressor::BUFFER_SIZE ), get_zstd_level() );
  return the_compressor;
}

Compressor::Compressor( const size_t bufSize, const uint8_t level )
  : bufferSize( bufSize ),
    buffer( malloc( bufSize ) ),
    cctx( ZSTD_createCCtx() ),
    dctx( ZSTD_createDCtx() ),
    level( level )
{
  if ( buffer == NULL ) {
    throw std::bad_alloc();
  }

  if ( cctx == NULL ) {
    throw std::bad_alloc();
  }

  if ( dctx == NULL ) {
    throw std::bad_alloc();
  }
}

Compressor::~Compressor()
{
  free( buffer );
  ZSTD_freeCCtx( cctx );
  ZSTD_freeDCtx( dctx );
}
