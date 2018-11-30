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

	auto mainSpore = Spore::newSpore("Main Space");

	auto entryPin = defaultSchema().addPin(mainSpore, "entry", Pin_Type::IN_PIN);


	auto L1 = mainSpore->addChild("sporeL1");
	auto fun_L1 = [&](Context& cc, DataPack& data) {
		//static std::mutex _lock;

		auto &nLevel = data.ref<int>("Level");
		auto &maxLevel = data.ref<int>("MaxLevel");
		auto &expCount = data.ref<int>("expCount");

		auto &currentLevel = cc.session.ref<int>("currentLevel");
		{
			cc.session.ref< std::atomic_ullong>("Count").fetch_add(1);
		}

		if (currentLevel < nLevel)
		{
			currentLevel = nLevel;
			//std::lock_guard<std::mutex> lock(_lock);
			//std::cout << "! new Level: " << nLevel;
			//auto _currentTime = std::chrono::high_resolution_clock::now();
			//auto elapsed_seconds = ((std::chrono::duration<double>)(_currentTime - _startTime)).count();
			//std::cout << "  elapsed time: " << elapsed_seconds << "s\n";
		}
		if (nLevel >= maxLevel)
			return;

		for (int n = 0; n < expCount; n++)
		{
			auto outData = std::make_shared<DataPack>();
			outData->ref<int>("Level") = nLevel + 1;
			outData->ref<int>("MaxLevel") = maxLevel;
			outData->ref<int>("expCount") = expCount;
			cc.push("out", outData);
		}
	};
	auto out_L1 = defaultSchema().addPin(L1, "out");
	auto in_L1 = defaultSchema().addPin(L1, "in", fun_L1);

	Path::connect(entryPin, in_L1);
	Path::connect(out_L1, in_L1);


	auto session = Session::newSession(entryPin);
	session->values().ref< std::atomic_ullong>("Count");
	AnyValues originData;
	originData.ref<int>("Level") = 0;
	originData.ref<int>("MaxLevel") = 5;
	originData.ref<int>("expCount") = 10;

	session->submit(originData);
	session->join();

	auto finishCount = session->values().ref<std::atomic_ullong>("Count").load();

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
};

class MySchema : public LightSchema<MyDataPack>
{
protected:
	virtual MyDataPack* toDataFunc(const AnyValues & values)
	{
		auto mypack = new MyDataPack();
		values.get<int>("Level", mypack->Level);
		values.get<int>("MaxLevel", mypack->MaxLevel);
		values.get<int>("expCount", mypack->expCount);
		return mypack;
	}

	virtual AnyValues toAnyValuesFunc(const MyDataPack & dataPack)
	{
		AnyValues anyValues;
		anyValues.ref<int>("Level") = dataPack.Level;
		anyValues.ref<int>("MaxLevel") = dataPack.MaxLevel;
		anyValues.ref<int>("expCount") = dataPack.expCount;
		return std::move(anyValues);
	}
};

MySchema& mySchema()
{
	static std::shared_ptr<MySchema> __this;
	if (!__this)
	{
		__this = std::make_shared<MySchema>();
	}
	return *__this.get();
}

void testMySchema()
{
	auto _startTime = std::chrono::high_resolution_clock::now();
	auto _currentTime = std::chrono::high_resolution_clock::now();
	double elapsed_seconds = 0.0f;


	auto mainSpore = Spore::newSpore("Main Space");
	auto entryPin = mySchema().addPin(mainSpore, "entry", Pin_Type::IN_PIN);

	auto L1 = mainSpore->addChild("sporeL1");
	auto fun_L1 = [&](Context& cc, MyDataPack& data) {
		static std::mutex _lock;

		auto &nLevel = data.Level;
		auto &maxLevel = data.MaxLevel;
		auto &expCount = data.expCount;

		auto &currentLevel = cc.session.ref<int>("currentLevel");
		cc.session.ref< std::atomic_ullong>("Count").fetch_add(1);
		if (currentLevel < nLevel)
		{
			currentLevel = nLevel;
			std::lock_guard<std::mutex> lock(_lock);
			std::cout << "! new Level: " << nLevel;
			auto _currentTime = std::chrono::high_resolution_clock::now();
			auto elapsed_seconds = ((std::chrono::duration<double>)(_currentTime - _startTime)).count();
			std::cout << "  elapsed time: " << elapsed_seconds << "s\n";
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
	};
	auto out_L1 = mySchema().addPin(L1, "out");
	auto in_L1 = mySchema().addPin(L1, "in", fun_L1);

	Path::connect(entryPin, in_L1);
	Path::connect(out_L1, in_L1);


	auto session = Session::newSession(entryPin);
	session->values().ref< std::atomic_ullong>("Count");
	AnyValues originData;
	originData.ref<int>("Level") = 0;
	originData.ref<int>("MaxLevel") = 5;
	originData.ref<int>("expCount") = 10;

	session->submit(originData);
	session->join();

	auto finishCount = session->values().ref<std::atomic_ullong>("Count").load();

	std::cout << "Session finish, the result:" << finishCount << "\n";

	_currentTime = std::chrono::high_resolution_clock::now();
	elapsed_seconds = ((std::chrono::duration<double>)(_currentTime - _startTime)).count();
	std::cout << "  elapsed time: " << elapsed_seconds << "s | avg: " << finishCount / elapsed_seconds << "\n";
	task_pool &_tp = task_pool::get_instance();
	std::cout << "All task finish,ths  count is :" << _tp.get_finish_count() << "\n";
}

int main()
{
	Maze maze;
	maze.run();
	testDefaultSchema();
	testMySchema();
	char _c;
	std::cin >> _c;
	return 0;

}

