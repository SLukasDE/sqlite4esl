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

#include <sqlite4esl/Module.h>
#include <sqlite4esl/database/ConnectionFactory.h>

#include <esl/object/Interface.h>
#include <esl/database/Interface.h>
#include <esl/Module.h>

namespace sqlite4esl {

void Module::install(esl::module::Module& module) {
	esl::setModule(module);

	module.addInterface(esl::object::Interface::createInterface(
			database::ConnectionFactory::getImplementation(),
			&database::ConnectionFactory::createObject));

	module.addInterface(esl::database::Interface::createInterface(
			database::ConnectionFactory::getImplementation(),
			&database::ConnectionFactory::createConnectionFactory));
}

} /* namespace sqlite4esl */
