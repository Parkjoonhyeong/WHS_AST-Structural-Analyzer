#include <stdio.h>
#include "json_c.c"

int main(void)
{   
    const char *str = "{\"data\":[{\"color\": \"red\", \"value\": \"#f00\"}, {\"color\": \"green\", \"value\": \"#0f0\"}, {\"color\": \"blue\", \"value\": \"#00f\"}, {\"color\": \"cyan\", \"value\": \"#0ff\"}, {\"color\": \"magenta\", \"value\": \"#f0f\"}, {\"color\": \"yellow\", \"value\": \"#ff0\"}, {\"color\": \"black\", \"value\": \"#000\"}]}";
    
    json_value json = json_create(str);

    json_value obj = json_get(json, "data");
    int obj_size = json_len(obj);

    //요구사항2
    int input_size = 0;
    printf("몇개 파싱할거야? ");
    scanf("%d", &input_size);

    if(obj_size != input_size)
    {
        printf("넌 틀렸어\n"); // ✅ print -> printf 로 수정
    }

    //요구사항 3
    int select = 0;
    printf("[1] 전체, [2] 선택: "); // ✅ print -> printf 로 수정
    scanf("%d", &select);
    if(select == 1)
    {
        json_print(obj);
    }
    else
    {
        // 특정 번호 3 
        json_value ob = json_get(obj, 3);
        json_print(ob); // ✅ 선택한 오브젝트 출력이 누락되어 있어 추가
    }

    return 0;
}

  // 요구사항 2 :  json 몇 개? 파싱할 오브젝트 갯수와 일치하는지 확인하는 로직 (함수화)

  // 요구사항 3 : 전체 출력 or 특전 번호 오브젝트 출력 (함수화)
  
  // 요구사항 4 : object한개 추가해서 총 8개 json을 파일에 저장 (함수화)