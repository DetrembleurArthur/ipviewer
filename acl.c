#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

unsigned strip_to_intip(char *strip)
{
	unsigned value = 0;
	char *token = strtok(strip, ".");
	int i = 0;
	while(token)
	{
		value <<= 8;
		value += (unsigned char)atoi(token);
		token = strtok(NULL, ".");
	}
	return value;
}

void show_acls(unsigned number,
unsigned fromSrcIp,
unsigned toSrcIp,
const char *dstIp,
const char *dstMaskIp,
const char *f,
const char *t,
const char *protocol)
{
	unsigned current = fromSrcIp;
	unsigned temp = 0;
	int counter = 0;
	printf("src: [%u - %u]\tdst: %u %u\n", fromSrcIp, toSrcIp, dstIp, dstMaskIp);
	while(current <= toSrcIp)
	{
		temp = current;
		counter = 0;
		while(temp > 0)
		{
			if(temp % 2 == 0)
			{
				counter++;
				temp >>= 1;
			}
			else
			{
				temp = 0;
			}
		}
		temp = pow(2, counter) - 1;
		while(current + temp > toSrcIp)
		{
			temp = pow(2, --counter) - 1;
		}
		printf("access-list %u permit %s %u.%u.%u.%u %u.%u.%u.%u %s %s %s %s\n", number,
			protocol != NULL ? protocol : "",
			(current >> 24) & 0xff,
			(current >> 16) & 0xff,
			(current >> 8) & 0xff,
			current & 0xff,
			(temp >> 24) & 0xff,
			(temp >> 16) & 0xff,
			(temp >> 8) & 0xff,
			temp & 0xff,
			f != NULL ? f : "",
			(dstIp != NULL) ? dstIp : "any",
			(dstMaskIp != NULL) ? dstMaskIp : (dstIp != NULL ? "0.0.0.0" : ""),
			t != NULL ? t : "");
		current += temp + 1;
	}
}

int main(int argc, char *const *argv)
{
	int val = 0;
	char optstring[] = ":s:S:d:m:f:t:p:n:h";
	char *srcIp1 = NULL;
	char *srcIp2 = NULL;
	char *dstIp = NULL;
	char *maskDstIp = NULL;
	char *protocol = NULL;
	char *f = NULL;
	char *t = NULL;
	unsigned number = 0;
	

	while((val=getopt(argc, argv, optstring))!=EOF)
	{
		switch(val)
		{
		case 's':
			srcIp1 = (char *)malloc(strlen(optarg) + 1);
			if(!srcIp1) break;
			strcpy(srcIp1, optarg);
			break;
		case 'S':
			srcIp2 = (char *)malloc(strlen(optarg) + 1);
			if(!srcIp2) break;
			strcpy(srcIp2, optarg);
			break;
		case 'd':
			dstIp = (char *)malloc(strlen(optarg) + 1);
			if(!dstIp) break;
			strcpy(dstIp, optarg);
			break;
		case 'm':
			maskDstIp = (char *)malloc(strlen(optarg) + 1);
			if(!maskDstIp) break;
			strcpy(maskDstIp, optarg);
			break;
		case 'p':
			protocol = (char *)malloc(strlen(optarg) + 1);
			if(!protocol) break;
			strcpy(protocol, optarg);
			break;
		case 'f':
			f = (char *)malloc(strlen(optarg) + 1);
			if(!f) break;
			strcpy(f, optarg);
			break;
		case 't':
			t = (char *)malloc(strlen(optarg) + 1);
			if(!t) break;
			strcpy(t, optarg);
			break;
		case 'n':
			number = atoi(optarg);
			break;
		case 'h':
			printf("-s : first source ip address\n-S : last source ip address\n-d : destination ip address\n-m : destination wildcard mask\n-p : protocol\n-n : ACL number\n-f : source details\n-t : destination details\n");
			return 0;
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
	unsigned srcIpValue1 = strip_to_intip(srcIp1);
	unsigned srcIpValue2 = strip_to_intip(srcIp2);
	show_acls(
		number,
		srcIpValue1,
		srcIpValue2,
		dstIp,
		maskDstIp, f,t, protocol);
	if(srcIp1)
		free(srcIp1);
	if(srcIp2)
		free(srcIp2);
	if(dstIp)
		free(dstIp);
	if(maskDstIp)
		free(maskDstIp);
	if(protocol)
		free(protocol);
	return 0;
}
