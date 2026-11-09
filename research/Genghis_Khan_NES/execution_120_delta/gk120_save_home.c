#include "gk120_save_home.h"
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#ifndef _WIN32
# include <sys/stat.h>
# include <errno.h>
#endif

static size_t bounded_length(const char *s,size_t max){
 size_t n=0;
 if(!s)return 0;
 while(n<max && s[n])n++;
 return n;
}
static int absolute_clean(const char *p,size_t *length){
 size_t n=bounded_length(p,1024u);
 if(!n||n>=1024u||p[0]!='/')return 0;
 /* Reject ambiguity from traversal and relative segments without forbidding
  * ordinary dot-prefixed user paths such as ~/.local. */
 for(size_t i=0;i<n;){
  if(p[i]=='/') {i++;continue;}
  size_t start=i;
  while(i<n&&p[i]!='/')i++;
  size_t count=i-start;
  if((count==1u&&p[start]=='.')||
     (count==2u&&p[start]=='.'&&p[start+1u]=='.'))return 0;
 }
 *length=n;
 return 1;
}
static int plain_filename(const char *f,size_t *length){
 size_t n=bounded_length(f,129u);
 if(!n||n>127u||strstr(f,".."))return 0;
 for(size_t i=0;i<n;i++){
  unsigned char c=(unsigned char)f[i];
  if(!isalnum(c)&&c!='_'&&c!='-'&&c!='.')return 0;
 }
 *length=n;
 return 1;
}
int gk120_linux_path(const char *xdg,const char *home,
                     const char *filename,char *out,size_t capacity){
 if(!out||!capacity)return 0;
 size_t filename_len,root_len=0;
 if(!plain_filename(filename,&filename_len))return 0;
 const char *root=xdg;
 int use_home=0;
 if(!absolute_clean(xdg,&root_len)){
  if(!absolute_clean(home,&root_len))return 0;
  root=home;use_home=1;
 }
 /* Trailing '/' is already supplied by the caller's chosen directory. */
 while(root_len>1u&&root[root_len-1u]=='/')root_len--;
 const char *relative=use_home?"/.local/share/gk1-native-research/":
                                  "/gk1-native-research/";
 const size_t middle_len=strlen(relative);
 if(root_len>SIZE_MAX-middle_len ||
    root_len+middle_len>SIZE_MAX-filename_len-1u)return 0;
 size_t need=root_len+middle_len+filename_len+1u;
 if(need>capacity || need>1024u)return 0; /* compatible with 3-slot gk58 */
 char candidate[1024];
 memcpy(candidate,root,root_len);
 memcpy(candidate+root_len,relative,middle_len);
 memcpy(candidate+root_len+middle_len,filename,filename_len);
 candidate[need-1u]='\0';
 memcpy(out,candidate,need);
 return 1;
}
int gk120_prepare_linux_dir(const char *full_path){
#ifdef _WIN32
 (void)full_path;
 return 0;
#else
 size_t n=0;
 if(!absolute_clean(full_path,&n)||n>=1024u)return 0;
 size_t slash=n;
 while(slash>0u&&full_path[slash-1u]!='/')slash--;
 if(slash<=1u||slash>=n)return 0;
 char path[1024];
 memcpy(path,full_path,slash);
 path[slash]='\0';
 /* Respect the user's chosen absolute XDG/HOME directory.  Create missing
  * components one by one, but never create a save FILE or overwrite one. */
 for(size_t i=1;i<slash;i++){
  if(path[i]!='/'&&i!=slash-1u)continue;
  char stored=path[i];
  if(stored=='/')path[i]='\0';
  if(path[0]){
   struct stat st;
   if(stat(path,&st)!=0){
    if(errno!=ENOENT){path[i]=stored;return 0;}
    if(mkdir(path,0700)!=0&&errno!=EEXIST){path[i]=stored;return 0;}
    if(stat(path,&st)!=0){path[i]=stored;return 0;}
   }
   if(!S_ISDIR(st.st_mode)){path[i]=stored;return 0;}
  }
  path[i]=stored;
 }
 return 1;
#endif
}
