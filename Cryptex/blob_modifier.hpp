#ifndef BLOB_MODIFIER_H
#define BLOB_MODIFIER_H

#include "blob.hpp"

class blob_modifier
{
	public:
		blob_modifier();
		virtual ~blob_modifier();

		virtual void modify(blob &b) = 0;
};

#endif // BLOB_MODIFIER_H
