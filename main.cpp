/*************************************************************************
	> File Name: main.cpp
	> Author: jiachuang
	> Mail: jiachuang0210@qq.com 
	> Created Time: Wed 18 May 2016 02:55:14 AM PDT
 ************************************************************************/

#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <queue>
#include <thread>
#include <unistd.h>
#include "occi/OcciConnectPool.h"
#include "occi/OcciClient.h"
#include "log/Log.h"

static int const INIT_CLIENT_FAILED = 1;
static int const INIT_OPERATOR_FAILED = 2;
static int const OPERATOR_ERROR = 3;

//#define MYLOG(str) std::cout << "[" << __FILE__ ":" << __FUNCTION__ << " <" << __LINE__ << ">]" << str << std::endl
static unsigned int const THREAD_COUNT = 2000;
static unsigned int const EXECUTE_TIMES = 1;
using namespace std;

static void Initialize(void)
{
	CLog::Initialize();
	MYLOG(INFO, "success Initialize log");
	MYLOG(WARNING, "This is a normal log to create the logerr file.");
	std::string const strUserName = "spladmin";
	std::string const strPassWord = "spladmin";
	std::string const strDbName = "10.0.2.162:1521/orcl.localdomain";
	unsigned int iMaxConn = 10;
	unsigned int iMinConn = 5;
	unsigned int iIncrConn = 2;
	if (!COcciClient::Initialize(strUserName, strPassWord, strDbName, iMaxConn, iMinConn, iIncrConn))
	{
		exit(INIT_CLIENT_FAILED);
	}
	MYLOG(INFO, "success Initialize client");
	if (!COcciOperator::Initialize())
	{
		exit(INIT_OPERATOR_FAILED);
	}
	MYLOG(INFO, "success Initialize operator");
}

static void Finalize(void)
{
	COcciClient::Finalize();
	COcciOperator::Finalize();
	CLog::Finalize();
}

CLog *CLog::m_log = new CLog();
boost::log::sources::severity_logger<LogLevel> CLog::m_boostLog;
CLog::LogConfig CLog::m_logConfig;
CLog::GC CLog::m_gc;

COcciOperator *COcciOperator::m_pOperator = NULL;
COcciClient *COcciClient::m_pClient = NULL;
int main(int argc, char *argv[])
{
	Initialize();
	std::string strModelId = "1";
	std::string strTemp = "/root";
	std::string strPath = "/root";

	COcciOperator *pOcciOperator = COcciOperator::GetOcciOperator();
	stringstream ss;
	//unsigned int executeCount = 0;
	//std::thread vecDeleteThreads[THREAD_COUNT];
	//std::thread vecInsertThreads[THREAD_COUNT*2];
	//std::vector<std::thread> vecInsertThreads;
	//std::vector<std::thread> vecDeleteThreads;
	//std::queue<std::thread> vecInsertThreads;
	//std::queue<std::thread> vecDeleteThreads;
	std::queue<std::thread> vecThreads;
	do
	{
#if 0
		MYLOG(INFO, "The %d times to execute. ThreadCount:%d", executeCount, THREAD_COUNT);
		try
		{
			MYLOG(INFO, "start execute insert and select from model");
			for (unsigned int threadCount = 0; threadCount < THREAD_COUNT; threadCount++)
			{
				ss<<threadCount;
				strModelId = ss.str();
				ss.str(std::string());
				ss.clear();
				strPath = strTemp + strModelId;
				//vecInsertThreads.push_back(std::thread(&COcciOperator::InsertIntoModel, pOcciOperator, strModelId, strPath));
				//vecInsertThreads[threadCount] = std::thread(&COcciOperator::InsertIntoModel, pOcciOperator, strModelId, strPath);
				vecInsertThreads.push(std::thread(&COcciOperator::InsertIntoModel, pOcciOperator, strModelId, strPath));
				//vecInsertThreads.push_back(std::thread(&COcciOperator::SelectFromModel, pOcciOperator));
				//vecInsertThreads[THREAD_COUNT + threadCount] = std::thread(&COcciOperator::SelectFromModel, pOcciOperator);
				vecInsertThreads.push(std::thread(&COcciOperator::SelectFromModel, pOcciOperator));
			}
			//	for (auto &th:vecInsertThreads) th.join();
			while(!vecInsertThreads.empty())
			{
				vecInsertThreads.front().join();
				vecInsertThreads.pop();
			}
		}
		catch(...)
		{
			MYLOG(ERROR, "InsertInto or SelectFrom throw error");
		}

		try
		{
			MYLOG(INFO, "start execute delete from model");
			for (unsigned int threadCount = 0; threadCount < THREAD_COUNT; threadCount++)
			{
				ss<<threadCount;
				strModelId = ss.str();
				ss.str(std::string());
				ss.clear();
				//vecDeleteThreads.push_back(std::thread(&COcciOperator::DeleteFromModel, pOcciOperator, strModelId));
				//vecDeleteThreads[threadCount] = std::thread(&COcciOperator::DeleteFromModel, pOcciOperator, strModelId);
				vecDeleteThreads.push(std::thread(&COcciOperator::DeleteFromModel, pOcciOperator, strModelId));
			}
			/*
			for (auto &th:vecDeleteThreads) 
			{
				MYLOG(DEBUG, "Thread:%lx", th.get_id());
				th.join();
			}
			*/
			while(!vecDeleteThreads.empty())
			{
				vecDeleteThreads.front().join();
				vecDeleteThreads.pop();
			}
		}
		catch(...)
		{
			MYLOG(ERROR, "InsertInto or SelectFrom throw error");
		}
		executeCount++;
#endif
		/*
		std::vector<std::thread>::iterator iterDelete = vecDeleteThreads.begin();
		//for (; iterDelete != vecDeleteThreads.end();)
		while (iterDelete != vecDeleteThreads.end())
		{
			//delete *iterDelete;		
			iterDelete = vecDeleteThreads.erase(iterDelete);
		}
		//	std::vector<std::thread>(vecDeleteThreads).swap(vecDeleteThreads);
		MYLOG(INFO, "vecDeleteThreads:%d", vecDeleteThreads.capacity());
		MYLOG(INFO, "vecDeleteThreads:%d", vecDeleteThreads.size());


		std::vector<std::thread>::iterator iterInsert = vecInsertThreads.begin();
		while (iterInsert != vecInsertThreads.end())
		{
			//delete *iterInsert;		
			iterInsert = vecInsertThreads.erase(iterInsert);
		}
	//	std::vector<std::thread>(vecInsertThreads).swap(vecInsertThreads);
		MYLOG(INFO, "vecInsertThreads:%d", vecInsertThreads.capacity());
		MYLOG(INFO, "vecInsertThreads:%d", vecInsertThreads.size());
		*/
		try
		{
			for (unsigned int threadCount = 0; threadCount < THREAD_COUNT; threadCount++)
			{
				ss<<threadCount;
				strModelId = ss.str();
				ss.str(std::string());
				ss.clear();
				strPath = strTemp + strModelId;
				vecThreads.push(std::thread(&COcciOperator::DeleteInsertSelect, pOcciOperator, strModelId, strPath));
			}

			while(!vecThreads.empty())
			{
				vecThreads.front().join();
				vecThreads.pop();
			}
		}
		catch(std::exception &e)
		{
			MYLOG(ERROR, "throw an exception:%s", e.what());
		}
		catch(...)
		{
			MYLOG(ERROR, "unknow error.");
		}

	}while(0);
//	}while(1);

	Finalize();
	return 0;
}
