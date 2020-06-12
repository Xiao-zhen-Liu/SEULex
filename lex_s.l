%{
#include <stdio.h>


void comment();
%}

D			[0-9]
L			[a-zA-Z_]

%%
"if"			{ 
	 printf("IF"); 
	}
"int"			{ 
	 printf("INT"); 
	}
"void"			{ 
	 printf("VOID"); 
	}

[ \t\v\n\f] {
	printf("SPACE");
} 
.			{
	printf("ERROR!");
}

%%

void comment(){
	char c,prev=0;
	while(++yy_cp!=0)
	{
		c=*yy_cp;
		if(c=='/'&&prev=='*')
			return;
		prev=c;
	}
	printf("ERROR: unterminated comment!");
}