#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
int cur_free_block_num=0;
int *holes;
int num_holes=0;
int inoden = 0,dirn = 0;
struct block{
	char *elements;
};
struct block *all_blocks;
struct inode{
	mode_t file_mode;
	int inode_num;
	int *is_in;
	int num_blocks;
	char *filename;
};
struct dir{
	int dir_num;
	char *dir_name;
	struct inode *files;
	struct dir *sub;
	int num_subdirs;
	int num_files;
};
struct dir *root;
//-------------------------------------------------------------------------------------------------------------------------------------------------------
char *last_only(char *pth){
	char *ans=(char *)malloc(sizeof(strlen(pth)));
	int len=strlen(pth);
	if(strlen(pth)==1)return "/";
	while(pth[len-1]!='/'){
		len--;
	}
	int i,k=0;
	for(i=len;i<strlen(pth);i++){
		ans[k++]=pth[i];
	}
	ans[k]='\0';
	return ans;
}
char *par(char *pth){
	int i=0,k=0;
	char *ans=(char *)malloc(sizeof(char)*100);
	if(strlen(pth)==1)return "/";
	i++;
	while(pth[i]!='/' && pth[i]!='\0'){
		ans[k++]=pth[i];
		i++;
	}
	ans[k]='\0';
	return ans;
}
char *new_path(char *pth){
	int i=0,k=0;
	char *ans = (char *)malloc(sizeof(char)*strlen(pth));
	if(strlen(pth)==1)return "/";
	else{
		i++;
		while(pth[i]!='/' && pth[i]!='\0'){
			i++;
		}
		while(pth[i]!='\0'){
			ans[k++]=pth[i];
			i++;
		}
	}
	ans[k]='\0';
	if(strlen(ans)==0)return "/";
	return ans;
}
char *new_path_2(char *pth){
	char *ans=(char *)malloc(sizeof(char)*strlen(pth));
	char *ans_2=(char *)malloc(sizeof(char)*strlen(pth));
	int i=strlen(pth)-1,k=0,k2=0;
	while(pth[i]!='/'){
		ans[k++]=pth[i];
		i--;
	}
	for(i=0;i<strlen(pth)-k-1;i++){
		ans_2[k2++]=pth[i];
	}
	ans_2[k2]='\0';
	if(ans_2[0]!='/')return "/";
	return ans_2;
}
struct dir *from_path_get_dir(struct dir *direc,char *pth){
	int i;	
	char *ans = par(pth);
	if(strlen(pth)==1)return direc;
	for(i=0;i < direc->num_subdirs ; i++){
		if(strcmp(direc->sub[i].dir_name,ans)==0){
			pth = new_path(pth);
			if(strlen(pth)==1){
				return &(direc->sub[i]);
			}
			else{
				return from_path_get_dir(&(direc->sub[i]),pth);
			}
		}
	}
	return NULL;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------
static int getattr_callback(const char *path, struct stat *stbuf){
	printf("-----------------------------------------------------\n\n---[ call to getattr_callback ]---\n");
	char *pth=(char *)malloc(sizeof(char)*strlen(path));
	strcpy(pth,path);
	printf("---[ pth here is %s ]---\n",pth);
	struct dir *ans = from_path_get_dir(root,pth);
	if(ans != NULL){
		printf("---[ immediate parent is %s ]---\n",ans->dir_name);
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return 0;
	}
	else{
		int i;
		char *file=last_only(pth);
		printf("---[ file data requested is %s ]---\n",last_only(pth));
		struct dir *ans = from_path_get_dir(root,new_path_2(pth));
		printf("---[ immediate parent is %s ]---\n",ans->dir_name);
		for(i=0;i<ans->num_files;i++){
			if(strcmp(ans->files[i].filename,file)==0){
				stbuf->st_mode = S_IFREG | 0777;
				stbuf->st_nlink = 1;
				stbuf->st_size = 100;
				return 0;
			}
		}
	}
	memset(stbuf, 0, sizeof(struct stat));
	return -ENOENT;
}
static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi){
	(void) offset;
	(void) fi;
	int i;
	printf("-----------------------------------------------------\n\n---[ call to readdir_callback ]---\n");
	char *pth = (char *)malloc(sizeof(char)*strlen(path));
	strcpy(pth,path);
	printf("---[ pth here is %s ]---\n",pth);
		struct dir *ans = from_path_get_dir(root,pth);
	printf("---[ immediate parent is %s ]---\n",ans->dir_name);
		for(i=0;i<ans->num_subdirs;i++){
			printf("---[ writing into buf ]---\n");
			filler(buf ,ans->sub[i].dir_name , NULL ,0);
		}
		for(i=0;i<ans->num_files;i++){
			printf("---[ writing into buf ]---\n");
			filler(buf , ans->files[i].filename , NULL ,0);
		}
	return 0;
}
static int open_callback(const char *path, struct fuse_file_info *fi){
	return 0;
}
static int do_mkdir(const char *path, mode_t m){
	printf("-----------------------------------------------------\n\n---[ call to do_mkdir ]---\n");
	char *pth=(char *)malloc(sizeof(char)*strlen(path));
	strcpy(pth,path);
	struct dir *ans = from_path_get_dir(root,new_path_2(pth));
	ans->sub[ans->num_subdirs].dir_num = ++dirn;
	ans->sub[ans->num_subdirs].sub = (struct dir *)malloc(sizeof(struct dir)*10);
	ans->sub[ans->num_subdirs].files = (struct inode *)malloc(sizeof(struct inode)*10);
	ans->sub[ans->num_subdirs].dir_name = (char *)malloc(sizeof(char)*10);
	strcpy(ans->sub[ans->num_subdirs].dir_name,last_only(pth));
	printf("---[ directory with name = %s made ! ]---\n",ans->sub[ans->num_subdirs].dir_name);
	ans->num_subdirs++;
	return 0;
}
static int do_mknod(const char *path, mode_t m, dev_t d){
	printf("-----------------------------------------------------\n\n---[ call to do_mknod ]---\n");
	char *pth=(char *)malloc(sizeof(char)*strlen(path));
	strcpy(pth,path);
	struct dir *ans = from_path_get_dir(root,new_path_2(pth));
	ans->files[ans->num_files].inode_num = ++inoden;
	ans->files[ans->num_files].is_in=(int *)malloc(sizeof(int)*1);
	// not checking for holes
		ans->files[ans->num_files].is_in[0] = cur_free_block_num;
		cur_free_block_num++;
		ans->files[ans->num_files].num_blocks=1;
	ans->files[ans->num_files].file_mode = m;
	ans->files[ans->num_files].filename = (char *)malloc(sizeof(char)*10);
	strcpy(ans->files[ans->num_files].filename,last_only(pth));
	printf("---[ file with name = %s made ! ]---\n",ans->files[ans->num_files].filename);
	ans->num_files++;
	return 0;
}
static int read_callback(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi){
	printf("-----------------------------------------------------\n\n---[ call to read_callback ]---\n");
	char *pth=(char *)malloc(sizeof(char)*strlen(path));
	strcpy(pth,path);
	int i;
	char *file=last_only(pth);
	struct dir *ans = from_path_get_dir(root,new_path_2(pth));
	for(i=0;i < ans->num_files ;i++){
		if(strcmp(file,ans->files[i].filename) == 0){
			//do stuff
			
			int limit = ans->files[i].num_blocks;
			int off=0;
			int j=0;
			while(j<limit){
				memcpy(buf+off, &( all_blocks[ans->files[i].is_in[j]]) , 100);
				j++;
				off += 100;
			}
			return (100*limit);
		}
	}
	return -ENOENT;
}
int write_callback(const char *path, const char *buf, size_t size, off_t offset,struct fuse_file_info *i_really_dontcare){
	printf("-----------------------------------------------------\n\n---[ call to write_callback ]---\n");
	char *pth=(char *)malloc(sizeof(char)*strlen(path));
	strcpy(pth,path);
	printf("---[ pth here is %s ]---\n",pth);
	char *file=last_only(pth);
	printf("---[ file to be written is %s ]---\n",file);
	int i;
	struct dir *ans = from_path_get_dir(root,new_path_2(pth));
	printf("---[ immediate parent is %s ]---\n",ans->dir_name);
	for(i=0;i < ans->num_files ; i++){
		if(strcmp(file,ans->files[i].filename) == 0){
			printf("---[found file to be written = %s ]---\n",ans->files[i].filename);
			int inode = ans->files[i].inode_num;
			int s_buf = strlen(buf);
			float blocks_need = s_buf/100;
			int blocks_needed;
			if(blocks_need == (float)blocks_needed) blocks_needed=blocks_need;
			else	blocks_needed = 1 + (int)blocks_need;
			printf("---[ number of blocks needed = %d ]---\n",blocks_needed);
			int index = cur_free_block_num;
			int limit = blocks_needed+cur_free_block_num;
			int off = 0 ;
			//update block numbers;
			printf("1----------check----------\n");
			ans->files[i].is_in = (int *)realloc(ans->files[i].is_in,sizeof(int)*blocks_needed);
			int k=0;
			printf("2----------check----------\n");
			while(index<limit){
				all_blocks[index].elements = (char *)malloc(sizeof(char)*100);
				printf("3----------check----------\n");
				memcpy( &(all_blocks[index].elements) , buf+off , 100);
				printf("4----------check----------\n");
				//copy each block and increment the offset
				off+=100;
				//update block_info
				ans->files[i].is_in[k] = index ;
				k++; 
				index++;
			}
					printf("5----------check----------\n");
			//update the number of blocks file has occupied
			ans->files[i].num_blocks=blocks_needed;
			return 100*(blocks_needed);
		}
	}
	return -ENOENT;
}
/*int rmdir_callback(const char *path){
	char *pth=(char *)malloc(sizeof(char)*strlen(path));
	strcpy(pth,path);
	printf("call to rmdir_callback = path is =---------%s--------\n",pth);
	int i;
	struct dir *ans = from_path_get_dir(root,new_path_2(pth));
	for(i=0 ; i < ans->num_subdirs ;i++){
		if(strcmp(ans->sub[i].dir_name,last_only(pth))==0){
			//delete directory entirely
		}
	}
	return 0;
}
int unlink_callback(const char *path){
	char *pth=(char *)malloc(sizeof(char)*strlen(path));
	strcpy(pth,path);
	int i;
	printf("call to unlink_callback = path is =---------%s--------\n",pth);
	struct dir *ans = from_path_get_dir(root,new_path_2(pth));
	for(i=0 ; i < ans->num_files ;i++){
		if(strcmp(ans->files[i].filename,last_only(pth))==0){
			//delete that file and move all other files ?
		}
	}
	return 0;
}*/
static struct fuse_operations fuse_example_operations = {
	.getattr = getattr_callback,
	.open = open_callback,
	.readdir = readdir_callback,
	.mknod = do_mknod,
	.mkdir = do_mkdir,
	.read = read_callback,
	.write = write_callback,
	//.unlink = unlink_callback,
	//.rmdir = rmdir_callback,
	/*.truncate = do_truncate,
	.opendir = do_opendir,
	.acess = do_acess,*/
	// what else ? , refer "fuse.h" for details about what each function is expected to do !yeah
};
int main(int argc, char *argv[]){
	root = (struct dir *)malloc(sizeof(struct dir));
	all_blocks = (struct block *)malloc(sizeof(struct block)*1000);
	root->dir_num=++dirn;
	root->num_subdirs=0;
	root->num_files=0;
	root->dir_name = (char *)malloc(sizeof(char)*10);
	strcpy(root->dir_name,"root");
	root->files = (struct inode *)malloc(sizeof(struct inode)*10);
	root->sub = (struct dir *)malloc(sizeof(struct dir)*10);
	return fuse_main(argc, argv, &fuse_example_operations, NULL);
}
// gcc usp_fuse_new.c `pkg-config fuse --cflags --libs`
