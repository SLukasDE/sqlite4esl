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
#include <esl/object/Settings.h>
#include <esl/object/Values.h>
#include <esl/object/Interface.h>

#include <sqlite3.h>

#include <string>
#include <mutex>
#include <memory>

namespace sqlite4esl {
namespace database {

class Connection;

class ConnectionFactory : public esl::database::Interface::ConnectionFactory, public esl::object::Settings {
public:
	static std::unique_ptr<esl::database::Interface::ConnectionFactory> create(const esl::object::Values<std::string>& settings);
	static std::unique_ptr<esl::object::Interface::Object> createObject();

	ConnectionFactory(const esl::object::Values<std::string>& settings);
	~ConnectionFactory();

	const sqlite3& getConnectionHandle() const;

	std::unique_ptr<esl::database::Connection> createConnection() override;

	void addSetting(const std::string& key, const std::string& value) override;

	void setConnectionString(std::string connectionString);
	void doUnlock();

private:
	sqlite3* connectionHandle = nullptr;
	std::string connectionString;
	int timeoutMS = 10000;
	std::timed_mutex timedMutex;
	Connection* connection = nullptr;
};

} /* namespace database */
} /* namespace sqlite4esl */

#endif /* SQLITE4ESL_DATABASE_CONNECTIONFACTORY_H_ */
