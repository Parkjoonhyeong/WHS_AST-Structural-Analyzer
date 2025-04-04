#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_c.c" // JSON 파서가 구현되어 있어야 함

typedef struct {
    char name[30];
    int age;
    int height;
} Student;

// ✅ JFR-002: 동적 메모리 할당
Student* create_student_array(int count) {
    Student *arr = (Student *)malloc(sizeof(Student) * count);
    if (!arr) {
        printf("❌ 메모리 할당 실패\n");
        exit(1);
    }
    return arr;
}

// ✅ JFR-003: JSON → 구조체 변환
int load_students(json_value json, Student *arr) {
    json_value data_arr = json_get(json, "data");
    int count = json_len(data_arr);

    for (int i = 0; i < count; i++) {
        json_value stu_obj = json_get(data_arr, i);
        char *name = json_get_string(stu_obj, "name");
        int age = json_get_int(stu_obj, "age");
        int height = json_get_int(stu_obj, "height");

        strcpy(arr[i].name, name);
        arr[i].age = age;
        arr[i].height = height;
    }

    return count;
}

// ✅ JFR-003: 전체 출력
void print_all_students(Student *arr, int count) {
    for (int i = 0; i < count; i++) {
        printf("학생[%d] 이름: %s, 나이: %d, 키: %d\n", i + 1, arr[i].name, arr[i].age, arr[i].height);
    }
}

// ✅ JFR-006: 파일에서 JSON 읽고 구조체로 저장
void load_students_from_file(const char *filename, Student *arr, int *out_count) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("❌ 파일 열기 실패: %s\n", filename);
        return;
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);

    char *buffer = (char *)malloc(fsize + 1);
    fread(buffer, 1, fsize, fp);
    buffer[fsize] = '\0';
    fclose(fp);

    json_value json = json_create(buffer);
    *out_count = load_students(json, arr);

    free(buffer);
}

// ✅ JFR-006: 일부만 출력
void print_some_students(Student *arr, int count, int from, int to) {
    for (int i = from; i < to && i < count; i++) {
        printf("학생[%d] 이름: %s, 나이: %d, 키: %d\n", i + 1, arr[i].name, arr[i].age, arr[i].height);
    }
}

// ✅ 메인 함수 (JFR-001 + 002 + 003 + 006)
int main(void) {
    // ✅ JFR-001: 직접 JSON 문자열 생성
    const char *json_str = "{ \"data\": ["
        "{\"name\":\"홍길동\", \"age\":20, \"height\":175},"
        "{\"name\":\"김철수\", \"age\":22, \"height\":180},"
        "{\"name\":\"이영희\", \"age\":21, \"height\":160},"
        "{\"name\":\"최자영\", \"age\":23, \"height\":170}"
        "] }";

    // JSON 파싱
    json_value json = json_create(json_str);

    // 동적 메모리 할당
    int student_count = 4;
    Student *students = create_student_array(student_count);

    // JSON → 구조체 저장
    int loaded = load_students(json, students);

    // 전체 출력
    printf("✅ 전체 학생 출력:\n");
    print_all_students(students, loaded);

    // 저장된 파일 불러와 일부 출력
    printf("\n📂 파일에서 일부 학생 출력 예시:\n");
    load_students_from_file("students.json", students, &loaded);
    print_some_students(students, loaded, 1, 3); // 1~2번 학생 출력

    free(students);
    return 0;
}
