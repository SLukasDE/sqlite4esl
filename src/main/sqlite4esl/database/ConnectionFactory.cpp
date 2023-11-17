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

#include <sqlite4esl/database/ConnectionFactory.h>
#include <sqlite4esl/database/Connection.h>

#include <esl/database/exception/SqlError.h>
#include <esl/Logger.h>
#include <esl/monitoring/Streams.h>
#include <esl/system/Stacktrace.h>

#include <chrono>
#include <stdexcept>
#include <string>

namespace sqlite4esl {
inline namespace v1_6 {
namespace database {

namespace {
esl::Logger logger("sqlite4esl::database::ConnectionFactory");
}

ConnectionFactory::ConnectionFactory(esl::database::SQLiteConnectionFactory::Settings aSettings)
: settings(std::move(aSettings))
{ }

ConnectionFactory::~ConnectionFactory() {
	if(connectionHandle) {
		std::lock_guard<std::timed_mutex> lock(timedMutex);

		esl::monitoring::Streams::Location location;
		location.file = __FILE__;
		location.function = __func__;

		try {
			int rc = sqlite3_close(connectionHandle);
			if(rc != SQLITE_OK) {
				logger.warn << "sqlite3_close(...) returned " << rc << ": " << sqlite3_errstr(rc) << "\n";
				logger.warn << "Trying to close connection with sqlite3_close_v2(...) ...\n";
				rc = sqlite3_close_v2(connectionHandle);
				if(rc != SQLITE_OK) {
			        throw esl::system::Stacktrace::add(std::runtime_error(std::string("Cannot close database connection: ") + sqlite3_errstr(rc)));
				}
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
	}
}

const sqlite3& ConnectionFactory::getConnectionHandle() const {
	if(connectionHandle == nullptr) {
        throw esl::system::Stacktrace::add(std::runtime_error("Calling ConnectionFactory::getConnectionHandle() but db is still not opened"));
	}

	return *connectionHandle;
}

std::unique_ptr<esl::database::Connection> ConnectionFactory::createConnection() {
	if(connectionHandle == nullptr) {
		int rc = sqlite3_open_v2(settings.uri.c_str(), &connectionHandle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI | SQLITE_OPEN_NOMUTEX, nullptr);

		if(connectionHandle == nullptr) {
			throw esl::system::Stacktrace::add(std::runtime_error("SQLite is unable to allocate memory to open database \"" + settings.uri + "\""));
		}

		if(rc != SQLITE_OK) {
			std::string message = "Can't open database \"" + settings.uri + "\": " + sqlite3_errmsg(connectionHandle);
			sqlite3_close(connectionHandle);

	        throw esl::system::Stacktrace::add(std::runtime_error(message));
		}

		rc = sqlite3_extended_result_codes(connectionHandle, 1);
		if(rc != SQLITE_OK) {
			std::string message = std::string("Can't enable extended result codes: ") + sqlite3_errmsg(connectionHandle);
			sqlite3_close(connectionHandle);

	        throw esl::system::Stacktrace::add(std::runtime_error(message));
		}
	}

	if(sqlite3_threadsafe() == 0) {
		if(timedMutex.try_lock_for(std::chrono::milliseconds(settings.timeoutMS)) == false) {
			// should we throw an exception?
			return nullptr;
		}
	}
	return std::unique_ptr<esl::database::Connection>(new Connection(*this, *connectionHandle));
}

void ConnectionFactory::doUnlock() {
	if(sqlite3_threadsafe() == 0) {
		timedMutex.unlock();
	}
}

} /* namespace database */
} /* inline namespace v1_6 */
} /* namespace sqlite4esl */
