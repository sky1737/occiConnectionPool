/*************************************************************************
	> File Name: OcciOperater.h
	> Author: jiachuang
	> Mail: jiachuang0210@qq.com 
	> Created Time: Wed 18 May 2016 06:48:58 PM PDT
 ************************************************************************/

#ifndef _DBOPERATER_
#define _DBOPERATER_

#include <oracle/occi.h>
using namespace oracle::occi;

class COcciOperator
{
public:
	//typedef void (COcciOperator::*fpHandleResult)(ResultSet *pRset);
	COcciOperator(){};
	virtual ~COcciOperator();

	static bool Initialize();
	static bool Finalize();
	static COcciOperator *GetOcciOperator()
	{
		return m_pOperator;
	}
	//void HandleSelectFromModel(ResultSet *pRset);
	virtual bool SelectFromModel();
	virtual bool InsertIntoModel(std::string strModelId, std::string strPath);
	virtual bool DeleteFromModel(std::string strModelId);
	virtual bool DeleteInsertSelect(std::string strModelId, std::string strPath);
private:
	static COcciOperator *m_pOperator;
};

#endif
