#ifndef ESL_DATABASE_SQLITECONNECTIONFACTORY_H_
#define ESL_DATABASE_SQLITECONNECTIONFACTORY_H_

#include <esl/database/Connection.h>
#include <esl/database/ConnectionFactory.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esl {
inline namespace v1_6 {
namespace database {

class SQLiteConnectionFactory : public ConnectionFactory {
public:
	struct Settings {
		Settings(const std::vector<std::pair<std::string, std::string>>& settings);

		std::string uri;
		int timeoutMS = 10000;
	};

	SQLiteConnectionFactory(const Settings& settings);

	static std::unique_ptr<ConnectionFactory> create(const std::vector<std::pair<std::string, std::string>>& settings);

    std::unique_ptr<Connection> createConnection() override;

private:
	std::unique_ptr<ConnectionFactory> connectionFactory;
};

} /* namespace database */
} /* inline namespace v1_6 */
} /* namespace esl */

#endif /* ESL_DATABASE_SQLITECONNECTIONFACTORY_H_ */
