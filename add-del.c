
void add(QTYPE *queue, Byte x)
{
	if(queue->front ==-1 && queue->rear == -1)
	{
		queue->data[++queue->rear]=x;
		queue->front=queue->rear;
		queue->count++;
		return;
	}
	else if(queue->front == (queue->rear+1)%(queue->front))
	{
		printf("Circular Queue over flow");
		return;
	}
	queue->rear= (queue->rear+1)%(queue->count);
	queue->data[queue->rear]=x;
}

void del(QTYPE *queue, )
{
	if(queue->front==-1 && queue->rear == -1)
	{
		printf("under flow");
		return;
	}
	else if( queue->front== queue->rear  )
	{
		queue->front=queue->rear=-1;
		return;
	}
	queue->front= (queue->front+1)%(queue->count);
}
