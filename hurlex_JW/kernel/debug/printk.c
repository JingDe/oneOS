#include"debug.h"

// 调用形式 printk("hello, %s, %d\n", str1, x)
void printk(const char* format, ...)
{
	static char buff[1024];
	va_list args;
	int i;
	
	va_start(args, format);//args只想参数列表的第一个参数
	i=vsprintf(buff, format, args);
	va_end(args);
	
	buff[i]='\0';
	
	console_write(buff);	
}

static int vsprintf(char *buff, const char* format, va_list args)
{
	int len;
	int i;
	char *str;
	char *s;
	int *ip;
	
	int flags;
	
	int field_width;
	int precision;
	
	for(str=buff; *format; ++format)
	{//循环的每一躺，处理一个%及其后的字符串
		if(*format!='%')
		{
			*str++=*format;
			continue;
		}
		
		flags=0;
		repeat:
			++format;
			switch(*format)
			{
				case '-':
					flags |= LEFT;
					goto repeat;
				case '+':
					flags |= PLUS;
					goto repeat;
				case ' ':
					flags |= SPACE;
					goto repeat;
				case '#':
					flags |= SPECIAL;
					goto repeat;
				case '0':
					flags |= ZEROPAD;
					goto repeat;
			}
			
		// get field_width
		filed_width=-1;
		if(is_digit(*format))
			field_width=skip_atoi(&format);
		else  if(*format=='*')
		{
			field_width=va_arg(args, int);
			if(field_width<0)
			{
				field_width=-field_width;
				flags |= LEFT;
			}
		}
		
		// get precision
		precision=-1;
		if(*format=='.')
		{
			++format;
			if(is_digit(*format))
				precision=skip_atoi(&format);
			else if(*format=='*')
				precision=va_arg(args, int);
			if(precision<0)
				precision=0;
		}
		
		// 整型数的长度
		if(*format=='h'  ||  *format=='l'  ||  *format=='L')
			++format;
		
		// 根据类型转换说明符不同进行处理
		switch(*format)
		{
		case 'c':
			if(!(flags  &  LEFT))
			{
				while(--field_width >0)
					*str++=' ';
			}
			*str++=(unsigned char) va_arg(args, int);
			while(--filed_width>0)
				*str++=' ';
			break;
			
		case 's':
			s=va_arg(args, char*);
			len=strlen(s);
			if(precision<0)
				precision=len;
			else if(len>precision)
				len=precision;
			
			if(!(flags  &  LEFT))
			{
				while(len<filed_width--)
					*str++=' ';
			}
			for(i=0; i<len; ++i)
				*str++=*s++;
			while(len<filed_width--)
				*str++' ';
			break;
			
		case 'o':// octal
			str=number(str, va_arg(args, unsigned long), 8, field_width, precision, flags));
			break;
			
		case 'p':// pointer
			if(field_width==1)
			{
				field_width=8;
				flags |= ZEROPAD;
			}
			str=number(str, (unsigned long)va_arg(args, void*), 16, field_width, precision, flags);
			break;
			
		case 'x': // hex abcdef
			flags |= SMALL;
			
		case 'X': // HEX ABCEDF
			str = number(str, va_arg(args, unsigned long), 16, field_width, precision, flags);
			break;
			
		case 'd':// signed dec
		case 'i':// signed dec
			flags |= SIGN;
		case 'u': // unsigned dec
			str = number(str, va_arg(args, unsigned long), 10, field_width, precision, flags);
			break;
		case 'b'://bool ??
			str=number(str, va_arg(args, unsigned long), 2, field_width, precision, flags);
			break;
		
		case 'n':// 目前以写入的字符个数
			ip=va_arg(args, int*);
			*ip=(str-buff);
			break;
			
		default:
			if(*format!='%')
				*str++='%';
			if(*format)
				*str++=*format;
			else
				--format;
			break;
		}
	}
	*str='\0';
	
	return (str-buff);
}

