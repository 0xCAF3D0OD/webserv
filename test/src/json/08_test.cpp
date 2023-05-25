#include "Stream.hpp"

#include "utils_json.hpp"

static int
test_config(json::t_object *config)
{
	Stream output;

	output.open();
	std::cout << *config << std::endl;
	output.close();

	if (output.check("{\"anotherObject\" : {\"anotherString\" : \"my another string\"}, \"object\" : "
					 "{\"string\" : \"my string\"}}\n",
					 ""))
		return 1;

	return 0;
}

int
test_json_object01(void)
{
	Stream			output;
	json::t_object *config;

	output.open();
	config = json::read("test/src/json/config_object01.json");
	output.close();

	if (config == 0)
		return 1;

	if (output.check("", ""))
		return 1;

	if (test_config(config))
	{
		delete config;
		return 1;
	}

	delete config;

	return 0;
}
