//
// Created by 孙健淞 on 25-6-7.
//
#include <stdio.h>   // FILE, printf, scanf, fopen, fgetc, fputc, fprintf, perror, EOF 等
#include <stdlib.h>  // malloc, free, exit
#include <string.h>  // strcmp, strdup
#include <stdbool.h> // bool, true, false

#define MAX 256

typedef struct HuffmanNode//创建哈夫曼树节点的结构体
{
    char character;
    double weight;
    struct HuffmanNode* left;
    struct HuffmanNode* right;
}hfmnode;

typedef struct minheap//创建最小堆结构体，用于存放所有哈夫曼树叶子节点，便于高效获取最小频率节点。
{
    hfmnode * nodes[MAX];
    int size;//当前有效节点数
}minheap;

void minheapify(minheap *heap, int index)//上浮，最小堆辅助函数，维护以 index 为根的子树，使其满足最小堆性质
{
    int smallest = index;
    int left = 2 * index + 1;//索引为index的节点的左子节点，索引为2*index+1
    int right = 2 * index + 2;//索引为index的节点的右子节点，索引为2*index+2

    //找到index,left,right中的最小权值索引
    if (left < heap->size && heap->nodes[left]->weight < heap->nodes[smallest]->weight)
        smallest = left;
    if (right < heap->size && heap->nodes[right]->weight < heap->nodes[smallest]->weight)
        smallest = right;
    //判断找到的最小权值索引是否为index,如果不是，则交换index和最小权值索引
    if (smallest != index)
    {
        hfmnode *temp = heap->nodes[index];
        heap->nodes[index] = heap->nodes[smallest];
        heap->nodes[smallest] = temp;
        minheapify(heap, smallest);
    }
}

minheap* buildminheap(hfmnode * nodes[MAX], int n)// 构建最小堆,传入一组叶子节点指针数组和大小 n，返回一个堆
{
    minheap *heap = (minheap*)malloc(sizeof(minheap));
    heap->size = n;
    for (int i = 0; i < n; ++i)//给堆中节点赋初值
        heap->nodes[i] = nodes[i];
    for (int i = (n - 1) / 2; i >= 0; --i)// 从最后一个非叶子节点开始上浮
        minheapify(heap, i);
    return heap;
}

hfmnode* extractmin(minheap *heap)// 提取堆顶最小权值节点
{
    if (heap->size == 0)
        return NULL;
    hfmnode *min = heap->nodes[0];//提取出堆顶最小权值节点
    heap->nodes[0] = heap->nodes[--heap->size];//heap->size-1后，再将当前堆中最大权值的节点状态赋给堆顶节点
    minheapify(heap, 0);//再次进行上浮，确保最小堆的状态保持
    return min;
}

void insertheap(minheap *heap, hfmnode *node)// 插入节点到堆
{
    int i = heap->size++;//当前heap->size值赋给i后，size+1
    heap->nodes[i] = node;//添加新节点
    while (i > 0 && heap->nodes[(i - 1) / 2]->weight > heap->nodes[i]->weight)// 上浮
    {
        hfmnode *temp = heap->nodes[i];
        heap->nodes[i] = heap->nodes[(i - 1) / 2];
        heap->nodes[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

hfmnode * buildtree(char chars[], double freq[], int n)// 构建哈夫曼树，chars[]: 存放字符；freq[]: 与之对应的权值；n: 字符个数
{
    hfmnode * hfmnode_ptr[MAX];// 哈夫曼树节点指针数组，临时存放所有叶子节点的指针
    for (int i = 0; i < n; i++)//为每个字符集节点创建一个哈夫曼树叶子节点,字符和权值一一赋给哈夫曼树叶子节点，左右子节点指针初始置空
    {
        hfmnode_ptr[i] = (hfmnode*)malloc(sizeof(hfmnode));
        *hfmnode_ptr[i]=(hfmnode){
            .character=chars[i],
            .weight=freq[i],
            .left=NULL,
            .right=NULL
        };
    }
    minheap* heap=buildminheap(hfmnode_ptr,n);  // 把所有叶子节点指针放入最小堆，建立堆结构
    while (heap->size > 1)// 反复取出最小的两个节点，合并成一个新的内部节点，再插入堆中
    {
        hfmnode *left = extractmin(heap);
        hfmnode *right= extractmin(heap);

        hfmnode *internal=(hfmnode*)malloc(sizeof(hfmnode));
        *internal=(hfmnode){
            .character='\0', // 内部节点不对应任何字符
            .weight=left->weight+right->weight, // 合并权值
            .left=left,
            .right=right
        };
        insertheap(heap,internal);
    }
    hfmnode* root=extractmin(heap);// 最后堆中只剩一个节点，就是哈夫曼树的根
    free(heap);// 释放堆
    heap=NULL;
    return root;
}

char *codes[MAX];//储存码表 ,codes[i] 存储字符 i 对应的二进制编码
char codeBuf[512];//储存二进制序列, 递归时临时存放“0/1”，最长 511 位足够大部分文本

void generateCodes(hfmnode *root,  int depth)//生成哈夫曼编码表, 深度优先遍历哈夫曼树，遇到叶子就把codeBuf[0..depth-1] 当作该字符的编码
{
    if (!root)
        return;
    if (!root->left && !root->right)// 如果是叶子节点，就把当前的 codeBuf 以字符串形式保存下来
    {
        codeBuf[depth] = '\0';
        codes[(unsigned char)root->character] = strdup(codeBuf);// strdup 会在堆上分配一块内存，存放该字符串
    }
    else
    {
        codeBuf[depth] = '0';// 向左分支标“0”
        generateCodes(root->left, depth+1);
        codeBuf[depth] = '1'; // 向右分支标“1”
        generateCodes(root->right, depth+1);
    }
}

hfmnode *Root = NULL;//哈夫曼树根节点
bool codesReady = false;//判断编码表是否生成

void Initialize()
{
    int n;//字符集大小
    puts("Enter the size of charset:");
    scanf("%d",&n);

    // 申请两段动态内存：一个存字符，一个存对应的频率
    char *chars =(char *) malloc(n * sizeof(char));//储存字符
    double *freq  =(double *) malloc(n * sizeof(double));//储存每个字符的频率
    printf("use '_' for space: ");//输入时用'_'代替空格，之后再进行转换
    for (int i = 0; i < n; i++)
    {
        printf("Char #%d and freq: ", i+1);
        scanf(" %c %lf", &chars[i], &freq[i]);//字符和权值
        if (chars[i] == '_')
            chars[i] = ' ';
    }
    Root= buildtree(chars,freq,n);//生成哈夫曼树
    for (int i = 0; i < MAX; i++) // 先把 codes[] 全部置 NULL，防止 generateCodes 时出现垃圾指针
    {
        codes[i] = NULL;
    }
    generateCodes(Root,0);//生成编码表
    FILE * fd=fopen("hfmTree.txt","w");//打开hfmTree文件
    if (!fd)
    {
        perror("fopen hfmTree"); // 这里只算失败了，但树和 codes 仍然可以后续使用
    }
    else
    {
        for (int i = 0; i < n; ++i)//向hfmTree文件中写入字符及其对应码字
        {
            fprintf(fd, "%d %s\n", (unsigned char)chars[i], codes[(unsigned char)chars[i]]);
        }
    }
    fclose(fd);//关闭hfmTree文件
    codesReady = true;//编码表已生成
    // 释放临时的 chars[] 和 freq[]
    free(chars);
    chars=NULL;
    free(freq);
    freq=NULL;
    printf("Initialization complete; tree saved to 'hfmTree'.\n");
}

/*void loadCodes()//加载码表
{
    if (codesReady)
        return;
    for (int i = 0; i < MAX; ++i)// 先把 codes[] 全部置 NULL
        codes[i] = NULL;
    FILE *fd = fopen("hfmTree", "r");
    if (!fd)
    {
        perror("fopen hfmTree");
        return;
    }
    int val;
    char buf[512];
    while (fscanf(fd, "%d %s", &val, buf) == 2) // 从文件流fd里逐行读：整数 + 字符串到val和buf中
    {
        codes[val] = strdup(buf);//复制字符串 buf 的内容到新分配的内存中,返回指向新字符串的指针。
    }
    fclose(fd);
    codesReady = true;
}*/

void Encode()//编码函数
{
    //loadCodes();
    FILE *in = fopen("ToBeTran.txt", "r");//打开ToBeTran文件
    if (!in) {
        perror("fopen ToBeTran");
        return;
    }
    FILE *out = fopen("CodeFile.txt", "w");//打开CodeFile文件
    if (!out) {
        perror("fopen CodeFile");
        fclose(in);
        return;
    }

    int c;
    while ((c = fgetc(in)) != EOF)//对ToBeTran的内容进行编码，结果存入CodeFile
    {
        char *code = codes[c];
        if (code)
            fputs(code, out);
    }
    fclose(in);
    fclose(out);
    printf("Encoding complete; output in 'CodeFile'.\n");

}

void Decode()//译码函数
{
    //loadCodes();
    typedef struct Map //定义一个映射结构体，将字符与码字一一对应
    {
        char *code;//哈夫曼编码（二进制字符串）
        char ch;//对应的字符
    } Map;
    Map rev[ MAX ]; // 反向映射数组
    int m = 0; // 有效映射数量
    for (int i = 0; i < MAX; ++i)//创建每个字符的映射结构体
    {
        if (codes[i])
        {
            rev[m].code = codes[i];
            rev[m].ch = (char)i;
            m++;
        }
    }
    FILE *in = fopen("CodeFile.txt", "r");//打开CodeFile文件
    if (!in) {
        perror("fopen CodeFile");
        return;
    }
    FILE *out= fopen("TextFile.txt", "w");//打开TextFile文件
    if (!out) {
        perror("fopen TextFile");
        fclose(in);
        return;
    }
    char buf[512] = {0};
    int len = 0;
    int bit;
    while ((bit = fgetc(in)) != EOF)//从CodeFile里读字符
    {
        buf[len++] = bit;
        buf[len] = '\0';
        for (int i = 0; i < m; ++i)// 在 rev[] 里暴力匹配当前的 buf
        {
            if (strcmp(buf, rev[i].code) == 0)// 找到对应字符
            {
                fputc(rev[i].ch, out);
                len = 0;
                break;
            }
        }
    }
    fclose(in);
    fclose(out);
    printf("Decoding complete; output in 'TextFile'.\n");
}
void Print()//打印码字
{
    FILE *in = fopen("CodeFile.txt", "r");//打开CodeFile文件
    if (!in)
    {
        perror("fopen CodeFile");
        return;
    }
    FILE *out= fopen("CodePrin.txt", "w");//打开CodePrin文件
    if (!out)
    {
        perror("fopen CodePrin");
        fclose(in);
        return;
    }
    int c, count=0;
    while ((c = fgetc(in)) != EOF)
    {
        putchar(c);// 打印到控制台
        fputc(c, out); // 写到文件
        if (++count == 50)
        {   putchar('\n');
            fputc('\n', out);
            count = 0;
        }
    }
    if (count)// 最后一行不足 50 也要换行
    {
        putchar('\n');
        fputc('\n', out);
    }
    fclose(in);
    fclose(out);
    printf("Printed CodeFile to console and 'CodePrin'.\n");
}

void TreeprintRec(hfmnode *root, int depth, FILE *fd)
{
    if (!root)
        return;
    for (int i = 0; i < depth; ++i)
        printf("  ");
    if (root->character)
        printf("'%c':%lf\n", root->character, root->weight);
    else
        printf("*:%lf\n", root->weight);
    TreeprintRec(root->left, depth+1, fd);
    TreeprintRec(root->right, depth+1, fd);

    for (int i = 0; i < depth; ++i)
        fputs("  ", fd);
    if (root->character)
        fprintf(fd, "'%c':%lf\n", root->character, root->weight);
    else
        fprintf(fd, "*:%lf\n", root->weight);
}

void Treeprint()//打印哈夫曼树
{
    if (!Root)
    {   printf("Tree not built.\n");
        return;
    }
    FILE *fd = fopen("TreePrint.txt", "w");
    if(!fd)
    {   perror("fopen TreePrint");
        return;
    }
    TreeprintRec(Root, 0, fd);
    fclose(fd);
    printf("Tree printed to console and 'TreePrint'.\n");

}