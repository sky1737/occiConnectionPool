/*************************************************************************
	> File Name: OcciOperater.cpp
	> Author: jiachuang
	> Mail: jiachuang0210@qq.com 
	> Created Time: Wed 18 May 2016 06:58:12 PM PDT
 ************************************************************************/

#include <iostream>
#include <sstream>
#include <cassert>
#include "OcciOperator.h"
#include "OcciClient.h"
#include "log/Log.h"

//#define MYLOG(str) std::cout << "[" << __FILE__ ":" << __FUNCTION__ << " <" << __LINE__ << ">]" << str << std::endl

using namespace oracle::occi;

bool COcciOperator::Initialize()
{
	m_pOperator = new COcciOperator();
	assert(NULL != m_pOperator);
	if (NULL == m_pOperator)
	{
		MYLOG(ERROR, "new occi operator failed.");
		return false;
	}
	return true;
}

bool COcciOperator::Finalize()
{
	assert(NULL != m_pOperator);
	if (NULL != m_pOperator)
	{
		delete m_pOperator;	
		m_pOperator = NULL;
		return true;
	}
	return false;
}
COcciOperator::~COcciOperator()
{
}

static void HandleSelectFromModel(ResultSet *pRset)
{
	assert(NULL != pRset);
	while(pRset->next())
	{
		//MYLOG(DEBUG, "id:%s", pRset->getString(1).c_str());
		//std::cout << "id" << pRset->getString(1) << std::endl;
		//std::cout << "path" << pRset->getString(2) << std::endl;
	}
}

bool COcciOperator::SelectFromModel()
{
	COcciClient *client = COcciClient::GetClient();
	std::string strSql = "SELECT * FROM MODEL WHERE ROWNUM < 2";
	MYLOG(DEBUG, strSql.c_str());
	// TRY TODO WITH lamba
	/*
	if (!client->ExecuteQuery(strSql, [](ResultSet *pRset){
				assert(NULL != pRset);
				while(pRset->next())
				{
					MYLOG(DEBUG, "id:%s", pRset->getString(1).c_str());
				//std::cout << "path" << pRset->getString(2) << std::endl;
				}
				}))
	*/
	if (!client->ExecuteQuery(strSql, HandleSelectFromModel))
	{
		MYLOG(ERROR, "Select failed.");
		return false;
	}
	return true;
}

bool COcciOperator::InsertIntoModel(std::string strModelId, std::string strPath)
{
	COcciClient *client = COcciClient::GetClient();
	std::stringstream query;

	query << "INSERT INTO MODEL SELECT "
		<< "\'" << strModelId << "\', "
		<< "\'" << strPath << "\', "
		<< "1, SYSTIMESTAMP FROM DUAL WHERE NOT EXISTS(SELECT id FROM MODEL WHERE id = "
		<< "\'" << strModelId << "\')";
	//MYLOG(DEBUG, query.str().c_str());
	if (!client->Execute(query.str()))
	{
		MYLOG(ERROR, "Insert failed.");
		return false;
	}

	return true;
}

bool COcciOperator::DeleteFromModel(std::string strModelId)
{
	COcciClient *client = COcciClient::GetClient();
	std::stringstream query;

	query << "DELETE FROM MODEL WHERE id = "
		<< "\'" << strModelId << "\'";
	//MYLOG(DEBUG, query.str().c_str());
	if(!client->Execute(query.str()))
	{
		MYLOG(ERROR, "Delete failed.");
		return false;
	}

	return true;
}

bool COcciOperator::DeleteInsertSelect(std::string strModelId, std::string strPath)
{
	unsigned executeTimes = 0;
	do
	{
		executeTimes++;
		MYLOG(INFO, "The %d times to execute.", executeTimes);
		try		
		{
			DeleteFromModel(strModelId);
		}
		catch(...)
		{
			MYLOG(ERROR, "Delete from model error");
			return false;
		}

		try		
		{
			InsertIntoModel(strModelId, strPath);
		}
		catch(...)
		{
			MYLOG(ERROR, "Insert from model error");
			return false;
		}

		try		
		{
			SelectFromModel();
		}
		catch(...)
		{
			MYLOG(ERROR, "Select from model error");
			return false;
		}
	}while(1);
	return true;
}
