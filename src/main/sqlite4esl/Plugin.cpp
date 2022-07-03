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

#include <sqlite4esl/Plugin.h>
#include <sqlite4esl/database/ConnectionFactory.h>

#include <esl/database/ConnectionFactory.h>
#include <esl/object/Object.h>

namespace sqlite4esl {

void Plugin::install(esl::plugin::Registry& registry, const char* data) {
	esl::plugin::Registry::set(registry);

	registry.addPlugin<esl::object::Object>(
			"sqlite4esl/database/ConnectionFactory",
			&database::ConnectionFactory::createObject);

	registry.addPlugin<esl::database::ConnectionFactory>(
			"sqlite4esl/database/ConnectionFactory",
			&database::ConnectionFactory::createConnectionFactory);
}

} /* namespace sqlite4esl */
