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

#ifndef SQLITE4ESL_DATABASE_STATEMENTHANDLE_H_
#define SQLITE4ESL_DATABASE_STATEMENTHANDLE_H_

#include <esl/database/Column.h>

#include <sqlite3.h>

#include <cstdint>
#include <string>

namespace sqlite4esl {
inline namespace v1_6 {
namespace database {

class StatementHandle {
public:
	StatementHandle() = default;
	StatementHandle(const StatementHandle&) = delete;
	StatementHandle(StatementHandle&& statementHandle);
	StatementHandle(sqlite3_stmt& handle);

	~StatementHandle();

	StatementHandle& operator=(const StatementHandle&) = delete;
	StatementHandle& operator=(StatementHandle&& other);

	explicit operator bool() const noexcept;

	bool step() const;
	void reset() const;
	std::size_t columnCount() const;
	std::string columnName(std::size_t index) const;
	std::string columnDeclType(std::size_t index) const;

	esl::database::Column::Type columnType(std::size_t index) const;
	bool columnValueIsNull(std::size_t index) const;
	std::int64_t columnInteger(std::size_t index) const;
	double columnDouble(std::size_t index) const;
	std::string columnText(std::size_t index) const;
	std::string columnBlob(std::size_t index) const;

	std::size_t bindParameterCount() const;
	void bindNull(std::size_t index) const;
	void bindInteger(std::size_t index, std::int64_t value) const;
	void bindDouble(std::size_t index, double value) const;
	void bindText(std::size_t index, const std::string& value) const;
	void bindBlob(std::size_t index, const std::string& value) const;

	sqlite3_stmt& getHandle() const;

protected:
	sqlite3_stmt* handle = nullptr;
};

} /* namespace database */
} /* inline namespace v1_6 */
} /* namespace sqlite4esl */

#endif /* SQLITE4ESL_DATABASE_STATEMENTHANDLE_H_ */
