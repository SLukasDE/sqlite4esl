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

#include <sqlite4esl/database/ResultSetBinding.h>
#include <sqlite4esl/database/Driver.h>
#include <sqlite4esl/Logger.h>

#include <esl/Stacktrace.h>

#include <stdexcept>

namespace sqlite4esl {
namespace database {

namespace {
Logger logger("sqlite4esl::database::ResultSetBinding");
}

ResultSetBinding::ResultSetBinding(StatementHandle&& aStatementHandle, const std::vector<esl::database::Column>& resultColumns)
: esl::database::ResultSet::Binding(resultColumns),
  statementHandle(std::move(aStatementHandle))
{ }

bool ResultSetBinding::fetch(std::vector<esl::database::Field>& fields) {
	if(fields.size() != getColumns().size()) {
        throw esl::addStacktrace(std::runtime_error("Called 'fetch' with wrong number of fields. Given " + std::to_string(fields.size()) + " fields, but it should be " + std::to_string(getColumns().size()) + " fields."));
	}

	if(isFirstFetch) {
		isFirstFetch = false;
	}
	else if(Driver::getDriver().step(statementHandle) == false) {
		return false;
	}

	for(std::size_t i=0; i<getColumns().size(); ++i) {
		/* check if column value was NULL */
		if(Driver::getDriver().columnValueIsNull(statementHandle, i)) {
			fields[i] = nullptr;
			continue;
		}

		switch(Driver::getDriver().columnType(statementHandle, i)) {
		case esl::database::Column::Type::sqlInteger:
		case esl::database::Column::Type::sqlSmallInt:
			logger.debug << "Set integer\n";
			fields[i] = Driver::getDriver().columnInteger(statementHandle, i);
			logger.debug << "Set integer done\n";
			break;

		case esl::database::Column::Type::sqlDouble:
		case esl::database::Column::Type::sqlNumeric:
		case esl::database::Column::Type::sqlDecimal:
		case esl::database::Column::Type::sqlFloat:
		case esl::database::Column::Type::sqlReal:
			logger.debug << "Set double\n";
			fields[i] = Driver::getDriver().columnDouble(statementHandle, i);
			logger.debug << "Set double done\n";
			break;

		case esl::database::Column::Type::sqlVarChar:
		case esl::database::Column::Type::sqlChar:
		default:
			logger.debug << "Set string\n";
			fields[i] = Driver::getDriver().columnText(statementHandle, i);
			logger.debug << "Set string done\n";
			break;
		}
	}

	return true;
}

bool ResultSetBinding::isEditable(std::size_t columnIndex) {
	return false;
}

void ResultSetBinding::add(std::vector<esl::database::Field>& fields) {
    throw esl::addStacktrace(std::runtime_error("add not allowed for query result set."));
}

void ResultSetBinding::save(std::vector<esl::database::Field>& fields) {
    throw esl::addStacktrace(std::runtime_error("save not allowed for query result set."));
}

} /* namespace database */
} /* namespace sqlite4esl */
