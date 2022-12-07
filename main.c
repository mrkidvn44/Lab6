#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

struct PAGE_S

{
    int number;
    int wait;
    int next;
    bool blank;
};

typedef struct PAGE_S PAGE;

// defaut value

void init_defaut(PAGE **List, int *n, char **f_list)
{
    *n = 11;
    const int df_list[11] = {2, 1, 5, 2, 1, 6, 7, 9, 0, 0, 7};
    *f_list = (char *)malloc(sizeof(char) * *n);
    *List = (PAGE *)malloc(sizeof(PAGE) * *n);
    for (int i = 0; i < *n; i++)
    {
        PAGE temp;
        temp.number = df_list[i];
        temp.blank = temp.wait = 0; // init wait time at 0;
        temp.next = *n;             // init next at the end, out of list
        (*List)[i] = temp;
        (*f_list)[i] = ' ';
    }
}

// maual input

void init_manual(PAGE **List, int *n, char **f_list)
{
    printf("--- Page Replacement algorithm ---\n");
    printf("Input number of reference pages:\n");
    scanf("%d", n);
    *f_list = (char *)malloc(sizeof(char) * (*n));
    *List = (PAGE *)malloc(sizeof(PAGE) * (*n));
    printf("Input sequence of %d pages:\n", *n);
    for (int i = 0; i < *n; i++)
    {
        int temp;
        scanf("%d", &temp);
        PAGE tmp;
        tmp.number = temp;
        tmp.blank = tmp.wait = 0;
        tmp.next = *n;
        (*List)[i] = tmp;
        (*f_list)[i] = ' ';
    }
}

// initialize table

void table_init(PAGE ***out_list, PAGE *List, int *n, int *m)
{
    // viet su ly truoc khi viet ham init nen tao cot truoc dong :v
    (*out_list) = (PAGE **)malloc(sizeof(PAGE *) * (*m));
    for (int i = 0; i < *m; i++)
        (*out_list)[i] = (PAGE *)malloc(sizeof(PAGE) * (*n));
    for (int i = 0; i < *n; i++)
    {
        PAGE blank;
        blank.blank = 1; // blank page is set at 1
        (*out_list)[0][i] = List[i];
        for (int j = 1; j < *m; j++)
            (*out_list)[j][i] = blank; // all page in queue from row 1 to n-1 is blank at first
    }
}

// first come first serve
// optimize this

void FIFO(PAGE **List, int *n, int *m, char *f_list, int *f_num)
{
    List[1][0] = List[0][0]; // first page always fault
    f_list[0] = '*';         // f_list store where fault happened to pirnt
    int cur_q = 1;           // cur_q is the first page at the head of queue
    *f_num = 1;
    for (int i = 1; i < *n; i++) // for loop goes through every page at index[0][i]
    {
        bool found = 0; // flag found to check if there already the page in queue
        for (int j = 1; j < *m; j++)
        {
            List[j][i] = List[j][i - 1];                                        // copy frame from the last time in queue
            if (List[j][i - 1].blank || List[j][i].number == List[0][i].number) // check if page is blank or equal
            {
                found = 1;
                if (List[j][i].blank)
                {
                    List[j][i] = List[0][i];
                    f_list[i] = '*';
                    (*f_num)++;
                    break; // if blank break because there is no more page in queue to copy
                }
            }
        }
        if (!found) // if not found replace the page at head of queue
        {
            List[cur_q][i] = List[0][i];
            f_list[i] = '*';
            cur_q++; // increment cur_q by 1
            (*f_num)++;
        }
        if (cur_q >= *m) // check if the cur_q loop back
            cur_q = 1;
    }
}

// optimal

void OPT(PAGE **List, int *n, int *m, char *f_list, int *f_num)
{
    List[1][0] = List[0][0];
    f_list[0] = '*';
    for (int k = 1; k < *n; k++)
        if (List[1][0].number == List[0][k].number) // update the next page with the same number
        {
            List[1][0].next = k;
            break;
        }
    *f_num = 1;
    for (int i = 1; i < *n; i++)
    {
        bool found = 0;
        for (int j = 1; j < *m; j++)
        {
            List[j][i] = List[j][i - 1];
            if (List[j][i].blank || List[j][i].number == List[0][i].number)
            {
                if (List[j][i].blank && found == 1)
                {
                    break;
                }
                else if (List[j][i].blank)
                {
                    f_list[i] = '*';
                    (*f_num)++;
                }
                List[j][i] = List[0][i];
                for (int k = i + 1; k < *n; k++)
                    if (List[j][i].number == List[0][k].number) // update the next page with the same number
                    {
                        List[j][i].next = k;
                        break;
                    }
                found = 1;
            }
        }
        if (!found)
        {
            int cur_max = 1; // find max in the queue
            for (int j = 2; j < *m; j++)
                if (List[cur_max][i].next < List[j][i].next)
                    cur_max = j;
            List[cur_max][i] = List[0][i];
            for (int k = i + 1; k < *n; k++)
                if (List[cur_max][i].number == List[0][k].number) // update the next page with the same number
                {
                    List[cur_max][i].next = k;
                    break;
                }
            (*f_num)++;
            f_list[i] = '*';
        }
    }
}

// last ...

void LRU(PAGE **List, int *n, int *m, char *f_list, int *f_num)
{
    List[1][0] = List[0][0];
    f_list[0] = '*';
    *f_num = 1;
    for (int i = 1; i < *n; i++)
    {
        bool found = 0;
        for (int j = 1; j < *m; j++)
        {
            List[j][i] = List[j][i - 1];
            List[j][i].wait++; // increment waiting time queue by 1
            if (List[j][i - 1].blank || List[j][i].number == List[0][i].number)
            {
                if (List[j][i].blank && found == 1)
                {
                    break;
                }
                else if (List[j][i].blank)
                {
                    f_list[i] = '*';
                    (*f_num)++;
                }
                List[j][i] = List[0][i];
                found = 1;
            }
        }
        if (!found)
        {
            int cur_max = 1;
            for (int j = 2; j < *m; j++) // find max wait time in queue
                if (List[cur_max][i].wait < List[j][i].wait)
                    cur_max = j;
            List[cur_max][i] = List[0][i];
            (*f_num)++;
            f_list[i] = '*';
        }
    }
}

void INPUT(PAGE ***out_list, int *n, int *m, char **f_list, int *f_num)
{
    int choice;
    PAGE *seq_list;
    bool input = 1;
    while (input)
    {
        printf("--- Page Replacement algorithm ---\n");
        printf("1. Default referenced sequence\n");
        printf("2. Manual input sequence\n");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            init_defaut(&seq_list, n, f_list);
            break;
        case 2:
            init_manual(&seq_list, n, f_list);
            break;
        default:
            printf("Enter \"1\" or \"2\" \n");
        }
        printf("--- Page Replacement algorithm ---\n");
        printf("Input page frames:\n");
        scanf("%d", m);
        (*m)++;
        table_init(out_list, seq_list, n, m);
        printf("--- Page Replacement algorithm ---\n");
        printf("1. FIFO algorithm\n");
        printf("2. OPT algorithm\n");
        printf("3. LRU algorithm\n");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            FIFO(*out_list, n, m, *f_list, f_num);
            input = 0;
            break;
        case 2:
            OPT(*out_list, n, m, *f_list, f_num);
            input = 0;
            break;
        case 3:
            LRU(*out_list, n, m, *f_list, f_num);
            input = 0;
            break;
        default:
            printf("Enter \"1\" , \"2\" or \"3\" \n");
        }
        input = 0;
    }
}

void OUTPUT(PAGE **out_list, int n, int m, char *f_list, int f_num)
{
    printf("--- Page Replacement algorithm ---\n");
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
        {
            if (out_list[i][j].blank)
                printf("  ");
            else
                printf("%d ", out_list[i][j].number);
            if (j == n - 1)
                printf("\n");
        }
    for (int i = 0; i < n; i++)
        printf("%c ", f_list[i]);
    printf("\nNumber of Page Fault: %d\n", f_num);
}

int main()
{
    PAGE **out_list;
    char *fault_list;
    // n la so trang, m la khung trang
    int n, m, fault_num;
    INPUT(&out_list, &n, &m, &fault_list, &fault_num);
    OUTPUT(out_list, n, m, fault_list, fault_num);
}