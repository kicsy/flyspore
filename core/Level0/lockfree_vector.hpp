#pragma once
namespace fs {
	namespace L0
	{
		template<typename T>
		class lockfree_vector
		{
			struct Item
			{
				T _p;
				Item* _next{ nullptr };
			};
			Item* head{nullptr};
			std::atomic<int> _ref{0 };
			std::atomic<bool> _need_recycle{false};
			std::vector<Item*> _garbage;
		public:
			lockfree_vector()
			{

			}
			~lockfree_vector()
			{
				recycle();
			}
			bool add(const T& t)
			{
				auto* pt = new Item();
				pt->_p = t;
				pt->_next = head;
				head = pt;
				return true;
			}

			int remove(const T& t)
			{
				_ref.fetch_add(1, std::memory_order_relaxed);
				Item* pItem = head;
				Item* front = nullptr;
				int count = 0;
				while (pItem)
				{
					if (pItem->_p == t)
					{
						if (front)
						{
							front->_next = pItem->_next;
						}
						else
						{
							head = nullptr;
						}
						_garbage.push_back(pItem);
						_need_recycle.store(true, std::memory_order_release);
						count++;
					}
					front = pItem;
					pItem = pItem->_next;
				}
				bool need_recycle = true;
				if (_ref.fetch_sub(1, std::memory_order_relaxed) == 1 &&
					_need_recycle.compare_exchange_strong(need_recycle, true, std::memory_order_seq_cst))
				{
					recycle();
				}
				return count;
			}

			template<typename Func >
			void for_each(Func func)
			{
				_ref.fetch_add(1, std::memory_order_relaxed);
				Item* pItem = head;
				while (pItem)
				{
					func(pItem->_p);
					pItem = pItem->_next;
				}
				bool need_recycle = true;
				if (_ref.fetch_sub(1, std::memory_order_relaxed) == 1 &&
					_need_recycle.compare_exchange_strong(need_recycle, true, std::memory_order_seq_cst))
				{
					recycle();
				}
			}
		private:

			void recycle()
			{
				for (auto i : _garbage)
				{
					delete  i;
				}
				_garbage.clear();
			}
		};
	}
}