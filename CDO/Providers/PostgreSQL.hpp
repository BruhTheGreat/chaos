/**
 @file PosgtreSQL.hpp
 @date 28.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#ifndef Chaos_CDO_PosgtreSQL_hpp
#define Chaos_CDO_PosgtreSQL_hpp

#include "../SQL.hpp"
#include "../Query/CreateQuery.hpp"
#include "../Query/DeleteQuery.hpp"
#include "../Query/DropQuery.hpp"
#include "../Query/InsertQuery.hpp"
#include "../Query/SelectQuery.hpp"



namespace chaos { namespace cdo {
	class postgresql : public sql
	{
	/** @name Constructors */
	/** @{ */
	public:
		postgresql() = default;
		postgresql(const postgresql&) = delete;
		postgresql& operator=(const postgresql&) = delete;
		virtual ~postgresql() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	/** @} */

	/** @name Procedures */
	/** @{ */
	protected:
        virtual std::string	generateSelectQuery(const select& query, bool isSubquery = false) const override;
		virtual std::string	generateCreateQuery(const create& query) const override;
		virtual std::string	generateDeleteQuery(const delete_query& query, bool isSubquery = false) const override;
		virtual std::string	generateDropQuery(const drop& query, bool isSubquery = false) const override;
		virtual std::string	generateInsertQuery(const insert& query, bool isSubquery = false) const override;
		virtual std::string	generateUpdateQuery(const abstract_query& query) const override {return "";};

	private:
		std::string escape_string(const std::string& input) const;
		std::string generateCTE(const abstract_query& query) const;
		std::string processCTE(const abstract_query& query) const;
		std::string generateReturning(const std::vector<std::string>& returning) const;
		std::string generateWhere(const std::vector<abstract_query::Condition>& whereConditions) const;
		std::string processFieldCreation(const abstract_field& field) const;
		std::string postProcessOutput(const std::string& out) const;


		bool isLiteral(const abstract_query::AbstractVariant& v) const;
		void printValue(std::ostream& out, const abstract_query::AbstractVariant& v) const;
		void printName(std::ostream& out, const abstract_query::AbstractVariant& v) const;
		/** @} */
	/** @name Getters */
	/** @{ */
	public:
	/** @} */
	};
}}
#endif // POSTGRESQL_HPP
