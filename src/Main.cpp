#include "app/App.hpp"

int main()
{
	Config config {
		"Harrax"
	};

	App::Get()->Run(config);
	return 0;
}
