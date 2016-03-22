#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <dirent.h>
#include <turbojpeg.h>
#include "getpot/GetPot"

int width = 640, height=480;
int fps = 10;

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

void loadAllImages(const std::string &path)
{
    DIR *dirp = opendir(path.c_str());
    struct dirent* dp;
    
    std::vector<std::string> fileNames;
    while ((dp = readdir(dirp)) != NULL){
        if (getExt(dp->d_name) == "jpg") {
            fileNames.push_back(std::string(dp->d_name));
        }
    }
    
    std::sort(fileNames.begin(), fileNames.end());
    
    for(size_t i=0;i<fileNames.size();i++){
        unsigned char* rgbdata;
        int width,height;
        readJPEG(path + fileNames[i], rgbdata, width, height);
        write(1,rgbdata,width*height*3);
        free(rgbdata);
    }
}

inline char separator()
{
#if defined _WIN32 || defined __CYGWIN__
    return '\\';
#else
    return '/';
#endif
}

int main(int argc, char **argv)
{
    GetPot cl(argc,argv);
    
    std::string path   = cl.follow("",2,"-i","--input");
    
    if (path.length() ==  0 ){
        printf("Error: path not found\n");
        exit(-1);
    }
    
    if ( path[path.length()-1] != separator() ){
        path = path + separator();
    }
    
    std::string defaultMovie = path + std::string("movie.mp4");
    std::string output = cl.follow(defaultMovie.c_str(),2,"-o","--output");
    
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
        close(fd[1]);
        dup2(fd[0],0);
        close(fd[0]);
        
        execl("/usr/bin/avconv","/usr/local/bin/avconv", "-y","-r","10","-f", "rawvideo", "-vcodec", "rawvideo","-s", "640x480", "-pix_fmt", "rgb24", "-i", "-","-an", output.c_str(),(void*)0);

        printf("Failed to execute program");
        exit(-1);
    }
    else{
        close(fd[0]);
        dup2(fd[1],1);
        close(fd[1]);
        loadAllImages(path);
        close(1);
        exit(0);
    }
}
