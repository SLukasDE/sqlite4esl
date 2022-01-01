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

#include <sqlite4esl/database/Driver.h>
#include <sqlite4esl/database/ConnectionFactory.h>
#include <sqlite4esl/database/Connection.h>
#include <sqlite4esl/database/StatementHandle.h>
#include <sqlite4esl/Logger.h>

#include <esl/database/exception/SqlError.h>
#include <esl/logging/Location.h>
#include <esl/Stacktrace.h>

#include <cstring>
#include <memory>

namespace sqlite4esl {
namespace database {

namespace {
Logger logger("sqlite4esl::database::Driver");
}

const Driver& Driver::getDriver() {
	static std::unique_ptr<Driver> driver;

	if(!driver) {
		driver.reset(new Driver);
	}

	return *driver;
}

bool Driver::isThreadsafe() const {
	return(sqlite3_threadsafe() != 0);
}

sqlite3& Driver::open(const std::string connectionString) const {
	sqlite3* db = nullptr;

	int rc = sqlite3_open_v2(connectionString.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI, nullptr);

	if(db == nullptr) {
		throw esl::addStacktrace(std::runtime_error("SQLite is unable to allocate memory to open database \"" + connectionString + "\""));
	}

	if(rc != SQLITE_OK) {
		std::string message = "Can't open database \"" + connectionString + "\": " + sqlite3_errmsg(db);
		sqlite3_close(db);

        throw esl::addStacktrace(std::runtime_error(message));
	}

	rc = sqlite3_extended_result_codes(db, 1);
	if(rc != SQLITE_OK) {
		std::string message = std::string("Can't enable extended result codes: ") + sqlite3_errmsg(db);
		sqlite3_close(db);

        throw esl::addStacktrace(std::runtime_error(message));
	}

	return *db;
}

void Driver::close(const ConnectionFactory& connectionFactory) const {
	int rc = sqlite3_close(const_cast<sqlite3*>(&connectionFactory.getConnectionHandle()));
	if(rc != SQLITE_OK) {
		logger.warn << "sqlite3_close(...) returned " << rc << ": " << sqlite3_errstr(rc) << "\n";
		logger.warn << "Trying to close connection with sqlite3_close_v2(...) ...\n";
		rc = sqlite3_close_v2(const_cast<sqlite3*>(&connectionFactory.getConnectionHandle()));
		if(rc != SQLITE_OK) {
	        throw esl::addStacktrace(std::runtime_error(std::string("Cannot close database connection: ") + sqlite3_errstr(rc)));
		}
	}
}

StatementHandle Driver::prepare(const Connection& connection, const std::string& sql) const {
	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(const_cast<sqlite3*>(&connection.getConnectionHandle()), sql.c_str(), sql.length() + 1, &stmt, nullptr);
	if(rc != SQLITE_OK) {
        throw esl::addStacktrace(std::runtime_error(std::string("Can't prepare SQL statement \"" + sql + "\": ") + sqlite3_errstr(rc)));
	}

	return StatementHandle(*stmt);
}

void Driver::finalize(StatementHandle& statementHandle) const {
	int rc = sqlite3_finalize(&statementHandle.getHandle());
	if(rc != SQLITE_OK) {
        throw esl::addStacktrace(std::runtime_error(std::string("Can't close statement handle: ") + sqlite3_errstr(rc)));
	}
}

int Driver::step(StatementHandle& statementHandle) const {
	int rc = sqlite3_step(&statementHandle.getHandle());

	switch(rc) {
	case SQLITE_DONE:
	case SQLITE_ROW:
		break;
	case SQLITE_BUSY:
        throw esl::addStacktrace(std::runtime_error(std::string("Cannot fetch, because sqlite3_step returned SQLITE_BUSY: ") + sqlite3_errstr(rc)));
	case SQLITE_MISUSE:
        throw esl::addStacktrace(std::runtime_error(std::string("Cannot fetch, because sqlite3_step returned SQLITE_MISUSE: ") + sqlite3_errstr(rc)));
	case SQLITE_ERROR:
        throw esl::addStacktrace(std::runtime_error(std::string("Cannot fetch, because sqlite3_step returned SQLITE_ERROR: ") + sqlite3_errstr(rc)));
	default:
        throw esl::addStacktrace(std::runtime_error("Cannot fetch, because sqlite3_step returned " + std::to_string(rc) + ": " + sqlite3_errstr(rc)));
	}

	return rc == SQLITE_ROW;
}

void Driver::reset(StatementHandle& statementHandle) const {
	int rc = sqlite3_reset(&statementHandle.getHandle());

	if(rc != SQLITE_OK) {
        throw esl::addStacktrace(std::runtime_error(std::string("Can't reset statement handle: ") + sqlite3_errstr(rc)));
	}
}

std::size_t Driver::columnCount(StatementHandle& statementHandle) const {
	int count = sqlite3_column_count(&statementHandle.getHandle());
	if(count < 0) {
        throw esl::addStacktrace(std::runtime_error("sqlite3_column_count returned a negative value: " + std::to_string(count)));
	}

	return static_cast<std::size_t>(count);
}

std::string Driver::columnName(StatementHandle& statementHandle, std::size_t index) const {
	const char* name = sqlite3_column_name(&statementHandle.getHandle(), static_cast<int>(index));
	if(name == nullptr) {
        throw esl::addStacktrace(std::runtime_error("sqlite3_column_name returned a null pointer for index " + std::to_string(index)));
	}

	return name;
}

std::string Driver::columnDeclType(StatementHandle& statementHandle, std::size_t index) const {
	const char* declType = sqlite3_column_decltype(&statementHandle.getHandle(), static_cast<int>(index));
	return declType ? declType : "";
}

esl::database::Column::Type Driver::columnType(StatementHandle& statementHandle, std::size_t index) const {
	int rc = sqlite3_column_type(&statementHandle.getHandle(), static_cast<int>(index));
	switch(rc) {
	case SQLITE_INTEGER:
		return esl::database::Column::Type::sqlInteger;
	case SQLITE_FLOAT:
		return esl::database::Column::Type::sqlDouble;
	case SQLITE_TEXT:
	case SQLITE_BLOB:
		return esl::database::Column::Type::sqlVarChar;
	case SQLITE_NULL:
		break;
	default:
		break;
	}
	return esl::database::Column::Type::sqlUnknown;
}

bool Driver::columnValueIsNull(StatementHandle& statementHandle, std::size_t index) const {
	int rc = sqlite3_column_type(&statementHandle.getHandle(), static_cast<int>(index));
	return rc == SQLITE_NULL;
}

int Driver::columnInteger(StatementHandle& statementHandle, std::size_t index) const {
	return sqlite3_column_int(&statementHandle.getHandle(), static_cast<int>(index));
}

double Driver::columnDouble(StatementHandle& statementHandle, std::size_t index) const {
	return sqlite3_column_double(&statementHandle.getHandle(), static_cast<int>(index));
}

std::string Driver::columnText(StatementHandle& statementHandle, std::size_t index) const {
	//const char* data = static_cast<const char*>(sqlite3_column_text(&statementHandle.getHandle(), static_cast<int>(index)));
	const char* data = reinterpret_cast<const char*>(sqlite3_column_text(&statementHandle.getHandle(), static_cast<int>(index)));
	if(data == nullptr) {
        throw esl::addStacktrace(std::runtime_error("sqlite3_column_text returned a null pointer"));
	}

	int length = sqlite3_column_bytes(&statementHandle.getHandle(), static_cast<int>(index));
	if(length < 0) {
        throw esl::addStacktrace(std::runtime_error("sqlite3_column_bytes returned a negative value: " + std::to_string(length)));
	}

	return std::string(data, static_cast<std::size_t>(length));
}

std::string Driver::columnBlob(StatementHandle& statementHandle, std::size_t index) const {
	const char* data = static_cast<const char*>(sqlite3_column_blob(&statementHandle.getHandle(), static_cast<int>(index)));
	if(data == nullptr) {
		return "";
	}

	int length = sqlite3_column_bytes(&statementHandle.getHandle(), static_cast<int>(index));
	if(length < 0) {
        throw esl::addStacktrace(std::runtime_error("sqlite3_column_bytes returned a negative value: " + std::to_string(length)));
	}

	return std::string(data, static_cast<std::size_t>(length));
}

std::size_t Driver::bindParameterCount(StatementHandle& statementHandle) const {
	int count = sqlite3_bind_parameter_count(&statementHandle.getHandle());
	if(count < 0) {
        throw esl::addStacktrace(std::runtime_error("sqlite3_bind_parameter_count returned a negativ value: " + std::to_string(count)));
	}

	return static_cast<std::size_t>(count);
}

void Driver::bindNull(StatementHandle& statementHandle, std::size_t index) const {
	int rc = sqlite3_bind_null(&statementHandle.getHandle(), static_cast<int>(index));

	if(rc != SQLITE_OK) {
		std::string message = "Cannot bind null value to parameter[" + std::to_string(index+1) + "]: " + sqlite3_errstr(rc);

        throw esl::addStacktrace(std::runtime_error(message));
	}
}

void Driver::bindInteger(StatementHandle& statementHandle, std::size_t index, int value) const {
	int rc = sqlite3_bind_int(&statementHandle.getHandle(), static_cast<int>(index+1), value);

	if(rc != SQLITE_OK) {
		std::string message = "Cannot bind integer value " + std::to_string(value) + " to parameter[" + std::to_string(index) + "]: " + sqlite3_errstr(rc);

        throw esl::addStacktrace(std::runtime_error(message));
	}
}

void Driver::bindDouble(StatementHandle& statementHandle, std::size_t index, double value) const {
	int rc = sqlite3_bind_double(&statementHandle.getHandle(), static_cast<int>(index+1), value);

	if(rc != SQLITE_OK) {
		std::string message = "Cannot bind double value " + std::to_string(value) + " to parameter[" + std::to_string(index) + "]: " + sqlite3_errstr(rc);

        throw esl::addStacktrace(std::runtime_error(message));
	}
}

void Driver::bindText(StatementHandle& statementHandle, std::size_t index, const std::string& value) const {
	const char* str = value.c_str();
	std::size_t length = std::strlen(str);

	int rc = sqlite3_bind_text(&statementHandle.getHandle(), static_cast<int>(index+1), str, static_cast<int>(length), SQLITE_TRANSIENT);

	if(rc != SQLITE_OK) {
		std::string message = "Cannot bind text value \"" + value + "\" to parameter[" + std::to_string(index) + "]: " + sqlite3_errstr(rc);

        throw esl::addStacktrace(std::runtime_error(message));
	}
}

void Driver::bindBlob(StatementHandle& statementHandle, std::size_t index, const std::string& value) const {
	int rc = sqlite3_bind_blob(&statementHandle.getHandle(), static_cast<int>(index+1), value.data(), static_cast<int>(value.size()), SQLITE_TRANSIENT);

	if(rc != SQLITE_OK) {
		std::string message = "Cannot bind blob value \"" + value + "\" to parameter[" + std::to_string(index) + "]: " + sqlite3_errstr(rc);

        throw esl::addStacktrace(std::runtime_error(message));
	}
}

} /* namespace database */
} /* namespace sqlite4esl */
