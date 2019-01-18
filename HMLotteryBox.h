#pragma once
#include <limits>
#include <list>
#include <iostream>

template <typename T1>
class CHMLotteryBox
{
public:
	CHMLotteryBox() : m_nCurrentLotteryCount(0) { m_listLotteryPool.clear(); }
	~CHMLotteryBox() {};

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Draw a lottery from this box.
	// t1Lottery:	If this call succeed, the drawn lottery will be put into t1Lottery.
	// nRand:		It decides which lottery will be drawn. It should be a positive random
	//				int value(recommended), or -1.
	// Return:		Return true if succeed, false if failed.
	bool Draw(T1& t1Lottery, const int nRand = -1)
	{
		if (0 >= m_nCurrentLotteryCount) return false;
		if (nRand < 0 && -1 != nRand) return false;

		auto nKeyNum = ((-1 == nRand) ? rand() : nRand) % m_nCurrentLotteryCount;
		if (!FindLotteryByRandKey(t1Lottery, nKeyNum)) return false;
		if (!ModifyLotteryPool(t1Lottery, -1)) return false;
		m_nCurrentLotteryCount--;

		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Put in or take out lotteries from this box, so that we can get a proper
	//				lottery box we wanted.
	// pLottery:	A pointer to the storage of lottery that you want to put in or take out.
	// pCount:		A pointer to the storage of every lottery's count.
	// nLen:		The length of storage.
	// Return:		None.
	void Modify(const T1* const pLottery, const int* const pCount, const int nLen = 1)
	{
		if (NULL == pLottery || NULL == pCount || 0 >= nLen) return;

		for (auto i = 0; i < nLen; i++)
		{
			if (0 == pCount[i] || m_scnLotteryBoxCapacity - m_nCurrentLotteryCount < pCount[i]) continue;

			if (!ModifyLotteryPool(pLottery[i], pCount[i])) continue;

			m_nCurrentLotteryCount += pCount[i];
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Put in or take out lotteries from this box, so that we can get a proper
	//				lottery box we needed.
	// t2Lottery:	A user-defined container which contains the lotteries we want to put in
	//				or take out and their counts.
	// Return:		None.
	template <typename T2>
	void Modify(const T2& t2Lottery)
	{
		for (auto it = t2Lottery.cbegin(); it != t2Lottery.cend(); it++)
		{
			if (0 == it->second || m_scnLotteryBoxCapacity - m_nCurrentLotteryCount < it->second) continue;

			if (!ModifyLotteryPool(it->first, it->second)) continue;

			m_nCurrentLotteryCount += it->second;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Clear this box to make it empty.
	// Return:		None.
	void Clear()
	{
		m_nCurrentLotteryCount = 0;
		m_listLotteryPool.clear();
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Get particular lotteries or total lotteries counts, depend on pLottery.
	// pLottery:	A pointer of the specific lottery, if it == NULL, get the total count.
	// Return:		The count.
	int GetCount(const T1* const pLottery = NULL) const
	{
		if (NULL == pLottery) return m_nCurrentLotteryCount;

		for (auto it = m_listLotteryPool.cbegin(); it != m_listLotteryPool.cend(); it++)
		{
			if (*pLottery == it->first) return it->second;
		}
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Dump the details of this box.
	// Return:		None.
	void Dump() const
	{
		std::cout << "CHMLotteryBoxVersion " << m_cszCHMLotteryBoxVersion << std::endl;
		std::cout << "Current total lottery count " << m_nCurrentLotteryCount << "." << std::endl;
		std::cout << "Lottery box capacity " << m_scnLotteryBoxCapacity << std::endl;

		int nIndex = 0;
		for (auto it = m_listLotteryPool.cbegin(); it != m_listLotteryPool.cend(); it++)
		{
			nIndex++;
			std::cout << "Lottery index " << nIndex << ", count " << it->second << "." << std::endl;
		}
	}

private:
	int m_nCurrentLotteryCount;
	std::list<std::pair<T1, int>> m_listLotteryPool;
	static const int m_scnLotteryBoxCapacity = INT_MAX;
	static const char m_cszCHMLotteryBoxVersion[];

	bool ModifyLotteryPool(const T1& t1Lottery, const int nCount)
	{
		for (auto it = m_listLotteryPool.begin(); it != m_listLotteryPool.end(); it++)
		{
			if (t1Lottery == it->first)
			{
				if (0 > it->second + nCount) return false;	// 要删除的奖券不够数，返回错误
				else if (0 == it->second + nCount) m_listLotteryPool.erase(it);	// 此类奖券被删除完了，移除此项
				else it->second += nCount;	// 加上此类奖券的数量
				return true;
			}
		}
		if (0 > nCount) return false;	// 删除不存在的奖券，返回错误
		else if (0 < nCount)
		{
			try
			{
				m_listLotteryPool.push_back(std::make_pair(t1Lottery, nCount));
			}
			catch (const std::exception& e)
			{
				std::cout << __FILE__ << "(" << __LINE__ << "), exception: " << e.what() << std::endl;
			}
			catch (...)
			{
				std::cout << __FILE__ << "(" << __LINE__ << "), unknow exception" << std::endl;
			}
		}
		return true;
	}

	bool FindLotteryByRandKey(T1& t1Lottery, const int nRandKey) const
	{
		auto nBotton = 0, nTop = 0;
		for (auto it = m_listLotteryPool.cbegin(); it != m_listLotteryPool.cend(); it++)
		{
			nBotton = nTop;
			nTop += it->second;
			if (nRandKey >= nBotton && nRandKey < nTop)
			{
				t1Lottery = it->first;
				return true;
			}
		}
		return false;
	}
};

template <typename T1>
const char CHMLotteryBox<T1>::m_cszCHMLotteryBoxVersion[] = "1.0.0.0";
