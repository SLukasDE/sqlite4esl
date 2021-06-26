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

#ifndef SQLITE4ESL_DATABASE_CONNECTIONFACTORY_H_
#define SQLITE4ESL_DATABASE_CONNECTIONFACTORY_H_

#include <esl/database/Interface.h>
#include <esl/database/Connection.h>
#include <esl/object/Interface.h>

#include <sqlite3.h>

#include <string>
#include <mutex>
#include <memory>

namespace sqlite4esl {
namespace database {

class Connection;

class ConnectionFactory : public esl::database::Interface::ConnectionFactory {
public:
	static inline const char* getImplementation() {
		return "sqlite4esl";
	}

	static std::unique_ptr<esl::object::Interface::Object> createObject(const esl::database::Interface::Settings& settings);
	static std::unique_ptr<esl::database::Interface::ConnectionFactory> createConnectionFactory(const esl::database::Interface::Settings& settings);

	ConnectionFactory(const esl::database::Interface::Settings& settings);
	~ConnectionFactory();

	const sqlite3& getConnectionHandle() const;

	std::unique_ptr<esl::database::Connection> createConnection() override;

	void doUnlock();

private:
	sqlite3* connectionHandle = nullptr;
	std::string uri;
	int timeoutMS = 10000;
	std::timed_mutex timedMutex;
	Connection* connection = nullptr;
};

} /* namespace database */
} /* namespace sqlite4esl */

#endif /* SQLITE4ESL_DATABASE_CONNECTIONFACTORY_H_ */
