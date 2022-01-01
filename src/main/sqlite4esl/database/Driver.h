/*
 * This file is part of sqlite4esl.
 * Copyright (C) 2020-2022 Sven Lukas
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

#ifndef SQLITE4ESL_DATABASE_DRIVER_H_
#define SQLITE4ESL_DATABASE_DRIVER_H_

#include <esl/database/Column.h>
#include <esl/database/Diagnostic.h>

#include <sqlite3.h>

#include <string>

namespace sqlite4esl {
namespace database {

class ConnectionFactory;
class Connection;
class StatementHandle;

class Driver {
public:
	static const Driver& getDriver();

	bool isThreadsafe() const;
	sqlite3& open(const std::string connectionString) const;
	void close(const ConnectionFactory& connectionFactory) const;
	StatementHandle prepare(const Connection& connection, const std::string& sql) const;
	void finalize(StatementHandle& statementHandle) const;
	int step(StatementHandle& statementHandle) const;
	void reset(StatementHandle& statementHandle) const;

	std::size_t columnCount(StatementHandle& statementHandle) const;
	std::string columnName(StatementHandle& statementHandle, std::size_t index) const;
	std::string columnDeclType(StatementHandle& statementHandle, std::size_t index) const;

	esl::database::Column::Type columnType(StatementHandle& statementHandle, std::size_t index) const;
	bool columnValueIsNull(StatementHandle& statementHandle, std::size_t index) const;
	int columnInteger(StatementHandle& statementHandle, std::size_t index) const;
	double columnDouble(StatementHandle& statementHandle, std::size_t index) const;
	std::string columnText(StatementHandle& statementHandle, std::size_t index) const;
	std::string columnBlob(StatementHandle& statementHandle, std::size_t index) const;

	std::size_t bindParameterCount(StatementHandle& statementHandle) const;
	void bindNull(StatementHandle& statementHandle, std::size_t index) const;
	void bindInteger(StatementHandle& statementHandle, std::size_t index, int value) const;
	void bindDouble(StatementHandle& statementHandle, std::size_t index, double value) const;
	void bindText(StatementHandle& statementHandle, std::size_t index, const std::string& value) const;
	void bindBlob(StatementHandle& statementHandle, std::size_t index, const std::string& value) const;
};

} /* namespace database */
} /* namespace sqlite4esl */

#endif /* SQLITE4ESL_DATABASE_DRIVER_H_ */
