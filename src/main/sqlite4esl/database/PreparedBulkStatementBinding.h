/*
 * This file is part of sqlite4esl.
 * Copyright (C) 2020-2023 Sven Lukas
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

#ifndef SQLITE4ESL_DATABASE_PREPAREDBULKSTATEMENTBINDING_H_
#define SQLITE4ESL_DATABASE_PREPAREDBULKSTATEMENTBINDING_H_

#include <sqlite4esl/database/Connection.h>
#include <sqlite4esl/database/StatementHandle.h>

#include <esl/database/PreparedBulkStatement.h>
#include <esl/database/Column.h>
#include <esl/database/Field.h>

#include <string>
#include <vector>

namespace sqlite4esl {
inline namespace v1_6 {
namespace database {

class PreparedBulkStatementBinding : public esl::database::PreparedBulkStatement::Binding {
public:
	PreparedBulkStatementBinding(const Connection& connection, const std::string& sql);

	const std::vector<esl::database::Column>& getParameterColumns() const override;
	void execute(const std::vector<esl::database::Field>& fields) override;
	void* getNativeHandle() const override;

private:
	const Connection& connection;
	std::string sql;
	StatementHandle statementHandle;
	std::vector<esl::database::Column> parameterColumns;
};

} /* namespace database */
} /* inline namespace v1_6 */
} /* namespace sqlite4esl */

#endif /* SQLITE4ESL_DATABASE_PREPAREDBULKSTATEMENTBINDING_H_ */
