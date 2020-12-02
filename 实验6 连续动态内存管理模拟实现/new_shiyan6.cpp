#include<iostream>
#include<vector>
#include<cstdio>
#include<cstring>
using namespace std;
#define MAX_SIZE 4096
#define MIN_SIZE 0

// ���з���
struct KongXianFenQu{
    int startAddr;// ��ʼ��ַ
    int size;// ��С
};
// ���з�����  ˫����
struct KongXianLian{
    KongXianFenQu *fenqu;// ������ָ��Ŀ��з���
    KongXianLian *pre;// ��һ�����з���
    KongXianLian *next;// ��һ�����з���
};
// ��ʹ�÷���
struct usedFenQu{
    int startAddr;// ��ʼ��ַ
    int size;// ��С
};
// ���з�����
vector<KongXianFenQu *> kongXianFenQuTable;
// ��ʹ�÷�����
vector<usedFenQu *> usedTable;
// ���з�����
KongXianLian *chain;



// ��ʼ��
void init(){
    // ����һ�����з���
    KongXianFenQu *fenqu = new KongXianFenQu;
    fenqu->startAddr = 0;
    fenqu->size = MAX_SIZE;

    // ���з����Ž����з�����
    kongXianFenQuTable.push_back(fenqu);

    // ��ʼ�����з�����
    KongXianLian *chainNode = new KongXianLian;
    chainNode->fenqu = fenqu;
    chainNode->next = chainNode;
    chainNode->pre = chainNode;

    chain = chainNode;
}

// �������
void ps(){
    cout<<endl;
    cout<<"-------���з�����-------"<<endl;
    for(int i=0;i<kongXianFenQuTable.size();i++){
        cout<<"��ʼ��ַ:"<<kongXianFenQuTable[i]->startAddr<<", ��С:"<<kongXianFenQuTable[i]->size<<"MB"<<endl;
    }
    cout<<"-------��ʹ�÷�����------"<<endl;
    if(usedTable.size()==0){
        cout<<"----------��---------"<<endl;
    }else{
        for(int i=0;i<usedTable.size();i++){
            cout<<"���̺�:"<<i<<", ��ʼ��ַ:"<<usedTable[i]->startAddr<<", ��С:"<<usedTable[i]->size<<"MB"<<endl;
        }
    }
    cout<<endl;
    cout<<endl;
    cout<<endl;
}

// �����ڴ�
// ram: �����ڴ�Ĵ�С
void allocate(int ram){
    if(ram>MAX_SIZE || ram<=0){
        cout<<"�����ڴ�Ĵ�С����С�ڵ���0���������ڴ�4096MB!!"<<endl;
        return;
    }

    //����Ƿ��п��з���
    if(kongXianFenQuTable.size()==0){
        cout<<"���з����Ѿ�û�ˣ������ڴ�ʧ��"<<endl;
        return;
    }



    //Ѱ�����������ڴ��С�ķ���
    KongXianLian *chainPtr = chain;
    bool isFound = false;

    do{
        if((chainPtr->fenqu->size - ram) >= MIN_SIZE){
            isFound = true;
            break;
        }else{
            chainPtr = chainPtr->next;
        }
    }while(chainPtr != chain);// ����һ������

    if(isFound == false){
        cout<<"�Ҳ�������"<<ram<<"MB�Ŀ��з���������ʧ��"<<endl;
        return;
    }

    // �����ڴ棬���޸��ڴ������
    // ��¼��ʹ�õ��ڴ�
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

    // �������պ÷����꣬�ʹӷ��������Ƴ�
    if(chainPtr->fenqu->size == ram){
        kongXianFenQuTable.erase(kongXianFenQuTable.begin() + pos);// �ӷ��������Ƴ�
        if(kongXianFenQuTable.size() == 0){
            chain = NULL;
        }else{

            // Ҫɾ��chainPtr, chainͷָ��Ҫ������
            if(chain == chainPtr){
                chain = chainPtr->next;
            }

            chainPtr->next->pre = chainPtr->pre;
            chainPtr->pre->next = chainPtr->next;
        }
        delete chainPtr;
    }else{// �������ʣ��ռ�
        chainPtr->fenqu->size -= ram;
        chainPtr->fenqu->startAddr += ram;// ��ʼ��ַ������
    }
}



// �����ڴ�
// index: ���̺�
void release(int index){
    if(index<0 || index>usedTable.size()){
        cout<<"����Ľ��̺Ų���С��0������Ѿ�ʹ���ڴ���е������̺�!!"<<endl;
        return;
    }

    // �ҵ���Ҫ���յķ���
    usedFenQu *used = usedTable[index];

    // ���������Ϊ�գ����½�һ�����з���
    if(chain == NULL){
        KongXianFenQu *fenqu = new KongXianFenQu;
        fenqu->size = used->size;
        fenqu->startAddr = used->startAddr;
        kongXianFenQuTable.push_back(fenqu);

        // �½�һ����
        KongXianLian *chainNode = new KongXianLian;
        chainNode->fenqu = fenqu;
        chainNode->next = chainNode;
        chainNode->pre = chainNode;

        chain = chainNode;

        // ��ʹ���ڴ�����Ƴ�����
        usedTable.erase(usedTable.begin() + index);
    }else{// ����Ϊ��
        KongXianLian *chainPtr = chain;// ����һ��������ָ��
        do{
            // used: ������
            // chainPtr: ǰ����
            // chainPtr->next: �����


            // ��������ǰ�����ڽ�
            if(chainPtr->fenqu->startAddr + chainPtr->fenqu->size == used->startAddr){
                chainPtr->fenqu->size += used->size;

                usedTable.erase(usedTable.begin() + index);
                return;
            }

            // �������������ڽ�
            if(used->startAddr + used->size == chainPtr->fenqu->startAddr){
                chainPtr->fenqu->startAddr = used->startAddr;
                chainPtr->fenqu->size += used->size;
                usedTable.erase(usedTable.begin() + index);
                return;
            }


            // ��ǰ�����ڽӣ�Ҳ�������ڽ�
            if((chainPtr->fenqu->startAddr + chainPtr->fenqu->size == used->startAddr) && (used->startAddr + used->size == chainPtr->next->fenqu->startAddr)){
                for(int i=0;i<kongXianFenQuTable.size();i++){
                    // �ҵ����������ȡ��������������ϲ�
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
        }while(chainPtr != chain);// ������һ��

        // �Ȳ���ǰ�����ڽӣ�Ҳ���������ڽ�
        // ��������һ���·���
        chainPtr = chain;

        KongXianFenQu *newfenqu = new KongXianFenQu;
        newfenqu->startAddr = used->startAddr;
        newfenqu->size = used->size;

        // ������ַ���������ʵ�λ��
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

        // ���뵽������

        // �½�һ��������
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

// ����̨
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
            printf("������ڴ��С(MB): ");
            cin>>input;
            allocate(input);
            ps();
            continue;
        }

        if (!strcmp(cmdstr, "release"))
        {
            printf("�����ڴ�����Ľ��̺�: ");
            cin>>input;
            release(input);
            ps();
            continue;
        }

        printf("cmd: δ֪�����\n");
    }
}

int main(){
    init();
    ps();
    cout<<"�������:"<<endl;
    cout<<"ps: �鿴���з��������ʹ�÷�����"<<endl;
    cout<<"allocate: ����һ���ڴ�"<<endl;
    cout<<"release: ����һ���ڴ�"<<endl;
    cout<<"quit: �˳�"<<endl;
    cout<<"------------------------------------"<<endl;
    terminal();
    return 0;
}






