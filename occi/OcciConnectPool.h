/*************************************************************************
	> File Name: COcciConnectPool.h
	> Author: jiachuang
	> Mail: jiachuang0210@qq.com 
	> Created Time: Tue 17 May 2016 09:46:03 PM PDT
 ************************************************************************/
#ifndef _COCCICONNECTPOOL_
#define _COCCICONNECTPOOL_

#include <oracle/occi.h>
using namespace oracle::occi;
class COcciConnectPool
{
public:
	COcciConnectPool(const std::string &strUserName,
					 const std::string &strPassword,
					 const std::string &strDBName,
					 unsigned int iMinConn,
					 unsigned int iMaxConn,
					 unsigned int iIncrConn);
	~COcciConnectPool();
	bool Init(void);
	Connection *GetConnection();
	bool ReleaseConnection(Connection *pConnect);
private:
	Environment *m_pEnv;
	StatelessConnectionPool *m_pConnPool;

	std::string m_strUserName;
	std::string m_strPassword;
	std::string m_strDBName;
	unsigned int m_iMinConn;
	unsigned int m_iMaxConn;
	unsigned int m_iIncrConn;
};
#endif
