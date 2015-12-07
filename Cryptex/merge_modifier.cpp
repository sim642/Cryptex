#include "merge_modifier.hpp"

merge_modifier::merge_modifier()
{

}

merge_modifier::~merge_modifier()
{

}

void merge_modifier::add_modifier(blob_modifier& modifier)
{
	modifiers.push_back(&modifier);
}

void merge_modifier::modify(blob& b)
{
	for (blob_modifier *modifier : modifiers)
		modifier->modify(b);
}
