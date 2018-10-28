#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

static char rwTab[32][20] = {   //　保留字
    "auto", "break", "case", "char", "const", "continue",
    "default", "do", "double", "else", "enum", "extern",
    "float", "for", "goto", "if", "int", "long",
    "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void",
    "volatile", "while"
};
static char oodTab[36][10] = {   //　运算符或界符
   "+", "-", "*", "/", "<", "<=", ">", ">=", "=", "==",
   "!=", ";", "(", ")", "^", ",", "\"", "\'", "#", "&",
   "&&", "|", "||", "%", "~", "<<", ">>", "[", "]", "{",
   "}", "\\", ".", "\?", ":", "!"
};

int count = 0;  // 记录标识符表的位置
static  char IDTab[1000][50] = { "" };//标识符表

void filter(char* sourceCode);  //　过滤掉注释
void printCode(char* code); // 打印源代码至控制台
void filter(char* sourceCode);   //　过滤掉注释
int isUnderLine(char ch);    //　判断是不是下划线
int isLetter(char ch);   //　判断是不是字母
int isDigit(char ch);  //　判断是不是数字
int findPosInrwTab(char* s); //　查找字符串s在保留字表中的位置
int findPosInoodTab(char* s);   //　查找字符串s在运算符、界符表中的位置
int scanner(char* source, char* token, int* pCode);  // 扫描源代码
int isInIDTab(char* s);  // 判断字符串s是否在标识符表中;

int main(void)
{
    char token[50];
    char sourceCode[10000];
    int pCode = 0;
    char ch;
    int i = 0;
    FILE* fp;
    int flag = 1;

    if ((fp = fopen("/home/zero/codes/princ2compile/test.c", "r")) == NULL) {
        printf("open file\n");
        exit(-1);
    }
    ch = fgetc(fp);
    while (ch != EOF) {
        sourceCode[pCode++] = ch;
        ch = fgetc(fp);
    }
    sourceCode[pCode] = EOF;
    printf("source code :\n");
    printCode(sourceCode);
    printf("\ncode after filter :\n");
    filter(sourceCode);
    printCode(sourceCode);
    printf("\n");

    if ((fp = fopen("/home/zero/codes/princ2compile/compile.txt", "wr")) == NULL) {
        printf("open file\n");
        exit(-1);
    }
    pCode = 0;
    while (flag) {
        int ret = scanner(sourceCode, token, &pCode);
        if (ret == 100) {   // 标识符
            if (!isInIDTab(token)) {
                if (count < 1000)
                    strcpy(IDTab[count++], token);
                else {
                    printf("ID table is full\n");
                    exit(-1);
                }
            }  
            printf("（标识符，%s）\n", token); 
            fprintf(fp, "（标识符，%s）\n", token);             
        }
        else if (ret >= 1 && ret <= 32) {   //　保留字
            printf("（保留字，%s）\n", rwTab[ret]); 
            fprintf(fp, "（保留字，%s）\n", rwTab[ret]); 
        }
        else if (ret == 99) {   //　常数
            printf("(常数, %s）\n", token); 
            fprintf(fp, "（常数，%s）\n", token); 
        }
        else if (ret >= 33 && ret <= 68) {  // 操作符、界符
            printf("(操作符/界符, %s）\n", oodTab[ret - 32]); 
            fprintf(fp, "（操作符/界符，%s）\n", oodTab[ret - 32]);
        } 
        else if (ret == 2000) {
            printf("complete~\n");
            flag = 0;
        }
        else {
            printf("error\n");
            flag = 0;
        }
    }
    for (int i = 0; i < count; ++i) {
        printf("第%d个标识符:%s\n", i+1, IDTab[i]);
        fprintf(fp, "第%d个标识符:%s\n", i+1, IDTab[i]);
    }

    return 0;
}

void printCode(char* code)  //打印代码至控制台
{
    int i = 0;
    while (code[i] != EOF) {
        printf("%c", code[i++]);
    }
}

void filter(char* sourceCode)   //　过滤掉注释
{
    int i = 0;
    char tempCode[10000];
    int k = 0;

    while (sourceCode[i] != EOF) {
        if (sourceCode[i] == '/' && sourceCode[i+1] == '/') {   // 行注释
            while (sourceCode[i] != '\n') {
                ++i;
            }
            ++i;    // 跳过回车换行
        }
        if (sourceCode[i] == '/' && sourceCode[i+1] == '*') {   // 多行注释
            while (!(sourceCode[i] == '*' && sourceCode[i] == '/')) {
                if (sourceCode[i] == EOF) {
                    printf("format error\n");
                    exit(-1);
                }
                ++i;
            } 
            ++i;
        }    
        if (sourceCode[i] == '\n' || sourceCode[i] == '\t') {
            sourceCode[i] = ' ';
        }
        tempCode[k++] = sourceCode[i++];
    }
    tempCode[k++] = EOF;
    tempCode[k] = '\0';    
    memset(sourceCode, '\0', 10000);
    strcpy(sourceCode, tempCode);
}

int isUnderLine(char ch)    //　判断是不是下划线
{
    return (ch == '_');
}

int isLetter(char ch)   //　判断是不是字母
{
    return ((ch >= 'a' && ch <= 'z') || 
            (ch >= 'A' && ch <= 'Z'));
}

int isDigit(char ch)  //　判断是不是数字
{
    return (ch >= '0' && ch <= '9');
}

int findPosInrwTab(char* s) //　查找字符串s在保留字表中的位置
{   
    for (int i = 0; i < 32; ++i) {
        if (strcmp(rwTab[i], s) == 0) {
            return i;
        }
    }
    return -1;
}

int findPosInoodTab(char* s)    //　查找字符串s在运算符、界符表中的位置
{
    for (int i = 0; i < 36; ++i) {
        if (strcmp(oodTab[i], s) == 0) {
            return i;
        }
    }
    return -1;
}

int isInIDTab(char* s)  // 判断字符串s是否在标识符表中
{
    for (int i = 0; i < 1000; ++i) {
        if (strcmp(IDTab[i], s) == 0) {
            return 1;
        }
    }
    return 0;
}

int scanner(char* source, char* token, int* pCode)  // 扫描源代码
{
    int flag = 0;  //　标志是否有下划线
    int k = 0;
    while (source[*pCode] == ' ' || source[*pCode] == '\n' || source[*pCode] == '\0')
        ++(*pCode);
    if (source[*pCode] == '\\' && source[(*pCode) + 1] == 'n') {
        *pCode += 2;
    }
    if (isLetter(source[*pCode]) || isUnderLine(source[*pCode])) {
        token[k++] = source[*pCode];   
        // printf("%c\n", source[*pCode]);       
        if (isUnderLine(source[(*pCode)++])) {
            flag = 1;
        }   
        // printf("%c\n", source[*pCode]);               
        while (isDigit(source[*pCode]) || isLetter(source[*pCode]) || isUnderLine(source[*pCode])) {
            if (isUnderLine(source[*pCode])) {
                flag = 1;
            }
            // printf("%c\n", source[*pCode]);       
            token[k++] = source[*pCode];           
            ++(*pCode);
        }     
        token[k] = '\0';
        if (!flag) {   // 可能是保留字
            int ret = findPosInrwTab(token);
            if (ret != -1) {    //　是保留字
                return ret;
            }
        }
        return 100; // 标识符
    }
    else if (isDigit(source[*pCode])) { //　常数
        token[k++] = source[(*pCode)++];
        while (isDigit(source[*pCode])) {
            token[k++] = source[(*pCode)++];
        }
        token[k] = '\0';
        return 99;
    }
    else if (source[*pCode] == '+' || source[*pCode] == '-' || source[*pCode] == '*' || source[*pCode] == '/' 
            || source[*pCode] == ';' || source[*pCode] == '(' || source[*pCode] == ')' || source[*pCode] == '^'
            || source[*pCode] == ',' || source[*pCode] == '\"' || source[*pCode] == '\'' || source[*pCode] == '~' 
            || source[*pCode] == '#' || source[*pCode] == '%' || source[*pCode] == '['|| source[*pCode] == ']' 
            || source[*pCode] == '{' || source[*pCode] == '}' || source[*pCode] == '\\' || source[*pCode] == '.' 
            || source[*pCode] == '\?' || source[*pCode] == ':') {
        token[k++] = source[(*pCode)++];
        token[k] = '\0';
        int ret = findPosInoodTab(token);
        if (ret != -1) {
            return ret + 32;
        }
        return -1;
    }
    else if (source[*pCode] == '<') {
        token[k++] = source[(*pCode)++];
        if (source[*pCode] == '<') {    // << 
            token[k++] = source[(*pCode)++];
            token[k] = '\0';
            return 58;
        }
        else if (source[*pCode] == '=') {   // <=
            token[k++] = source[(*pCode)++];
            token[k] = '\0';
            return 38;
        } 
        else {
            token[k] = '\0';
            return 37;
        }
        ++(*pCode);
        return -1;
    }
    else if (source[*pCode] == '>') {   
        token[k++] = source[(*pCode)++];
        if (source[*pCode] == '>') {    // >>
            token[k++] = source[*pCode];
            token[k] = '\0';
            return 59;
        }
        else if (source[*pCode] == '=') {   // >=
            token[k++] = source[*pCode];
            token[k] = '\0';
            return 40;
        }
        else {  // >
            token[k] = '\0';
            return 39;
        }
        ++(*pCode);
        return -1;
    }
    else if (source[*pCode] == '=') {
        token[k++] = source[(*pCode)++];
        if (source[*pCode] == '=') {    // ==
            token[k++] = source[(*pCode)++];
            token[k] = '\0';
            return 42;
        }
        else {
            token[k] = '\0';
            return 41;
        }
        ++(*pCode);
        return -1;
    }
    else if (source[*pCode] == '!') {
        token[k++] = source[(*pCode)++];
        if (source[*pCode] == '=') {    // !=
            token[k++] = source[(*pCode)++];
            token[k] = '\0';
            return 43;
        }
        else {
            token[k] = '\0';
            return 68;
        }
    }
    else if (source[*pCode] == '&') {
        token[k++] = source[(*pCode)++];
        if (source[*pCode] == '&') {
            token[k++] = source[(*pCode)++];
            token[k] = '\0';
            return 53;
        }
        else {
            token[k] = '\0';
            return 52;
        }
    }
    else if (source[*pCode] == '|') {
        token[k++] = source[(*pCode)++];
        if (source[*pCode] == '|') {
            token[k++] = source[(*pCode)++];
            token[k] = '\0';
            return 55;
        }
        else {
            token[k] = '\0';
            return 54;
        }
    }
    else if (source[*pCode] == EOF) {
        return 2000;
    }
    else {
        return -1;
    }
}