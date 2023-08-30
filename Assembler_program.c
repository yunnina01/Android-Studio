#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Node{
    char lbl[10], opc[5], oper[20];
    int ad;
    struct Node *prev, *next;
}Node;

// 리스트 초기화
void init(Node *head_node){
    head_node->prev = head_node->next = head_node;
}


// Node 생성
Node* get_node(char *label, char *opcode, char *operand){
    Node *new_node = (Node*)malloc(sizeof(Node));
    if(new_node == NULL)
        puts("Memory Allocation Error");
    else{
        strcpy(new_node->lbl, label);
        strcpy(new_node->opc, opcode);
        strcpy(new_node->oper, operand);
    }
    return new_node;
}

// 리스트의 마지막에 삽입, 이분탐색 사용
void insert(Node *head_node, Node *new_node){
    if(new_node == NULL)
        return;
    new_node->prev = head_node->prev;
    new_node->next = head_node;
    head_node->prev->next = new_node;
    head_node->prev = new_node;
}

// Symbol Table에서 원하는 Symbol을 찾아 반환
Node* search(Node *head_node, char *key){
    Node *p = head_node->next;
    while(p != head_node){
        if(!strcmp(p->lbl, key))
            break;
        p = p->next;
    }
    return p;
}

// 리스트 주소 연산
int ad_operation(Node *head_node, int locctr){
    Node *p = head_node->next;
    int cmp;

    while(p != head_node){
        cmp = strcmp(p->opc, "RESB");
        if(cmp < 0){
            cmp = strcmp(p->opc, "END");
            if(cmp < 0)                                         // BYTE 작업
                p->ad = locctr++;                               // 주소값을 부여하고 1증가
            else if(!cmp)                                       // END 작업
                p->ad = locctr;                                 // 현재 주소값만 부여
            else{
                p->ad = locctr;                                 // LDA, LDCH 작업
                locctr += 3;                                    // 주소값을 부여하고 oper만큼 증가
            }
        }
        else if(cmp > 0){
            if(!strcmp(p->opc, "RESW")){                        // RESW 작업
                p->ad = locctr;                                 // 주소값을 부여하고 oper * 3 만큼 증가
                locctr += atoi(p->oper) * 3;
            }
            else{
                p->ad = locctr;                                 // STA, STCH, WORD 작업
                locctr += 3;                                    // 주소값을 부여하고 3만큼 증가
            }
        }
        else{                                                   // RESB 작업
            p->ad = locctr;                                     // 주소값을 부여하고 oper만큼 증가
            locctr += atoi(p->oper);
        }     
        p = p->next;
    }
    return locctr;                                              // 마지막 주소값 반환
}

// Symbol Table을 출력, Symbol Table 리스트 생성
void symbol_table(Node *head_node, Node *head_symbol){
    Node *p = head_node->next, *symbol;
    while(p != head_node){
        if(strcmp(p->lbl, "**")){
            printf("%s %d\n", p->lbl, p->ad);
            symbol = get_node(p->lbl, p->opc, p->oper);
            symbol->ad = p->ad;
            insert(head_symbol, symbol);
        }
        p = p->next;
    }
}

// Intermediate File 출력
void interm_file(Node *head_node){
    Node *p = head_node->next;
    while(p != head_node){
        printf("%d %s %s %s\n", p->ad, p->lbl, p->opc, p->oper);
        p = p->next;
    }
}

/*
Text Line의 Opcode 부분 출력
시간 복잡도가 O(N)인 strcmp의 사용을 최소화 하기 위해 이분 탐색 사용
*/ 
void opc_convert(Node *head_node, Node *head_symbol, char code[][3]){
    Node *p = head_node->next, *temp;
    int cmp;
    
    while(p != head_node){
        cmp = strcmp(p->lbl, "**");                                 // 명령어냐 아니냐를 분리해줌
        if(cmp == 0){
            temp = search(head_symbol, p->oper);
            cmp = strcmp(p->opc, "LDCH");
            if(cmp < 0){
                if(!strcmp(p->opc, "LDA"))                          // LDA 작업
                    printf("^%s%04d", code[0], temp->ad);
                else                                                // END 작업
                    break;
            }
            else if(cmp > 0)
                if(!strcmp(p->opc, "STA"))                          // STA 작업
                    printf("^%s%04d", code[1], temp->ad);
                else                                                // STCH 작업
                    printf("^%s%04d", code[3], temp->ad);
            else                                                    // LDCH 작업
                printf("^%s%04d", code[2], temp->ad);
        }
        else{
            cmp = strcmp(p->opc, "RESW");
            if(cmp < 0){
                if(!strcmp(p->opc, "BYTE")){                        // BYTE 작업
                    char res[18];
                    printf("^");
                    strcpy(res, p->oper + 2);
                    if(p->oper[0] == 'C'){                          // 문자 상수
                        for(int i = 0; res[i] != '\''; i++)         // '가 나올 때까지 res를 16진수로 변환
                            printf("%x", res[i]);
                    }
                    else{                                           // 16진수
                        int i;
                        for(i = 0; res[i] != '\''; i++);            // '가 오는 위치
                        res[i] = '\0';                              // '를 \0로 변환
                        printf("%s", res);                          // res 출력
                    }
                }
            }
            if(cmp > 0)                                             // WORD 작업
                printf("^%06x", atoi(p->oper));
        }    
        p = p->next;
    }
}

int main(int argc, char *argv[]){
    FILE *fs;
    Node head_node, head_symbol, *new_node;
    char  label[10], opcode[6], operand[20], ad[10];
    char code[4][3] = {"33", "44", "53", "57"};
    int locctr, start, length, opt;

    // 리스트 초기화
    init(&head_node);
    init(&head_symbol);
    
    // 파일 입력
    while((opt = getopt(argc, argv, "i:")) != -1){
        switch(opt){
            case 'i':
                fs = fopen(optarg, "r");
                break;
            default:
                puts("File Input Error");
        }
    }

    // 리스트 생성
    puts("The contents of Input file:\n");
    fscanf(fs, "%s %s %s", label, opcode, ad);
    printf("%s %s %s\n", label, opcode, ad);
    while(fscanf(fs, "%s", label) != EOF){
        fscanf(fs, "%s %s", opcode, operand);
        printf("%s %s %s\n", label, opcode, operand);
        new_node = get_node(label, opcode, operand);
        insert(&head_node, new_node);
    }
    fclose(fs);

    // 주소 연산
    locctr = start = atoi(ad);
    locctr = ad_operation(&head_node, locctr);
    length = locctr - start;

    // Symbol Table 출력, Symbol Table 리스트 생성
    printf("\n\nLength of the input program is %d.\n\n", length);
    puts("The contents of Symbol Table:\n");
    symbol_table(&head_node, &head_symbol);

    // Intermediate File 출력
    puts("\n\nThe contents of Intermed File:\n");
    printf("COPY START %s\n", ad);
    interm_file(&head_node);

    // Object Program 출력
    puts("\nObject Program has been generated.\n");
    puts("Object Program:\n");
    printf("H^COPY^%06d^%06d\n", start, locctr);
    printf("T^%06d^%d", start, length - 1);
    opc_convert(&head_node, &head_symbol, code);
    printf("\nE^%06d\n", start);

    return 0;
}