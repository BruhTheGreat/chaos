// /**
//  @file FieldSet.cpp
//  @date 01.05.2022
//  @copyright Cadabra / Ace
//  @author daniil@megrabyan.pro
// */

// #include "FieldSet.hpp"

// namespace chaos { namespace cdo {
// 	bool field_set::comparator::operator()(const std::unique_ptr<abstract_field>& lhs, const std::unique_ptr<abstract_field>& rhs) const
// 	{
// 		return (!lhs ? true : (!rhs ? false : lhs->get_name() < rhs->get_name()));
// 	}

// 	field_set::field_set()
// 	{

// 	}

// 	field_set::~field_set()
// 	{
// 		/// @todo cleanup set
// 	}

// 	const signed_integer& field_set::operator()(const std::string& name, bool nullable, std::int32_t value)
// 	{
// 		// std::unique_ptr<signed_integer> field(new signed_integer(name, nullable, value));
// 		// signed_integer& retval(*(field.get()));
// 		// _set.emplace(std::move(field));

// 		return signed_integer{};
// 	}
// /*
// 	const biginteger_field& field_set::operator()(const std::string& name, bool nullable = true, std::int64_t value = 0)
// 	{

// 	}

// 	const string_field& field_set::operator()(const std::string& name, std::size_t length = 0, bool nullable = true, const std::string& value = "")
// 	{

// 	}
// */
// } }
