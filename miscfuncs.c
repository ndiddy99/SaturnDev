#define fixedToUint16(fixed) (fixed >> 16)

SpriteNode createSpriteNode();
SpriteNode addSpriteNode(SpriteNode head, SPRITE_INFO data);
void deleteSpriteNode(SpriteNode* head, SpriteNode node);

SpriteNode createSpriteNode(void)
{
	SpriteNode tmp = (SpriteNode)malloc(sizeof(struct SpriteList));
	tmp->next = NULL;
	return tmp;
}

SpriteNode addSpriteNode(SpriteNode head, SPRITE_INFO data)
{
	slPrint("addSpriteNode", slLocate(0,0));
	SpriteNode tmp, ptr;
	tmp = createSpriteNode();
	tmp->sprite = data;
	if (head == NULL) {
		head = tmp;
		return head;
	}
	else {
		ptr = head;
		while (ptr->next != NULL)
			ptr = ptr->next;
		ptr->next = tmp;
	}
	return tmp;
}

void deleteSpriteNode(SpriteNode* head, SpriteNode node)
{
	slPrint("deleteSpriteNode", slLocate(0,0));
	if (node->next != NULL) {
		slPrint("norm", slLocate(0,1));
		SpriteNode tmp = node->next;
		node->sprite = tmp->sprite;
		node->next = tmp->next;
		free(tmp);
	}
	else if (*head==node) { //if only one node
		slPrint("only", slLocate(0,1));
		*head = NULL;
	}
	else {
		slPrint("last", slLocate(0,1));
		SpriteNode tmp = *head;
		while (tmp->next != node) {
			tmp = tmp->next;
			if (tmp == NULL)
				return;
		}
		free(tmp->next);
		tmp->next = NULL;
	}
}
