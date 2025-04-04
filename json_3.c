#include <stdio.h>
#include <memory.h>
#include "json_c.c"
#include <string.h>

typedef struct {
	char id[10];
	char type[20];
} Batter;

int main(void)
{   
    const char *str = "{\"id\": \"0001\",\"type\": \"donut\",\"name\": \"Cake\",\"ppu\": 0.55,\"batters\": {\"batter\": [{\"id\": \"1001\", \"type\": \"Regular\"},{\"id\": \"1002\", \"type\": \"Chocolate\"},{\"id\": \"1003\", \"type\": \"Blueberry\"},{\"id\": \"1004\", \"type\": \"Devil's Food\"}]}, \"topping\": [{\"id\": \"5001\", \"type\": \"None\"},{\"id\": \"5002\", \"type\": \"Glazed\"},{\"id\": \"5005\", \"type\": \"Sugar\"},{\"id\": \"5007\", \"type\": \"Powdered Sugar\"},{\"id\": \"5006\", \"type\": \"Chocolate with Sprinkles\"},{\"id\": \"5003\", \"type\": \"Chocolate\"},{\"id\": \"5004\", \"type\": \"Maple\"}]}";
	//Creation and assignment of a json_value
	//Dynamic allocation used
	json_value json = json_create(str);

	json_value root = json_get(json, "batters");
	json_value batter_obj = json_get(root, "batter");
	int batter_len = json_len(batter_obj);

	Batter *batter_array = (Batter *)malloc(sizeof(Batter) * batter_len);


	for (int i = 0; i < batter_len; i++)
	{
		printf("batter[%d] : ",i);

		json_value ob = json_get(batter_obj, i);
		char *id = json_get_string(ob, "id");
		char *type = json_get_string(ob, "type");

    strcpy(batter_array[i].id, id);
		strcpy(batter_array[i].type, type);

		printf("%s \n", batter_array[i].id);
		printf("%s \n", batter_array[i].type);
		printf("=================\n");
	}

	return 0;
}

