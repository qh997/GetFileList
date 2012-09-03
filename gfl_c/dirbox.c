#include "dirbox.h"
#include <stdio.h>

void traversal_dir(char *dir_path)
{
    DIR *dir;

    if (NULL != (dir = opendir(dir_path)))
    {
        char olddir[FILENAME_MAX];
        getcwd(olddir, FILENAME_MAX);

        chdir(dir_path);

        struct dirent *entry;
        while (NULL != (entry = readdir(dir)))
        {
            struct stat statbuf;
            lstat(entry->d_name, &statbuf);
            if(S_ISDIR(statbuf.st_mode))
            {
                if(0 == strcmp(".", entry->d_name) || 0 == strcmp("..", entry->d_name))
                    continue;

                printf("dir : %s\n", entry->d_name);
            }
            else if(S_ISREG(statbuf.st_mode))
            {
                printf("file : %s\n", entry->d_name);
            }
            else if(S_ISLNK(statbuf.st_mode))
            {
                printf("link : %s\n", entry->d_name);
            }
        }

        chdir(olddir);
        closedir(dir);
    }
    else
    {
        fprintf(stderr,"Cannot open directory: %s\n", dir_path);
    }
}
