/**
 @file AbstractQuery.hpp
 @date 25.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
 */

#ifndef Chaos_CDO_Abstract_Query_hpp
#define Chaos_CDO_Abstract_Query_hpp

#include "AbstractField.hpp"
#include "RowSet.hpp"

#include <memory>
#include <unordered_set>
#include <vector>
#include <string>

namespace chaos { namespace cdo {

	class select;
	class create;
	class insert;
	class delete_query;
	class drop;
	class abstract_query;

	class abstract_query
	{
	/** @name Aliases */
	/** @{ */
	public:
		using AbstractVariant = std::variant<std::shared_ptr<abstract_field>, std::shared_ptr<row_set>, std::shared_ptr<abstract_query>, int, std::string>;
	/** @} */
	/** @name Classes */
	/** @{ */
	public:
		enum class ECompareOp {
			Equal = 0,
			NotEqual,
			Greater,
			Less,
			GreaterOrEqual,
			LessOrEqual,
			IN,
			NOT_IN,
			NOT_LIKE
		};

		enum class EJoinType {
			Inner,			// Внутреннее соединение
			Left,			// Левое соединение
			Right,			// Правое соединение
			Full,			// Полное внешнее соединение
			Cross,			// Декартово произведение
			Natural,		// Естественное соединение
			LeftExclusion,  // Только строки из левой таблицы, которых нет в правой
			RightExclusion, // Только строки из правой таблицы, которых нет в левой
			FullExclusion,  // Только строки без пересечений (из обеих таблиц)
			SelfJoin        // Соединение таблицы с самой собой
		};

		enum class ELogicOp {
			AND_,
			OR_,
			NOTHING
		};

		inline static std::string to_string(const ECompareOp& op)
		{
			switch (op)
			{
			case ECompareOp::Equal:           return "=";
			case ECompareOp::NotEqual:        return "<>";
			case ECompareOp::Greater:         return ">";
			case ECompareOp::Less:            return "<";
			case ECompareOp::GreaterOrEqual:  return ">=";
			case ECompareOp::LessOrEqual:     return "<=";
			case ECompareOp::IN:			  return "IN";
			case ECompareOp::NOT_IN:		  return "NOT IN";
			case ECompareOp::NOT_LIKE:		  return "NOT LIKE";}
			return "";
		}
		inline static std::string to_string(const EJoinType& jt)
		{
			switch (jt)
			{
			case EJoinType::Inner:           return "INNER JOIN";
			case EJoinType::Left:            return "LEFT JOIN";
			case EJoinType::Right:           return "RIGHT JOIN";
			case EJoinType::Full:            return "FULL JOIN";
			case EJoinType::Cross:           return "CROSS JOIN";
			case EJoinType::Natural:         return "NATURAL JOIN";
			case EJoinType::LeftExclusion:   return "LEFT EXCLUSION JOIN";
			case EJoinType::RightExclusion:  return "RIGHT EXCLUSION JOIN";
			case EJoinType::FullExclusion:   return "FULL EXCLUSION JOIN";
			case EJoinType::SelfJoin:        return "SELF JOIN";
			}
			return "";
		}
		inline static std::string to_string(const ELogicOp& jt)
		{
			switch (jt)
			{
			case ELogicOp::AND_:           return "AND";
			case ELogicOp::OR_:            return "OR";
			case ELogicOp::NOTHING:        return "";
			default: return "";
			}
			return "";
		}

		/**
		* @brief Структура для хранения одного условия WHERE
		*/
		struct Condition {
			AbstractVariant left_field;
			ECompareOp op;
			AbstractVariant right_value;
			ELogicOp logicOp = ELogicOp::NOTHING;
			bool operator==(const Condition& rhs) const
			{
				return left_field == rhs.left_field &&
					   op == rhs.op &&
					   right_value == rhs.right_value &&
					   logicOp == rhs.logicOp;
			}

			bool operator!=(const abstract_query::Condition& rhs) const
			{
				return !(*this == rhs);
			}
		};

		struct JoinInfo {
			AbstractVariant joined_rs;
			EJoinType join_type;
			std::vector<Condition> on_conditions;

			bool operator==(const JoinInfo& rhs) const
			{
				return joined_rs == rhs.joined_rs &&
					   join_type == rhs.join_type &&
					   on_conditions == rhs.on_conditions;
			}

		};

		enum class QueryModifiers {
			DISTINCT,
			RECURSIVE
		};

		enum class QueryUnionType {
			Union,
			UnionAll
		};

		struct cte_info {
			std::string alias = "";
			bool is_recursive = false;
			std::shared_ptr<abstract_query> anchor = nullptr;
			std::shared_ptr<abstract_query> recursive = nullptr;
			QueryUnionType union_type;
			bool operator==(const cte_info& rhs) const {
				return alias == rhs.alias &&
						is_recursive == rhs.is_recursive &&
						anchor == rhs.anchor &&
						recursive == rhs.recursive &&
						union_type == rhs.union_type;
			}
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		abstract_query() = default;

		virtual ~abstract_query() = 0;
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		std::string _alias = "";
		std::string _name = "";
		std::vector<cte_info> _with_queries; // CTEs
		std::vector<Condition> _where_conditions; // WHERE statements
		std::vector<std::string> _returning; // RETURNING for INSERT, UPDATE, DELETE
		std::vector<std::pair<std::shared_ptr<abstract_query>, QueryUnionType>> _unions; // for uniting subqueries
		std::vector<JoinInfo> _joins; // to process JOINs in all queries
		std::unordered_set<QueryModifiers> _modifiers; // UNION modifiers
	/** @} */

	/** @name Setters */
	/** @{ */
	protected:
		void add_join(const AbstractVariant & joinable, EJoinType type) {_joins.push_back({joinable, type, {}});}
		void add_cte(const abstract_query& anchor, const std::string& alias = "");
		void add_cte(const abstract_query& anchor, const abstract_query& recursive, const std::string& alias = "", QueryUnionType type = QueryUnionType::UnionAll);
		void add_modifier(const QueryModifiers& modifier) {if(!has_modifier(modifier)) { _modifiers.insert(modifier);};}
		void add_union(const std::shared_ptr<abstract_query>& query, QueryUnionType type) { _unions.emplace_back(query, type);}
		void add_where_condition(const Condition& condition) {_where_conditions.push_back(condition);} // add check on null ptrs
		void remove_modifier(const QueryModifiers& modifier) { if(has_modifier(modifier)) _modifiers.erase(modifier);}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		inline std::string alias() const {return _alias;};
		inline std::string name() const {return _name;};
		inline bool has_modifier(const QueryModifiers& modifier) const { return _modifiers.find(modifier) != _modifiers.end();}
		inline std::vector<cte_info> with_queries() const {return _with_queries;};
		inline std::vector<Condition> where_conditions() const { return _where_conditions; }
		inline std::vector<std::string> returning_list() const {return _returning;};
		inline std::vector<std::pair<std::shared_ptr<abstract_query>, QueryUnionType>> unions() const {return _unions;}
		inline std::vector<JoinInfo> get_joins() const {return _joins;}
		inline std::unordered_set<QueryModifiers> get_modifiers() const { return _modifiers;}

	protected:
		std::shared_ptr<abstract_query> copy(const abstract_query& src);

	/** @} */
	};
}}
#endif
