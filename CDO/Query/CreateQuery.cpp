/**
 @file CreateQuery.cpp
 @date 05.01.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#include "CreateQuery.hpp"

namespace chaos { namespace cdo {

	create::create(const std::string& name, bool ifNotExists)
	:
	  abstract_query(),
	  _table_name(name),
	  _if_not_exists(ifNotExists)
	{
	}

	create::create(const table& tbl, bool ifNotExists)
	:
	  abstract_query(),
	  _table_name(tbl.name()),
	  _if_not_exists(ifNotExists)
	{
		columns(tbl.get_fields());
	}

	create& create::if_not_exists(bool state)
	{
		_if_not_exists = state;
		return *this;
	}

	create& create::columns(std::shared_ptr<abstract_field> field)
	{
		_columns.push_back(field);
		return *this;
	}

	create& create::columns(std::initializer_list<std::shared_ptr<abstract_field>> fields)
	{
		_columns.insert(_columns.end(), fields.begin(), fields.end());
		return *this;
	}

	create& create::columns(const std::vector<std::shared_ptr<abstract_field>>& fields)
	{
		for(const auto& field: fields) {
			_columns.push_back(field);
		}
		return *this;
	}

	create& create::primary_key(std::initializer_list<std::string> keys)
	{
		_primary_keys.insert(_primary_keys.end(), keys.begin(), keys.end());
		return *this;
	}

	create& create::primary_key(const std::vector<std::string>& keys)
	{
		for(const auto& pk:keys) {
			_primary_keys.push_back(pk);
		}

		return *this;
	}

	create& create::foreign_key(std::initializer_list<struct foreign_key> keys)
	{
		_foreign_keys.insert(_foreign_keys.end(), keys.begin(), keys.end());
		return *this;
	}

	create& create::foreign_key(const std::vector<struct foreign_key>& keys)
	{
		for(const auto& pk:keys) {
			_foreign_keys.push_back(pk);
		}

		return *this;
	}

}}
