#include "common.h"
#include "XLDebugUtil.h"


/**
 * MemoryToFile:		メモリをファイルに
 *
 * @param inFilename	ファイル名
 * @param inMemory		メモリ
 * @param inSize		メモリサイズ
 * @return void 
 */
void XLDebugUtil::MemoryToFile(const std::string& inFilename ,const void* inMemory , int inSize)
{
	FILE * fp = fopen( inFilename.c_str() ,"wb");
	if (fp == NULL)
	{
		assert(0);
		return;
	}

	fwrite( inMemory  ,sizeof(char) , inSize , fp);

	fclose(fp);
}

/**
 * MemoryToFile:		メモリをファイルに
 *
 * @param inFilename	ファイル名
 * @param inMemory		メモリ
 * @return void 
 */
void XLDebugUtil::MemoryToFile(const std::string& inFilename ,const char* inMemory )
{
	XLDebugUtil::MemoryToFile(inFilename , inMemory , strlen(inMemory) );
}

/**
 * MemoryToFile:		メモリをファイルに
 *
 * @param inFilename	ファイル名
 * @param inMemory		メモリ
 * @return void 
 */
void XLDebugUtil::MemoryToFile(const std::string& inFilename ,std::string& inMemory )
{
	XLDebugUtil::MemoryToFile(inFilename , inMemory.c_str() , inMemory.size() );
}

/**
 * FileCheck:			二つのファイルが同じかどうかチェック.
 *
 * @param inSrc			ファイル名1
 * @param inDest		ファイル名2
 * @return void 
 */
void XLDebugUtil::FileVerify(const std::string& inSrc,const std::string& inDest)
{
	FILE* src = fopen(inSrc.c_str() , "rb" );
	assert(src);
	FILE* dest = fopen(inDest.c_str() , "rb" );
 	assert(dest);

	std::string error;

	char* srcBuffer = new char[1024];
	char* destBuffer = new char[1024];
	while(1)
	{
		int srclen = fread( srcBuffer , 1 , 1024 , src);
		int destlen = fread( destBuffer , 1 , 1024 , dest);

		if (srclen == 0 && destlen == 0) break;

		if (srclen != destlen)
		{
			fclose(src);			fclose(dest);
			//サイズ不一致
			assert(0);
			break;
		}
		if ( memcmp(srcBuffer ,  destBuffer ,srclen) != 0)
		{
			fclose(src);			fclose(dest);
			//内容不一致
			assert(0);
			break;
		}
	}
	fclose(src);			fclose(dest);
	delete []  srcBuffer ;
	delete [] destBuffer ;
}
