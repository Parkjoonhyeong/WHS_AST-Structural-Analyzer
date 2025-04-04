#include <stdio.h>
#include "json_c.c"

int main(void)
{   
	
    const char *str = "{\"data\":[{\"color\": \"red\", \"value\": \"#f00\"}, {\"color\": \"green\", \"value\": \"#0f0\"}, {\"color\": \"blue\", \"value\": \"#00f\"}, {\"color\": \"cyan\", \"value\": \"#0ff\"}, {\"color\": \"magenta\", \"value\": \"#f0f\"}, {\"color\": \"yellow\", \"value\": \"#ff0\"}, {\"color\": \"black\", \"value\": \"#000\"}]}";
	//Creation and assignment of a json_value
	//Dynamic allocation used
	json_value json = json_create(str);

	//json_print(json);

	// jsgon_get_string()
	json_value obj = json_get(json, "data");
	int obj_size = json_len(obj);

	printf("%d", obj_size);
	for (int i = 0; i < obj_size; i++)
	{
		// obj array에서 몇번째 있는값
		json_value ob = json_get(obj,i);
		char *color = json_get_string(ob, "color");
		printf("%s \n", color);
	}

	return 0;
}

