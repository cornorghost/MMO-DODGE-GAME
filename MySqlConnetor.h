#ifndef  ____MYSQLCONNECTOR_H
#define  ____MYSQLCONNECTOR_H

#include <map> 
#include <vector> 
#include <string>
#include "IDefine.h"
#include "mysql\mysql.h"
#pragma comment(lib, "libmysql.lib")

namespace db
{
	const int	datalen = 32;
	//MySQL������
	class MySqlConnetor
	{
	public:
		MySqlConnetor();
		~MySqlConnetor();
	
		int		ExecQuery(const std::string& pData);
		void	GetQueryFields();
		int 	QueryNext();
		void	CloseQuery();
		void	r(const char *fieldname, uint8_t &value);
		void	r(const char *fieldname, int8_t &value);
		void	r(const char *fieldname, uint16_t &value);
		void	r(const char *fieldname, int16_t &value);
		void	r(const char *fieldname, uint32_t &value);
		void	r(const char *fieldname, int32_t &value);
		void	r(const char *fieldname, int64_t &value);
		void	r(const char *fieldname, uint64_t &value);
		void	r(const char *fieldname, char *  value);
		void	r(const char *fieldname, std::string&  value);
	public:
		bool	ConnectMySql(const char *ip, const char	*usename, const char *password, const char *dbname, int  port = 3306);
		void	Clear();
		bool	ReConnect();
		void	ClearRes();


		int				GetErrorCode() { if (m_IsGetErrorCode) return mysql_errno(mysql);  return -1000; }
		const char*		GetErrorStr() { return mysql_error(mysql); }

		int				GetQueryRowNum()   { return m_TotalRows; }
		int				GetQueryFieldNum() { return m_TotalFields; }

		MYSQL *			GetMySql() { return mysql; }
		MYSQL_RES *	    GetMyRes() { return myres; }
	public:
		MYSQL       *mysql;
		MYSQL_RES	*myres;
		int		    m_TotalRows;
		int         m_TotalFields;
		int		    m_curRowIdx;
		char m_IP[datalen];     //���ݿ�IP
		char m_UserName[datalen];//���ݿ��û���
		char m_UserPassword[datalen];//���ݿ�����
		char m_DBName[datalen];//���ݿ���
		uint16_t  m_Port;//���ݿ�˿ں�
		bool m_IsConnect;//����״̬
		bool m_IsGetErrorCode;
		std::vector<std::string> filedVec;//�ֶ��б�����
		std::map<std::string, std::string>  m_FieldsValue;//�ֶ�����
	};
}

#endif