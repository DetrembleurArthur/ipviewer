#ifndef __IP_H__
#define __IP_H__
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct Ip Ip;
typedef struct Column Column;
typedef struct Table Table;


Ip *create_ip(const char *ip, unsigned mask);
Ip *create_ip_from_mask(unsigned mask);
void destroy_ip(Ip **ip);
void print_ip_bits_table(const Ip *ip);
void print_ip_bits(const Ip *ip);
void print_ip(const Ip *ip);
unsigned char get_mask(const Ip *ip);
Ip *get_mask_ip(const Ip *ip);
Ip *get_network_ip(const Ip *ip);
Ip *get_broadcast(const Ip *ip);
Ip *get_wildcard_mask_ip(const Ip *ip);
Ip *get_sub_network(const Ip *ip, int i);
Ip *get_host(const Ip *ip, int i);
void set_ip_byte(Ip *ip, int i, unsigned char value);
int is_network(const Ip *ip);
int is_mask(const Ip *ip);
unsigned get_host_capacity(const Ip *ip);
unsigned get_network_capacity(const Ip *ip);
unsigned get_host_number(const Ip *ip);
void view_ip_info(const Ip *ip);

void view_as_table(Ip *ip, int *subnum, int n1, int *hostnum, int n2);

void set_view_mode(int mode);


Column *create_column(const char *name, unsigned capacity);
void insert_element(Column *column, const char *element);
void destroy_column(Column **column);

Table *create_table(unsigned capacity, unsigned lines);
Column *generate_column(Table *table, const char *name);
void destroy_table(Table **table);
void print_table(Table *table);


#endif
