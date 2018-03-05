#include "fuse_header.h"
//-------------------------------------------------------------------------------------------------------------------------------------------------------
//DISK OPERATIONS





tree_t* create_tree(char* fname)
{
	tree_t *p;
	p = ALLOC(tree_t); 
	strcpy(p->fname, "fname");	
	// create a new tree; does not take care of reading an existing tree
	printf("inside craete");
	p->fp = fopen(fname, "rw+");
	p->root = 0;
	p->next_pos = 0;
	return p;
}
void write_data(tree_t* ptr_tree, struct block* p, int pos){
	if(pos == -1)
	{
		pos = ptr_tree->next_pos++;
	}
	//printf("writing %s at %d\n",p->dir_name,p->offset);
	fseek(ptr_tree->fp, pos * sizeof(struct block), SEEK_SET);
	fwrite(p, sizeof(struct block), 1, ptr_tree->fp);

}

void write_file(tree_t* ptr_tree, struct dir* p, int pos){
	if(pos == -1)
	{
		pos = ptr_tree->next_pos++;
	}
	printf("writing %s at %d\n",p->dir_name,pos);
	fseek(ptr_tree->fp, pos * sizeof(struct dir), SEEK_SET);
	fwrite(p, sizeof(struct dir), 1, ptr_tree->fp);

}
void disp_node(struct dir *p)
{
	printf("name: %s\n",p->dir_name);
	printf("dirnum: %d\n",p->dir_num);
	printf("numdirs: %d\n",p->num_subdirs);
}
void read_file(tree_t* ptr_tree, struct dir* p, int pos)
{
	fseek(ptr_tree->fp, pos * sizeof(struct dir), SEEK_SET);
	
	fread(p, sizeof(struct dir), 1, ptr_tree->fp);
	//printf("reading %s at %d\n",p->dir_name,p->offset);
	
}
void read_data(tree_t* ptr_tree, struct block* p, int pos)
{
	fseek(ptr_tree->fp, pos * sizeof(struct block), SEEK_SET);
	
	fread(p, sizeof(struct block), 1, ptr_tree->fp);
	//printf("reading %s at %d\n",p->dir_name,p->offset);
	
}
struct dir* load_fs_from_disk(struct dir* root,tree_t* tt){
	if( access( "mytree.dat", F_OK ) != -1 ) {
    		// file exists
    		read_file(tt,root, 0);
    		printf(" root is- %s",root->dir_name);
    		printf("offset %d\n",root->offset);
		printf("dirnum: %d\n",root->dir_num);
		printf("numdirs: %d\n",root->num_subdirs);
    		int i=0;
    		read_data(data,all_blocks,0);
    		printf("data us %s",all_blocks[0].elements);
    		printf("its actually %s %d",root->files[6].filename , root->files[6].is_in[0]);
    		printf("that is %s",all_blocks[root->files[4].is_in[0]].elements);
    		int j = 0 ;
    		
    		while(j<100){
    			read_data(data,&(all_blocks[j]),j);
    			j++;
    		
    		}
    		
    		while(i<root->num_subdirs){
    			//reading all subdirs
    			if(i==0){
    				
    				read_file(tt,&(root->sub[i]),root->offset);
    				root->sub[i].sub = (struct dir *)malloc(sizeof(struct dir)*10);
    				
    				printf(" dir is- %s",root->sub[i].dir_name);
    				printf("offset %d\n",root->sub[i].offset);
    				
    				if(root->sub[i].num_subdirs>0){
    					int k = 0;
    					while(k<root->sub[i].num_subdirs){
    						read_file(tt,&(root->sub[i].sub[k]),(root->sub[i].offset)*10);
    						printf("subby %s\n",(root->sub[i]).sub[0].dir_name);
    						printf("subdirectory %s\n",(root->sub[i]).sub[0].dir_name);
    						printf("subdirectory filename %s\n",(root->sub[i]).sub[0].files[0].filename);
    						
    						k++;
    					}
    				}
    				
				
    			}
    			else{
    				
    				read_file(tt,&root->sub[i],root->sub[i-1].offset);
    				root->sub[i].sub = (struct dir *)malloc(sizeof(struct dir)*10);
    				
    				printf(" dir is- %s",root->sub[i].dir_name);
    				printf("offset %d\n",root->sub[i].offset);
    				
    				
    				
    			}
    			disp_node(&(root->sub[root->num_subdirs]));
    			i++;
    		}

    		
	} 
	else {
		printf("WELCOME TO FUSE FILESYSTEM");
		// file doesn't exist
	}
	return root;	
}

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
			printf("---[ writing into buf files ]---\n");
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
	printf("-----------------------------------------------------\n\n---[ call to do_mkdir ]---\n");
	ans->sub[ans->num_subdirs].dir_num = ++dirn;
	printf("-----------------------------------------------------\n\n---[ call to do_mkdir ]---\n");
	ans->sub[ans->num_subdirs].sub = (struct dir *)malloc(sizeof(struct dir)*10);
	ans->sub[ans->num_subdirs].offset = global_off++; 
	strcpy(ans->sub[ans->num_subdirs].dir_name,last_only(pth));
	printf("---[ directory with name = %s made ! ]---\n",ans->sub[ans->num_subdirs].dir_name);
	
	
	

	if(ans->num_subdirs==0 && ans == root){
		
		write_file(tt,&(ans->sub[ans->num_subdirs]),ans->offset);
		ans->num_subdirs++;
		printf("n = %d",ans->num_subdirs);
		write_file(tt,ans,0);
		
		read_file(tt,ans,0);
		printf("n = %d",ans->num_subdirs);
		
	}
	else{
		if(ans->num_subdirs==0 && ans !=root){
			printf(" write %s, %d",ans->dir_name,ans->offset);
			write_file(tt,&(ans->sub[ans->num_subdirs]),(ans->offset)*10);
			ans->num_subdirs++;
			printf("n = %d",root->num_subdirs);
			write_file(tt,ans,root->offset);
			read_file(tt,ans,root->offset);
			printf("n = %d",root->num_subdirs);
		}
		else{
			write_file(tt,&(ans->sub[ans->num_subdirs]),ans->sub[ans->num_subdirs-1].offset);
			ans->num_subdirs++;
			printf("n = %d",root->num_subdirs);
			write_file(tt,root,0);
		
		
			read_file(tt,root,0);
			printf("n = %d",root->num_subdirs);
		}
	
	}
	//read_file(tt,&(ans->sub[ans->num_subdirs]), 0);
	disp_node(&(ans->sub[ans->num_subdirs]));
	
	
	
	
	return 0;
}
static int do_mknod(const char *path, mode_t m, dev_t d){
	printf("-----------------------------------------------------\n\n---[ call to do_mknod ]---\n");
	char *pth=(char *)malloc(sizeof(char)*strlen(path));
	strcpy(pth,path);
	struct dir *ans = from_path_get_dir(root,new_path_2(pth));
	ans->files[ans->num_files].inode_num = ++inoden;
	ans->files[ans->num_files].is_in[0] = cur_free_block_num;
	cur_free_block_num++;
	ans->files[ans->num_files].num_blocks=1;
	ans->files[ans->num_files].file_mode = m;
	strcpy(ans->files[ans->num_files].filename,last_only(pth));
	printf("---[ file with name = %s made ! ]---\n",ans->files[ans->num_files].filename);
	ans->num_files++;
	if(ans==root){	
		write_file(tt,ans,0);
		printf("n = %d",ans->num_files);
	
		
	read_file(tt,ans,0);
	printf("---[ file with name = %s made ! ]---\n",ans->files[ans->num_files].filename);
	printf("n = %d",ans->num_files);
		
	}
	else{
		write_file(tt,ans,(ans->offset)*10);
		printf("MKNOD %d %s",(ans->offset),ans->dir_name);
		read_file(tt,ans,(ans->offset)*10);
	printf("---[ file with name = %s made ! ]---\n",ans->files[ans->num_files-1].filename);
	printf("n = %d",ans->num_files);
	
	}
		
	
	
	
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
			int k=0;
			printf("2----------check----------\n");
			while(index<limit){
				//all_blocks[index].elements = (char *)malloc(sizeof(char)*100);
				printf("3----------check----------\n");
				memcpy( &(all_blocks[index].elements) , buf+off , 100);
				printf("4----------check----------\n");
				//copy each block and increment the offset
				off+=100;
				//update block_info
				ans->files[i].is_in[k] = index ;
				write_data(data,&(all_blocks[index]),index);
				k++; 
				index++;
			}
					printf("5----------check----------\n");
			//update the number of blocks file has occupied
			ans->files[i].num_blocks=blocks_needed;
			write_file(tt,root,0);
			/*printf("blocks_written %d",ans->files[i].num_blocks);
			printf("%s",all_blocks[index-1].elements);
			write_data(data,all_blocks,0);
			read_data(data,all_blocks,0);
			printf("hhhh %s",all_blocks[index-1].elements);
			*/
			return 100*(blocks_needed);
		}
	}
	return -ENOENT;
}
int cpy_dir(struct dir* a,struct dir* b)
{
	/*a.dir_num=b.dir_num;
	a.offset = b.offset ;
	strcpy(a.dir_name,b.dir_name);
	int i=0;
	while(i<a.num_files){
		a.files[i]=b.files[i];
	}
	a.num_subdirs=b.num_subdirs;
	a.num_files=b.num_files;*/
	
	*a=*b;

}
int rmdir_callback(const char *path){
	char *pth=(char *)malloc(sizeof(char)*strlen(path));
	strcpy(pth,path);
	printf("call to rmdir_callback = path is =---------%s--------\n",pth);
	int i;
	struct dir *ans = from_path_get_dir(root,new_path_2(pth));
	int c=0;
	
	for(i=0 ; i < ans->num_subdirs ;i++){
		//delete directory entirely
			if(c==1)
			{
				cpy_dir(&ans->sub[i-1],&ans->sub[i]);
				if(i-1==0){
					write_file(tt,&ans->sub[i-1],root->offset);
				}
				else{
					write_file(tt,&ans->sub[i-1],ans->sub[i-2].offset);
				}
				write_file(tt,&ans->sub[i],ans->sub[i-1].offset);
				printf("directory being moved %s to %s\n",ans->sub[i-1].dir_name,ans->sub[i].dir_name);
			}
		if(strcmp(ans->sub[i].dir_name,last_only(pth))==0)
		{
			
			if(ans->sub[i].num_files==0 && ans->sub[i].num_subdirs==0)
			{
				c=1;
				printf("directory being moved %s\n",ans->sub[i].dir_name);
			}
		}
	}
	ans->num_subdirs--;
	write_file(tt,ans,0);
	return 0;
}

/*
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
	.rmdir = rmdir_callback,
	/*.truncate = do_truncate,
	.opendir = do_opendir,
	.acess = do_acess,*/
	// what else ? , refer "fuse.h" for details about what each function is expected to do !yeah
};
int main(int argc, char *argv[]){
	printf("h");
	root = (struct dir *)malloc(sizeof(struct dir));
	all_blocks = (struct block *)malloc(sizeof(struct block)*1000);
	data=create_tree("data.dat");
	//write_data(data,all_blocks,0);
	if( access( "mytree.dat", F_OK ) != -1 ) {
		//file system already exist
		
		tt = create_tree("mytree.dat");
		fseek(tt->fp, 0L, SEEK_END);
		
		if(ftell(tt->fp)==0){
			root->dir_num=++dirn;
			root->num_subdirs=0;
			root->num_files=0;
			printf("tree read   ");
			root->offset = global_off++;
			root->sub = (struct dir *)malloc(sizeof(struct dir)*10);
			strcpy(root->dir_name,"root");
			write_file(tt,root,-1);	
		}
		else{
			printf("tree read   ");
			root->sub = (struct dir *)malloc(sizeof(struct dir)*10);
			root = load_fs_from_disk(root,tt);
			printf(" root is- %s",root->dir_name);
	    		printf("offset %d\n",root->offset);
			printf("dirnum: %d\n",root->dir_num);
			printf("numdirs: %d\n",root->num_subdirs);
			//printf("root %s",root->dir_name);
		}
	}
	else{
		tt = create_tree("mytree.dat");
		printf("tree created");
		root->dir_num=++dirn;
		root->num_subdirs=0;
		root->num_files=0;
		root->offset = global_off++;
		root->sub = (struct dir *)malloc(sizeof(struct dir)*10);
		strcpy(root->dir_name,"root");
		write_file(tt,root,-1);
		}
	return fuse_main(argc, argv, &fuse_example_operations, NULL);
}
// gcc usp_fuse_new.c `pkg-config fuse --cflags --libs`
