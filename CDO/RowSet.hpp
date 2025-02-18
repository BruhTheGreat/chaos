/**
 @file RowSet.hpp
 @date 17.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
 */

#ifndef Chaos_CDO_RowSet_hpp
#define Chaos_CDO_RowSet_hpp

#include "AbstractField.hpp"

#include <string>

namespace chaos { namespace cdo {
	/**
	* @brief The row_set class
	* @todo Keep field order!
	*/
	class row_set
	{
	/** @name Constructors */
	/** @{ */
	public:
		row_set(const std::string& name);
		virtual ~row_set() = 0;
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		std::string _name;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		virtual std::vector<std::shared_ptr<abstract_field>>  get_fields() const = 0;
		std::string name() const {return _name;};
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		virtual void add_field(std::shared_ptr<abstract_field> field) = 0;
	/** @} */
	};
}}

#endif // ROWSET_HPP
