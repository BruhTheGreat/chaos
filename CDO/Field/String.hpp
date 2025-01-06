/**
 @file String.hpp
 @date 17.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
 */

#ifndef Chaos_CDO_Field_String_hpp
#define Chaos_CDO_Field_String_hpp

#include "../AbstractField.hpp"

namespace chaos { namespace cdo {
	class string: public abstract_field
	{
	/** @name Constructors */
	/** @{ */
	public:
		string(const std::string& name, bool nullable = true, const std::string& value = "", const uint8_t length = 255);
		string(string&) = delete;
		string& operator=(string&) = delete;
		virtual ~string() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _value;
		const int _length;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual std::string to_SQL() override;

		const std::string& get_value() const;

	/** @} */
	};
} }

#endif

