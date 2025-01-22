/**
 @file PosgtreSQL.cpp
 @date 28.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#include "PostgreSQL.hpp"

#include "../Field/IntegerField.hpp"
#include "../Field/BigSignedInteger.hpp"
#include "../Field/String.hpp"

#include <sstream>

namespace chaos { namespace cdo {

	std::string postgresql::escape_string(const std::string& input) const
	{
		std::string result;
		result.reserve(input.size() * 2);

		for (char c : input) {
			if (c == '\'') {
				result += "''";
			} else {
				result.push_back(c);
			}
		}
		return result;
	}

	void postgresql::printValue(std::ostream& out,
								const std::variant<
								std::shared_ptr<abstract_field>,
								std::shared_ptr<row_set>,
								std::shared_ptr<abstract_query>,
								int, std::string>& v) const
	{
		std::visit([&](auto&& val) {
			using T = std::decay_t<decltype(val)>;
			if constexpr (std::is_same_v<T, std::shared_ptr<abstract_field>>) {
				if (!val) {
					throw std::logic_error("Field is null in WHERE condition");
				}

				if (auto big = std::dynamic_pointer_cast<big_signed_integer>(val)) {
					out << big->get_value();
					return;
				}

				if (auto si = std::dynamic_pointer_cast<signed_integer>(val)) {
					out << si->get_value();
					return;
				}

				if (auto str = std::dynamic_pointer_cast<string>(val)) {
					auto safe = escape_string(str->get_value());
					out << "'" << safe << "'";
					return;
				}
			}

			if constexpr (std::is_same_v<T, std::shared_ptr<abstract_query>>) {

				if(auto query = std::dynamic_pointer_cast<select>(val)) {
					if(!query->alias().empty())
						out << query->alias();
				}
				else out << "(" << processQuery(*val, true) << ")";
				return;
			}

			if constexpr (std::is_same_v<T, int>) {
				out << val;
				return;
			}

			if constexpr (std::is_same_v<T, std::string>) {
				auto safe = escape_string(val);
				out << "'" << safe << "'";
				return;
			}
		}, v);
	}

	void postgresql::printName(std::ostream& out,
								const std::variant<
								std::shared_ptr<abstract_field>,
								std::shared_ptr<row_set>,
								std::shared_ptr<abstract_query>,
								int, std::string>& v) const
	{
		std::visit([&](auto&& val) {
			using T = std::decay_t<decltype(val)>;
			if constexpr (std::is_same_v<T, std::shared_ptr<abstract_field>>) {
				if (!val) {
					throw std::logic_error("Field is null in WHERE condition");
				}

				if (auto field = std::dynamic_pointer_cast<abstract_field>(val)) {
					out << field->get_name();
				} else {
					out << "NULL_FIELD";
				}
			}

			if constexpr (std::is_same_v<T, std::shared_ptr<row_set>>) {
				out << val->name();
			}
		}, v);
	}


	std::string postgresql::generateReturning(const std::vector<std::string>& returning) const
	{
		std::ostringstream out;
		bool firstRetVal = true;
		if(!returning.empty()) {
			out << " RETURNING ";
			for(const auto& retValue: returning) {
				if(!firstRetVal) {
					out << "," << " ";
				}
				out << retValue;
				firstRetVal = false;
			}
		}
		return out.str();
	}

	std::string postgresql::generateWhere(const std::vector<abstract_query::Condition>& whereConditions) const
	{
		std::ostringstream out;
		if (!whereConditions.empty()) {
			out << " WHERE (";
			bool firstCondition = true;
			for (const auto& cond : whereConditions) {
				if (!firstCondition) {
					out << " AND ";
				}
				printName(out, cond.left_field);
				out << " " << abstract_query::to_string(cond.op) << " ";
				printValue(out, cond.right_value);
				firstCondition = false;
			}
			out << ")";
		}
		return out.str();
	}

	std::string postgresql::processCTE(const abstract_query::cte_info& cte, int index) const
	{
		std::ostringstream out;



		return out.str();

	}


	std::string postgresql::generateCTE(const abstract_query& query) const
	{
		std::ostringstream out;

		const auto with_queries = query.with_queries();
		if(!with_queries.empty()) {
			for(auto i = 0; i < with_queries.size(); i++) {

				if(with_queries[i].anchor->has_modifier(abstract_query::QueryModifiers::RECURSIVE))
					out << generateCTE(*with_queries[i].anchor);
				else {
					out << (with_queries[i].alias.empty() ? std::string("cte") + std::to_string(i) : with_queries[i].alias);
					out << " AS (";
					out << processQuery(*with_queries[i].anchor, true);
					if(with_queries[i].is_recursive) {
						out << (with_queries[i].union_type == abstract_query::QueryUnionType::Union ? " UNION " : " UNION ALL ");
						out << processQuery(*with_queries[i].recursive, true);
					}
					out << ")";
				}

				if(i + 1 < with_queries.size()) {
					out << ", ";
				}
			}
		}

		return out.str();
	}

    std::string	postgresql::generateSelectQuery(const select& query, bool isSubquery) const
	{
		std::ostringstream out;

		const auto with_queries = query.with_queries();
		if(!with_queries.empty()) {
			out << "WITH ";
			if(std::any_of(with_queries.begin(), with_queries.end(),[] (const abstract_query::cte_info& cte) {return
					cte.is_recursive ||
					cte.anchor->has_modifier(abstract_query::QueryModifiers::RECURSIVE);}))
				out << "RECURSIVE ";
		}

		out << generateCTE(query);

		auto fields = query.selectable_fields();
		if(!isSubquery){
			out << " ";
		}
		out << "SELECT ";
		if (query.has_modifier(abstract_query::QueryModifiers::DISTINCT)) {
			out << "DISTINCT ";
		}
		if (!fields.empty()) {
			for (size_t i = 0; i < fields.size(); ++i) {
				out << (fields[i]->alias().empty() ? "" : fields[i]->alias() + " AS ");
				out << fields[i]->get_name();
				if (i + 1 < fields.size()) {
					out << ", ";
				}
			}
		} else {
			out << "*";
		}

		auto from_tables = query.from_tables();
		auto from_subqueries = query.from_subqueries();
		if (!from_tables.empty() || !from_subqueries.empty()) {
			out << " " << "FROM ";
			bool needComma = false;

			for (const auto& i : from_tables) {
				if (needComma) {
					out << ", ";
				}
				out << i->name();
				needComma = true;
			}

			for (size_t i = 0; i < from_subqueries.size(); ++i) {
				if (needComma) {
					out << ", ";
				}
				auto subSel = std::dynamic_pointer_cast<select>(from_subqueries[i]);
				if (subSel) {
					auto it = std::find_if(with_queries.begin(), with_queries.end(), [&subSel](const abstract_query::cte_info& query){
						auto existing = std::dynamic_pointer_cast<select>(query.anchor);
						return existing && *existing == *subSel;
					});
					if (it != with_queries.end()) {
						size_t cteIndex = std::distance(with_queries.begin(), it);
						out << "cte" << cteIndex;
					}
					else {
						out << "(" << generateSelectQuery(*subSel, true) << ") AS sub" << i;
					}
				} else {
					out << "(/* unknown or non-select subquery */) AS sub" << i;
				}
				needComma = true;
			}
		}

		auto joins = query.joins();
		for (size_t j = 0; j < joins.size(); ++j) {
			const auto& info = joins[j];

			out << " " << abstract_query::to_string(info.join_type) << " ";
			printValue(out, info.joined_rs);
			out << " ";

			if (!info.on_conditions.empty()) {
				out << "ON ";

				bool multipleConds = (info.on_conditions.size() > 1);
				if (multipleConds) {
					out << "(";
				}

				for (size_t c = 0; c < info.on_conditions.size(); ++c) {
					const auto& cond = info.on_conditions[c];
					printValue(out, cond.left_field);
					out << " " << abstract_query::to_string(cond.op) << " ";
					printValue(out, cond.right_value);

					if (c + 1 < info.on_conditions.size()) {
						out << " AND ";
					}
				}

				if (multipleConds) {
					out << ")";
				}
			}
		}

		auto where_conditions = query.where_conditions();
		out << generateWhere(where_conditions);

		auto group_by_conditions = query.groupBy();
		if (!group_by_conditions.empty()) {

			out << " " << "GROUP BY ";
			bool first = true;
			for (auto& fld : group_by_conditions) {
				if (!first) {
					out << ", ";
				}
				first = false;
				out << fld->get_name();
			}
		}

		auto order_by = query.orderBy();
		if (!order_by.empty()) {
			out << " " << "ORDER BY " << order_by;
		}

		auto unions = query.unions();
		if(!unions.empty()) {
			for(const auto& union_: unions) {
				out << (union_.second == abstract_query::QueryUnionType::Union ? " UNION " : " UNION ALL ");
				out << processQuery(*union_.first, true);
			}
		}

        if(!isSubquery) {
            out << ";";
        }

		return out.str();
	}

	std::string	postgresql::generateCreateQuery(const create& query) const
	{
		std::ostringstream out;

		out << "CREATE TABLE ";
		if(query.use_if_not_exists()) {
			out << "IF NOT EXISTS ";
		}

		out << query.table_name() << " (";

		const auto cols = query.columns_list();
		bool needComma = false;
		for(const auto& column: cols) {
			if(needComma) {
				out << ", ";
			}

			out << column->to_SQL();
			needComma = true;
		}

		const auto pKeys = query.primary_keys();
		if (!pKeys.empty()) {
			out << ", PRIMARY KEY (";
			for (const auto& key: pKeys) {
				out << key;
				if (key != pKeys.back()) {
					out << ", ";
				}
			}
			out << ")";
		}

		const auto fKeys = query.foreign_keys();
		for (const auto& fk : fKeys) {
			out << ", ";

			if (fk.constraint_name.empty()) {
				throw std::logic_error("FOREIGN_KEY_NAME CANNOT be empty if key EXISTS!");
			}
			out << "CONSTRAINT " << fk.constraint_name << " ";

			out << "FOREIGN KEY (";
			for (const auto& fkCols: fk.columns) {
				out << fkCols;
				if (fkCols != fk.columns.back()) {
					out << ", ";
				}
			}

			out << ") REFERENCES " << fk.ref_table << "(";
			for (const auto& fkRef: fk.ref_columns) {
				out << fkRef;
				if (fkRef != fk.ref_columns.back()) {
					out << ", ";
				}
			}

			out << ")";

			if (fk.on_update.empty()) {
				out << ";";
				return out.str();
			}
			else out << " ON UPDATE " << fk.on_update;

			if (fk.on_delete.empty()) {
				out << ");";
				return out.str();
			}
			else out << " ON DELETE " << fk.on_delete;
		}

		out << ");";
		return out.str();
	}

	std::string	postgresql::generateDeleteQuery(const delete_query &query, bool isSubquery) const
	{
		std::ostringstream out;

		out << generateCTE(query);

		out << "DELETE FROM ";

		if(query.table_name().empty())
			throw std::logic_error("table to drop name CANNOT be empty at DROP TABLE query!");

		out << query.table_name();

		auto where_conditions = query.where_conditions();
		out << generateWhere(where_conditions);

		auto returning = query.returning_list();
		out << generateReturning(returning);

		if(!isSubquery) {
			out << ";";
		}
		return out.str();
	}

	std::string	postgresql::generateDropQuery(const drop &query, bool isSubquery) const
	{
		std::ostringstream out;

		out << generateCTE(query);

		out << "DROP TABLE ";
		if (query.use_if_exists()) {
			out << "IF EXISTS ";
		}

		if(query.table_name().empty())
			throw std::logic_error("table to drop name CANNOT be empty at DROP TABLE query!");

		out << query.table_name();

		if(!isSubquery) {
			out << ";";
		}
		return out.str();
	}

	std::string	postgresql::generateInsertQuery(const insert& query, bool isSubquery) const
	{
		std::ostringstream out;
		out << generateCTE(query);
		out << "INSERT INTO " << query.table_name();

		const auto fields = query.columns_list();
		if(fields.empty()) {
			throw std::logic_error("INSERTABLE FIELDS CANNOT BE EMPTY!");
		}

		out << " (";
		for(const auto& field: fields) {
			out << field;
			if (field != fields.back()) {
				out << ", ";
			}
		}
		out << ")";

		const auto rows = query.rows();
		if(rows.empty()) {
			out << "DEFAULT VALUES";
			return out.str();
		}

		out << " VALUES ";
		bool isFirstRow = true;
		for(const auto& row: rows) {
			if(!isFirstRow) {
				out << "," << " ";
			}
			out << "(";
			bool isFirstValue = true;
			for(const auto& item: row) {
				if(!isFirstValue) {
					out << "," << " ";
				}

				printValue(out, item);
				isFirstValue = false;
			}
			out << ")";
			isFirstRow = false;
		}

		auto returning = query.returning_list();
		out << generateReturning(returning);

		if(!isSubquery) {
			out << ";";
		}
		return out.str();
	}
}}







