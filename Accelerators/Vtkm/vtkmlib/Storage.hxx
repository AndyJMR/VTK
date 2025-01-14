//=============================================================================
//
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//  Copyright 2012 Sandia Corporation.
//  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
//  the U.S. Government retains certain rights in this software.
//
//=============================================================================

#ifndef vtkmlib_Storage_hxx
#define vtkmlib_Storage_hxx

#include "vtkmConfig.h"

namespace vtkm {
namespace cont {
namespace internal {

//------------------------------------------------------------------------------
template <typename ValueType_>
void Storage<ValueType_, tovtkm::vtkCellArrayContainerTag>::ReleaseResources()
{
  if (this->NumberOfValues > 0)
  {
    VTKM_ASSERT(this->Array != nullptr);
    if (this->DeallocateOnRelease)
    {
      this->Array->Delete();
    }
    this->Array = nullptr;
    this->NumberOfValues = 0;
    this->AllocatedSize = 0;
  }
  else
  {
    VTKM_ASSERT(this->Array == nullptr);
  }
}

//------------------------------------------------------------------------------
template <typename ValueType_>
void Storage<ValueType_, tovtkm::vtkCellArrayContainerTag>::Allocate(
    vtkm::Id numberOfValues)
{
  if (numberOfValues <= this->AllocatedSize)
  {
    this->NumberOfValues = numberOfValues;
    return;
  }

  if (this->UserProvidedMemory)
  {
    throw vtkm::cont::ErrorBadValue(
        "User allocated arrays cannot be reallocated.");
  }

  this->ReleaseResources();
  try
  {
    if (numberOfValues > 0)
    {
      this->Array = vtkCellArray::New();
      this->Array->Allocate(numberOfValues);
      this->AllocatedSize = numberOfValues;
      this->NumberOfValues = numberOfValues;
    }
    else
    {
      // ReleaseResources should have already set AllocatedSize to 0.
      VTKM_ASSERT(this->AllocatedSize == 0);
    }
  }
  catch (std::bad_alloc&)
  {
    // Make sure our state is OK.
    this->Array = nullptr;
    this->NumberOfValues = 0;
    this->AllocatedSize = 0;
    throw vtkm::cont::ErrorBadAllocation(
        "Could not allocate basic control array.");
  }

  this->DeallocateOnRelease = true;
  this->UserProvidedMemory = false;
}

//------------------------------------------------------------------------------
template <typename ValueType_>
typename Storage<ValueType_, tovtkm::vtkCellArrayContainerTag>::PortalType
Storage<ValueType_, tovtkm::vtkCellArrayContainerTag>::GetPortal()
{
  return PortalType(this->Array->GetData(), this->NumberOfValues);
}

//------------------------------------------------------------------------------
template <typename ValueType_>
typename Storage<ValueType_, tovtkm::vtkCellArrayContainerTag>::PortalConstType
Storage<ValueType_, tovtkm::vtkCellArrayContainerTag>::GetPortalConst() const
{
  return PortalConstType(this->Array->GetData(), this->NumberOfValues);
}
}
}
}

#endif // vtkmlib_Storage_hxx
