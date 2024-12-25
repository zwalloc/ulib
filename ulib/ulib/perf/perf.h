#pragma once

#include <string>
#include <map>
#include <chrono>

#include <ulib/types.h>
#include <ulib/utility/average.h>


namespace ulib
{
	namespace perf
	{
		struct StatisticsDescription
		{
			StatisticsDescription()
			{
				min = 0xFFFFFFFFFFFFFFFF;
				max = 0;
				sum = 0;
				minCallIndex = 0;
				maxCallIndex = 0;
			}

			void OnWorking(uint64 delta)
			{
				if (delta < min)
				{
					min = delta;
					minCallIndex = avg.Count();
				}

				if (delta > max)
				{
					max = delta;
					maxCallIndex = avg.Count();
				}				

				avg.Add(delta);		
				sum += delta;
			}

			uint64 min;
			uint64 max;
			Average<uint64> avg;
			uint64 sum;

			uint64 maxCallIndex;
			uint64 minCallIndex;
		};

		struct Statistics : public StatisticsDescription
		{
			Statistics()
			{
				stamp = 0;
				closed = true;
				ready = false;
			}

			std::map<std::string, StatisticsDescription> mDescs;

			uint64 stamp;
			std::string desc;

			bool closed;
			bool ready;
		};

		class StatisticsSystem
		{
		public:
			static StatisticsSystem& Instance()
			{
				static StatisticsSystem inst;
				return inst;
			}

			StatisticsSystem()
			{}

			~StatisticsSystem()
			{}

			Statistics* GetHandle(const std::string& name)
			{
				Statistics* pStatistics = nullptr;
				auto it = mStats.find(name);
				if (it == mStats.end())
					pStatistics = new Statistics;
				else
					pStatistics = it->second;

				mStats[name] = pStatistics;

				return pStatistics;
			}

			Statistics* Find(const std::string& name)
			{
				auto it = mStats.find(name);
				if (it != mStats.end())
					return it->second;

				return nullptr;
			}

		private:
			std::map<std::string, Statistics*> mStats;
		};

		inline Statistics* Begin(const std::string& key, const std::string& desc = "")
		{
			Statistics* pStatistics = StatisticsSystem::Instance().GetHandle(key);
			
			pStatistics->desc = desc;
			pStatistics->closed = false;
			pStatistics->stamp = std::chrono::steady_clock::now().time_since_epoch().count();		

			return pStatistics;
		}

		inline void End(Statistics* pStatistics)
		{
			uint64 stamp = std::chrono::steady_clock::now().time_since_epoch().count();
			if (!pStatistics->closed)
			{
				uint64 delta = stamp - pStatistics->stamp;

				auto& desc = pStatistics->mDescs[pStatistics->desc];
				auto& globalDesc = *pStatistics;

				desc.OnWorking(delta);
				globalDesc.OnWorking(delta);

				pStatistics->closed = true;
				pStatistics->ready = true;
			}		
		}

		Statistics* Status(const std::string& key)
		{
			return StatisticsSystem::Instance().Find(key);
		}

		class Test
		{
		public:
			inline Test(const std::string& key, const std::string& desc = "")
			{
				mStatistics = Begin(key, desc);
			}

			inline ~Test()
			{
				End(mStatistics);
			}

			void Finish()
			{
				End(mStatistics);
			}

		private:
			Statistics* mStatistics;
		};

		inline float ToSeconds(uint64 ticks)
		{
			return float(double(ticks) / double(1000000000.));
		}

		/*
		
		
void StatDescDump(const ulib::perf::StatisticsDescription& desc)
{
	printf("- min: %.10fs\n- max: %.10fs\n- avg: %.10fs\n- sum: %.10fs\n- calls: %llu\n- minCall: %llu\n- maxCall: %llu\n",
		to_sec(desc.min), to_sec(desc.max), to_sec(desc.avg.Value()), to_sec(desc.sum), desc.avg.Count(), 
		desc.minCallIndex, desc.maxCallIndex);
}

void PerfDump(const std::string& name)
{
	if (ulib::perf::Statistics* pStatistics = ulib::perf::Status(name))
	{
		if (!pStatistics->ready)
		{
			printf("perf %s is not ready\n", name.c_str());
			return;
		}

		printf("\n--- perf dump start ---\n");

	
		if (pStatistics->mDescs.size() > 1)
		{
			printf("\n[%s]: (all)\n", name.c_str());
			StatDescDump(*pStatistics);
		}		

		for (auto& obj : pStatistics->mDescs)
		{
			printf("\n[%s]: (%s)\n", name.c_str(), obj.first.c_str());
			StatDescDump(obj.second);
		}

		printf("\n--- perf dump end --- \n\n");
	}
	else
	{
		printf("perf %s not found\n", name.c_str());
	}
}
		
		*/
	}

}