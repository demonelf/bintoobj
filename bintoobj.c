/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/31/2018 02:07:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

#if 0
objcopy -I binary -O elf64-x86-64 -B i386:x86-64 hello.txt hello.o
struct obj_st
{
    char path[128];
    char *start;
    char *end;
    char *size;
};

extern char _binary_hello_txt_end;
extern char _binary_hello_txt_start;
extern char _binary_hello_txt_size;

struct obj_st obj[] = {
    {"hello.txt", &_binary_hello_txt_start, &_binary_hello_txt_end, &_binary_hello_txt_size},
    {"hello.txt", &_binary_hello_txt_start, &_binary_hello_txt_end, &_binary_hello_txt_size},
    {"hello.txt", &_binary_hello_txt_start, &_binary_hello_txt_end, &_binary_hello_txt_size},
};

int main(void)
{
    char *p = obj[0].start;
    while (p != obj[0].end ) putchar(*p++);

    printf("_binary_hello_txt_start %ld \r\n", (unsigned long)obj[1].start);
    printf("_binary_hello_txt_end   %ld \r\n", (unsigned long)obj[1].end);
    printf("_binary_hello_txt_size %d \r\n", (char)obj[1].size);

    return 0;
}
#endif


char *fullpath;


char *str_obj_st =
{
    "struct obj_st          \r\n \ 
    {                       \r\n \
        char path[128];     \r\n \
        char *start;        \r\n \
        char *end;          \r\n \
        char *size;         \r\n \
    };"
};

#if 1
char *str_find_resource_fun =
{
    "struct obj_st* find_resouce_by_path(char *path)                                  \r\n \
    {                                                                                 \r\n \
        int i = 0;                                                                        \r\n \
            for(; i < sizeof(resource_obj)/(sizeof(struct obj_st)); i++)                      \r\n \
                if(strncmp(resource_obj[i].path, path, strlen(resource_obj[i].path)) == 0)    \r\n \
                    return &resource_obj[i];                                                  \r\n \
                        return NULL;                                                                      \r\n \
    }"
};
#endif

FILE *fp;
FILE *fp_tmp;

/* just get lastest info */
int _System(const char * cmd, char *pRetMsg, int msg_len)
{
    FILE * fp;
    char * p = NULL;
    int res = -1;
    if (cmd == NULL || pRetMsg == NULL || msg_len < 0)
    {
        printf("Param Error!\n");
        return -1;
    }
    if ((fp = popen(cmd, "r") ) == NULL)
    {
        printf("Popen Error!\n");
        return -2;
    }
    else
    {
        memset(pRetMsg, 0, msg_len);
        //get lastest result
        while(fgets(pRetMsg, msg_len, fp) != NULL)
        {
            printf("Msg:%s",pRetMsg); //print all info
        }

        if ( (res = pclose(fp)) == -1)
        {
            printf("close popenerror!\n");
            return -3;
        }
        pRetMsg[strlen(pRetMsg)-1] = '\0';
        return 0;
    }
}

unsigned int num = 0;
/*bintoobj*/
int bintoobj(char *filename)
{
    char cmd[1024] = "";
    num++;
    sprintf(cmd, "objcopy -I binary -O elf64-x86-64 -B i386:x86-64 %s ./.tmp/%d.o", fullpath, num);
    system(cmd);

    char a8Result[2018] = {0};
    char symbol[2048];
    char symbol_start[2048];
    char symbol_end[2048];
    char symbol_size[2048];
    char symbol_tmp[2048];
    char start[1024];
    int ret = 0;

    sprintf(cmd, "nm ./.tmp/%d.o", num);
    ret  = _System(cmd, a8Result, sizeof(a8Result));
    printf("ret = %d \na8Result = %s\nlength = %d \n", ret, a8Result, strlen(a8Result));
    sscanf(a8Result, "%*s D %s", symbol);
    if(strlen(symbol) > 5)
        symbol[strlen(symbol) - 5] = 0;
    printf("symbol: [%s]\r\n", symbol);

    sprintf(symbol_start, "extern char* %sstart;\r\n", symbol);
    sprintf(symbol_end, "extern char* %send;\r\n", symbol);
    sprintf(symbol_size, "extern char* %ssize;\r\n", symbol);

    fwrite(symbol_start, sizeof(char), strlen(symbol_start), fp);  
    fwrite(symbol_end, sizeof(char), strlen(symbol_end), fp);  
    fwrite(symbol_size, sizeof(char), strlen(symbol_size), fp);  

    sprintf(symbol_start, "%sstart", symbol);
    sprintf(symbol_end, "%send", symbol);
    sprintf(symbol_size, "%ssize", symbol);

    /*{"hello.txt", &_binary_hello_txt_start, &_binary_hello_txt_end, &_binary_hello_txt_size},*/
    sprintf(symbol_tmp, "   {\"%s\", &%s, &%s, &%s},\r\n", fullpath, symbol_start, symbol_end, symbol_size);
    fwrite(symbol_tmp, sizeof(char), strlen(symbol_tmp), fp_tmp);  
    return 0;
}

int readfile(char *k)
{
    DIR * dp;
    struct dirent * dirrp;
    struct stat buf;

    //创建一个p指针指向fullpath的最末尾
    char *p;
    p = fullpath + strlen(fullpath);

    if(-1 == lstat(fullpath,&buf))
    {
        printf("%s[lstat]%s\n",fullpath,strerror(errno));
        return -1;
    }

    //***************如果不是目录文件则返回********************************//
    if(S_ISDIR(buf.st_mode)==0)
    {
        //printf("%s ",k);
        printf("%s \n", fullpath);
        bintoobj(k);
        return 1;
    }

    //***************如果是目录文件则继续********************************//

    //如果是目录且在尾部没有/符号,那么就在这个文件名尾部加一个/符号
    if('/' != *(p-1))
    {
        *p++ = '/';
        *p = 0;
    }

    //输出这个目录
    printf("%s:\n",fullpath);
    if(NULL == (dp=opendir(fullpath)))
    {
        printf("%s[opendir]%s\n",fullpath,strerror(errno));
        return -1;
    }
    while(NULL != (dirrp=readdir(dp)))
    {
        //将目录下的..和.文件忽略
        if((strcmp(dirrp->d_name,".")==0) || (strcmp(dirrp->d_name,"..")==0))
            continue;
        strcpy(p,dirrp->d_name);

        if(0 == readfile(p)) //遍历完一个目录文件
            printf("\n");
    }
    p[-1]=0;	//将后面递归增加的名称全部去掉
    if( -1 == closedir(dp))
    {
        printf("%s[closedir]%s\n",fullpath,strerror(errno));
        return -1;
    }

    return 0;
}



int main(int argc,char *argv[])
{
    if(argc != 2)
    {
        printf("Usage:%s <Dir name>\n",argv[0]+2);
        exit(EXIT_FAILURE);
    }

    fp= fopen("./resource.c","w");  
    fp_tmp= fopen("./resource_tmp.c","w+");  

    fwrite("#include <stdio.h>\r\n", sizeof(char), strlen("#include <stdio.h>\r\n"), fp);  
    fwrite(str_obj_st, sizeof(char), strlen(str_obj_st), fp);  
    fwrite("\r\n\r\n", sizeof(char), strlen("\r\n\r\n"), fp);  
    fwrite("struct obj_st resource_obj[] = {\r\n", sizeof(char), strlen("struct obj_st resource_obj[] = {\r\n"), fp_tmp);  


    system("rm ./.tmp/*");
    //最终文件的路径是保存在fullpath中的，这里对fullpath进行了初始化
    fullpath = (char *)malloc(sizeof(char)*PATH_MAX);
    memset(fullpath,0,PATH_MAX);

    strcpy(fullpath,argv[1]);
    readfile(fullpath);
    printf("\n");

    fwrite("};\r\n", sizeof(char), strlen("};\r\n"), fp_tmp);  
    /* 查找文件的开头 */
    fseek(fp_tmp, 0, SEEK_SET);
    char tmp;
    while(fread(&tmp, sizeof(char), sizeof(char), fp_tmp) == 1)
        fwrite(&tmp, sizeof(char), sizeof(char), fp);  

    fwrite(str_find_resource_fun, sizeof(char), strlen(str_find_resource_fun), fp);  

    //system("gcc -c resource.c -o ./.tmp/resource.o");

    system("ar rcs ./libresource.a ./.tmp/*.o");
    fclose(fp_tmp);
    fclose(fp);  
    return 0;
}
