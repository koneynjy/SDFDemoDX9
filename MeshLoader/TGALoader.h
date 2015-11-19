//========================================================
/**
*  @file      TGALoader.h
*
*  项目描述： TGA文件载入类
*  文件描述:  纹理映射
*
*
*/
//========================================================
#ifndef __TGALOADER_H__
#define __TGALOADER_H__
/** TGA文件载入类 */
#include "baseDefine.h"
using std::string;
class CTGALoader
{
public:
	uint16 imageWidth;                         /**< 图像宽度 */
	uint16 imageHeight;                        /**< 图像高度 */
	uchar byteCount;
	uchar *image;                   /**< 指向图像数据的指针 */
	uchar ctType;
	uint32 size;
	uint32 ID;                        /**< 生成纹理的ID号 */
	uchar depth;
	CTGALoader();                             /**< 构造函数 */
	~CTGALoader();
	bool LoadTGA(const string& filename);          /**< 载入TGA文件 */
	void FreeImage();                        /**< 释放内存 */
	
};
#endif