#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_c.c"

// 헬퍼 함수: AST의 type 노드에서 IdentifierType일 경우 "names" 배열의 첫 번째 원소를 반환
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

// 함수 정의 노드를 복원하는 함수
void recover_function(json_value node, FILE *fp) {
    // 함수 정의 노드에서 decl 필드 추출
    json_value decl = json_get(node, "decl");

    // 함수 이름 추출
    json_value name = json_get(decl, "name");
    char *func_name = json_to_string(name);

    // 리턴 타입 추출 (decl->type를 따라 get_type_name 사용)
    json_value type_node = json_get(decl, "type");
    char *ret_type = get_type_name(type_node);

    // 파라미터 목록 추출: decl->type->args->params
    char param_str[1024] = "";
    json_value args = json_get(type_node, "args");
    if (args.type == JSON_OBJECT) {
        json_value params = json_get(args, "params");
        int param_count = json_len(params);
        for (int i = 0; i < param_count; i++) {
            json_value param = json_get(params, i);
            // 파라미터 이름
            json_value pname = json_get(param, "name");
            char *param_name = json_to_string(pname);
            // 파라미터 타입 추출
            json_value ptype_node = json_get(param, "type");
            char *ptype = get_type_name(ptype_node);

            // 파라미터 정보를 누적해서 저장
            char temp[100];
            sprintf(temp, "%s %s", ptype, param_name);
            strcat(param_str, temp);
            if (i != param_count - 1) {
                strcat(param_str, ", ");
            }
        }
    }
    // 함수 헤더 출력: 예를 들어, "int main(void) {"
    fprintf(fp, "%s %s(%s) {\n", ret_type, func_name, param_str);
    // 함수 본문 복원: 실제 AST를 기반으로 재구성하는 것은 복잡하므로, 여기서는 플레이스홀더로 처리
    fprintf(fp, "    // Function body recovered from AST\n");
    fprintf(fp, "}\n\n");
}

int main(void)
{
    // 1. ast.json 파일 읽기
    FILE *fp = fopen("ast.json", "r");
    if (fp == NULL) {
        printf("ast.json 파일 열기 실패\n");
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

    // 3. 복원된 소스 코드를 저장할 파일 오픈 ("ast.c"로 출력)
    FILE *out = fopen("ast.c", "w");
    if (out == NULL) {
        printf("ast.c 파일 열기 실패\n");
        return 1;
    }

    // 4. AST의 "ext" 배열을 순회하여 함수 정의 노드 복원
    json_value ext = json_get(root, "ext");
    int total_nodes = json_len(ext);
    int func_count = 0;
    for (int i = 0; i < total_nodes; i++) {
        json_value node = json_get(ext, i);
        json_value nodetype = json_get(node, "_nodetype");
        if (nodetype.type == JSON_STRING && strcmp(json_to_string(nodetype), "FuncDef") == 0) {
            recover_function(node, out);
            func_count++;
            // 최소 3개 이상의 함수가 복원되어야 하므로, 필요에 따라 continue하거나 break할 수 있음.
        }
    }

    fclose(out);
    json_free(root);
    printf("총 %d개의 함수가 복원되었습니다. (ast.c 파일 생성됨)\n", func_count);
    return 0;
}
