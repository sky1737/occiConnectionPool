/*************************************************************************
	> File Name: OcciClient.cpp
	> Author: jiachuang
	> Mail: jiachuang0210@qq.com 
	> Created Time: Wed 18 May 2016 01:20:59 AM PDT
 ************************************************************************/

#include <iostream>
#include <cassert>
#include <oracle/occi.h>
#include "OcciClient.h"
#include "log/Log.h"

//#define MYLOG(str) std::cout << "[" << __FILE__ ":" << __FUNCTION__ << " <" << __LINE__ << ">]" << str << std::endl

using namespace oracle::occi;

bool COcciClient::Initialize(std::string const &strUserName, 
						   std::string const &strPassWord,
						   std::string const &strDbName,
						   unsigned int iMaxConn,
						   unsigned int iMinConn,
						   unsigned int iIncrConn)
{
	try
	{
		m_pClient = new COcciClient(strUserName, strPassWord, strDbName, iMaxConn,iMinConn,iIncrConn);
	}
	catch(std::exception &e)
	{
		MYLOG(FATAL, "Init OcciClient failed.");
		return false;
	}
	return true;
}
bool COcciClient::Finalize()
{
	assert(nullptr != m_pClient);
	//TODO -DDEBUG assert 会消失么
	if (nullptr != m_pClient)
	{
		delete m_pClient;	
		m_pClient = nullptr;
		return true;
	}

	return false;
}


COcciClient::~COcciClient()
{
	assert(nullptr != m_pPool);
	if (nullptr != m_pPool)
	{
		delete m_pPool;
		m_pPool = nullptr;
	}
}

COcciClient::COcciClient(std::string const &strUserName,
						   std::string const &strPassWord,
						   std::string const &strDbName,
						   unsigned int iMaxConn,
						   unsigned int iMinConn,
						   unsigned int iIncrConn)
{
	//m_strUserName = strUserName;
	//m_strPassword = strPassWord;
	//m_strDBName = strDbName;
	//m_iMaxConn = iMaxConn;
	//m_iMinConn = iMinConn;
	//m_iIncrConn = iIncrConn;
	
	m_pPool = new COcciConnectPool(strUserName, strPassWord, strDbName, iMaxConn, iMinConn, iIncrConn);
	//m_pOperator = new OcciOperator();
}

//bool COcciClient::ExecuteQuery(std::string const &strSql, ResultSet *pRset)
bool COcciClient::ExecuteQuery(std::string const &strSql, fpHandleResult fHandleResult)
{
	assert(NULL != m_pPool);
	Statement *pStmt = NULL;
	Connection *pConn = NULL;
	ResultSet *pRset = NULL;
	try
	{
		pConn = m_pPool->GetConnection();
		if (pConn != NULL)
		{
			MYLOG(DEBUG, "GetConnect Success!");
			pStmt = pConn->createStatement(strSql);
			pRset = pStmt->executeQuery();
			MYLOG(DEBUG, strSql.c_str());
			fHandleResult(pRset);
			pStmt->closeResultSet(pRset);
			pConn->terminateStatement(pStmt);
		}
		else
		{
			MYLOG(ERROR, "new connection from pool failed");
			return false;
		}
		m_pPool->ReleaseConnection(pConn);
	}
	catch(SQLException sqlEx)
	{
		MYLOG(ERROR, "Exception throw when executeQuery.errorCode:%d, errorMessage:%s", sqlEx.getErrorCode(), sqlEx.getMessage().c_str());
		return false;
	}
	return true;
}
bool COcciClient::Execute(std::string const &strSql)
{
	assert(NULL != m_pPool);
	Statement *pStmt = NULL;
	Connection *pConn = NULL;
	try
	{
		pConn = m_pPool->GetConnection();
		if (pConn != NULL)
		{
			MYLOG(DEBUG, "GetConnect Success!");
			pStmt = pConn->createStatement(strSql);
			MYLOG(DEBUG, strSql.c_str());
			pStmt->execute();
			pConn->terminateStatement(pStmt);
		}
		else
		{
			MYLOG(ERROR, "GetConnect failed!");
			return false;
		}
		m_pPool->ReleaseConnection(pConn);
	}
	catch(SQLException sqlEx)
	{
		MYLOG(ERROR, "Exception throw when execute.errorCode:%d, errorMessage:%s", sqlEx.getErrorCode(), sqlEx.getMessage().c_str());
		return false;
	}
	return true;
}
