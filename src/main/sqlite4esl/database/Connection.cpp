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

#include <sqlite4esl/database/Connection.h>
#include <sqlite4esl/database/ConnectionFactory.h>
#include <sqlite4esl/database/PreparedStatementBinding.h>
#include <sqlite4esl/database/PreparedBulkStatementBinding.h>

#include <esl/Logger.h>

#include <esl/database/Diagnostic.h>
#include <esl/database/exception/SqlError.h>
#include <esl/database/PreparedStatement.h>
#include <esl/system/Stacktrace.h>

#include <stdexcept>

namespace sqlite4esl {
inline namespace v1_6 {
namespace database {

namespace {
esl::Logger logger("sqlite4esl::database::Connection");

std::set<std::string> implementations{{"SQLite"}};
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

esl::database::PreparedBulkStatement Connection::prepareBulk(const std::string& sql) const {
	return esl::database::PreparedBulkStatement(std::unique_ptr<esl::database::PreparedBulkStatement::Binding>(new PreparedBulkStatementBinding(*this, sql)));
}

StatementHandle Connection::prepareSQLite(const std::string& sql) const {
	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(const_cast<sqlite3*>(&connectionHandle), sql.c_str(), sql.length() + 1, &stmt, nullptr);
	if(rc != SQLITE_OK) {
        throw esl::system::Stacktrace::add(std::runtime_error(std::string("Can't prepare SQL statement \"" + sql + "\": ") + sqlite3_errstr(rc)));
	}

	return StatementHandle(*stmt);
}

void Connection::commit() const {
	prepare("COMMIT;").execute();
}

void Connection::rollback() const {
	prepare("ROLLBACK;").execute();
}

bool Connection::isClosed() const {
	return false;
	//return connectionHandle == nullptr;
}

void* Connection::getNativeHandle() const {
	return const_cast<void*>(static_cast<const void*>(&connectionHandle));
}

const std::set<std::string>& Connection::getImplementations() const {
	return implementations;
}

} /* namespace database */
} /* inline namespace v1_6 */
} /* namespace sqlite4esl */
