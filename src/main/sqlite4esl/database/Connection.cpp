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

#include <sqlite4esl/database/Connection.h>
#include <sqlite4esl/database/ConnectionFactory.h>
#include <sqlite4esl/database/Driver.h>
#include <sqlite4esl/database/PreparedStatementBinding.h>
#include <sqlite4esl/Logger.h>

#include <esl/database/PreparedStatement.h>
#include <esl/database/Diagnostic.h>
#include <esl/database/exception/SqlError.h>
#include <esl/Stacktrace.h>

#include <stdexcept>

namespace sqlite4esl {
namespace database {

namespace {
Logger logger("sqlite4esl::database::Connection");
}

Connection::Connection(ConnectionFactory& aConnectionFactory, const sqlite3& aConnectionHandle)
: connectionFactory(aConnectionFactory),
  connectionHandle(aConnectionHandle)
{
}

Connection::~Connection() {
	connectionFactory.doUnlock();
}

const sqlite3& Connection::getConnectionHandle() const {
/*
	if(connectionHandle == nullptr) {
        throw esl::addStacktrace(std::runtime_error("Calling ConnectionFactory::getConnectionHandle() but db is still not opened"));
	}
*/
	return connectionHandle;
}

esl::database::PreparedStatement Connection::prepare(const std::string& sql) const {
	return esl::database::PreparedStatement(std::unique_ptr<esl::database::PreparedStatement::Binding>(new PreparedStatementBinding(*this, sql)));
}

esl::database::ResultSet Connection::getTable(const std::string& tableName) {
	return esl::database::ResultSet();
}

void Connection::commit() const {
	esl::database::PreparedStatement preparedStatement = prepare("COMMIT;");
	preparedStatement.execute();
}

void Connection::rollback() const {
	esl::database::PreparedStatement preparedStatement = prepare("ROLLBACK;");
	preparedStatement.execute();
}

bool Connection::isClosed() const {
	return false;
	//return connectionHandle == nullptr;
}

void* Connection::getNativeHandle() const {
	return const_cast<void*>(static_cast<const void*>(&connectionHandle));
}


} /* namespace database */
} /* namespace sqlite4esl */
