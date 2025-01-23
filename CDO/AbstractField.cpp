/**
 @file AbstractField.cpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author daniil@megrabyan.pro
*/

#include "AbstractField.hpp"

namespace chaos { namespace cdo {
	abstract_field::abstract_field(const std::string& name, const std::string& alias, bool nullable)
	:
		_alias(alias),
		_name(name),
		_nullable(nullable)
	{
	}

	std::string abstract_field::get_name() const
	{
		return _name;
	}

	bool abstract_field::is_nullable() const
	{
		return _nullable;
	}

	std::string abstract_field::alias() const
	{
		return _alias;
	}

} }
