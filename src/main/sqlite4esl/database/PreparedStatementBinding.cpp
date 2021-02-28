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

#include <sqlite4esl/database/PreparedStatementBinding.h>
#include <sqlite4esl/database/Driver.h>
#include <sqlite4esl/database/ResultSetBinding.h>
#include <sqlite4esl/Logger.h>

#include <esl/Stacktrace.h>

#include <stdexcept>

namespace sqlite4esl {
namespace database {

namespace {
Logger logger("sqlite4esl::database::PreparedStatementBinding");
}

PreparedStatementBinding::PreparedStatementBinding(const Connection& aConnection, const std::string& aSql)
: connection(aConnection),
  sql(aSql),
  statementHandle(Driver::getDriver().prepare(connection, sql))
{
	// Get number of columns from prepared statement
	std::size_t resultColumnsCount = Driver::getDriver().columnCount(statementHandle);
	for(std::size_t i=0; i<resultColumnsCount; ++i) {
		std::string resultColumnName = Driver::getDriver().columnName(statementHandle, i);
		esl::database::Column::Type resultColumnType = esl::database::Column::Type::sqlUnknown;

		resultColumns.emplace_back(std::move(resultColumnName), resultColumnType, true, 0, 0, 0, 0, 0);
	}

	std::size_t parameterColumnsCount = Driver::getDriver().bindParameterCount(statementHandle);
	for(std::size_t i=0; i<parameterColumnsCount; ++i) {
		esl::database::Column::Type parameterColumnType = esl::database::Column::Type::sqlUnknown;

		resultColumns.emplace_back("", parameterColumnType, true, 0, 0, 0, 0, 0);
	}
}


const std::vector<esl::database::Column>& PreparedStatementBinding::getParameterColumns() const {
	return parameterColumns;
}

const std::vector<esl::database::Column>& PreparedStatementBinding::getResultColumns() const {
	return resultColumns;
}

esl::database::ResultSet PreparedStatementBinding::execute(const std::vector<esl::database::Field>& parameterValues) {
	if(!statementHandle) {
		logger.trace << "RE-Create statement handle\n";
		statementHandle = StatementHandle(Driver::getDriver().prepare(connection, sql));
	}

	if(parameterColumns.size() != parameterValues.size()) {
	    throw esl::addStacktrace(std::runtime_error("Wrong number of arguments. Given " + std::to_string(parameterValues.size()) + " parameters but required " + std::to_string(parameterColumns.size()) + " parameters."));
	}

	for(std::size_t i=0; i<parameterValues.size(); ++i) {
		logger.debug << "Bind parameter[" << i << "]\n";

		switch(parameterColumns[i].getType()) {
		case esl::database::Column::Type::sqlInteger:
		case esl::database::Column::Type::sqlSmallInt:
			logger.debug << "  USE field.asInteger\n";
			if(parameterValues[i].isNull()) {
				Driver::getDriver().bindNull(statementHandle, i);
			}
			else {
				Driver::getDriver().bindInteger(statementHandle, i, parameterValues[i].asInteger());
			}
			break;

		case esl::database::Column::Type::sqlDouble:
		case esl::database::Column::Type::sqlNumeric:
		case esl::database::Column::Type::sqlDecimal:
		case esl::database::Column::Type::sqlFloat:
		case esl::database::Column::Type::sqlReal:
			logger.debug << "  USE field.asDouble\n";
			if(parameterValues[i].isNull()) {
				Driver::getDriver().bindNull(statementHandle, i);
			}
			else {
				Driver::getDriver().bindDouble(statementHandle, i, parameterValues[i].asDouble());
			}
			break;

		case esl::database::Column::Type::sqlVarChar:
		case esl::database::Column::Type::sqlChar:
		case esl::database::Column::Type::sqlDateTime:
		case esl::database::Column::Type::sqlDate:
		case esl::database::Column::Type::sqlTime:
		case esl::database::Column::Type::sqlTimestamp:
			logger.debug << "  USE field.asString\n";
			if(parameterValues[i].isNull()) {
				Driver::getDriver().bindNull(statementHandle, i);
			}
			else {
				Driver::getDriver().bindText(statementHandle, i, parameterValues[i].asString());
			}
			break;

		default:
			logger.debug << "  USE field.asString\n";
			if(parameterValues[i].isNull()) {
				Driver::getDriver().bindNull(statementHandle, i);
			}
			else {
				Driver::getDriver().bindBlob(statementHandle, i, parameterValues[i].asString());
			}
			break;
		}
	}

	esl::database::ResultSet resultSet;

	/* ResultSetBinding makes the "execute" */

	/* make a fetch and check, if there is a row available (e.g. no INSERT, UPDATE, DELETE) */
	if(Driver::getDriver().step(statementHandle)) {
		std::unique_ptr<esl::database::ResultSet::Binding> resultSetBinding(new ResultSetBinding(std::move(statementHandle), resultColumns));

		resultSet = esl::database::ResultSet(std::unique_ptr<esl::database::ResultSet::Binding>(std::move(resultSetBinding)));
	}
	else {
		Driver::getDriver().reset(statementHandle);
	}

	return resultSet;
}

void PreparedStatementBinding::executeBulk(const std::vector<std::vector<esl::database::Field>>& fieldArrays) {
	for(const auto& fields : fieldArrays) {
		execute(fields);
	}
}

} /* namespace database */
} /* namespace sqlite4esl */
