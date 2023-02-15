#include "ip.h"
#include <getopt.h>


int main(int argc, char *const *argv)
{
	Ip *ip = NULL;
	int val = 0;
	char optstring[] = ":i:m:b:s:h:S:H:";
	char *ipstr = NULL;
	unsigned mask = 0;
	int binary = ~0;
	char *temp = NULL;
	int subs[1000] = {0};
	int n1 = 0;
	int hosts[1000] = {0};
	int n2 = 0;
	int i, j, k;

	while((val=getopt(argc, argv, optstring))!=EOF)
	{
		switch(val)
		{
		case 'i':
			ipstr = (char *)malloc(strlen(optarg) + 1);
			if(!ipstr) break;
			strcpy(ipstr, optarg);
			break;
		case 'm':
			mask = atoi(optarg);
			break;
		case 'b':
			binary = atoi(optarg);
			break;
		case 's':
			temp = strtok(optarg, ",");
			while(temp)
			{
				if(n1>=1000) break;
				subs[n1++] = atoi(temp);
				temp = strtok(NULL, ",");
			}
			break;
		case 'S':
			temp = strtok(optarg, "-");
			i = temp ? atoi(temp) : 1;
			temp = strtok(NULL, "-");
			j = temp ? atoi(temp) : (i+9);
			temp = strtok(NULL, "-");
			k = temp ? atoi(temp) : 1;

			for(; i <= j; i+=k)
			{
				if(n1>=1000) break;
				subs[n1++] = i;
			}
			break;
		case 'h':
			temp = strtok(optarg, ",");
			while(temp)
			{
				if(n2>=1000) break;
				hosts[n2++] = atoi(temp);
				temp = strtok(NULL, ",");
			}
			break;
		case 'H':
			temp = strtok(optarg, "-");
			i = temp ? atoi(temp) : 1;
			temp = strtok(NULL, "-");
			j = temp ? atoi(temp) : (i+9);
			temp = strtok(NULL, "-");
			k = temp ? atoi(temp) : 1;

			for(; i <= j; i+=k)
			{
				if(n2>=1000) break;
				hosts[n2++] = i;
			}
			break;
		case '?':
			printf("unknown option: %c\n", optopt);
			exit(0);
			break;
		case ':':
			printf("missing arg: %c\n", optopt); 
			exit(0);
			break;
		default:
			exit(0);
		}
	}
	if(mask != 0)
	{
		ip = ipstr ? create_ip(ipstr, mask) : create_ip_from_mask(mask);
		set_view_mode(binary);
		view_as_table(ip, subs, n1, hosts, n2);
		destroy_ip(&ip);
	}
	free(ipstr);
	return 0;
}
