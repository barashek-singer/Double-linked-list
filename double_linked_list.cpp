#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// [MAW] //*- выполнено
// [mew] //*- в процессе
// [X-X] //*- ну понятно думаю

// тудушки 
// [MAW] Напиши хоть что-то нормальное
// [MAW] Дебаг
// [mew] graphviz //* define GRAPHVIZ
// [X-X] html

#define LIST_CHECK(expression) do {   \
    list_err_t _err = expression;      \
    if (_err != LIST_OK)                \
        return _err;                     \
} while (0)

#define list_elem_specifier "d"
typedef int list_elem_t;
typedef ssize_t list_iterator_t;

const list_elem_t POISON = 0xDEADDEAD;
const list_iterator_t NULL_NODE = -1;
//*мэйби в константу вынести нулевую вершину
const size_t LIST_START_CAPACITY = 8;
const size_t LIST_COEFF_CAPACITY = 2;


enum list_err_t {
    LIST_OK, //* гыгыггыгыгыы Листок
    LIST_NOT_INIT,
    LIST_BAD_ARG,
    LIST_OOM,
    LIST_DOUBLE_INIT,
    LIST_NOT_VALID_NODE
};

struct list_node_t {
    list_iterator_t next;
    list_iterator_t prev;
    list_elem_t elem;
};

struct list_t {
    list_node_t* data;
    list_iterator_t free_node;
    size_t size; //считая нулевую вершину
    size_t capacity;
};

list_err_t         ListInit(list_t* list);
list_err_t       ListVerify(list_t* list);
bool        ListIsValidNode(list_t* list, list_iterator_t node);
list_err_t         ListNext(list_t* list, list_iterator_t* dest, list_iterator_t src);
list_err_t         ListPrev(list_t* list, list_iterator_t* dest, list_iterator_t src);
list_err_t      ListGetHead(list_t* list, list_iterator_t* head);
list_err_t      ListGetTail(list_t* list, list_iterator_t* tail);
list_err_t    ListReallocUp(list_t* list);
list_err_t  ListInsertAfter(list_t* list, list_iterator_t left_node, list_elem_t elem);
list_err_t ListInsertBefore(list_t* list, list_iterator_t right_node, list_elem_t elem);
list_err_t        ListErase(list_t* list, list_iterator_t node);
list_err_t         ListFree(list_t* list);

void ListShowError(list_err_t err);

int main() {
    list_t list = {0};
    list_err_t err = LIST_OK;
    list_iterator_t cur_node = NULL_NODE;
    size_t cnt_meow = 0;

    printf("meow %zu\n", ++cnt_meow);

    err = ListInit(&list);
    if (err != LIST_OK) {
        ListShowError(err);
        return 1;
    }

    printf("meow %zu\n", ++cnt_meow);

    err = ListGetHead(&list, &cur_node);
    if (err != LIST_OK) {
        ListShowError(err);
        return 1;
    }

    printf("meow %zu\n", ++cnt_meow);

    for (int i = 1; i < 5; i++) {
        err = ListInsertAfter(&list, cur_node, i);
        if (err != LIST_OK) {
            ListShowError(err);
            return 1;
        }

        printf("meow %zu\n", ++cnt_meow);
    }

    err = ListNext(&list, &cur_node, cur_node);
    if (err != LIST_OK) {
        ListShowError(err);
        return 1;
    }

    printf("meow %zu\n", ++cnt_meow);

    err = ListNext(&list, &cur_node, cur_node);
    if (err != LIST_OK) {
        ListShowError(err);
        return 1;
    }

    printf("meow %zu\n", ++cnt_meow);

    err = ListInsertAfter(&list, cur_node, 5);
    if (err != LIST_OK) {
        ListShowError(err);
        return 1;
    }

    printf("meow %zu\n", ++cnt_meow);
    
    for (list_iterator_t i = 1; i < (list_iterator_t)list.size; i++)
        printf("%d ", list.data[i].elem);

    putchar('\n');

    err = ListErase(&list, cur_node);
    if (err != LIST_OK) {
        ListShowError(err);
        return 1;
    }

    printf("meow %zu\n", ++cnt_meow);
    
    for (list_iterator_t i = 1; i < (list_iterator_t)list.size; i++)
        printf("%d ", list.data[i].elem);5

    printf("list->free_node = %zd\n", list->free_node);

    putchar('\n');

    printf("meow %zu\n", ++cnt_meow);

    ListFree(&list);

    printf("meow %zu\n", ++cnt_meow); 
}

list_err_t ListInit(list_t* list) {
    if (list == NULL)
        return LIST_BAD_ARG;

    if (list->data != NULL)
        return LIST_DOUBLE_INIT;

    list->data = (list_node_t*)calloc(LIST_START_CAPACITY, sizeof(list_node_t)); //LIST_START_CAPACITY - стартовое значение капасити листа
    if (list->data == NULL)
        return LIST_OOM;

    list->size = 1;
    list->capacity = LIST_START_CAPACITY;
    list->free_node = 1;
    list->data[0].next = 0;
    list->data[0].prev = 0;

    for (list_iterator_t i = 1; i < (list_iterator_t)LIST_START_CAPACITY; i++) { //Заполнение free_data
        list->data[i].next = i + 1;
        list->data[i].prev = NULL_NODE;
        list->data[i].elem = POISON;
    }

    list->data[LIST_START_CAPACITY - 1].next = NULL_NODE; //последняя free'шная вершина ведёт на null

    return LIST_OK;
}

list_err_t ListVerify(list_t* list) {
    if (list == NULL)
        return LIST_BAD_ARG;

    if (list->data == NULL)
        return LIST_NOT_INIT;

    return LIST_OK;
}

//? короче скорее всего нужно будет переписать с развратом ошибки и делать
//? соответственно проверку по типу итоговая ошибка равна ошибке или содержит её
//* Или похуй?

bool ListIsValidNode(list_t* list, list_iterator_t node) {
    assert(list); //*ну это чисто на случай, если я затуплю 
    return (node >= 0 && node < (list_iterator_t)list->capacity && list->data[node].prev != NULL_NODE);
}

list_err_t ListGetElem(list_t* list, list_iterator_t node, list_elem_t* elem) {
    LIST_CHECK(ListVerify(list));

    if (elem == NULL)
        return LIST_BAD_ARG;

    if (!ListIsValidNode(list, node))
        return LIST_NOT_VALID_NODE;

    *elem = list->data[node].elem;

    return LIST_OK;
}

list_err_t ListNext(list_t* list, list_iterator_t* dest, list_iterator_t src) {
    LIST_CHECK(ListVerify(list));

    if (dest == NULL)
        return LIST_BAD_ARG;

    if (!ListIsValidNode(list, src))
        return LIST_NOT_VALID_NODE;

    *dest = list->data[src].next;

    return LIST_OK;
}

list_err_t ListPrev(list_t* list, list_iterator_t* dest, list_iterator_t src) {
    LIST_CHECK(ListVerify(list));

    if (dest == NULL)
        return LIST_BAD_ARG;

    if (!ListIsValidNode(list, src))
        return LIST_NOT_VALID_NODE;

    *dest = list->data[src].prev;

    return LIST_OK;
}

list_err_t ListGetHead(list_t* list, list_iterator_t* head) {
    LIST_CHECK(ListVerify(list));
    
    if (head == NULL)
        return LIST_BAD_ARG;

    LIST_CHECK(ListNext(list, head, 0));

    return LIST_OK;
}

list_err_t ListGetTail(list_t* list, list_iterator_t* tail) {
    LIST_CHECK(ListVerify(list));
    
    if (tail == NULL)
        return LIST_BAD_ARG;

    LIST_CHECK(ListPrev(list, tail, 0));

    return LIST_OK;
}

list_err_t ListReallocUp(list_t* list) {
    LIST_CHECK(ListVerify(list));

    list_node_t* new_data = (list_node_t*)realloc(list->data, list->capacity * LIST_COEFF_CAPACITY);
    
    if (new_data == NULL)
        return LIST_OOM;

    list->data = new_data;

    list->free_node = list->capacity;

    for (list_iterator_t i = list->capacity; i < (list_iterator_t)(list->capacity * LIST_COEFF_CAPACITY); i++) {
        list->data[i].next = i + 1;
        list->data[i].prev = NULL_NODE;
        list->data[i].elem = POISON;
    }

    list->capacity *= LIST_COEFF_CAPACITY;

    list->data[list->capacity - 1].next = NULL_NODE;
    
    return LIST_OK;
}

list_err_t ListInsertAfter(list_t* list, list_iterator_t left_node, list_elem_t elem) { //// пожалуйста распили меня болгаркой
    LIST_CHECK(ListVerify(list));

    if (!ListIsValidNode(list, left_node)) //проверка вершины на корректность
        return LIST_NOT_VALID_NODE;

    if (list->free_node == NULL_NODE)
        LIST_CHECK(ListReallocUp(list));

    list_iterator_t new_node = list->free_node;
    list->free_node = list->data[list->free_node].next;

    list_iterator_t right_node = NULL_NODE;
    LIST_CHECK(ListNext(list, &right_node, left_node));

    list->data[left_node].next = new_node;

    list->data[new_node].prev = left_node;
    list->data[new_node].next = right_node;

    list->data[right_node].prev = new_node;

    list->data[new_node].elem = elem;

    ++list->size;

    return LIST_OK;
}

list_err_t ListInsertBefore(list_t* list, list_iterator_t right_node, list_elem_t elem) { //// пожалуйста распили меня йокрфглоб
    LIST_CHECK(ListVerify(list));

    if (!ListIsValidNode(list, right_node)) //проверка вершины на корректность
        return LIST_NOT_VALID_NODE;

    if (list->free_node == NULL_NODE)
        LIST_CHECK(ListReallocUp(list)); //TODO написать эту функцию

    list_iterator_t new_node = list->free_node;
    list->free_node = list->data[list->free_node].next;

    list_iterator_t left_node = NULL_NODE;
    LIST_CHECK(ListPrev(list, &left_node, right_node));

    list->data[left_node].next = new_node;

    list->data[new_node].prev = left_node;
    list->data[new_node].next = right_node;

    list->data[right_node].prev = new_node;

    list->data[new_node].elem = elem;

    ++list->size;

    return LIST_OK;
}

list_err_t ListErase(list_t* list, list_iterator_t node) {
    LIST_CHECK(ListVerify(list));

    if (!ListIsValidNode(list, node)) //проверка вершины на корректность
        return LIST_NOT_VALID_NODE;

    list_iterator_t left_node = NULL_NODE;
    LIST_CHECK(ListPrev(list, &left_node, node));

    list_iterator_t right_node = NULL_NODE;
    LIST_CHECK(ListNext(list, &right_node, node));

    list->data[left_node].next = right_node;
    list->data[right_node].next = left_node;

    list->data[node].next = list->free_node;
    list->data[node].prev = NULL_NODE;
    list->data[node].elem = POISON;

    list->free_node = node;

    return LIST_OK;
}

list_err_t ListFree(list_t* list) {
    if (list == NULL)
        return LIST_BAD_ARG;

    free(list->data);

    return LIST_OK;
}

void ListShowError(list_err_t err) {
    switch (err) {
        case LIST_BAD_ARG:
            puts("LIST_BAD_ARG");
            break;
        case LIST_NOT_INIT:
            puts("LIST_NOT_INIT");
            break;
        case LIST_OOM:
            puts("LIST_OOM");
            break;
        case LIST_DOUBLE_INIT:
            puts("LIST_DOUBLE_INIT");
            break;
        case LIST_NOT_VALID_NODE:
            puts("LIST_NOT_VALID_NODE");
            break;
        case LIST_OK:
            break;
        default:
            puts("???");
            break;
    }
}
//*    LIST_BAD_ARG,
//*    LIST_OOM,
//*    LIST_DOUBLE_INIT,
//*    LIST_NOT_VALID_NODE
//* 