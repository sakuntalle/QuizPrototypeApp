#include "stdafx.h"
#include "CommMessageBase.h"

template<class Archive>
void CommMessageBase::serialize(Archive &ar, const unsigned int /*version*/)
{
}

BOOST_CLASS_EXPORT_IMPLEMENT(CommMessageBase)

