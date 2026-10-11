#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 20
#define ID_LEN 20

// 乘客订单节点
typedef struct Order
{
    int order_id;
    char name[NAME_LEN];
    char id_card[ID_LEN];
    int need_ticket;
    struct Order* next;
} Order;

// 队列结构
typedef struct Queue
{
    Order* front;
    Order* rear;
} Queue;

// 创建空队列
Queue* CreateQueue()
{
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = NULL;
    q->rear = NULL;
    return q;
}

// 判断队列是否为空
int IsEmpty(Queue* q)
{
    return q->front == NULL;
}

// 入队：新增订单排到队尾
void EnQueue(Queue* q, Order* new_order)
{
    if (IsEmpty(q))
    {
        q->front = new_order;
        q->rear = new_order;
    }
    else
    {
        q->rear->next = new_order;
        q->rear = new_order;
    }
    new_order->next = NULL;
}

// 出队：取出队头用户
Order* DeQueue(Queue* q)
{
    if (IsEmpty(q))
        return NULL;
    Order* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;
    return temp;
}

// 1. 添加候补订单
void AddOrder(Queue* q, int id)
{
    char name[NAME_LEN];
    char id_card[ID_LEN];
    int need;

    printf("\n===== 新增候补订单 =====\n");
    printf("请输入乘客姓名：");
    fgets(name, NAME_LEN, stdin);
    name[strcspn(name, "\n")] = '\0';
    if (strlen(name) == 0)
    {
        printf("姓名不能为空，添加取消！\n");
        return;
    }

    printf("请输入身份证号：");
    fgets(id_card, ID_LEN, stdin);
    id_card[strcspn(id_card, "\n")] = '\0';
    if (strlen(id_card) == 0)
    {
        printf("身份证号不能为空，添加取消！\n");
        return;
    }

    printf("请输入需要票数（正整数）：");
    if (scanf("%d", &need) != 1 || need <= 0)
    {
        while (getchar() != '\n');
        printf("票数输入非法，订单创建失败！\n");
        return;
    }
    getchar();

    Order* new_ord = (Order*)malloc(sizeof(Order));
    if (new_ord == NULL)
    {
        fprintf(stderr, "内存分配失败！\n");
        return;
    }
    new_ord->order_id = id;
    strcpy(new_ord->name, name);
    strcpy(new_ord->id_card, id_card);
    new_ord->need_ticket = need;

    EnQueue(q, new_ord);
    printf("添加成功！订单号：%d，姓名：%s，需票：%d张\n", id, name, need);
}

// 2. 发放车票
void SendTicket(Queue* q)
{
    if (IsEmpty(q))
    {
        printf("\n队列为空，暂无候补用户！\n");
        return;
    }

    int k;
    printf("\n===== 发放余票 =====\n");
    printf("请输入本次释放车票总数：");
    if (scanf("%d", &k) != 1 || k <= 0)
    {
        while (getchar() != '\n');
        printf("输入非法，返回主菜单\n");
        return;
    }
    getchar();

    int remain = k;
    int success = 0;
    printf("开始分配车票，总票数：%d\n", k);

    while (remain > 0 && !IsEmpty(q))
    {
        Order* head = q->front;
        if (head->need_ticket <= remain)
        {
            Order* suc = DeQueue(q);
            printf("候补成功 | 订单号：%d 姓名：%s 分配票数：%d\n",
                   suc->order_id, suc->name, suc->need_ticket);
            remain -= suc->need_ticket;
            success++;
            free(suc);
        }
        else
        {
            break;
        }
    }

    printf("\n分配完成！成功出票 %d 单，剩余未分配车票：%d\n", success, remain);
    if (!IsEmpty(q))
        printf("仍有乘客排队等待下次放票\n");
    else
        printf("所有候补用户全部出票完成\n");
}

// 3. 查看排队所有人
void ShowQueue(Queue* q)
{
    printf("\n===== 当前候补排队队列 =====\n");
    if (IsEmpty(q))
    {
        printf("暂无排队订单\n");
        return;
    }
    Order* p = q->front;
    int num = 1;
    while (p != NULL)
    {
        printf("第%d位 | 订单号：%d | 姓名：%s | 需票：%d张\n",
               num++, p->order_id, p->name, p->need_ticket);
        p = p->next;
    }
}

// 释放全部动态内存
void FreeAll(Queue* q)
{
    while (!IsEmpty(q))
    {
        Order* t = DeQueue(q);
        free(t);
    }
    free(q);
}

int main()
{
    Queue* queue = CreateQueue();
    int choice;
    int order_id = 1;

    while (1)
    {
        printf("\n================ 12306车票候补系统 ================\n");
        printf("1. 新增候补订单\n");
        printf("2. 发放余票\n");
        printf("3. 查看排队队列\n");
        printf("4. 退出程序\n");
        printf("===================================================\n");
        printf("请输入操作序号(1~4)：");

        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n');
            printf("输入错误！请输入数字1-4\n");
            continue;
        }
        getchar();

        switch (choice)
        {
        case 1:
            AddOrder(queue, order_id++);
            break;
        case 2:
            SendTicket(queue);
            break;
        case 3:
            ShowQueue(queue);
            break;
        case 4:
            FreeAll(queue);
            printf("程序正常退出，内存全部释放\n");
            return 0;
        default:
            printf("序号无效，请选择1~4\n");
        }
    }
}
