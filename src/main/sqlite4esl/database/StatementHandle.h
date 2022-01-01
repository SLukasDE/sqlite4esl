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

#ifndef SQLITE4ESL_DATABASE_STATEMENTHANDLE_H_
#define SQLITE4ESL_DATABASE_STATEMENTHANDLE_H_

#include <sqlite3.h>

namespace sqlite4esl {
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

	sqlite3_stmt& getHandle() const noexcept;

protected:
	sqlite3_stmt* handle = nullptr;
};

} /* namespace database */
} /* namespace sqlite4esl */

#endif /* SQLITE4ESL_DATABASE_STATEMENTHANDLE_H_ */
