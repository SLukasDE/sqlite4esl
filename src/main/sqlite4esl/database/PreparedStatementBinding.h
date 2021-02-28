/*
 * This file is part of sqlite4esl.
 * Copyright (C) 2021 Sven Lukas
 *
 * Sqlite4esl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sqlite4esl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser Public License
 * along with mhd4esl.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SQLITE4ESL_DATABASE_PREPAREDSTATEMENTBINDING_H_
#define SQLITE4ESL_DATABASE_PREPAREDSTATEMENTBINDING_H_

#include <sqlite4esl/database/Connection.h>
#include <sqlite4esl/database/StatementHandle.h>

#include <esl/database/PreparedStatement.h>
#include <esl/database/Column.h>
#include <esl/database/Field.h>

#include <string>
#include <vector>

#include <sqlite3.h>

namespace sqlite4esl {
namespace database {

class PreparedStatementBinding : public esl::database::PreparedStatement::Binding {
public:
	PreparedStatementBinding(const Connection& connection, const std::string& sql);

	const std::vector<esl::database::Column>& getParameterColumns() const override;
	const std::vector<esl::database::Column>& getResultColumns() const override;
	esl::database::ResultSet execute(const std::vector<esl::database::Field>& fields) override;
	void executeBulk(const std::vector<std::vector<esl::database::Field>>& fieldArrays) override;

private:
	const Connection& connection;
	std::string sql;
	StatementHandle statementHandle;
	std::vector<esl::database::Column> parameterColumns;
	std::vector<esl::database::Column> resultColumns;
};

} /* namespace database */
} /* namespace sqlite4esl */

#endif /* SQLITE4ESL_DATABASE_PREPAREDSTATEMENTBINDING_H_ */
