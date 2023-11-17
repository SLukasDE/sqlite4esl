#include <esl/database/SQLiteConnectionFactory.h>

#include <sqlite4esl/database/ConnectionFactory.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace database {

SQLiteConnectionFactory::Settings::Settings(const std::vector<std::pair<std::string, std::string>>& settings) {
	bool hasTimeoutMS = false;

	for(const auto& setting : settings) {
		if(setting.first == "URI") {
			if(!uri.empty()) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" at SQLiteConnectionFactory");
			}
			uri = setting.second;
			if(uri.empty()) {
				throw std::runtime_error("Invalid value \"\" for parameter key \"" + setting.first + "\" at SQLiteConnectionFactory");
			}
		}
		else if(setting.first == "timeout") {
			if(hasTimeoutMS) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" at SQLiteConnectionFactory");
			}
			hasTimeoutMS = true;
			timeoutMS = std::stoi(setting.second);
		}
		else {
			throw std::runtime_error("Key \"" + setting.first + "\" is unknown at SQLiteConnectionFactory");
		}
	}

	if(uri.empty() == false) {
		throw std::runtime_error("Key \"URI\" is missing at SQLiteConnectionFactory");
	}
}

SQLiteConnectionFactory::SQLiteConnectionFactory(const Settings& settings)
: connectionFactory(new sqlite4esl::database::ConnectionFactory(settings))
{ }

std::unique_ptr<ConnectionFactory> SQLiteConnectionFactory::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<ConnectionFactory>(new SQLiteConnectionFactory(Settings(settings)));
}

std::unique_ptr<Connection> SQLiteConnectionFactory::createConnection() {
	return connectionFactory->createConnection();
}

} /* namespace database */
} /* inline namespace v1_6 */
} /* namespace esl */
