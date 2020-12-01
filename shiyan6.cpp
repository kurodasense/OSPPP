#include<iostream>
#include<vector>
#include<cstdio>
#include<cstring>
using namespace std;
# define MAX_SIZE 4096
# define MIN_SIZE 0
//  空闲分区
struct KongXianFenQu{
    int startAddr;//空闲分区起始地址
    int size;//空闲分区大小
};
//  空闲链
struct KongXianLian{
    KongXianFenQu *fenqu;//空闲链连着的空闲分区
    KongXianLian *pre;//上一个空闲分区
    KongXianLian *next;//下一个空闲分区
};
//  已使用的内存分区
struct usedFenQu{
    int startAddr;// 起始地址
    int size;// 大小
};

//空闲分区表
vector<KongXianFenQu *> kongXianFenQuTable;
//已使用分区表
vector<usedFenQu *> usedTable;
//空闲分区链
KongXianLian *chain;

//  初始化
void init(){
    //创建一个空闲分区
    KongXianFenQu *tableNode = new KongXianFenQu;
    tableNode->startAddr = 0;//初始起始地址为0
    tableNode->size = MAX_SIZE;//设置内存大小为MAX_SIZE

    //把空闲分区放进空闲分区表中
    kongXianFenQuTable.push_back(tableNode);

    //初始化空闲分区链
    //创建一个链的结点
    KongXianLian *chainNode = new KongXianLian;
    chainNode->fenqu = tableNode;
    chainNode->next = chainNode;
    chainNode->pre = chainNode;
    chain = chainNode;
}
//  查看内存使用情况
void ps(){
    cout<<"-------空闲分区表-------"<<endl;
    for(int i = 0;i<kongXianFenQuTable.size();i++){
        cout<<"起始地址:"<<kongXianFenQuTable[i]->startAddr<<", 大小(Mb):"<<kongXianFenQuTable[i]->size<<endl;
    }


    cout<<"-------已使用分区表------"<<endl;
    if(usedTable.size() == 0){
        cout<<"        无"<<endl;
    }else{
        for(int i = 0;i<usedTable.size();i++){
            cout<<"进程号:"<<i<<"起始地址:"<<usedTable[i]->startAddr<<", 大小(Mb):"<<usedTable[i]->size<<endl;
        }
    }
}
//  分配内存
void allocateRam(int ram){//ram为申请的内存大小
    // 检查申请的内存大小是否符合
    if(ram <= 0 || ram > MAX_SIZE){
        cout<<"申请内存的大小不能小于等于0或大于最大内存4096Mb!!"<<endl;
        return;
    }

    // 检查是否还有空闲内存分区
    if(kongXianFenQuTable.size() == 0){
        cout<<"内存分区已经没了，申请内存失败"<<endl;
        return;
    }


    // 寻找满足申请内存大小的分区
    // 创建一个链结点的指针，以方便遍历整条空闲分区链
    KongXianLian *chainPtr = chain;
    KongXianFenQu *fenQuPtr = chainPtr->fenqu;
    bool isFound = false;

    do{
        if((fenQuPtr->size - ram) >= MIN_SIZE){// 找到所需大小的分区
            isFound = true;
            break;
        }else{
            chainPtr = chainPtr->next;// 指向下一个链结点
            fenQuPtr = chainPtr->fenqu;// 获取下一个链结点指向的分区
        }
    }while(chainPtr != chain);// 因为是双向链，所以如果链回到了链的开头，说明就遍历完了一次

    //没找到一块满足大小的内存分区
    if(isFound == false){
        cout<<"内存空间不够，申请失败"<<endl;
        return;
    }

    //分配内存，并修改空闲分区表的信息
    //创建一个已使用分区
    usedFenQu *usedPtr = new usedFenQu;
    usedPtr->startAddr = fenQuPtr->startAddr;// 已使用分区的起始地址为该空闲分区的起始地址
    usedPtr->size = ram;// 已使用分区的内存大小为申请的内存大小
    usedTable.push_back(usedPtr);// 将该已使用分区装入已使用内存表


    // 从空闲分区表中，找到要修改的内存分区信息
    int pos = 0;
    for(int i = 0;i < kongXianFenQuTable.size();i++){
        if(kongXianFenQuTable[i]->startAddr == usedPtr->startAddr){
            pos = i;
            break;
        }
    }
    // 若空闲分区刚好分配完，就从空闲分区表中删除该分区，并且从空闲分区链中删除指向该空闲分区的指针
    if(fenQuPtr->size - ram == MIN_SIZE){
        kongXianFenQuTable.erase(kongXianFenQuTable.begin() + pos);//删除空闲分区表中，该分区的信息
        if(kongXianFenQuTable.size() == 0){
            chain = NULL;
        }else{
            // chainPtr为找到的满足内存大小的分区指针，现在chainPtr指向的那个分区已经被删除，所以链表头部chain要往后移，即为chainPtr->next
            if(chainPtr == chain){
                chain = chainPtr->next;
            }
            chainPtr->pre->next = chainPtr->next;
            chainPtr->next->pre = chainPtr->pre;
        }
        delete chainPtr;
    }else{// 分配内存后还有剩余空间
        fenQuPtr->size -= ram;
        fenQuPtr->startAddr += ram;
    }
}

// 回收内存
void release(int index){// index为进程号
    if(index<0||index>=usedTable.size()){
        cout<<"输入的进程号不能小于0或大于已经使用内存表中的最大进程号!!"<<endl;
        return;
    }
    // 找到想要回收的分区
    usedFenQu *usedPtr = usedTable[index];

    // 如果空闲分区链为空，则直接创建一个新的空闲分区并插入
    if(chain == NULL){
        // 创建一个空闲分区
        KongXianFenQu *fenQuNode = new KongXianFenQu;
        fenQuNode->startAddr = usedPtr->startAddr;
        fenQuNode->size = usedPtr->size;
        // 装入空闲分区表中
        kongXianFenQuTable.push_back(fenQuNode);

        // 创建一个空闲分区链结点
        KongXianLian *chainNode = new KongXianLian;
        chainNode->fenqu = fenQuNode;
        chainNode->next = chainNode;
        chainNode->pre = chainNode;

        chain = chainNode;

        // 从已使用内存表中移出
        usedTable.erase(usedTable.begin() + index);
        return;
    }

    // 若空闲链不是为空
    // 创建一个遍历空闲链的指针
    KongXianLian *chainPtr = chain;
    // 得到链结点指向的分区
    KongXianFenQu *fenquPtr = chainPtr->fenqu;
    do{
        // chainPtr: 前分区
        // usedPtr: 回收区
        // chainPtr->next: 后分区

        // 回收区与前后分区相连，三个分区合并
        if((fenquPtr->startAddr + fenquPtr->size == usedPtr->startAddr) && (usedPtr->startAddr + usedPtr->size == chainPtr->next->fenqu->startAddr)){
            for(int i=0;i<kongXianFenQuTable.size();i++){
                // 找到后分区，并取消后分区的表项
                if(chainPtr->next->fenqu->startAddr == kongXianFenQuTable[i]->startAddr){
                    kongXianFenQuTable.erase(kongXianFenQuTable.begin() + i);
                }
            }

            chainPtr->next->next->pre = chainPtr;
            chainPtr->next = chainPtr->next->next;
            usedTable.erase(usedTable.begin() + index);
            return;
        }

        //与前面空闲分区相邻，回收区与前分区合并，修改前分区的大小
        if(fenquPtr->startAddr + fenquPtr->size == usedPtr->startAddr){
            fenquPtr->size += usedPtr->size;

            usedTable.erase(usedTable.begin() + index);
            return;
        }

        //与后面空闲分区相邻，两个分区合并，用回收区的首址作为新空闲区的首址，大小为两者之和
        if(usedPtr->startAddr + usedPtr->size == fenquPtr->startAddr){
            fenquPtr->startAddr = usedPtr->startAddr;
            fenquPtr->size += usedPtr->size;
            usedTable.erase(usedTable.begin() + index);
            return;
        }

        //指向下一个空闲分区
        chainPtr = chainPtr->next;
        fenquPtr = chainPtr->fenqu;
    }while(chainPtr != chain);

    // 回收区既不与前分区邻接，也不与后分区邻接。应为回收区单独建立一个新表项，填写回收区的首址和大小，并根据其首址插入到空闲链中的适当位置
    chainPtr = chain;
    fenquPtr = chainPtr->fenqu;

    //新建一个空闲分区
    KongXianFenQu *newFenqu = new KongXianFenQu;
    newFenqu->size = usedPtr->size;
    newFenqu->startAddr = usedPtr->startAddr;

    // 根据其首址插入到空闲链中的适当位置
    do{
        // 找到一个起始地址比新分区起始地址大的空闲分区
        if(fenquPtr->startAddr > usedPtr->startAddr){
            break;
        }else{
            chainPtr = chainPtr->next;
            fenquPtr = chainPtr->fenqu;
        }
    }while(chainPtr != chain);
    cout<<"插入点位于:"<<fenquPtr->startAddr<<endl;
    // 就是把起始地址比新分区起始地址大的其他空闲分区往后移
    for(int i=0;i<kongXianFenQuTable.size();i++){
        if(kongXianFenQuTable[i]->startAddr == fenquPtr->startAddr){
            cout<<"找到插入点:"<<kongXianFenQuTable[i]->startAddr<<endl;
            kongXianFenQuTable.insert(kongXianFenQuTable.begin() + i, newFenqu);
            break;
        }
    }

    // 插入到双向链表中

    // 新建一个链结点
    KongXianLian *newChainnode = new KongXianLian;

    newChainnode->fenqu = newFenqu;
    newChainnode->next = chainPtr;
    newChainnode->pre = chainPtr->pre;
    chainPtr->pre->next = newChainnode;
    chainPtr->pre = newChainnode;

    // 如果链结点指针指的是链头，那么链头就为新的空闲分区
    if(chainPtr == chain){
        chain = newChainnode;
    }

    usedTable.erase(usedTable.begin() + index);
}

// 控制台
void terminal()
{
    char cmdstr[32];
    int input;
    while (1)
    {
        printf("cmd: ");
        scanf("%s", cmdstr);

        if (!strcmp(cmdstr, "quit"))
        {
            return;
        }

        if (!strcmp(cmdstr, "ps"))
        {
            ps();
            continue;
        }

        if (!strcmp(cmdstr, "allocate"))
        {
            printf("要申请的内存大小(kb): ");
            scanf("%d", &input);
            allocateRam(input);
            ps();
            continue;
        }

        if (!strcmp(cmdstr, "release"))
        {
            printf("要释放内存的进程号: ");
            scanf("%d", &input);
            release(input);
            ps();
            continue;
        }

        printf("cmd: 未知的命令！\n");
    }
}

int main(){
    init();
    ps();
    cout<<"命令介绍:"<<endl;
    cout<<"ps: 查看空闲分区表和已使用分区表"<<endl;
    cout<<"allocate: 申请一个内存"<<endl;
    cout<<"release: 回收一个内存"<<endl;
    cout<<"------------------------------------"<<endl;
    terminal();
    return 0;
}


