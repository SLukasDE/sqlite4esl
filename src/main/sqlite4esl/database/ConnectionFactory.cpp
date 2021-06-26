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

#include <sqlite4esl/database/ConnectionFactory.h>
#include <sqlite4esl/database/Connection.h>
#include <sqlite4esl/database/Driver.h>
#include <sqlite4esl/Logger.h>

#include <esl/logging/Location.h>
#include <esl/database/exception/SqlError.h>
#include <esl/Stacktrace.h>

#include <stdexcept>
#include <chrono>

namespace sqlite4esl {
namespace database {

namespace {
Logger logger("sqlite4esl::database::ConnectionFactory");
}

std::unique_ptr<esl::object::Interface::Object> ConnectionFactory::createObject(const esl::database::Interface::Settings& settings) {
	return std::unique_ptr<esl::object::Interface::Object>(new ConnectionFactory(settings));
}

std::unique_ptr<esl::database::Interface::ConnectionFactory> ConnectionFactory::createConnectionFactory(const esl::database::Interface::Settings& settings) {
	return std::unique_ptr<esl::database::Interface::ConnectionFactory>(new ConnectionFactory(settings));
}

ConnectionFactory::ConnectionFactory(const esl::database::Interface::Settings& settings)
: esl::database::Interface::ConnectionFactory()
{
	/*
	if(Driver::getDriver().isThreadsafe() == false) {
        throw esl::addStacktrace(std::runtime_error("SQLite3 implementation is not thread safe."));
	}
	*/

	bool hasURI = false;

	for(const auto& setting : settings) {
		if(setting.first == "URI") {
			uri = setting.second;
			hasURI = true;
		}
		else if(setting.first == "timeout") {
			timeoutMS = std::stoi(setting.second);
		}
		else {
			throw esl::addStacktrace(std::runtime_error("Key \"" + setting.first + "\" is unknown"));
		}
	}

	if(hasURI == false) {
		throw esl::addStacktrace(std::runtime_error("Key \"URI\" is missing"));
	}
}

ConnectionFactory::~ConnectionFactory() {
	if(connectionHandle) {
		std::lock_guard<std::timed_mutex> lock(timedMutex);

		esl::logging::Location location;
		location.file = __FILE__;
		location.function = __func__;

		try {
	        Driver::getDriver().close(*this);
		}
		catch (const esl::database::exception::SqlError& e) {
			ESL__LOGGER_WARN_THIS("esl::database::exception::SqlError exception occured\n");
			ESL__LOGGER_WARN_THIS(e.what(), "\n");
			location.line = __LINE__;
			e.getDiagnostics().dump(logger.warn, location);

			const esl::Stacktrace* stacktrace = esl::getStacktrace(e);
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

			const esl::Stacktrace* stacktrace = esl::getStacktrace(e);
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
        throw esl::addStacktrace(std::runtime_error("Calling ConnectionFactory::getConnectionHandle() but db is still not opened"));
	}

	return *connectionHandle;
}

std::unique_ptr<esl::database::Connection> ConnectionFactory::createConnection() {
	if(connectionHandle == nullptr) {
		connectionHandle = &Driver::getDriver().open(uri);
	}

	if(timedMutex.try_lock_for(std::chrono::milliseconds(timeoutMS)) == false) {
		// should we throw an exception?
		return nullptr;
	}

	return std::unique_ptr<esl::database::Connection>(new Connection(*this, *connectionHandle));
}

void ConnectionFactory::doUnlock() {
	timedMutex.unlock();
}

} /* namespace database */
} /* namespace sqlite4esl */
