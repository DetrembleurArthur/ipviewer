#include "ip.h"


int view_mode = 0xffffffff;

void set_view_mode(int mode)
{
	view_mode = mode;
}

struct Ip
{
	union
	{
		unsigned int ip;
		unsigned char s[4];
	};
	unsigned char mask;
};

Ip *create_ip(const char *sip, unsigned mask)
{
	char *token = NULL;
	char *cp = NULL;
	int i = 0;
	Ip *ip = (Ip *)malloc(sizeof(Ip));
	if(ip)
	{
		ip->mask = mask;
		cp = (char *)malloc(strlen(sip) + 1);
		if(cp)
		{
			strcpy(cp, sip);
			token = strtok(cp, ".");
			while(token)
			{
				ip->s[4-i++-1] = (unsigned char)atoi(token);
				token = strtok(NULL, ".");
			}
		}
	}
	return ip;
}

Ip *create_ip_from_mask(unsigned mask)
{
	Ip *ip = malloc(sizeof(Ip));
	if(ip)
	{
		ip->ip = ~((0x1 << (32-mask))-1);
		ip->mask = mask;
	}
	return ip;
}

void destroy_ip(Ip **ip)
{
	if(ip && *ip)
	{
		free(*ip);
		*ip = NULL;
	}
}

void print_ip_bits(const Ip *ip)
{
	int i, j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 8; j++)
		{
			printf("%d", ((unsigned char)ip->s[4-i-1] & (0x1 << (8 - j - 1))) != 0);
		}
		if(i != 3) printf(".");
	}
	printf("\n");
}

void print_ip_bits_table(const Ip *ip)
{
	if(view_mode & 1)
	{
		int i = 0, j = 0;
		printf("%5d\t%5d\t%5d\t%5d\t%5d\t%5d\t%5d\t%5d\t\n", 128, 64, 32, 16, 8, 4, 2, 1);
		printf("---------------------------------------------------------------------------\n");
		while(i < 4)
		{
			j = 0;
			while(j < 8)
			{
				printf("%5d\t", ((unsigned char)ip->s[4-i-1] & (0x1 << (8 - j - 1))) != 0);
				j++;
			}
			printf("=%5hhu\n", ip->s[4-i-1]);
			i++;
		}
	}
}

void print_ip(const Ip *ip)
{
	printf("%hhu.%hhu.%hhu.%hhu/%hhu", ip->s[3], ip->s[2], ip->s[1], ip->s[0], ip->mask);
	if(view_mode & 2)
	{
		printf(" => ");
		print_ip_bits(ip);
	}
	else
	{
		printf("\n");
	}
}

unsigned char get_mask(const Ip *ip)
{
	return ip->mask;
}


Ip *get_mask_ip(const Ip *ip)
{
	return create_ip_from_mask(ip->mask);
}

Ip *get_network_ip(const Ip *ip)
{
	Ip *net = (Ip *)malloc(sizeof(Ip));
	if(net)
	{
		net->ip = ip->ip & ~((0x1 << (32-ip->mask))-1);
		net->mask = ip->mask;
	}
	return net;
}

Ip *get_wildcard_mask_ip(const Ip *ip)
{
	Ip *wip = create_ip_from_mask(ip->mask);
	wip->ip = ~wip->ip;
	return wip;
}

Ip *get_sub_network(const Ip *ip, int i)
{
	Ip *sub = (Ip *)malloc(sizeof(Ip));
	if(sub)
	{
		sub->ip = (ip->ip & ~((0x1 << (32-ip->mask))-1)) + ((i-1) << (32-ip->mask));
		sub->mask = ip->mask;
	}
	return sub;
}

Ip *get_host(const Ip *ip, int i)
{
	Ip *host = (Ip *)malloc(sizeof(Ip));
	if(host)
	{
		host->ip = (ip->ip & ~((0x1 << (32-ip->mask))-1)) + i;
		host->mask = ip->mask;
	}
	return host;
}

Ip *get_broadcast(const Ip *ip)
{
	Ip *host = get_network_ip(ip);
	if(host)
	{
		host->ip |= ((0x1 << (32-ip->mask))-1);
	}
	return host;
}

void set_ip_byte(Ip *ip, int i, unsigned char value)
{
	ip->s[i] = value;
}

int is_network(const Ip *ip)
{
	return !is_mask(ip) && (ip->ip & ~((0x1 << (32-ip->mask))-1)) == ip->ip;
}

int is_mask(const Ip *ip)
{
	return ip->ip == ~((0x1 << (32-ip->mask))-1);
}

unsigned get_host_capacity(const Ip *ip)
{
	return ((0x1 << (32-ip->mask))-1) - 1; // broadcast
}

unsigned get_network_capacity(const Ip *ip)
{
	return ((0x1 << (ip->mask))-1);
}

unsigned get_host_number(const Ip *ip)
{
	return (ip->ip & ((0x1 << (32-ip->mask))-1));
}

void view_ip_info(const Ip *ip)
{
	Ip *temp = NULL;
	printf("\nIp: ");
	print_ip(ip);
	print_ip_bits_table(ip);
	temp = get_mask_ip(ip);
	printf("\nMask: ");
	print_ip(temp);
	print_ip_bits_table(temp);
	destroy_ip(&temp);
	temp = get_wildcard_mask_ip(ip);
	printf("\nWildcard mask: ");
	print_ip(temp);
	print_ip_bits_table(temp);
	destroy_ip(&temp);
	temp = get_network_ip(ip);
	printf("\nNetwork: ");
	print_ip(temp);
	print_ip_bits_table(temp);
	destroy_ip(&temp);
	temp = get_broadcast(ip);
	printf("\nBroadcast: ");
	print_ip(temp);
	print_ip_bits_table(temp);
	destroy_ip(&temp);
	printf("\nHost number: %u\n", get_host_number(ip));
	printf("Host capacity: %u\n", get_host_capacity(ip));
	printf("Network capacity: %u\n", get_network_capacity(ip));
}

static void bufferize_ip_bits(char *buffer, int ip);

void place_into_columns(Ip *ip, Column **columns)
{
	Ip *temp = NULL;
	char buffer[1024] = {0};

	sprintf(buffer, "%hhu.%hhu.%hhu.%hhu/%hhu", ip->s[3], ip->s[2], ip->s[1], ip->s[0], ip->mask);
	insert_element(columns[0], buffer);

	temp = get_mask_ip(ip);
	sprintf(buffer, "%hhu.%hhu.%hhu.%hhu", temp->s[3], temp->s[2], temp->s[1], temp->s[0]);
	destroy_ip(&temp);
	insert_element(columns[1], buffer);

	temp = get_wildcard_mask_ip(ip);
	sprintf(buffer, "%hhu.%hhu.%hhu.%hhu", temp->s[3], temp->s[2], temp->s[1], temp->s[0]);
	destroy_ip(&temp);
	insert_element(columns[2], buffer);

	temp = get_network_ip(ip);
	sprintf(buffer, "%hhu.%hhu.%hhu.%hhu", temp->s[3], temp->s[2], temp->s[1], temp->s[0]);
	destroy_ip(&temp);
	insert_element(columns[3], buffer);

	temp = get_broadcast(ip);
	sprintf(buffer, "%hhu.%hhu.%hhu.%hhu", temp->s[3], temp->s[2], temp->s[1], temp->s[0]);
	destroy_ip(&temp);
	insert_element(columns[4], buffer);

	sprintf(buffer, "%u / %u", get_host_number(ip), get_host_capacity(ip));
	insert_element(columns[5], buffer);

	sprintf(buffer, "%u", get_network_capacity(ip));
	insert_element(columns[6], buffer);

	bufferize_ip_bits(buffer, ip->ip);
	insert_element(columns[7], buffer);
}

struct Table
{
	Column **columns;
	unsigned freeElement;
	unsigned capacity;
	unsigned lines;
	char hborder;
	char vborder;
	char coin;
};


void bufferize_ip_bits(char *buffer, int ip)
{
	int j = 0;
	for(int i = 0; i < 32; i++, j++)
	{
		if(i > 0 && i % 8 == 0)
		{
			buffer[j++] = ' ';
		}
		buffer[j] = ((ip >> (32 - i - 1)) & 1) + 48;
	}
	buffer[j] = 0;
}


static int columns_available();

int columns_available()
{
	int count = 0;
	for(int i = 0; i < 9; i++)
	{
		count += (view_mode >> i) & 1;
	}
	return count;
}


void view_as_table(Ip *ip, int *subnum, int n1, int *hostnum, int n2)
{
	Ip *temp = NULL;
	char buffer[1024] = {0};
	Table *table = create_table(9, 1 + n1 + n2);
	Column *idcol = generate_column(table, "Id");
	Column *ipcol = generate_column(table, "Ip address");
	Column *maskcol = generate_column(table, "Mask");
	Column *wmaskcol = generate_column(table, "Wildcard");
	Column *netcol = generate_column(table, "Network");
	Column *broadcol = generate_column(table, "Broadcast");
	Column *hncol = generate_column(table, "Host");
	Column *netccol = generate_column(table, "Netw cap");
	Column *bitscol = generate_column(table, "Bits");

	insert_element(idcol, "root");
	place_into_columns(ip, table->columns + 1);
	for(int i = 0; i < n1; i++)
	{
		sprintf(buffer, "sub n.%d", subnum[i]);
		insert_element(idcol, buffer);
		temp = get_sub_network(ip, subnum[i]);
		place_into_columns(temp, table->columns + 1);
		destroy_ip(&temp);
	}
	for(int i = 0; i < n2; i++)
	{
		sprintf(buffer, "host n.%d", hostnum[i]);
		insert_element(idcol, buffer);
		temp = get_host(ip, hostnum[i]);
		place_into_columns(temp, table->columns + 1);
		destroy_ip(&temp);
	}
	

	print_table(table);
	destroy_table(&table);
}

struct Column
{
	char *name;
	char **data;
	unsigned capacity;
	unsigned freeElement;
	unsigned margin;
	unsigned width;
};

Column *create_column(const char *name, unsigned capacity)
{
	Column *column = (Column *)malloc(sizeof(Column));
	if(column)
	{
		column->name = (char *)malloc(strlen(name) + 1);
		if(column->name)
		{
			strcpy(column->name, name);
			column->data = (char **)calloc(capacity, sizeof(char *));
			if(column->data)
			{
				column->margin = 3;
				column->width = 0;
				column->capacity = capacity;
				column->freeElement = 0;
			}
			else
			{
				free(column->name);
				free(column);
				column = NULL;
			}
		}
		else
		{
			free(column);
			column = NULL;
		}
	}
	return column;
}

void insert_element(Column *column, const char *element)
{
	char *temp = NULL;
	if(column->freeElement != column->capacity)
	{
		temp = (char *)malloc(strlen(element) + 1);
		if(temp)
		{
			strcpy(temp, element);
			column->data[column->freeElement++] = temp;
			if(strlen(temp) + column->margin * 2 > column->width)
			{
				column->width = strlen(temp) + column->margin * 2;
			}
		}
	}
}

void destroy_column(Column **column)
{
	int i = 0;
	free((*column)->name);
	while(i < (*column)->capacity)
	{
		if((*column)->data[i])
		{
			free((*column)->data[i]);
		}
		else
			break;
		i++;
	}
	free((*column)->data);
	free(*column);
	*column = NULL;
}


Table *create_table(unsigned capacity, unsigned lines)
{
	Table *table = (Table *)malloc(sizeof(Table));
	if(table)
	{
		table->columns = (Column **)calloc(capacity, sizeof(Column *));
		if(table->columns)
		{
			table->freeElement = 0;
			table->hborder = '-';
			table->vborder = '|';
			table->coin = '+';
			table->capacity = capacity;
			table->lines = lines;
		}
		else
		{
			free(table);
			table = NULL;
		}
	}
	return table;
}

Column *generate_column(Table *table, const char *name)
{
	Column *column = NULL;
	if(table->freeElement < table->capacity)
	{
		column = create_column(name, table->lines);
		table->columns[table->freeElement++] = column;
	}
	return column;
}

void destroy_table(Table **table)
{
	int i = 0;
	while(i < (*table)->capacity)
	{
		if((*table)->columns[i])
		{
			destroy_column(&(*table)->columns[i]);
		}
		else
			break;
		i++;
	}
	free((*table)->columns);
	free(*table);
	*table = NULL;
}

void print_table(Table *table)
{
	int i, j;
	for(i = 0; i < table->capacity; i++)
	{
		printf("%-17s", table->columns[i]->name);
	}
	printf("\n\n");
	for(j = 0; j < table->lines; j++)
	{
		for(i = 0; i < table->capacity; i++)
		{
			printf("%-17s", table->columns[i]->data[j]);
		}
		printf("\n");
	}
}
