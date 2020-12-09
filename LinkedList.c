
void removeItem(ListHead *head, ListItem *item)
{
    ListItem *cursor = head->headItem;
    if (item == NULL)
        return;
    /**如果要删除的是头结点**/
    if (head->headItem == item)
    {
        head->headItem = head->headItem->next;
        head->headItem->prev = NULL;
        if (cursor->content != NULL)
            free(cursor->content);
        free(cursor);
        cursor = NULL;
        head->length--;
        return;
    }
    /**如果要删除的是尾结点**/
    if (head->lastItem == item)
    {
        head->lastItem = head->lastItem->prev;
        head->lastItem->next = NULL;
        if (item->content != NULL)
            free(item->content);
        free(item);
        head->length--;
        return;
    }
    if (cursor == NULL)
    {
        return;
    }
    /**其他结点**/
    while (cursor != NULL)
    {
        if (cursor == item)
        {
            cursor->next->prev = cursor->prev;
            cursor->prev->next = cursor->next;
            if (item->content != NULL)
                free(item->content);
            free(item);
            head->length--;
            return;
        }
        cursor = cursor->next;
    }
}

void removeItemByPosition(ListHead *head, int pos)
{
    if (head == NULL)
        return;
    ListItem *cursor = head->headItem;
    /**如果要删除的是头结点**/
    if (pos == 0)
    {
        head->headItem = head->headItem->next;
        head->headItem->prev = NULL;
        if (cursor->content != NULL)
            free(cursor->content);
        free(cursor);
        cursor = NULL;
        head->length--;
        return;
    }
    /**如果要删除的是尾结点**/
    if (pos == head->length - 1)
    {
        head->lastItem = head->lastItem->prev;
        head->lastItem->next = NULL;
        if (cursor->content != NULL)
            free(cursor->content);
        free(cursor);
        cursor = NULL;
        head->length--;
        return;
    }
    if (cursor == NULL)
    {
        return;
    }
    /**其他结点**/
    for (; pos > 0; pos--)
    {
        if (cursor == NULL)
            return;
        cursor = cursor->next;
    }
    cursor->next->prev = cursor->prev;
    cursor->prev->next = cursor->next;
    if (cursor->content != NULL)
        free(cursor->content);
    free(cursor);
    head->length--;
    return;
}

void add(ListHead *head, void *content)
{
    if (content == NULL)
        return;
    /**如果表中没有项就新建第一项**/
    if (head->headItem == NULL)
    {
        head->headItem = new (ListItem);
        head->lastItem = head->headItem;
        head->headItem->prev = NULL;
        head->headItem->next = NULL;
        head->headItem->content = content;
    }
    else
    {
        ListItem *newItem = new (ListItem);
        /**新加项的前序地址是当前列表最后一项**/
        newItem->prev = head->lastItem;
        newItem->next = NULL;
        newItem->content = content;
        /**新加项成为列表最后一项，之前的最后一项成为倒数第二项**/
        head->lastItem->next = newItem;
        head->lastItem = newItem;
    }
    head->length++;
}

void *get(ListHead *head, int pos)
{
    ListItem *cursor = head->headItem;
    for (; pos > 0; pos--)
    {
        if (cursor == NULL)
            return NULL;
        cursor = cursor->next;
    }
    return cursor->content;
}

void insert(ListHead *head, void *content, int pos)
{
    if (head == NULL)
    {
        return;
    }
    ListItem *cursor = head->headItem;
    /**如果插入到第一项**/
    if (pos == 0)
    {
        if (head != NULL)
        {
            ListItem *newItem = new (ListItem);
            newItem->content = content;
            newItem->prev = NULL;
            head->headItem->prev = newItem;
            newItem->next = head->headItem;
            head->headItem = newItem;
            head->length++;
            return;
        }
    }
    /**如果插入到最后一项**/
    if (pos == head->length - 1)
    {
        if (head != NULL)
        {
            ListItem *newItem = new (ListItem);
            newItem->content = content;
            newItem->next = NULL;
            head->lastItem->next = newItem;
            newItem->prev = head->lastItem;
            head->lastItem = newItem;
            head->length++;
            return;
        }
    }
    /**其他**/
    for (; pos > 1; pos--)
    {
        if (cursor == NULL)
            return;
        cursor = cursor->next;
    }
    {
        ListItem *newItem = new (ListItem);
        newItem->content = content;
        newItem->next = cursor->next;
        newItem->prev = cursor;
        cursor->next->prev = newItem;
        cursor->next = newItem;
        head->length++;
    }
}

void destory(ListHead *head)
{
    if (head == NULL)
        return;
    ListItem *cursor = head->headItem;
    ListItem *next = NULL;
    int i = 0;
    while (cursor != NULL)
    {
        next = cursor->next;
        if (cursor->content != NULL)
            free(cursor->content);
        cursor->content = NULL;
        free(cursor);
        cursor = NULL;
        cursor = next;
        //printf("clear:%d\n", i++);
    }
    free(head);
    //printf("done\n");
}

ListHead *createList()
{
    ListHead *head = new (ListHead);
    /**新条目没有内容**/
    head->headItem = NULL;
    head->length = 0;
    head->removeItem = removeItem;
    head->removeItemByPosition = removeItemByPosition;
    head->add = add;
    head->get = get;
    head->insert = insert;
    return head;
}