/**
 @file IntegerField.hpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author danil@megrabyan.pro
 */

#ifndef Chaos_CDO_Field_IntegerField_hpp
#define Chaos_CDO_Field_IntegerField_hpp

#include "../AbstractField.hpp"

namespace chaos { namespace cdo {
	class signed_integer: public abstract_field
	{
	/** @name Constructors */
	/** @{ */
	public:
		signed_integer(const std::string& name, bool nullable = true, std::int32_t value = 0);
		signed_integer(signed_integer&) = delete;
		signed_integer& operator=(signed_integer&) = delete;
		virtual ~signed_integer() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::int32_t _value;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual std::string to_SQL() override;

		std::int32_t get_value() const;

	/** @} */
	};
} }

#endif
