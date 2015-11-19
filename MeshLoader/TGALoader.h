//========================================================
/**
*  @file      TGALoader.h
*
*  ��Ŀ������ TGA�ļ�������
*  �ļ�����:  ����ӳ��
*
*
*/
//========================================================
#ifndef __TGALOADER_H__
#define __TGALOADER_H__
/** TGA�ļ������� */
#include "baseDefine.h"
using std::string;
class CTGALoader
{
public:
	uint16 imageWidth;                         /**< ͼ���� */
	uint16 imageHeight;                        /**< ͼ��߶� */
	uchar byteCount;
	uchar *image;                   /**< ָ��ͼ�����ݵ�ָ�� */
	uchar ctType;
	uint32 size;
	uint32 ID;                        /**< ���������ID�� */
	uchar depth;
	CTGALoader();                             /**< ���캯�� */
	~CTGALoader();
	bool LoadTGA(const string& filename);          /**< ����TGA�ļ� */
	void FreeImage();                        /**< �ͷ��ڴ� */
	
};
#endif