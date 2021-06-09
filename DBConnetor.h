#ifndef  ____MYSQLMANAGER_H
#define  ____MYSQLMANAGER_H
#include <string>
#include <functional>
#include <atomic>
#include <queue>
#include <mutex> //������
#include <thread> 

#include "DBBuffer.h"
#include "ThreadSafeQueue.h"
#include "MySqlConnetor.h"



namespace db
{
	typedef std::function<void(DBBuffer*)> Callback;
	typedef std::function<void()> BeginCallback;

	struct S_DBXML
	{
		int	    port;//�˿ں�
		char	ip[20];//IP
		char	username[20];//���ݿ��˺�
		char	userpass[20];//���ݿ�����
		char	dbname[20];//���ݿ�����

		S_DBXML() { memset(this, 0, sizeof(*this)); }
	};
	//�����߳�����
	enum E_THREAD_TYPE
	{
		ETT_USERREAD  = 0x01,
		ETT_USERWRITE  = 0x02,
		ETT_ACCOUNT   = 0x03
	};

	class DBConnetor
	{
	public:
		DBConnetor(S_DBXML* cfg);
		~DBConnetor();
	private:
		MySqlConnetor *mysql;//mysql������
		S_DBXML*	   m_mysqlcfg;//���ݿ����ñ�

		//�߳��������
		int       m_ThreadID;//�߳�ID
		Callback  m_Callback;//�ص�����
		BeginCallback  m_BeginCallback;//�ص�����

		std::mutex	m_Mutex;
		std::queue<DBBuffer*> m_Queue;//�������ݶ���
		std::queue<DBBuffer*> m_SwapQueue;//�������ݶ���
		std::condition_variable m_ConditionVar;//��������
		ThreadSafePool  m_BuffsPool;//BUFF��
		atomic<int>  m_WorkCount;

	public:
		int					Stop();
		int					StartRun(int id,  Callback callback, BeginCallback bcallback = nullptr);
		DBBuffer*        PopBuffer();
		void PushToThread(DBBuffer* buffer);
		inline MySqlConnetor*	GetMysqlConnector() { return mysql; }
		inline int GetThreadID() { return m_ThreadID; }
		inline int	GetWorkCount()const { return m_WorkCount; }
		void Run();
	};

	
}


#endif