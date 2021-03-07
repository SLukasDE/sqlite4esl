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

#ifndef SQLITE4ESL_DATABASE_RESULTSETBINDING_H_
#define SQLITE4ESL_DATABASE_RESULTSETBINDING_H_

#include <sqlite4esl/database/StatementHandle.h>

#include <esl/database/ResultSet.h>
#include <esl/database/Column.h>
#include <esl/database/Field.h>

#include <vector>

namespace sqlite4esl {
namespace database {

class Environment;

class ResultSetBinding : public esl::database::ResultSet::Binding {
public:
	ResultSetBinding(StatementHandle&& statementHandle, const std::vector<esl::database::Column>& resultColumns);

	bool fetch(std::vector<esl::database::Field>& fields) override;
	bool isEditable(std::size_t columnIndex) override;
	void add(std::vector<esl::database::Field>& fields) override;
	void save(std::vector<esl::database::Field>& fields) override;

private:
	StatementHandle statementHandle;
	bool isFirstFetch = true;
};

} /* namespace database */
} /* namespace sqlite4esl */

#endif /* SQLITE4ESL_DATABASE_RESULTSETBINDING_H_ */
