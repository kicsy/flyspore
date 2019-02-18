// flyspore.cpp: 定义控制台应用程序的入口点。
//

#include "core/Level0/task_pool.h"
#include "core/Level1/Level1.h"
#include "core/Level2/Maze.h"
#include "Threads.h"

using namespace fs::L0;
using namespace fs::L1;
using namespace fs::L2;

void testDefaultSchema()
{
	auto _startTime = std::chrono::high_resolution_clock::now();
	auto _currentTime = std::chrono::high_resolution_clock::now();
	double elapsed_seconds = 0.0f;
	
	auto nest = make_shared<DefaultNest>();
	
	auto mainSpore = nest->createSpore("Main Space");

	auto entryPin =mainSpore->addPin(nest->createPin("entry", Pin_Type::IN_PIN));


	auto L1 =  mainSpore->addSpore(nest->createSpore("sporeL1"));
	auto in_L1 = L1->addPin(nest->createPin("in", 
		[&](Context& cc, DataPack& data) {
		//static std::mutex _lock;
		auto nLevel = data.get<int>("Level");
		auto maxLevel = data.get<int>("MaxLevel");
		auto expCount = data.get<int>("expCount");
		{
			AnyValuesLock lcc(cc.session);
			lcc.ref< std::atomic_ullong>("Count").fetch_add(1);
			if (lcc.ref<int>("currentLevel") < nLevel)
			{
				lcc.ref<int>("currentLevel") = nLevel;
			}
		}
		if (nLevel >= maxLevel)
			return;

		for (int n = 0; n < expCount; n++)
		{
			auto outData = std::make_shared<DataPack>();
			outData->set<int>("Level", nLevel + 1);
			outData->set<int>("MaxLevel", maxLevel);
			outData->set<int>("expCount", expCount);
			cc.push("out", outData);
		}
	}));
	auto out_L1 = L1->addPin(nest->createPin("out"));

	entryPin->connect(in_L1);
	out_L1->connect(in_L1);

	auto session = Session::newSession(entryPin);

	{
		AnyValuesLock lcc(session->values());
		lcc.ref<std::atomic_ullong>("Count");
		lcc.ref<int>("currentLevel") = 0;
	}

	AnyValues originData;
	originData.set<int>("Level", 0);
	originData.set<int>("MaxLevel", 6);
	originData.set<int>("expCount", 10);

	session->submit(originData);
	session->join();

	unsigned long long finishCount = 0;
	{
		AnyValuesLock lcc(session->values());
		finishCount = lcc.ref<std::atomic_ullong>("Count").load();
	}
	std::cout << "Session finish, the result:" << finishCount << "\n";

	_currentTime = std::chrono::high_resolution_clock::now();
	elapsed_seconds = ((std::chrono::duration<double>)(_currentTime - _startTime)).count();
	std::cout << "  elapsed time: " << elapsed_seconds << "s | avg: " << finishCount / elapsed_seconds << "\n";
	task_pool &_tp = task_pool::get_instance();
	std::cout << "All task finish,ths  count is :" << _tp.get_finish_count() << "\n";
}

class MyDataPack : public Data
{
public:
	MyDataPack() :Data() {}
	int Level{ 0 };
	int MaxLevel{ 0 };
	int expCount{ 0 };

	virtual unsigned int hashCode() override
	{
		return *(const unsigned int*)("MySchema");
	}
	static MyDataPack* toData(const AnyValues & values)
	{
		auto mypack = new MyDataPack();
		mypack->Level = values.get<int>("Level");
		mypack->MaxLevel = values.get<int>("MaxLevel");
		mypack->expCount = values.get<int>("expCount");
		return mypack;
	}
	static AnyValues toAnyValues(const MyDataPack & dataPack)
	{
		AnyValues anyValues;
		anyValues.set<int>("Level", dataPack.Level);
		anyValues.set<int>("MaxLevel", dataPack.MaxLevel);
		anyValues.set<int>("expCount", dataPack.expCount);
		return std::move(anyValues);
	}
};

class MySchema : public LightSchema<MyDataPack>
{
public:
	virtual unsigned int hashCode() override
	{
		return *(const unsigned int*)("MySchema");
	}
	virtual MyDataPack* toDataFunc(const AnyValues & values) override
	{
		auto mypack = new MyDataPack();
		mypack->Level = values.get<int>("Level");
		mypack->MaxLevel = values.get<int>("MaxLevel");
		mypack->expCount = values.get<int>("expCount");
		return mypack;
	}
	virtual AnyValues toAnyValuesFunc(const MyDataPack & dataPack) override
	{
		AnyValues anyValues;
		anyValues.set<int>("Level", dataPack.Level);
		anyValues.set<int>("MaxLevel", dataPack.MaxLevel);
		anyValues.set<int>("expCount", dataPack.expCount);
		return std::move(anyValues);
	}
};

void testMySchema()
{
	auto _startTime = std::chrono::high_resolution_clock::now();
	auto _currentTime = std::chrono::high_resolution_clock::now();
	double elapsed_seconds = 0.0f;


	auto nest = make_shared<DefaultNest>();

	auto mainSpore = nest->createSpore("Main Space");

	auto entryPin = mainSpore->addPin(nest->createPin("entry", Pin_Type::IN_PIN));

	auto L1 = mainSpore->addSpore(nest->createSpore("sporeL1"));
	auto in_L1 = L1->addPin(nest->createPin<MySchema>("in",
		[&](Context& cc, MyDataPack& data) {
		static std::mutex _lock;

		auto &nLevel = data.Level;
		auto &maxLevel = data.MaxLevel;
		auto &expCount = data.expCount;

		auto currentLevel = cc.session.get<int>("currentLevel");
		{
			AnyValuesLock lcc(cc.session);
			lcc.ref< std::atomic_ullong>("Count").fetch_add(1);
			if (lcc.ref<int>("currentLevel") < nLevel)
			{
				lcc.ref<int>("currentLevel") = nLevel;
				std::lock_guard<std::mutex> lock(_lock);
				std::cout << "! new Level: " << nLevel;
				auto _currentTime = std::chrono::high_resolution_clock::now();
				auto elapsed_seconds = ((std::chrono::duration<double>)(_currentTime - _startTime)).count();
				std::cout << "  elapsed time: " << elapsed_seconds << "s\n";
			}
		}
		if (nLevel >= maxLevel)
			return;

		for (int n = 0; n < expCount; n++)
		{
			auto outData = std::make_shared<MyDataPack>();
			outData->Level = nLevel + 1;
			outData->MaxLevel = maxLevel;
			outData->expCount = expCount;
			cc.push("out", outData);
		}
	}));
	auto out_L1 = L1->addPin(nest->createPin("out"));

	entryPin->connect(in_L1);
	out_L1->connect(in_L1);

	auto session = Session::newSession(entryPin);
	{
		AnyValuesLock lcc(session->values());
		lcc.ref<std::atomic_ullong>("Count");
		lcc.ref<int>("currentLevel") = 0;
	}
	AnyValues originData;
	originData.set<int>("Level", 0);
	originData.set<int>("MaxLevel", 7);
	originData.set<int>("expCount", 10);

	session->submit(originData);
	session->join();

	unsigned long long finishCount = 0;
	{
		AnyValuesLock lcc(session->values());
		finishCount = lcc.ref<std::atomic_ullong>("Count").load();
	}

	std::cout << "Session finish, the result:" << finishCount << "\n";

	_currentTime = std::chrono::high_resolution_clock::now();
	elapsed_seconds = ((std::chrono::duration<double>)(_currentTime - _startTime)).count();
	std::cout << "  elapsed time: " << elapsed_seconds << "s | avg: " << finishCount / elapsed_seconds << "\n";
	task_pool &_tp = task_pool::get_instance();
	std::cout << "All task finish,ths  count is :" << _tp.get_finish_count() << "\n";
}

int main()
{
	//Maze maze;
	//maze.run();
	//testDefaultSchema();
	testMySchema();
	char _c;
	std::cin >> _c;
	return 0;

}

