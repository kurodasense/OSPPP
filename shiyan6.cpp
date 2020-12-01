#include<iostream>
#include<vector>
#include<cstdio>
#include<cstring>
using namespace std;
# define MAX_SIZE 4096
# define MIN_SIZE 0
//  ���з���
struct KongXianFenQu{
    int startAddr;//���з�����ʼ��ַ
    int size;//���з�����С
};
//  ������
struct KongXianLian{
    KongXianFenQu *fenqu;//���������ŵĿ��з���
    KongXianLian *pre;//��һ�����з���
    KongXianLian *next;//��һ�����з���
};
//  ��ʹ�õ��ڴ����
struct usedFenQu{
    int startAddr;// ��ʼ��ַ
    int size;// ��С
};

//���з�����
vector<KongXianFenQu *> kongXianFenQuTable;
//��ʹ�÷�����
vector<usedFenQu *> usedTable;
//���з�����
KongXianLian *chain;

//  ��ʼ��
void init(){
    //����һ�����з���
    KongXianFenQu *tableNode = new KongXianFenQu;
    tableNode->startAddr = 0;//��ʼ��ʼ��ַΪ0
    tableNode->size = MAX_SIZE;//�����ڴ��СΪMAX_SIZE

    //�ѿ��з����Ž����з�������
    kongXianFenQuTable.push_back(tableNode);

    //��ʼ�����з�����
    //����һ�����Ľ��
    KongXianLian *chainNode = new KongXianLian;
    chainNode->fenqu = tableNode;
    chainNode->next = chainNode;
    chainNode->pre = chainNode;
    chain = chainNode;
}
//  �鿴�ڴ�ʹ�����
void ps(){
    cout<<"-------���з�����-------"<<endl;
    for(int i = 0;i<kongXianFenQuTable.size();i++){
        cout<<"��ʼ��ַ:"<<kongXianFenQuTable[i]->startAddr<<", ��С(Mb):"<<kongXianFenQuTable[i]->size<<endl;
    }


    cout<<"-------��ʹ�÷�����------"<<endl;
    if(usedTable.size() == 0){
        cout<<"        ��"<<endl;
    }else{
        for(int i = 0;i<usedTable.size();i++){
            cout<<"���̺�:"<<i<<"��ʼ��ַ:"<<usedTable[i]->startAddr<<", ��С(Mb):"<<usedTable[i]->size<<endl;
        }
    }
}
//  �����ڴ�
void allocateRam(int ram){//ramΪ������ڴ��С
    // ���������ڴ��С�Ƿ����
    if(ram <= 0 || ram > MAX_SIZE){
        cout<<"�����ڴ�Ĵ�С����С�ڵ���0���������ڴ�4096Mb!!"<<endl;
        return;
    }

    // ����Ƿ��п����ڴ����
    if(kongXianFenQuTable.size() == 0){
        cout<<"�ڴ�����Ѿ�û�ˣ������ڴ�ʧ��"<<endl;
        return;
    }


    // Ѱ�����������ڴ��С�ķ���
    // ����һ��������ָ�룬�Է�������������з�����
    KongXianLian *chainPtr = chain;
    KongXianFenQu *fenQuPtr = chainPtr->fenqu;
    bool isFound = false;

    do{
        if((fenQuPtr->size - ram) >= MIN_SIZE){// �ҵ������С�ķ���
            isFound = true;
            break;
        }else{
            chainPtr = chainPtr->next;// ָ����һ�������
            fenQuPtr = chainPtr->fenqu;// ��ȡ��һ�������ָ��ķ���
        }
    }while(chainPtr != chain);// ��Ϊ��˫����������������ص������Ŀ�ͷ��˵���ͱ�������һ��

    //û�ҵ�һ�������С���ڴ����
    if(isFound == false){
        cout<<"�ڴ�ռ䲻��������ʧ��"<<endl;
        return;
    }

    //�����ڴ棬���޸Ŀ��з��������Ϣ
    //����һ����ʹ�÷���
    usedFenQu *usedPtr = new usedFenQu;
    usedPtr->startAddr = fenQuPtr->startAddr;// ��ʹ�÷�������ʼ��ַΪ�ÿ��з�������ʼ��ַ
    usedPtr->size = ram;// ��ʹ�÷������ڴ��СΪ������ڴ��С
    usedTable.push_back(usedPtr);// ������ʹ�÷���װ����ʹ���ڴ��


    // �ӿ��з������У��ҵ�Ҫ�޸ĵ��ڴ������Ϣ
    int pos = 0;
    for(int i = 0;i < kongXianFenQuTable.size();i++){
        if(kongXianFenQuTable[i]->startAddr == usedPtr->startAddr){
            pos = i;
            break;
        }
    }
    // �����з����պ÷����꣬�ʹӿ��з�������ɾ���÷��������Ҵӿ��з�������ɾ��ָ��ÿ��з�����ָ��
    if(fenQuPtr->size - ram == MIN_SIZE){
        kongXianFenQuTable.erase(kongXianFenQuTable.begin() + pos);//ɾ�����з������У��÷�������Ϣ
        if(kongXianFenQuTable.size() == 0){
            chain = NULL;
        }else{
            // chainPtrΪ�ҵ��������ڴ��С�ķ���ָ�룬����chainPtrָ����Ǹ������Ѿ���ɾ������������ͷ��chainҪ�����ƣ���ΪchainPtr->next
            if(chainPtr == chain){
                chain = chainPtr->next;
            }
            chainPtr->pre->next = chainPtr->next;
            chainPtr->next->pre = chainPtr->pre;
        }
        delete chainPtr;
    }else{// �����ڴ����ʣ��ռ�
        fenQuPtr->size -= ram;
        fenQuPtr->startAddr += ram;
    }
}

// �����ڴ�
void release(int index){// indexΪ���̺�
    if(index<0||index>=usedTable.size()){
        cout<<"����Ľ��̺Ų���С��0������Ѿ�ʹ���ڴ���е������̺�!!"<<endl;
        return;
    }
    // �ҵ���Ҫ���յķ���
    usedFenQu *usedPtr = usedTable[index];

    // ������з�����Ϊ�գ���ֱ�Ӵ���һ���µĿ��з���������
    if(chain == NULL){
        // ����һ�����з���
        KongXianFenQu *fenQuNode = new KongXianFenQu;
        fenQuNode->startAddr = usedPtr->startAddr;
        fenQuNode->size = usedPtr->size;
        // װ����з�������
        kongXianFenQuTable.push_back(fenQuNode);

        // ����һ�����з��������
        KongXianLian *chainNode = new KongXianLian;
        chainNode->fenqu = fenQuNode;
        chainNode->next = chainNode;
        chainNode->pre = chainNode;

        chain = chainNode;

        // ����ʹ���ڴ�����Ƴ�
        usedTable.erase(usedTable.begin() + index);
        return;
    }

    // ������������Ϊ��
    // ����һ��������������ָ��
    KongXianLian *chainPtr = chain;
    // �õ������ָ��ķ���
    KongXianFenQu *fenquPtr = chainPtr->fenqu;
    do{
        // chainPtr: ǰ����
        // usedPtr: ������
        // chainPtr->next: �����

        // ��������ǰ��������������������ϲ�
        if((fenquPtr->startAddr + fenquPtr->size == usedPtr->startAddr) && (usedPtr->startAddr + usedPtr->size == chainPtr->next->fenqu->startAddr)){
            for(int i=0;i<kongXianFenQuTable.size();i++){
                // �ҵ����������ȡ��������ı���
                if(chainPtr->next->fenqu->startAddr == kongXianFenQuTable[i]->startAddr){
                    kongXianFenQuTable.erase(kongXianFenQuTable.begin() + i);
                }
            }

            chainPtr->next->next->pre = chainPtr;
            chainPtr->next = chainPtr->next->next;
            usedTable.erase(usedTable.begin() + index);
            return;
        }

        //��ǰ����з������ڣ���������ǰ�����ϲ����޸�ǰ�����Ĵ�С
        if(fenquPtr->startAddr + fenquPtr->size == usedPtr->startAddr){
            fenquPtr->size += usedPtr->size;

            usedTable.erase(usedTable.begin() + index);
            return;
        }

        //�������з������ڣ����������ϲ����û���������ַ��Ϊ�¿���������ַ����СΪ����֮��
        if(usedPtr->startAddr + usedPtr->size == fenquPtr->startAddr){
            fenquPtr->startAddr = usedPtr->startAddr;
            fenquPtr->size += usedPtr->size;
            usedTable.erase(usedTable.begin() + index);
            return;
        }

        //ָ����һ�����з���
        chainPtr = chainPtr->next;
        fenquPtr = chainPtr->fenqu;
    }while(chainPtr != chain);

    // �������Ȳ���ǰ�����ڽӣ�Ҳ���������ڽӡ�ӦΪ��������������һ���±����д����������ַ�ʹ�С������������ַ���뵽�������е��ʵ�λ��
    chainPtr = chain;
    fenquPtr = chainPtr->fenqu;

    //�½�һ�����з���
    KongXianFenQu *newFenqu = new KongXianFenQu;
    newFenqu->size = usedPtr->size;
    newFenqu->startAddr = usedPtr->startAddr;

    // ��������ַ���뵽�������е��ʵ�λ��
    do{
        // �ҵ�һ����ʼ��ַ���·�����ʼ��ַ��Ŀ��з���
        if(fenquPtr->startAddr > usedPtr->startAddr){
            break;
        }else{
            chainPtr = chainPtr->next;
            fenquPtr = chainPtr->fenqu;
        }
    }while(chainPtr != chain);
    cout<<"�����λ��:"<<fenquPtr->startAddr<<endl;
    // ���ǰ���ʼ��ַ���·�����ʼ��ַ����������з���������
    for(int i=0;i<kongXianFenQuTable.size();i++){
        if(kongXianFenQuTable[i]->startAddr == fenquPtr->startAddr){
            cout<<"�ҵ������:"<<kongXianFenQuTable[i]->startAddr<<endl;
            kongXianFenQuTable.insert(kongXianFenQuTable.begin() + i, newFenqu);
            break;
        }
    }

    // ���뵽˫��������

    // �½�һ�������
    KongXianLian *newChainnode = new KongXianLian;

    newChainnode->fenqu = newFenqu;
    newChainnode->next = chainPtr;
    newChainnode->pre = chainPtr->pre;
    chainPtr->pre->next = newChainnode;
    chainPtr->pre = newChainnode;

    // ��������ָ��ָ������ͷ����ô��ͷ��Ϊ�µĿ��з���
    if(chainPtr == chain){
        chain = newChainnode;
    }

    usedTable.erase(usedTable.begin() + index);
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
            printf("Ҫ������ڴ��С(kb): ");
            scanf("%d", &input);
            allocateRam(input);
            ps();
            continue;
        }

        if (!strcmp(cmdstr, "release"))
        {
            printf("Ҫ�ͷ��ڴ�Ľ��̺�: ");
            scanf("%d", &input);
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
    cout<<"------------------------------------"<<endl;
    terminal();
    return 0;
}


