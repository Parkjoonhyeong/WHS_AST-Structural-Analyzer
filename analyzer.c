#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_c.c"

typedef struct {
    char name[100];
    char return_type[50];
    int param_count;
    char param_names[10][50];
    char param_types[10][50];
    int if_count;
} FunctionInfo;

// 헬퍼 함수: type_node에서 IdentifierType일 경우 "names" 배열의 첫 번째 원소를 반환
char* get_type_name(json_value type_node) {
    while (type_node.type == JSON_OBJECT) {
        json_value nodetype = json_get(type_node, "_nodetype");
        if (nodetype.type == JSON_STRING) {
            if (strcmp(json_to_string(nodetype), "IdentifierType") == 0) {
                json_value names = json_get(type_node, "names");
                if (names.type == JSON_ARRAY && json_len(names) > 0) {
                    return json_to_string(json_get(names, 0));
                } else {
                    return "(unknown)";
                }
            }
        }
        // IdentifierType이 아니면 다음 "type" 노드로 이동
        json_value next = json_get(type_node, "type");
        if (next.type == JSON_UNDEFINED)
            break;
        type_node = next;
    }
    return "(unknown)";
}

// 재귀적으로 if문 개수 세기
int count_if(json_value node) {
    int count = 0;
    if (node.type == JSON_OBJECT) { 
        json_value nodetype = json_get(node, "_nodetype");
        if (nodetype.type == JSON_STRING && strcmp(json_to_string(nodetype), "If") == 0) {
            count++;
        }
        int len = json_get_last_index(node);
        for (int i = 0; i <= len; i++) {
            json_value child = json_get(node, i);
            count += count_if(child);
        }
    } else if (node.type == JSON_ARRAY) {
        int len = json_len(node);
        for (int i = 0; i < len; i++) {
            json_value child = json_get(node, i);
            count += count_if(child);
        }
    }
    return count;
}

int main(void)
{
    // 1. ast.json 파일 읽기
    FILE *fp = fopen("ast.json", "r");
    if (fp == NULL) {
        printf("❌ ast.json 파일 열기 실패\n");
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *buffer = (char *)malloc(size + 1);
    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    fclose(fp);

    // 2. JSON 파싱
    json_value root = json_create(buffer);
    free(buffer);

    // 3. "ext" 배열 추출 → 전체 노드 순회
    json_value ext = json_get(root, "ext");
    int total_nodes = json_len(ext);

    FunctionInfo functions[50]; // 함수 정보를 저장할 배열
    int func_index = 0;

    // 4. "FuncDef" 노드만 찾아서 함수 정보 추출
    for (int i = 0; i < total_nodes; i++) {
        json_value node = json_get(ext, i);
        json_value nodetype = json_get(node, "_nodetype");

        if (nodetype.type == JSON_STRING && strcmp(json_to_string(nodetype), "FuncDef") == 0) {
            // 함수 이름
            json_value decl = json_get(node, "decl");
            json_value name = json_get(decl, "name");
            strcpy(functions[func_index].name, json_to_string(name));

            // 리턴 타입
            json_value t1 = json_get(decl, "type");
            char* ret_type = get_type_name(t1);
            strcpy(functions[func_index].return_type, ret_type);

            // 파라미터
            json_value args = json_get(t1, "args");
            if (args.type == JSON_OBJECT) {
                json_value params = json_get(args, "params");
                int param_count = json_len(params);
                functions[func_index].param_count = param_count;

                for (int j = 0; j < param_count; j++) {
                    json_value param = json_get(params, j);

                    // 파라미터 이름
                    json_value pname = json_get(param, "name");
                    strcpy(functions[func_index].param_names[j], json_to_string(pname));

                    // 파라미터 타입
                    json_value ptype_node = json_get(param, "type");
                    char* param_type = get_type_name(ptype_node);
                    strcpy(functions[func_index].param_types[j], param_type);
                }
            } else {
                functions[func_index].param_count = 0;
            }

            // if문 개수
            json_value body = json_get(node, "body");
            functions[func_index].if_count = count_if(body);

            func_index++;
        }
    }

    // 5. 결과 출력
    // 5-1) 함수 개수
    printf("함수 개수 : %d\n\n", func_index);

    // 5-2) 함수들의 리턴 타입
    printf("함수들의 리턴 타입:\n");
    for (int i = 0; i < func_index; i++) {
        printf("  - %s: %s\n", functions[i].name, functions[i].return_type);
    }

    // 5-3) 함수들의 이름
    printf("\n함수들의 이름:\n");
    for (int i = 0; i < func_index; i++) {
        printf("  - %s\n", functions[i].name);
    }

    // 5-4) 함수들의 파라미터 (타입, 변수명)
    printf("\n함수들의 파라미터 타입, 변수명:\n");
    for (int i = 0; i < func_index; i++) {
        if (functions[i].param_count == 0) {
            printf("  - %s: (no params)\n", functions[i].name);
        } else {
            printf("  - %s:\n", functions[i].name);
            for (int j = 0; j < functions[i].param_count; j++) {
                printf("    [%d] %s %s\n", j + 1,
                       functions[i].param_types[j],
                       functions[i].param_names[j]);
            }
        }
    }

    // 5-5) 함수들의 if문 개수
    printf("\n함수들의 if조건 개수:\n");
    for (int i = 0; i < func_index; i++) {
        printf("  - %s: %d\n", functions[i].name, functions[i].if_count);
    }

    // 리소스 정리
    json_free(root);
    return 0;
}
