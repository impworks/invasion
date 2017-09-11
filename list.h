// dynamic list template

template <class Type>
list<Type>::list(int nmax)
{
    this->first = this->last = NULL;
    this->length = 0;
    this->max = nmax;
}

template <class Type>
list<Type>::~list()
{
    while(this->length > 0)
    {
        Type *next = (Type *)this->first->next;
        delete this->first;
        this->first = next;
        this->length--;
    }
}

template <class Type>
void list<Type>::add(Type *obj)
{
    //if(this->length == this->max) return;

    if(this->length == 0)
    {
        this->first = this->last = obj;
    }
    else
    {
        this->last->next = obj;
        obj->prev = this->last;
        this->last = obj;
    }
    this->length++;
}

template <class Type>
void list<Type>::del(Type *obj)
{
    if(obj == this->first)
    {
        this->first = (Type *)this->first->next;
    }
    else if(obj == this->last)
    {
        this->last = (Type *)this->last->prev;
    }
    else
    {
        obj->prev->next = (Type *)obj->next;
        obj->next->prev = (Type *)obj->prev;
    }
    this->length--;
}

template <class Type>
Type * list<Type>::get(int id)
{
    if(id > this->length-1) return NULL;
    Type *curr = this->first;
    for(int idx=0; idx<id; idx++)
    {
        curr = curr->next;
    }
    return curr;
}

template <class Type>
void list<Type>::empty()
{
    Type *tmp, *curr = this->first;
    for(int idx=0; idx<this->length; idx++)
    {
        tmp = (Type *)curr->next;
        CloseHandle(curr->thread);
        delete curr->trail;
        delete curr->body;
        if(curr->explosion) delete curr->explosion;
        delete curr;
        curr = tmp;
    }
    this->length = 0;
}

template <class Type>
void list<Type>::render()
{
    Type *curr = this->first;
    for(int idx=0; idx<this->length; idx++)
    {
        curr->trail->Render();
        if(curr->alive) curr->body->RenderEx(curr->x, curr->y, curr->angle, 1);
        if(curr->explosion) curr->explosion->Render();
        curr = (Type *)curr->next;
    }
}
