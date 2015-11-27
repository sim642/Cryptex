#ifndef MERGE_MODIFIER_H
#define MERGE_MODIFIER_H

#include "blob_modifier.hpp"
#include <vector>

class merge_modifier : public blob_modifier
{
	public:
		merge_modifier();
		virtual ~merge_modifier();

		void add_modifier(blob_modifier &modifier);

		virtual void modify(blob &b);

	private:
		std::vector<blob_modifier*> modifiers;
};

#endif // MERGE_MODIFIER_H
