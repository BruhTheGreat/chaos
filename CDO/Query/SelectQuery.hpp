/**
 @file SelectQuery.hpp
 @date 25.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#ifndef Chaos_CDO_Select_Query_hpp
#define Chaos_CDO_Select_Query_hpp

#include "../AbstractQuery.hpp"
#include "../Table.hpp"
#include "../View.hpp"

#include <string>
#include <memory>
#include <vector>

namespace chaos { namespace cdo {
	class select : public abstract_query
	{
	/** @name Classes */
	/** @{ */
	/** @} */
	/** @name Constructors */
	/** @{ */
	public:
		select() = default;
		select(const select&) = default;
		select& operator=(const select&) = default;
		select(select&&) = default;
		select& operator=(select&&) = default;

		bool operator==(const select& other) const;

		virtual ~select() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		// SELECTABLE FIELDS
		std::vector<std::shared_ptr<abstract_field>> _selectable_fields;

		// FROM
		std::vector<std::shared_ptr<row_set>> _from_tables;
		std::vector<std::shared_ptr<abstract_query>> _from_subqueries;

		// GROUP BY
		std::vector<std::shared_ptr<abstract_field>> _group_by_fields;

		// ORDER BY
		std::string _order_by;

	/** @} */

	/** @name Procedures */
	/** @{ */
	private:
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		select& as(const std::string& name);
		select& asAlias(const std::string& alias);

		select& with(const abstract_query& anchor, const std::string& alias = "");
		select& with(const abstract_query& anchor, const abstract_query& reqursive, const std::string& alias = "", QueryUnionType type = QueryUnionType::UnionAll);

		select& fields(std::shared_ptr<abstract_field> field);
		select& fields(const std::vector<std::shared_ptr<abstract_field>>& fields);
		select& fields(std::initializer_list<std::shared_ptr<abstract_field>> fields);

		select& from(const table& t);
		select& from(const view& v);
		select& from(const abstract_query& subq);

		select& where(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		select& where(std::shared_ptr<abstract_field> left, ECompareOp op, const abstract_query& rightVal);
		select& where(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal);
		select& where(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal);

		select& where(const abstract_query& left, ECompareOp op, const abstract_query& rightVal);
		select& where(const abstract_query& left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		select& where(const abstract_query& left, ECompareOp op, int rightVal);
		select& where(const abstract_query& left, ECompareOp op, const std::string& rightVal);

		select& and_(std::shared_ptr<abstract_field> left, ECompareOp op, const abstract_query& rightVal);
		select& and_(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		select& and_(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal);
		select& and_(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal);

		select& and_(const abstract_query& left, ECompareOp op, const abstract_query& rightVal);
		select& and_(const abstract_query& left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		select& and_(const abstract_query& left, ECompareOp op, int rightVal);
		select& and_(const abstract_query& left, ECompareOp op, const std::string& rightVal);

		select& or_(std::shared_ptr<abstract_field> left, ECompareOp op, const abstract_query& rightVal);
		select& or_(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		select& or_(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal);
		select& or_(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal);

		select& or_(const abstract_query& left, ECompareOp op, const abstract_query& rightVal);
		select& or_(const abstract_query& left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		select& or_(const abstract_query& left, ECompareOp op, int rightVal);
		select& or_(const abstract_query& left, ECompareOp op, const std::string& rightVal);


		select& distinct(bool state);
		select& recursive(bool state);


		select& join_inner(const row_set& rs);
		select& join_left(const row_set& rs);
		select& join_right(const row_set& rs);
		select& join_full(const row_set& rs);
		select& join_cross(const row_set& rs);
		select& join_natural(const row_set& rs);
		select& join_left_exclusion(const row_set& rs);
		select& join_right_exclusion(const row_set& rs);
		select& join_full_exclusion(const row_set& rs);
		select& join_self(const row_set& rs);

		select& join_inner(const abstract_query& query);
		select& join_left(const abstract_query& query);
		select& join_right(const abstract_query& query);
		select& join_full(const abstract_query& query);
		select& join_cross(const abstract_query& query);
		select& join_natural(const abstract_query& query);
		select& join_left_exclusion(const abstract_query& query);
		select& join_right_exclusion(const abstract_query& query);
		select& join_full_exclusion(const abstract_query& query);
		select& join_self(const abstract_query& query);


		select& on(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> right);

		select& order(std::shared_ptr<abstract_field> field, bool ascending = true);
		select& group(std::shared_ptr<abstract_field> field);

		select& union_(const abstract_query& query, const QueryUnionType& type);
		select& union_(std::shared_ptr<abstract_query> query, const QueryUnionType& type);
	private:
		select& join(const row_set& rs, EJoinType type);
		select& join(const abstract_query& query, EJoinType type);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:

		bool distinct() const {return has_modifier(QueryModifiers::DISTINCT);};
		bool recursive() const {return has_modifier(QueryModifiers::RECURSIVE);};
		std::vector<std::shared_ptr<abstract_field>> selectable_fields() const {return _selectable_fields;};
		std::vector<std::shared_ptr<row_set>> from_tables() const {return _from_tables;};
		std::vector<std::shared_ptr<abstract_query>> from_subqueries() const {return _from_subqueries;};
		std::vector<JoinInfo> joins() const {return _joins;};
		std::vector<Condition> where_conditions() const {return _where_conditions;};
		std::vector<std::shared_ptr<abstract_field>> groupBy() const {return _group_by_fields;};

		std::string orderBy() const {return _order_by;};
	/** @} */
	};
}}

#endif
