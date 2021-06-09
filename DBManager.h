#ifndef  ____DBMANAGER_H
#define  ____DBMANAGER_H

#include <concurrent_queue.h>

#include "DBConnetor.h"
#include "DBBuffer.h"
namespace db
{
	//���ݿ������
	class DBManager
	{
	public:
		DBManager();
		~DBManager();
	public:

		S_DBXML*  __GameDBXML;
		S_DBXML*  __AccountXML;

		 
		Concurrency::concurrent_queue<DBBuffer*>  __logicBuffs;//��ȫ�Ĳ�������
		ThreadSafePool  __poolBuffs; //buff���ջ����


		//mysql����
		std::vector<DBConnetor* > DBRead;//��Ҷ�ȡ���ݿ����
		std::vector<DBConnetor* > DBWrite;//���д���ݿ����
		DBConnetor *DBAccount;//����˺�ȫ�����ݿ����


		DBConnetor* GetDBSource(int type);
		DBBuffer* PopPool();
		int     LoadDBXML(const char* filename);
		void    InitDB();
		void    update();
		void    PushToMainThread(DBBuffer * buffer);
		
	
		void	Thread_BeginAccount();
		void	Thread_UserRead(DBBuffer* buff);
		void	Thread_UserWrite(DBBuffer* buff);
		void	Thread_UserAccount(DBBuffer* buff);
		

		void    Read_UserLogin(DBBuffer* buff,DBConnetor* db);

		void    Write_UserLoginTime(DBBuffer* buff, DBConnetor* db);
		void    Write_UserRegister(DBBuffer* _buff, DBConnetor* db);
		void    Write_UserSave(DBBuffer* _buff, DBConnetor* db);

	};
	extern DBManager* __DBManager;
	
}
#endif