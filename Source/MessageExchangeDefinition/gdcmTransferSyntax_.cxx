/*=========================================================================

  Program: GDCM (Grassroots DICOM). A DICOM library
  Module:  $URL$

  Copyright (c) 2006-2010 Mathieu Malaterre
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "gdcmTransferSyntax_.h"
#include "gdcmSwapper.h"

namespace gdcm
{
namespace network
{
const uint8_t TransferSyntax_::ItemType = 0x40;
const uint8_t TransferSyntax_::Reserved2 = 0x00;

TransferSyntax_::TransferSyntax_()
{
  UpdateName( "1.2.840.10008.1.1" );
}

void TransferSyntax_::SetName( const char *name )
{
  Name = name;
  ItemLength = Name.size();
}

std::istream &TransferSyntax_::Read(std::istream &is)
{
  uint8_t itemtype = 0x0;
  is.read( (char*)&itemtype, sizeof(ItemType) );
  assert( itemtype == ItemType );
  uint8_t reserved2;
  is.read( (char*)&reserved2, sizeof(Reserved2) );
  uint16_t itemlength;
  is.read( (char*)&itemlength, sizeof(ItemLength) );
  SwapperDoOp::SwapArray(&itemlength,1);
  ItemLength = itemlength;

  char name[256];
  assert( itemlength < 256 );
  is.read( name, itemlength );
  Name = name;

  return is;
}

const std::ostream &TransferSyntax_::Write(std::ostream &os) const
{
  os.write( (char*)&ItemType, sizeof(ItemType) );
  os.write( (char*)&Reserved2, sizeof(Reserved2) );
  //os.write( (char*)&ItemLength, sizeof(ItemLength) );
  uint16_t copy = ItemLength;
  SwapperDoOp::SwapArray(&copy,1);
  os.write( (char*)&copy, sizeof(ItemLength) );

  os.write( Name.c_str(), Name.size() );
  return os;
}

size_t TransferSyntax_::Size() const
{
  size_t ret = 0;
  assert( Name.size() == ItemLength );
  ret += sizeof(ItemType);
  ret += sizeof(Reserved2);
  ret += sizeof(ItemLength);
  ret += ItemLength;
  return ret;
}

void TransferSyntax_::UpdateName( const char *name )
{
  if( name )
    {
    gdcm::UIDs uids;
    bool b = uids.SetFromUID( name );
    if( b )
      {
      Name = name;
      ItemLength = Name.size();
      assert( ItemLength + 4 == Size() );
      return;
      }
    }

  gdcmErrorMacro( "Invalid Name: " << name );
  throw "Invalid Name";
}

void TransferSyntax_::SetNameFromUID( gdcm::UIDs::TSName tsname )
{
  const char *name = gdcm::UIDs::GetUIDString( tsname );
  UpdateName( name );
}

} // end namespace network
} // end namespace gdcm
