#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <time.h>

#define RL_BUFSIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"


int ar=0;
int check=0;
int alias_idx=0;
char* cmd_list[100];
int cmd_index=0;

int msh_cd(char** args,int argc);
int msh_help(char**args,int argc);
int msh_exit(char** args,int argc);
int msh_ls(char** args,int argc);
int msh_cat(char** args, int argc);
int msh_pwd(char** args, int argc);
int msh_history(char** args, int argc);
int msh_cp(char** args, int argc);
int msh_stat(char** args, int argc);
int msh_mkdir(char** args, int argc);
int msh_ln(char** args, int argc);
int msh_chmod(char** args, int argc);
int msh_ps(char** args, int argc);
int msh_alias(char** args, int argc);
int msh_rm(char** args, int argc);

typedef struct alias_list{
	int idx;
	char* command;
	char* origin_cm;
}alias_list;

alias_list alist[20];
char * builtin_str[30] = {
	"cd",
	"help",
	"exit",
	"ls",
	"cat",
	"pwd",
	"history",
	"cp",
	"stat",
	"mkdir",
	"ln",
	"chmod",
	"ps",
	"alias",
	"rm"

};

int (*builtin_func[])(char**,int) ={
	&msh_cd,
	&msh_help,
	&msh_exit,
	&msh_ls,
	&msh_cat,
	&msh_pwd,
	&msh_history,
	&msh_cp,
	&msh_stat,
	&msh_mkdir,
	&msh_ln,
	&msh_chmod,
	&msh_ps,
	&msh_alias,
	&msh_rm
};

int msh_num_builtins(char* str[]){
	int i=0;
	while(str[i] != '\0')
	{
		i++;
	}
	return i;
}

int msh_rm(char** args, int argc)
{

	if(argc !=2)
	{
		fprintf(stderr,"No enough arguments\n");
		return 1;
	}
	if(remove(args[1]) ==-1)
	{
		perror("delete failed.\n");
		return 1;
	}
	return 1;

}
int getCmdLine(char* file, char* buf)
{
	FILE* srcFp;
	srcFp = fopen(file,"r");

	memset(buf, 0,sizeof(buf));
	fgets(buf,256,srcFp);
	fclose(srcFp);

}

int msh_ps(char** args, int argc)
{
	DIR * dir;
	struct dirent *entry;
	struct stat fileStat;

	int pid=0;
	char cmdLine[256];
	char tempPath[256];
	char tty_self[256];
	char path[256];
	char *tty;
	int fd;
	int fd_self = open("/proc/self/fd/0",O_RDONLY);
	sprintf(tty_self,"%s",ttyname(fd_self));

	dir = opendir("/proc");

	printf("PID COMMAND\n");
	while((entry = readdir(dir)) !=NULL)
	{
		lstat(entry->d_name,&fileStat);
		if(!S_ISDIR(fileStat.st_mode))
			continue;
		pid = atoi(entry->d_name);
		if(pid <=0) continue;

		sprintf(path,"/proc/%d/fd/0",pid);
		fd = open(path,O_RDONLY);
		tty = ttyname(fd);

		sprintf(tempPath,"/proc/%d/cmdline",pid);
		getCmdLine(tempPath,cmdLine);
		
		if(tty&&strcmp(tty,tty_self) ==0)
		{
			printf("%d %s\n",pid,cmdLine);
		}

	}
	closedir(dir);

	return 1;
}

int msh_chmod(char **args, int argc)
{
	mode_t modes[] = {S_IRUSR,S_IWUSR,S_IXUSR,S_IRGRP,S_IWGRP,S_IXGRP,S_IROTH,S_IWOTH,S_IXOTH};
	mode_t mode=0;
	unsigned int omode;
	sscanf(args[1],"%o",&omode);
	for(int i=0; i<9; ++i)
	{
		mode |=(omode >>(8-i)& 1)? modes[i] :0;
	}
	
	if(argc !=3)
	{
		fprintf(stderr,"No enough arguments\n");
		return 1;
	}
	
	if(chmod(args[2],mode) !=0)
	{
		fprintf(stderr,"execute chmod failde.\n");
	}
	return 1;

}

int msh_ln(char** args, int argc)
{
	if(argc <3)
	{
		fprintf(stderr,"No enough arguments\n");
		return 1;
	}

	if(link(args[1],args[2])==-1)
	{
		fprintf(stderr,"Make link failed.\n");
	}
	return 1;
}

int msh_mkdir(char** args, int argc)
{
	if(argc < 2)
	{
		fprintf(stderr,"No enough arguments\n");

	}
	if(mkdir(args[1],0775))
	{
		fprintf(stderr,"Make directory failed.\n");
	}
	return 1;
}

int msh_stat(char** args, int argc)
{
	struct stat sb;
	if(argc !=2)
	{
		fprintf(stderr,"Usage: %s <pathname>\n",args[0]);
		exit(EXIT_FAILURE);
	}
	if(lstat(args[1],&sb) ==-1)
	{
		perror("lstat");
		exit(EXIT_FAILURE);
	}
	printf("ID of containing device: [%1x,%1x]\n",
			(long)major(sb.st_dev),(long)minor(sb.st_dev));
	printf("File type:		");
	switch(sb.st_mode & S_IFMT)
	{
		case S_IFBLK: printf("block device\n");
		case S_IFCHR: printf("character device\n");
		case S_IFDIR: printf("directory\n");
		case S_IFIFO: printf("FIFO/pipe\n");
		case S_IFLNK: printf("symlink\n");
		case S_IFREG: printf("regular file\n");
		case S_IFSOCK: printf("socket\n");
		default: printf("unknown?\n");
	}
	printf("I-node number:			%1d\n",(long)sb.st_ino);
	printf("Mode:				%1o (octal)\n",
			(unsigned long)sb.st_mode);
	printf("Link count:			%1d\n",(long)sb.st_nlink);
	printf("Ownership:			UID=%1d  GID=%1d\n",
			(long)sb.st_uid,(long)sb.st_gid);
	printf("Preferred I/O block size:	%1d bytes\n",
			(long)sb.st_blksize);
	printf("File size:			%11d bytes\n",
			(long long) sb.st_size);
	printf("Blocks allocated:		%11d\n",
			(long long)sb.st_blocks);
	printf("Last status change:		%s",ctime(&sb.st_ctime));
	printf("Last file access:		%s",ctime(&sb.st_atime));
	printf("Last file modification:		%s",ctime(&sb.st_mtime));

	
}

int msh_cp(char** args, int argc)
{
	int fdin,fdout;
	ssize_t nread;
	char buf[1024];
	if(argc !=3)
	{
		fprintf(stderr,"no enough filename\n");
		return 1;
	}
	fdin = open(args[1],O_RDONLY);
	if(fdin ==-1){
		perror("open source");
		exit(1);
	}

	fdout = open(args[2],O_WRONLY|O_CREAT|O_EXCL,0644);
	if(fdout ==-1)
	{
		perror("open source");
		exit(1);
	}

	while((nread = read(fdin,buf,sizeof(buf)))>0)
	{
		
		if(write(fdout,buf,nread)>0)
		{
			break;
		}
	}

}

int msh_history(char** args, int argc)
{
	int check=0;
	int i= cmd_index;
	int n =1;
	while(check<=30)
	{
		if(i<=0)break;
		if(n>cmd_index)break;
		printf("%d %s\n",n,cmd_list+n);
		n++;
		i--;
		check++;
	}
	return 1;

}

int msh_cat(char** args, int argc)
{

	FILE * fp;
	char buf[255];
	char* filename = NULL;
	int line_number = 0;

	if(argc < 2)
	{
		printf("Usage:mini_shell filename\n");
		return 1;
	}
	for(int i=0; i<argc-1; i++)
	{

		filename = args[i+1];
		fp = fopen(filename,"r");
		if(fp ==NULL)
		{
			printf("Can`t open %s\n",filename);
			return 1;
		}
		while(fgets(buf,255,fp) != NULL)
		{
			printf("%d %s", line_number++,buf);
		}
		fclose(fp);

	}	


	return 1;
}

int msh_pwd(char** agrs, int argc)
{
	char buf[256];
	getcwd(buf,256);
	printf("%s\n",buf);
	return 1;
}


int msh_ls(char** args,int argc){
	char* cwd = (char*) malloc(sizeof(char)*1024);
	memset(cwd,0,1024);
	
	DIR* dir = NULL;
	struct dirent* entry;
	struct stat buf;
	getcwd(cwd,1024);

	if((dir = opendir(cwd)) == NULL)
	{
		printf("current directory error\n");
		exit(1);	
	}
	if(argc !=1)
	{
		printf("Error  arguments\n");
		return 1;
	}
	while((entry = readdir(dir)) !=NULL)
	{
		lstat(entry->d_name,&buf);
		if(S_ISREG(buf.st_mode))
		{
			if(entry->d_name[0] =='.')
				continue;
			printf("FILE   ");
		}
		else if(S_ISDIR(buf.st_mode))
		{

			if(entry->d_name[0] =='.')
				continue;
			printf("DIR    ");
		}
		else
			printf("???   ");
		
		printf("%s   \n",entry->d_name);
	}


	free(cwd);
	closedir(dir);
	return 1;
}

int msh_cd(char** args,int argc){
	if(args[1] ==NULL){
		fprintf(stderr,"hongjin:expected argument to \"cd\"\n");
	}else{
		if(chdir(args[1]) !=0){
			perror("cd error");
		}
	}
	return 1;
}

int msh_help(char** args,int argc)
{
	int i;
	printf("Hongjin`s Mini Shell \n");

	for(i=0;i<msh_num_builtins(builtin_str);i++){
		printf(" %s\n",builtin_str[i]);
	}

	return 1;
}


int msh_exit(char** args,int argc){
	return 0;
}

void eliminate(char* str, char ch)
{

	for(; *str!='\0';str++)
	{
		if(*str ==ch)
		{
			strcpy(str,str+1);
			str--;
		}
	}
}

int msh_alias(char** args, int argc)
{
	char temp[20];
	char* tok[2];
	int k=0;
	char* tmp;
	char* tmp_origin;
	tmp = (char*)malloc(sizeof(char)*msh_num_builtins(builtin_str));
	tmp_origin = (char*)malloc(sizeof(char)*10);
	
	strcpy(temp,args[1]);
	if(argc != 2)
	{
		fprintf(stderr,"No enough arguments\n");
	}
	
	eliminate(temp,'\'');

	char* ptr = strtok(temp,"=");
	while(ptr !=NULL)
	{
		tok[k] = ptr;
		k++;
		ptr = strtok(NULL,"=");
	}
	strcpy(tmp,tok[0]);	
	strcpy(tmp_origin,tok[1]);
	for(int i=0;i<msh_num_builtins(builtin_str);i++)
	{
		if(strcmp(builtin_str[i],tok[1])==0)
		{
			if(msh_num_builtins(builtin_str)<100)
			{
				alist[alias_idx].idx=i;
				alist[alias_idx].command=tmp;
				alist[alias_idx].origin_cm=tmp_origin;
				alias_idx++;
				builtin_str[msh_num_builtins(builtin_str)] = tmp;
				printf("Success!\n");
				return 1;
			}else
			{
				printf("builtin limit over\n");
				exit(1);
			}

		}
		
	}
free(tmp);
free(tmp_origin);
return 1;
}


int msh_execute(char** args,int argc){
	int i;
	char* ago_string;
	ago_string = strtok(args[0],"!");
	int ago_index = atoi(ago_string);
	int direc = 0;

	if(args[0] == NULL)
	{
		return 1;
	}

	for(i=0;i<msh_num_builtins(builtin_str);i++)
	{
		if(strcmp(args[0],builtin_str[i])==0)
		{
			check=0;
			if(strcmp(args[0],"alias") ==0)
			{
				if(argc == 1)
				{
					for(int j=0; j<alias_idx; j++)
						printf("alias %s=\'%s\'\n ",alist[j].command,alist[j].origin_cm);
				
					strcpy(cmd_list+(++cmd_index),args[0]);
					return 1;
				}
				else if( argc==2)
				{
					strcpy(cmd_list+(++cmd_index),args[1]);
				
				}
				else
				{
					printf("arguments Error\n");
					return 1;
				}

			}
			else
			{
				strcpy(cmd_list+(++cmd_index),args[0]);
			}
			if(i>14)
			{
				for(int k=0;k<alias_idx; k++)
				{
					if(strcmp(alist[k].command,args[0])==0)
					{
						return (*builtin_func[alist[k].idx])(args,argc);
					}
				}
			}
			else{
				return (*builtin_func[i])(args,argc);
			}
		}
		else if(strchr(args[0],'!')!=NULL)
		{
			direc = ago_index;
			if(direc>0)
			{
				strcpy(ago_string,(cmd_list+direc));
				if(strcmp(ago_string,builtin_str[i])==0)
				{
				
			
					if(i>14)
					{
						for(int k=0;k<alias_idx; k++)
						{
							if(strcmp(alist[k].command,ago_string)==0)
							{
								return (*builtin_func[alist[k].idx])(args,argc);
							}
						}
					}
					else{
						return (*builtin_func[i])(args,argc);
					}
				}
			}
			else{
				printf("not found index: %d\n",ago_index);
			}
		}
		else{
			check = 1;
		
		}

	}
	return 1;
}

char* msh_read_line(void)
{
	int bufsize = RL_BUFSIZE;
	int position = 0;
	char* buffer = malloc(sizeof(char*)*bufsize);
	char c;

	if(!buffer){
		fprintf(stderr,"hj: allocation error\n");
		exit(1);
	}
	while(1)
	{
		c = getchar();

		if(c ==EOF || c=='\n'){
			buffer[position] = '\0';
			return buffer;
		}else{
			buffer[position] = c;
		}
		position++;

		if(position >=bufsize)
		{
			bufsize += RL_BUFSIZE;
			buffer = realloc(buffer,bufsize);
			if(!buffer){
				fprintf(stderr,"hj: allocation error\n");
				exit(1);
			}
		}
	}
}


char** msh_split_line(char* line){
	int bufsize = TOK_BUFSIZE,position = 0;
	char**tokens = malloc(bufsize*sizeof(char*));
	char* token;

	if(!tokens){
		fprintf(stderr,"hj: allocation error\n");
		exit(1);
	}

	token = strtok(line,TOK_DELIM);
	while(token !=NULL)
	{
		tokens[position] = token;
		position++;

		if(position>=bufsize){
			bufsize +=TOK_BUFSIZE;
			tokens = realloc(tokens,bufsize*sizeof(char*));
			if(!tokens)
			{	
				fprintf(stderr,"hj: allocation error\n");
				exit(1);
			}
		}
		token = strtok(NULL, TOK_DELIM);
	}
	tokens[position] = NULL;
	ar = position;
	return tokens;
}


int main(int argc, char**argv)
{
	char* line;
	char** args;
	char status;	
	

	do{
		printf("> ");
		line = msh_read_line();
		args = msh_split_line(line);
		status=	msh_execute(args,ar);
		
		 if(check ==1)
		{
			printf("Command %s is not found\n",args[0]);
		}
		free(line);
		free(args);

	}while(status);
	return 0;
}


