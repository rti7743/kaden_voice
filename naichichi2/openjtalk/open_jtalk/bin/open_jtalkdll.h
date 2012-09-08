#ifndef OPEN_JTALK_CPP_H
#define OPEN_JTALK_CPP_H

struct OpenJTalk;
extern const char* OpenJTalk_GetLastError(OpenJTalk** openjtalk);
extern int OpenJTalk_Create(OpenJTalk** openjtalk,Mecab* mecab,int argc,const char **argv);
extern int OpenJTalk_synthesis_towav(OpenJTalk** openjtalk,const char* text, const char* wavfilename);
extern int OpenJTalk_synthesis_todata(OpenJTalk** openjtalk,const char* text, unsigned char** wavdata,int* wavesize);
extern int OpenJTalk_Delete(OpenJTalk** openjtalk);

#endif /* OPEN_JTALK_CPP_H */
