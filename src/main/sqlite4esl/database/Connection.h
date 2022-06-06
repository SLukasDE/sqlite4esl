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

#ifndef SQLITE4ESL_DATABASE_CONNECTION_H_
#define SQLITE4ESL_DATABASE_CONNECTION_H_

#include <esl/database/Connection.h>
#include <esl/database/PreparedStatement.h>
#include <esl/database/PreparedBulkStatement.h>
//#include <esl/database/ResultSet.h>
#include <esl/object/Implementations.h>

#include <memory>
#include <set>
#include <string>
#include <vector>

#include <sqlite3.h>

namespace sqlite4esl {
namespace database {

class ConnectionFactory;

class Connection : public esl::database::Connection {
public:
	Connection(ConnectionFactory& connectionFactory, const sqlite3& connectionHandle);
	~Connection();

	const sqlite3& getConnectionHandle() const;

	esl::database::PreparedStatement prepare(const std::string& sql) const override;
	esl::database::PreparedBulkStatement prepareBulk(const std::string& sql) const override;
	//esl::database::ResultSet getTable(const std::string& tableName);

	void commit() const override;
	void rollback() const override;
	bool isClosed() const override;

	void* getNativeHandle() const override;

	const std::set<std::string>& getImplementations() const override;

private:
	ConnectionFactory& connectionFactory;
	const sqlite3& connectionHandle;
	//sqlite3* connectionHandle = nullptr;
};

} /* namespace database */
} /* namespace sqlite4esl */

#endif /* SQLITE4ESL_DATABASE_CONNECTION_H_ */
