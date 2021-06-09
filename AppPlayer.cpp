#include "AppPlayer.h"
#include <list>
#include <time.h>
#include "../../share/ShareFunction.h"

namespace app
{
	IContainer* __AppPlayer = NULL;
	std::map<int, S_PLAYER_BASE*>  __Onlines;//玩家登陆在线数据
	std::list<S_PLAYER_BASE*>  __PlayersPool;//对象回收池
	u64  temptime = 0;
	u32  __GlobalMemid = 10000000;
	
	AppPlayer::AppPlayer()
	{
	}

    AppPlayer::~AppPlayer()
	{
	}

	void AppPlayer::onInit()
	{
	}

	void playerLeave(u32 memid)
	{
		auto it = __Onlines.begin();
		while (it != __Onlines.end())
		{
			auto player = it->second;
#ifdef  ____WIN32_
			auto c = __TcpServer->client((SOCKET)player->socketfd, true);
#else
			auto c = __TcpServer->client(player->socketfd, true);
#endif
			if (c == nullptr || player->memid == memid)
			{
				++it;
				continue;
			}
			__TcpServer->begin(c->ID, CMD_LEAVE);
			__TcpServer->sss(c->ID, memid);
			__TcpServer->end(c->ID);
			++it;
		}
	}

	void AppPlayer::onUpdate()
	{
		int value = clock() - temptime;
		if (value < 100) return;
		temptime = clock();

		auto it = __Onlines.begin();
		while (it != __Onlines.end())
		{
			auto player = it->second;
#ifdef  ____WIN32_
			auto c = __TcpServer->client((SOCKET)player->socketfd, true);
#else
			auto c = __TcpServer->client(player->socketfd, true);
#endif

			if (c == nullptr)
			{
				++it;
				continue;
			}
			if (c->state == func::S_NeedSave)
			{
				playerLeave(player->memid);
				it = __Onlines.erase(it);
				player->init();
				__PlayersPool.push_back(player);

				c->Reset();
				continue;
			}

			++it;
		}
	}

	bool app::AppPlayer::onServerCommand(net::ITcpServer* ts, net::S_CLIENT_BASE* c, const u16 cmd)
	{
		if (ts->isSecure_F_Close(c->ID, func::S_ConnectSecure))
		{
			LOG_MSG("AppPlayer err....line:%d \n", __LINE__);
			return false;
		}

		switch (cmd)
		{
		case CMD_LOGIN:onLogin(ts, c); break;
		case CMD_MOVE:onMove(ts, c); break;
		case CMD_PLAYERDATA:onGetPlayerData(ts, c); break;
		case 9999:
			{
				int len = 0;
				char* str1 = NULL;
				char str2[20];
				memset(str2, 0, 20);
			
				ts->read(c->ID, len);
				str1 = new char[len];
				ts->read(c->ID, str1, len);

				ts->read(c->ID, str2, 20);
				//返回
				ts->begin(c->ID, 9999);
				ts->sss(c->ID, len);
				ts->sss(c->ID, str1, len);
				ts->sss(c->ID, str2, 20);
				ts->end(c->ID);

				delete[] str1;

			}
			break;
		}

		return false;
	}

	S_PLAYER_BASE* findPlayer(u32 memid, net::S_CLIENT_BASE* c)
	{
		auto it = __Onlines.find(memid);
		if (it == __Onlines.end()) return NULL;
		auto player = it->second;
		if (player->socketfd != c->socketfd)
		{
			LOG_MSG("findplayer err...%d-%d-%d-%d \n", memid, c->ID, player->socketfd, c->socketfd);
			return NULL;
		}
		return player;
	}
	//1000 登陆
	void app::AppPlayer::onLogin(net::ITcpServer* ts, net::S_CLIENT_BASE* c)
	{
		if (c->state == func::S_Login)
		{
			LOG_MSG("onlogin is login....\n");
			return;
		}

		S_PLAYER_BASE* player = NULL;
		if (__PlayersPool.empty())
		{
			player = new S_PLAYER_BASE();
			player->init();
		}
		else
		{
			player = __PlayersPool.front();
			__PlayersPool.pop_front();
			player->init();
		}
		srand(time(NULL));

		c->state = func::S_Login;
		player->memid = __GlobalMemid;
		player->socketfd = c->socketfd;
		player->curhp = 3000;
		player->maxhp = 6000;
		sprintf(player->nick, "nick_%d", player->socketfd);
		__GlobalMemid++;
		__Onlines.insert(std::make_pair(player->memid, player));


		ts->begin(c->ID, CMD_LOGIN);
		ts->sss(c->ID, player, sizeof(S_PLAYER_BASE));
		ts->end(c->ID);

		LOG_MSG("player login successfully...%d-%d\n", player->memid, (int)c->socketfd);
	}
	//2000 玩家移动
	void app::AppPlayer::onMove(net::ITcpServer* ts, net::S_CLIENT_BASE* c)
	{
		s32 memid;
		f32 speed;
		S_VECTOR pos;
		S_VECTOR rot;
		ts->read(c->ID, memid);
		ts->read(c->ID, speed);
		ts->read(c->ID, &pos,sizeof(S_VECTOR));
		ts->read(c->ID, &rot, sizeof(S_VECTOR));

		auto player = findPlayer(memid,c);
		if (player == NULL) return;

		player->speed = speed;
		player->pos = pos;
		player->rot = rot;

		auto it = __Onlines.begin();
		while (it != __Onlines.end())
		{
			auto other = it->second;

#ifdef  ____WIN32_
			auto c2 = ts->client((SOCKET)other->socketfd, true);
#else
			auto c2 = ts->client(other->socketfd, true);
#endif

			if (c2 == nullptr || other->memid == memid)
			{
				++it;
				continue;
			}

			ts->begin(c2->ID, CMD_MOVE);
			ts->sss(c2->ID, memid);
			ts->sss(c2->ID, speed);
			ts->sss(c2->ID, &pos, sizeof(S_VECTOR));
			ts->sss(c2->ID, &rot, sizeof(S_VECTOR));
			ts->end(c2->ID);
			++it;
		}
	}
	//3000 获取其他玩家数据
	void AppPlayer::onGetPlayerData(net::ITcpServer* ts, net::S_CLIENT_BASE* c)
	{
		s32 memid;
		ts->read(c->ID, memid);
		auto it = __Onlines.find(memid);
		if (it == __Onlines.end()) return;

		auto player = it->second;
#ifdef  ____WIN32_
		auto c2 = ts->client((SOCKET)player->socketfd, true);
#else
		auto c2 = ts->client(player->socketfd, true);
#endif
		if (c2 == NULL) return;


		ts->begin(c->ID, CMD_PLAYERDATA);
		ts->sss(c->ID, player, sizeof(S_PLAYER_BASE));
		ts->end(c->ID);

	}


}
