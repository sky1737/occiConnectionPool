/*************************************************************************
	> File Name: occiConnectPool.cpp
	> Author: jiachuang
	> Mail: jiachuang0210@qq.com 
	> Created Time: Tue 17 May 2016 08:31:06 PM PDT
 ************************************************************************/

#include <iostream>
#include <exception>
#include "OcciConnectPool.h"
#include <oracle/occi.h>
#include <log/Log.h>

using namespace oracle::occi;
using namespace std;

COcciConnectPool::COcciConnectPool(const string &strUserName,
						 const string &strPassword,
						 const string &strDBName,
						 unsigned int iMaxConn,
						 unsigned int iMinConn,
						 unsigned int iIncrConn)
{
	m_pEnv = NULL;
	m_pConnPool = NULL;

	m_strUserName = strUserName;
	m_strPassword = strPassword;
	m_strDBName = strDBName;
	m_iMaxConn = iMaxConn;
	m_iMinConn = iMinConn;
	m_iIncrConn = iIncrConn;

	if (!Init())
	{
		MYLOG(FATAL, "new OcciConnectPool failed.");
		throw exception();
	}
}

bool COcciConnectPool::Init(void)
{
	try
	{
		//m_pEnv = Environment::createEnvironment();
		m_pEnv = Environment::createEnvironment(Environment::THREADED_MUTEXED);
		if (nullptr == m_pEnv)
		{
			MYLOG(FATAL, "Create occi environment failed.");
			return false;
		}
		MYLOG(INFO, "Create THREAD_MUTEXED occi environment.");
		/*
		std::cout << " m_strUserName:" << m_strUserName \
			  << " m_strPassword:" << m_strPassword \
			  << " m_strDBName:"   << m_strDBName \
			  << " m_iMaxConn:"    << m_iMaxConn \
			  << " m_iMinConn:"    << m_iMinConn \
			  << " m_iIncrConn:"   << m_iIncrConn << std::endl;
		*/
		m_pConnPool = m_pEnv->createStatelessConnectionPool(m_strUserName,
				m_strPassword,
				m_strDBName,
				m_iMaxConn,
				m_iMinConn,
				m_iIncrConn,
				StatelessConnectionPool::HOMOGENEOUS);
		if (nullptr == m_pConnPool)
		{
			MYLOG(FATAL, "Create occi statelessConnectionPool failed.");
			return false;
		}
		m_pConnPool->setTimeOut(900);
		//StatelessConnectionPool::BusyOption BusyOption = StatelessConnectionPool::FORCEGET;
		StatelessConnectionPool::BusyOption BusyOption = StatelessConnectionPool::WAIT;
		m_pConnPool->setBusyOption(BusyOption);
		MYLOG(INFO, "set statelessConnectionPool busyOption: WAIT.");
	}
	catch (SQLException &sqlEx)
	{
		MYLOG(FATAL, "ConnectionPool Init Error:%s", sqlEx.getMessage().c_str());
		return false;
	}
	catch (exception &e)
	{
		MYLOG(FATAL, "ConnectionPool Init Error:%s", e.what());
		return false;
	}
	return true;
}

COcciConnectPool::~COcciConnectPool(void)
{
	try
	{
		if (m_pConnPool	!= NULL)
		{
			m_pEnv->terminateStatelessConnectionPool(m_pConnPool);
			m_pConnPool = NULL;
		}
		if (m_pEnv != NULL)
		{
			Environment::terminateEnvironment(m_pEnv);
			m_pEnv = NULL;
		}
	}
	catch (SQLException &sqlEx)
	{
		MYLOG(ERROR, "In ~COcciConnectPool() Error:%s", sqlEx.getMessage().c_str());
	}
	catch (exception &e)
	{
		MYLOG(ERROR, "In ~COcciConnectPool() Error:%s", e.what());
	}
}

Connection *COcciConnectPool::GetConnection()
{
	Connection *pCon = NULL;
	try
	{
		if (m_pConnPool != NULL)	
		{
			MYLOG(DEBUG, "start get connection");
			MYLOG(DEBUG, "The count of opened connections:%u", m_pConnPool->getOpenConnections());
			MYLOG(DEBUG, "The count of busy  connections:%u", m_pConnPool->getBusyConnections());
			pCon = m_pConnPool->getConnection();
			MYLOG(DEBUG, "after get connection. address:%p", pCon);
		}
	}
	catch (SQLException &sqlEx)
	{
		MYLOG(ERROR, "Get connection from pool Error:%s", sqlEx.getMessage().c_str());
	}
	catch (exception &e)
	{
		MYLOG(ERROR, "Get connection from pool Error:%s", e.what());
	}

	return pCon;
}

bool COcciConnectPool::ReleaseConnection(Connection *pConnect)
{
	bool ret = false;
	try
	{
		if (m_pConnPool != NULL && pConnect != NULL)	
		{
			m_pConnPool->releaseConnection(pConnect);
			MYLOG(DEBUG, "release conncetion. address:%p", pConnect);
			pConnect = NULL;
			ret = true;
		}
	}
	catch (SQLException &sqlEx)
	{
		MYLOG(ERROR, "Release connection Error:%s", sqlEx.getMessage().c_str());
	}
	catch (exception &e)
	{
		MYLOG(ERROR, "Release connection Error:%s", e.what());
	}
	return ret;
}
