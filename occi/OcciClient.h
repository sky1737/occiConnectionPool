/*************************************************************************
	> File Name: OcciClient.cpp
	> Author: jiachuang
	> Mail: jiachuang0210@qq.com 
	> Created Time: Wed 18 May 2016 01:20:59 AM PDT
 ************************************************************************/
#ifndef _COCCICLIENT_
#define _COCCICLIENT_

#include <iostream>
#include "OcciConnectPool.h"
#include "OcciOperator.h"

using namespace std;
using namespace oracle::occi;
class COcciClient
{
public:
	typedef void (*fpHandleResult)(ResultSet *pRset);
	static bool Initialize(std::string const &strUserName, 
						   std::string const &strPassWord,
						   std::string const &strDbName,
						   unsigned int iMaxConn,
						   unsigned int iMinConn,
						   unsigned int iIncrConn);
	static bool Finalize();
	static COcciClient *GetClient()
	{
		assert(NULL != m_pClient);
		return m_pClient;
	}
	~COcciClient();
	//bool ExecuteQuery(std::string const &strSql, ResultSet *pRset);
	bool ExecuteQuery(std::string const &strSql, fpHandleResult fHandleResult);
	bool Execute(std::string const &strSql);
private:
	//std::string m_strUserName;
	//std::string m_strPassWord;
	//std::string m_strDbName;
	//unsigned int m_iMaxConn;
	//unsigned int m_iMinxConn;
	//unsigned int m_iIncrConn;
	COcciConnectPool *m_pPool;
	COcciOperator *m_pOperator;
	static COcciClient *m_pClient;
private:
	COcciClient(std::string const &strUserName, 
				   std::string const &strPassWord,
				   std::string const &strDbName,
				   unsigned int iMaxConn,
				   unsigned int iMinConn,
				   unsigned int iIncrConn);
};

#endif
