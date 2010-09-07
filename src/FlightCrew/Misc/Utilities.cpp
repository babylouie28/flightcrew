/************************************************************************
**
**  Copyright (C) 2010  Strahinja Markovic
**
**  This file is part of FlightCrew.
**
**  FlightCrew is free software: you can redistribute it and/or modify
**  it under the terms of the GNU Lesser General Public License as published
**  by the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  FlightCrew is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public License
**  along with FlightCrew.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#include <stdafx.h>
#include "Utilities.h"
#include <fstream>
#include <utf8.h>
#include <xercesc/util/TransService.hpp>

namespace FlightCrew
{

namespace Util
{

std::string ReadUnicodFile( const fs::path &filepath )
{
    std::ifstream file( filepath.c_str(), std::ios::in | std::ios::binary );

    if ( !file.is_open() )

          boost_throw( FileDoesNotExistEx() << ei_FilePath( filepath.generic_string() ) );

    std::vector< char > contents( (std::istreambuf_iterator< char>( file )), 
                                   std::istreambuf_iterator< char>() );

    // May as well be empty
    if ( contents.size() < 2 )
    
        return std::string();

    if ( utf8::is_valid( contents.begin(), contents.end() ) )

        return std::string( contents.data() );

    // UTF-16BE
    if ( static_cast< unsigned char >( contents[ 0 ] ) == 0xfeU &&
         static_cast< unsigned char >( contents[ 1 ] ) == 0xffU )
    {
        xc::TranscodeFromStr transcoder( 
            (const XMLByte*) contents.data(), contents.size() , "UTF-16BE" );

        xc::TranscodeToStr transcoder_utf8( transcoder.str(), "UTF-8" );

        return std::string( (char*) transcoder_utf8.str() );
    }

    // UTF-16LE
    else if ( static_cast< unsigned char >( contents[ 0 ] ) == 0xffU &&
              static_cast< unsigned char >( contents[ 1 ] ) == 0xfeU )
    {
        xc::TranscodeFromStr transcoder( 
            (const XMLByte*) contents.data(), contents.size(), "UTF-16LE" );

        xc::TranscodeToStr transcoder_utf8( transcoder.str(), "UTF-8" );

        return std::string( (char*) transcoder_utf8.str() );
    }

    else
    {
        boost_throw( FileNotInUnicodeEx() << ei_FilePath( filepath.generic_string() ) );
    }
}


std::string GetFirstNumChars( const std::string &string, uint num_chars )
{
    if ( string.empty() )

        return std::string();

    uint string_size   = static_cast< unsigned int >( string.size() );
    uint chars_to_copy = string_size < num_chars ? string_size : num_chars;

    std::string::const_iterator it = string.begin();
    std::advance( it, chars_to_copy );

    std::string line;
    line.resize( num_chars );

    std::copy( string.begin(), it, line.begin() );

    return line;
}

} // namespace Util

} // namespace FlightCrew