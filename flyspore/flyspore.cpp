// flyspore.cpp: 定义控制台应用程序的入口点。
//

#include "core/Level1/Level1.h"
#include "core/Level2/Maze.h"
#include "Threads.h"

using namespace fs::L0;
using namespace fs::L1;
using namespace fs::L2;

int calc_deep = 5;
int calc_range = 10;

void testCalc()
{
	auto _startTime = std::chrono::high_resolution_clock::now();
	auto _currentTime = std::chrono::high_resolution_clock::now();
	double elapsed_seconds = 0.0f;

	int finishCount = 0;
	int nr = 0;
	for (int n = 0; n < calc_deep; n ++)
	{
		nr += 1 << n;
	}
	nr = 1111111;
	for (int k = 0; k < nr; k++)
	{
		finishCount++;
	}
	std::cout << "Session finish, the result:" << finishCount << "\n";

	_currentTime = std::chrono::high_resolution_clock::now();
	elapsed_seconds = ((std::chrono::duration<double>)(_currentTime - _startTime)).count();
	std::cout << "  elapsed time: " << elapsed_seconds << "s | avg: " << finishCount / elapsed_seconds << "\n";
	//task_pool &_tp = task_pool::get_instance();
	//std::cout << "All task finish,ths  count is :" << _tp.get_finish_count() << "\n";

}

auto testDefaultSchema()
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
	originData.set<int>("MaxLevel", calc_deep);
	originData.set<int>("expCount", calc_range);

	session->submit(originData);
	session->join();

	unsigned long long finishCount = 0;
	{
		AnyValuesLock lcc(session->values());
		finishCount = lcc.ref<std::atomic_ullong>("Count").load();
	}
	_currentTime = std::chrono::high_resolution_clock::now();
	elapsed_seconds = ((std::chrono::duration<double>)(_currentTime - _startTime)).count();
	std::cout << "Session finish, the result:" << finishCount << "\n";
	std::cout << "  elapsed time: " << elapsed_seconds << "s | avg: " << finishCount / elapsed_seconds << "\n";
	return std::make_tuple(elapsed_seconds, finishCount);
	//task_pool &_tp = task_pool::get_instance();
	//std::cout << "All task finish,ths  count is :" << _tp.get_finish_count() << "\n";
}


class MyDataAdpter;
class MyDataPack : public BasicData<MyDataAdpter>
{
public:
	MyDataPack() :BasicData<MyDataAdpter>() {}
	int Level{ 0 };
	int MaxLevel{ 0 };
	int expCount{ 0 };
};

class MyDataAdpter : public DataAdapter
{
public:
	virtual unsigned int hashCode() const override
	{
		return *(const unsigned int*)("MySchema");
	}

	virtual std::shared_ptr<Data> toData(const AnyValues &values) const override
	{
		auto mypack = new MyDataPack();
		mypack->Level = values.get<int>("Level");
		mypack->MaxLevel = values.get<int>("MaxLevel");
		mypack->expCount = values.get<int>("expCount");
		return std::shared_ptr<Data>(mypack);

	}
	virtual AnyValues toAnyValues(const std::shared_ptr<Data>& pdata) const override
	{
		auto& dataPack = *(const MyDataPack*)pdata.get();
		AnyValues anyValues;
		anyValues.set<int>("Level", dataPack.Level);
		anyValues.set<int>("MaxLevel", dataPack.MaxLevel);
		anyValues.set<int>("expCount", dataPack.expCount);
		return anyValues;
	}
};


auto testMySchema()
{
	auto _startTime = std::chrono::high_resolution_clock::now();
	auto _currentTime = std::chrono::high_resolution_clock::now();
	double elapsed_seconds = 0.0f;


	auto nest = make_shared<DefaultNest>();

	auto mainSpore = nest->createSpore("Main Space");

	auto entryPin = mainSpore->addPin(nest->createPin("entry", Pin_Type::IN_PIN));

	auto L1 = mainSpore->addSpore(nest->createSpore("sporeL1"));
	auto in_L1 = L1->addPin(nest->createPin<MyDataPack>("in",
		[&](Context& cc, MyDataPack& data) {
		static std::mutex _lock;

		auto &nLevel = data.Level;
		auto &maxLevel = data.MaxLevel;
		auto &expCount = data.expCount;
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
	originData.set<int>("MaxLevel", calc_deep);
	originData.set<int>("expCount", calc_range);

	session->submit(originData);
	session->join();

	unsigned long long finishCount = 0;
	{
		AnyValuesLock lcc(session->values());
		finishCount = lcc.ref<std::atomic_ullong>("Count").load();
	}
	_currentTime = std::chrono::high_resolution_clock::now();
	elapsed_seconds = ((std::chrono::duration<double>)(_currentTime - _startTime)).count();
	std::cout << "Session finish, the result:" << finishCount << "\n";

	std::cout << "  elapsed time: " << elapsed_seconds << "s | avg: " << finishCount / elapsed_seconds << "\n";
	return std::make_tuple(elapsed_seconds, finishCount);

	//task_pool &_tp = task_pool::get_instance();
	//std::cout << "All task finish,ths  count is :" << _tp.get_finish_count() << "\n";
}

void testDataStructBuilder();

int main()
{
	//lockfree_array<std::shared_ptr<Path>> la;
	//la.push(std::shared_ptr<Path>(nullptr));
	//Maze maze;
	//maze.run();
	//testCalc();
	testDataStructBuilder();
	int ntemp = sizeof(std::any);
	ntemp = sizeof(std::string);
	ntemp = sizeof(std::unordered_map<std::string, std::any>);
	ntemp = sizeof(std::shared_mutex);
	ntemp = sizeof(std::shared_ptr<std::unordered_map<std::string, std::any>>);
	
	int size_AnyValues = sizeof(AnyValues);
	int size_Spore = sizeof(Spore);

	char _c;
	

	calc_deep = 5;
	calc_range = 10;

	auto fg = Actuator::get_instance().submit([](int i, int n)->int {return i + n; }, 5, 6);

	std::cout << "----------"<< fg.get() << "-------\n\n";

//	testDefaultSchema();
	//testMySchema();

	std::cout << "-----------------\n\n";
	Actuator::get_instance().setType(Actuator::ThreadPoolType::WorkStealingThreadpool);

	double e_d = 0.0f;
	unsigned long long t_d = 0;
	double e = 0.0f;
	unsigned long long t = 0;
	for (int n = 0; n < 10; n ++)
	{
		auto [elapsed_d, count_d] = testDefaultSchema();
		e_d += elapsed_d;
		t_d += count_d;
		auto [elapsed, count] = testMySchema();
		e += elapsed;
		t += count;
	}
	std::cout << "-----------------\n\n";
	std::cout << "default avg: " << t_d / e_d << "\n";
	std::cout << "my avg: " << t / e << "\n";

	std::cin >> _c;
	return 0;

}

void testDataStructBuilder()
{
		struct miniscp
		{

		};

		struct Int32Value
		{

		};

		struct StringValue
		{

		};

		struct mylink : public miniscp
		{
			Int32Value value1;
			StringValue string1;
			struct _segment001 : public miniscp
			{
				
			} segment001;
		};
}
