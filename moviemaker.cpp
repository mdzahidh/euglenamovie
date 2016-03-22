#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <turbojpeg.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

int width = 640, height=480;
int fps = 10;
std::string path = "/Users/zhossain/tmp/images/";

std::string getExt(const std::string filename)
{
    std::string::size_type idx;
    idx = filename.rfind('.');
    if(idx != std::string::npos)
    {
        std::string extension = filename.substr(idx+1);
        return extension;
    }
    return "";
}

void readJPEG(const std::string &fullpath, unsigned char* &rgbdata, int &width, int &height)
{
    FILE* fp = fopen(fullpath.c_str(),"rb");
    fseek(fp,0,SEEK_END);
    size_t flen = ftell(fp);
    fseek(fp,0,SEEK_SET);
    unsigned char *jpegdata = (unsigned char*)malloc( flen );
    fread(jpegdata,flen,1,fp);
    fclose(fp);
    
    int jpegSubsamp=0;
    tjhandle _jpegDecompressor = tjInitDecompress();
    tjDecompressHeader2(_jpegDecompressor, jpegdata, flen, &width, &height, &jpegSubsamp);
    
    rgbdata =(unsigned char*) malloc( width * height * 3);
    tjDecompress2(_jpegDecompressor, jpegdata, flen, rgbdata, width, 0/*pitch*/, height, TJPF_RGB, TJFLAG_FASTDCT);
    
    free(jpegdata);
    tjDestroy(_jpegDecompressor);
}

void loadAllImages()
{
    DIR *dirp = opendir(path.c_str());
    struct dirent* dp;
    
    while ((dp = readdir(dirp)) != NULL){
        if (getExt(dp->d_name) == "jpg") {
            unsigned char* rgbdata;
            int width,height;
            readJPEG(path + std::string(dp->d_name), rgbdata, width, height);
            write(1,rgbdata,width*height*3);
            free(rgbdata);
        }
    }
}

int main(int argc, char **argv)
{
    int fd[2];
    pid_t cpid;
    
    pipe(fd);
    
    if((cpid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    if(cpid == 0)
    {
        std::string output = "movie.mp4";
        close(fd[1]);
        dup2(fd[0],0);
        close(fd[0]);
        
        execl("/usr/local/bin/avconv","/usr/local/bin/avconv", "-y","-r","10","-f", "rawvideo", "-vcodec", "rawvideo","-s", "640x480", "-pix_fmt", "rgb24", "-i", "-","-an", output.c_str());

        printf("Failed to execute program");
        exit(-1);
    }
    else{
        close(fd[0]);
        dup2(fd[1],1);
        close(fd[1]);
        loadAllImages();
        close(1);
        exit(0);
    }
}
