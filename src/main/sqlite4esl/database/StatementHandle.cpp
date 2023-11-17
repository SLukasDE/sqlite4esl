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

#include <sqlite4esl/database/StatementHandle.h>

#include <esl/Logger.h>

#include <esl/database/exception/SqlError.h>
#include <esl/system/Stacktrace.h>
#include <esl/monitoring/Streams.h>

#include <stdexcept>

namespace sqlite4esl {
inline namespace v1_6 {
namespace database {

namespace {
esl::Logger logger("sqlite4esl::database::PreparedHandle");
}

StatementHandle::StatementHandle(StatementHandle&& other)
: handle(other.handle)
{
	other.handle = nullptr;
	logger.trace << "Statement handle constructed (moved)\n";
}

StatementHandle::StatementHandle(sqlite3_stmt& aHandle)
: handle(&aHandle)
{
}

StatementHandle::~StatementHandle() {
	if(handle == nullptr) {
		logger.debug << "Close statement handle (closed already)\n";
		return;
	}

	logger.debug << "Close statement handle\n";

	esl::monitoring::Streams::Location location;
	location.file = __FILE__;
	location.function = __func__;

	try {
		// free statement handle
		//Driver::getDriver().finalize(*this);
		int rc = sqlite3_finalize(&getHandle());
		if(rc != SQLITE_OK) {
	        throw esl::system::Stacktrace::add(std::runtime_error(std::string("Can't close statement handle: ") + sqlite3_errstr(rc)));
		}
	}
	catch (const esl::database::exception::SqlError& e) {
		ESL__LOGGER_WARN_THIS("esl::database::exception::SqlError exception occured\n");
		ESL__LOGGER_WARN_THIS(e.what(), "\n");
		location.line = __LINE__;
		e.getDiagnostics().dump(logger.warn, location);

		const esl::system::Stacktrace* stacktrace = esl::system::Stacktrace::get(e);
		if(stacktrace) {
			location.line = __LINE__;
			stacktrace->dump(logger.warn, location);
		}
		else {
			ESL__LOGGER_WARN_THIS("no stacktrace\n");
		}
	}
	catch(const std::exception& e) {
		ESL__LOGGER_WARN_THIS("std::exception exception occured\n");
		ESL__LOGGER_WARN_THIS(e.what(), "\n");

		const esl::system::Stacktrace* stacktrace = esl::system::Stacktrace::get(e);
		if(stacktrace) {
			location.line = __LINE__;
			stacktrace->dump(logger.warn, location);
		}
		else {
			ESL__LOGGER_WARN_THIS("no stacktrace\n");
		}
	}
	catch (...) {
		ESL__LOGGER_ERROR_THIS("unkown exception occured\n");
	}

	handle = nullptr;
}

StatementHandle& StatementHandle::operator=(StatementHandle&& other) {
	handle = other.handle;
	other.handle = nullptr;
	logger.trace << "Statement handle moved\n";
	return *this;
}

StatementHandle::operator bool() const noexcept {
	return handle != nullptr;
}

bool StatementHandle::step() const {
	int rc = sqlite3_step(&getHandle());

	switch(rc) {
	case SQLITE_DONE:
	case SQLITE_ROW:
		break;
	case SQLITE_BUSY:
        throw esl::system::Stacktrace::add(std::runtime_error(std::string("Cannot fetch, because sqlite3_step returned SQLITE_BUSY: ") + sqlite3_errstr(rc)));
	case SQLITE_MISUSE:
        throw esl::system::Stacktrace::add(std::runtime_error(std::string("Cannot fetch, because sqlite3_step returned SQLITE_MISUSE: ") + sqlite3_errstr(rc)));
	case SQLITE_ERROR:
        throw esl::system::Stacktrace::add(std::runtime_error(std::string("Cannot fetch, because sqlite3_step returned SQLITE_ERROR: ") + sqlite3_errstr(rc)));
	default:
        throw esl::system::Stacktrace::add(std::runtime_error("Cannot fetch, because sqlite3_step returned " + std::to_string(rc) + ": " + sqlite3_errstr(rc)));
	}

	return rc == SQLITE_ROW;
}

void StatementHandle::reset() const {
	int rc = sqlite3_reset(&getHandle());

	if(rc != SQLITE_OK) {
        throw esl::system::Stacktrace::add(std::runtime_error(std::string("Can't reset statement handle: ") + sqlite3_errstr(rc)));
	}
}

std::size_t StatementHandle::columnCount() const {
	int count = sqlite3_column_count(&getHandle());
	if(count < 0) {
        throw esl::system::Stacktrace::add(std::runtime_error("sqlite3_column_count returned a negative value: " + std::to_string(count)));
	}

	return static_cast<std::size_t>(count);
}

std::string StatementHandle::columnName(std::size_t index) const {
	const char* name = sqlite3_column_name(&getHandle(), static_cast<int>(index));
	if(name == nullptr) {
        throw esl::system::Stacktrace::add(std::runtime_error("sqlite3_column_name returned a null pointer for index " + std::to_string(index)));
	}

	return name;
}

std::string StatementHandle::columnDeclType(std::size_t index) const {
	const char* declType = sqlite3_column_decltype(&getHandle(), static_cast<int>(index));
	return declType ? declType : "";
}

esl::database::Column::Type StatementHandle::columnType(std::size_t index) const {
	int rc = sqlite3_column_type(&getHandle(), static_cast<int>(index));
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

bool StatementHandle::columnValueIsNull(std::size_t index) const {
	int rc = sqlite3_column_type(&getHandle(), static_cast<int>(index));
	return rc == SQLITE_NULL;
}

std::int64_t StatementHandle::columnInteger(std::size_t index) const {
	return static_cast<std::int64_t>(sqlite3_column_int64(&getHandle(), static_cast<int>(index)));
}

double StatementHandle::columnDouble(std::size_t index) const {
	return sqlite3_column_double(&getHandle(), static_cast<int>(index));
}

std::string StatementHandle::columnText(std::size_t index) const {
	//const char* data = static_cast<const char*>(sqlite3_column_text(&statementHandle.getHandle(), static_cast<int>(index)));
	const char* data = reinterpret_cast<const char*>(sqlite3_column_text(&getHandle(), static_cast<int>(index)));
	if(data == nullptr) {
        throw esl::system::Stacktrace::add(std::runtime_error("sqlite3_column_text returned a null pointer"));
	}

	int length = sqlite3_column_bytes(&getHandle(), static_cast<int>(index));
	if(length < 0) {
        throw esl::system::Stacktrace::add(std::runtime_error("sqlite3_column_bytes returned a negative value: " + std::to_string(length)));
	}

	return std::string(data, static_cast<std::size_t>(length));
}

std::string StatementHandle::columnBlob(std::size_t index) const {
	const char* data = static_cast<const char*>(sqlite3_column_blob(&getHandle(), static_cast<int>(index)));
	if(data == nullptr) {
		return "";
	}

	int length = sqlite3_column_bytes(&getHandle(), static_cast<int>(index));
	if(length < 0) {
        throw esl::system::Stacktrace::add(std::runtime_error("sqlite3_column_bytes returned a negative value: " + std::to_string(length)));
	}

	return std::string(data, static_cast<std::size_t>(length));
}

std::size_t StatementHandle::bindParameterCount() const {
	int count = sqlite3_bind_parameter_count(&getHandle());
	if(count < 0) {
        throw esl::system::Stacktrace::add(std::runtime_error("sqlite3_bind_parameter_count returned a negativ value: " + std::to_string(count)));
	}

	return static_cast<std::size_t>(count);
}

void StatementHandle::bindNull(std::size_t index) const {
	int rc = sqlite3_bind_null(&getHandle(), static_cast<int>(index));

	if(rc != SQLITE_OK) {
		std::string message = "Cannot bind null value to parameter[" + std::to_string(index+1) + "]: " + sqlite3_errstr(rc);

        throw esl::system::Stacktrace::add(std::runtime_error(message));
	}
}

void StatementHandle::bindInteger(std::size_t index, std::int64_t value) const {
	int rc = sqlite3_bind_int64(&getHandle(), static_cast<int>(index+1), static_cast<sqlite3_int64>(value));

	if(rc != SQLITE_OK) {
		std::string message = "Cannot bind integer value " + std::to_string(value) + " to parameter[" + std::to_string(index) + "]: " + sqlite3_errstr(rc);

        throw esl::system::Stacktrace::add(std::runtime_error(message));
	}
}

void StatementHandle::bindDouble(std::size_t index, double value) const {
	int rc = sqlite3_bind_double(&getHandle(), static_cast<int>(index+1), value);

	if(rc != SQLITE_OK) {
		std::string message = "Cannot bind double value " + std::to_string(value) + " to parameter[" + std::to_string(index) + "]: " + sqlite3_errstr(rc);

        throw esl::system::Stacktrace::add(std::runtime_error(message));
	}
}

void StatementHandle::bindText(std::size_t index, const std::string& value) const {
	const char* str = value.c_str();
	std::size_t length = std::strlen(str);

	int rc = sqlite3_bind_text(&getHandle(), static_cast<int>(index+1), str, static_cast<int>(length), SQLITE_TRANSIENT);

	if(rc != SQLITE_OK) {
		std::string message = "Cannot bind text value \"" + value + "\" to parameter[" + std::to_string(index) + "]: " + sqlite3_errstr(rc);

        throw esl::system::Stacktrace::add(std::runtime_error(message));
	}
}

void StatementHandle::bindBlob(std::size_t index, const std::string& value) const {
	int rc = sqlite3_bind_blob(&getHandle(), static_cast<int>(index+1), value.data(), static_cast<int>(value.size()), SQLITE_TRANSIENT);

	if(rc != SQLITE_OK) {
		std::string message = "Cannot bind blob value \"" + value + "\" to parameter[" + std::to_string(index) + "]: " + sqlite3_errstr(rc);

        throw esl::system::Stacktrace::add(std::runtime_error(message));
	}
}

sqlite3_stmt& StatementHandle::getHandle() const noexcept {
	if(handle == nullptr) {
        throw esl::system::Stacktrace::add(std::runtime_error("Calling StatementHandle::getHandle() but handle is null"));
	}
	return *handle;
}

} /* namespace database */
} /* inline namespace v1_6 */
} /* namespace sqlite4esl */
