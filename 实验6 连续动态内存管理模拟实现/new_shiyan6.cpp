#include<iostream>
#include<vector>
#include<cstdio>
#include<cstring>
using namespace std;
#define MAX_SIZE 4096
#define MIN_SIZE 0

// 空闲分区
struct KongXianFenQu{
    int startAddr;// 起始地址
    int size;// 大小
};
// 空闲分区链  双向链
struct KongXianLian{
    KongXianFenQu *fenqu;// 空闲链指向的空闲分区
    KongXianLian *pre;// 上一个空闲分区
    KongXianLian *next;// 下一个空闲分区
};
// 已使用分区
struct usedFenQu{
    int startAddr;// 起始地址
    int size;// 大小
};
// 空闲分区表
vector<KongXianFenQu *> kongXianFenQuTable;
// 已使用分区表
vector<usedFenQu *> usedTable;
// 空闲分区链
KongXianLian *chain;



// 初始化
void init(){
    // 创建一个空闲分区
    KongXianFenQu *fenqu = new KongXianFenQu;
    fenqu->startAddr = 0;
    fenqu->size = MAX_SIZE;

    // 空闲分区放进空闲分区表
    kongXianFenQuTable.push_back(fenqu);

    // 初始化空闲分区链
    KongXianLian *chainNode = new KongXianLian;
    chainNode->fenqu = fenqu;
    chainNode->next = chainNode;
    chainNode->pre = chainNode;

    chain = chainNode;
}

// 输出数据
void ps(){
    cout<<endl;
    cout<<"-------空闲分区表-------"<<endl;
    for(int i=0;i<kongXianFenQuTable.size();i++){
        cout<<"起始地址:"<<kongXianFenQuTable[i]->startAddr<<", 大小:"<<kongXianFenQuTable[i]->size<<"MB"<<endl;
    }
    cout<<"-------已使用分区表------"<<endl;
    if(usedTable.size()==0){
        cout<<"----------无---------"<<endl;
    }else{
        for(int i=0;i<usedTable.size();i++){
            cout<<"进程号:"<<i<<", 起始地址:"<<usedTable[i]->startAddr<<", 大小:"<<usedTable[i]->size<<"MB"<<endl;
        }
    }
    cout<<endl;
    cout<<endl;
    cout<<endl;
}

// 申请内存
// ram: 申请内存的大小
void allocate(int ram){
    if(ram>MAX_SIZE || ram<=0){
        cout<<"申请内存的大小不能小于等于0或大于最大内存4096MB!!"<<endl;
        return;
    }

    //检查是否还有空闲分区
    if(kongXianFenQuTable.size()==0){
        cout<<"空闲分区已经没了，申请内存失败"<<endl;
        return;
    }



    //寻找满足申请内存大小的分区
    KongXianLian *chainPtr = chain;
    bool isFound = false;

    do{
        if((chainPtr->fenqu->size - ram) >= MIN_SIZE){
            isFound = true;
            break;
        }else{
            chainPtr = chainPtr->next;
        }
    }while(chainPtr != chain);// 遍历一遍链表

    if(isFound == false){
        cout<<"找不到满足"<<ram<<"MB的空闲分区，申请失败"<<endl;
        return;
    }

    // 分配内存，并修改内存分区表
    // 记录已使用的内存
    usedFenQu *used = new usedFenQu;
    used->size = ram;
    used->startAddr = chainPtr->fenqu->startAddr;
    usedTable.push_back(used);


    int pos = 0;
    for(int i=0;i<kongXianFenQuTable.size();i++){
        if(used->startAddr == kongXianFenQuTable[i]->startAddr){
            pos = i;
            break;
        }
    }

    // 若分区刚好分配完，就从分区链中移出
    if(chainPtr->fenqu->size == ram){
        kongXianFenQuTable.erase(kongXianFenQuTable.begin() + pos);// 从分区表中移出
        if(kongXianFenQuTable.size() == 0){
            chain = NULL;
        }else{

            // 要删除chainPtr, chain头指针要往后移
            if(chain == chainPtr){
                chain = chainPtr->next;
            }

            chainPtr->next->pre = chainPtr->pre;
            chainPtr->pre->next = chainPtr->next;
        }
        delete chainPtr;
    }else{// 分配后还有剩余空间
        chainPtr->fenqu->size -= ram;
        chainPtr->fenqu->startAddr += ram;// 起始地址往后移
    }
}



// 回收内存
// index: 进程号
void release(int index){
    if(index<0 || index>usedTable.size()){
        cout<<"输入的进程号不能小于0或大于已经使用内存表中的最大进程号!!"<<endl;
        return;
    }

    // 找到想要回收的分区
    usedFenQu *used = usedTable[index];

    // 如果空闲链为空，则新建一个空闲分区
    if(chain == NULL){
        KongXianFenQu *fenqu = new KongXianFenQu;
        fenqu->size = used->size;
        fenqu->startAddr = used->startAddr;
        kongXianFenQuTable.push_back(fenqu);

        // 新建一个链
        KongXianLian *chainNode = new KongXianLian;
        chainNode->fenqu = fenqu;
        chainNode->next = chainNode;
        chainNode->pre = chainNode;

        chain = chainNode;

        // 已使用内存表中移出分区
        usedTable.erase(usedTable.begin() + index);
    }else{// 若不为空
        KongXianLian *chainPtr = chain;// 创建一个链遍历指针
        do{
            // used: 回收区
            // chainPtr: 前分区
            // chainPtr->next: 后分区


            // 回收区与前分区邻接
            if(chainPtr->fenqu->startAddr + chainPtr->fenqu->size == used->startAddr){
                chainPtr->fenqu->size += used->size;

                usedTable.erase(usedTable.begin() + index);
                return;
            }

            // 回收区与后分区邻接
            if(used->startAddr + used->size == chainPtr->fenqu->startAddr){
                chainPtr->fenqu->startAddr = used->startAddr;
                chainPtr->fenqu->size += used->size;
                usedTable.erase(usedTable.begin() + index);
                return;
            }


            // 与前分区邻接，也与后分区邻接
            if((chainPtr->fenqu->startAddr + chainPtr->fenqu->size == used->startAddr) && (used->startAddr + used->size == chainPtr->next->fenqu->startAddr)){
                for(int i=0;i<kongXianFenQuTable.size();i++){
                    // 找到后分区，并取消后分区，三区合并
                    if(chainPtr->next->fenqu->startAddr == kongXianFenQuTable[i]->startAddr){
                        kongXianFenQuTable.erase(kongXianFenQuTable.begin() + i);
                    }
                }
                chainPtr->next->next->pre = chain;
                chainPtr->next = chainPtr->next->next;
                usedTable.erase(usedTable.begin() + index);
                return;
            }
            chainPtr = chainPtr->next;
        }while(chainPtr != chain);// 遍历链一遍

        // 既不与前分区邻接，也不与后分区邻接
        // 单独建立一个新分区
        chainPtr = chain;

        KongXianFenQu *newfenqu = new KongXianFenQu;
        newfenqu->startAddr = used->startAddr;
        newfenqu->size = used->size;

        // 根据首址插入链的适当位置
        do{
            if(chainPtr->fenqu->startAddr > newfenqu->startAddr){
                break;
            }else{
                chainPtr = chainPtr->next;
            }
        }while(chainPtr != chain);
        for(int i=0;i<kongXianFenQuTable.size();i++){
            if(kongXianFenQuTable[i]->startAddr == chainPtr->fenqu->startAddr){
                kongXianFenQuTable.insert(kongXianFenQuTable.begin() + i, newfenqu);
                break;
            }
        }

        // 插入到链表中

        // 新建一个链分区
        KongXianLian *newchain = new KongXianLian;
        newchain->fenqu = newfenqu;
        newchain->next = chainPtr;
        newchain->pre = chainPtr->pre;
        chainPtr->pre->next = newchain;
        chainPtr->pre = newchain;

        if(chainPtr == chain){
            chain = newchain;
        }
        usedTable.erase(usedTable.begin() + index);
    }
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
            printf("申请的内存大小(MB): ");
            cin>>input;
            allocate(input);
            ps();
            continue;
        }

        if (!strcmp(cmdstr, "release"))
        {
            printf("回收内存分区的进程号: ");
            cin>>input;
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
    cout<<"quit: 退出"<<endl;
    cout<<"------------------------------------"<<endl;
    terminal();
    return 0;
}






