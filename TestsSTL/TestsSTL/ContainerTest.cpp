#include "stdafx.h"
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <algorithm>

using namespace ::testing;

namespace
{
    struct TestDTor : Mock
    {
        MOCK_METHOD0(dTor, void());
    };

    TestDTor mock;

    struct Human
    {
    public:
        Human(char sex, int number) : 
            m_sex(sex), m_number(number)
        {
        }

        ~Human()
        {
            mock.dTor();
        }

        inline char GetSex() { return m_sex; };
        inline int GetNum() { return m_number; };

        const bool operator < (const Human& other)
        {
            return m_number < other.m_number;
        }

    private:
        Human();

    private:
        int m_number = 0;
        char m_sex = ' ';
    };

    typedef std::shared_ptr<Human> HumanPtr;

    void FillRandomVectorPtr(std::vector<HumanPtr>& v, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            if (std::rand() % 2 == 1)
            {
                v.push_back(HumanPtr(new Human('M', i + 1)));
            }
            else
            {
                v.push_back(HumanPtr(new Human('W', i + 1)));
            } 
        }

        std::random_shuffle(v.begin(), v.end());
    }

    struct PtrVector : public Test
    {
        void SetUp() override
        {
            m_count = 20;
            FillRandomVectorPtr(m_humans, m_count);
        }

        std::vector<HumanPtr> m_humans;
        size_t m_count = 0;
    };
}

TEST_F(PtrVector, SortVector)
{
    EXPECT_CALL(mock, dTor())
        .Times(m_count);

    std::sort(m_humans.begin(), m_humans.end(), [](HumanPtr l, HumanPtr r) { return l->GetNum() < r->GetNum(); });

    int i = 1;
    for (const auto& human : m_humans)
    {
        
        ASSERT_EQ(i, human->GetNum());
        ++i;
    }
}

struct IsMan
{
    bool operator() (HumanPtr const& h) const { return h->GetSex() == 'M'; };
};

TEST_F(PtrVector, PartitionCopy)
{
    EXPECT_CALL(mock, dTor())
        .Times(m_count);

    std::vector<HumanPtr> men, women;

    std::partition_copy(m_humans.begin(), m_humans.end(),
        std::back_inserter(men), std::back_inserter(women), [](HumanPtr const& h) { return h->GetSex() == 'M'; });

    ASSERT_EQ(m_humans.size(), men.size() + women.size());

    for (const auto& man : men)
    {
        ASSERT_EQ('M', man->GetSex());
    }

    for (const auto& woman : women)
    {
        ASSERT_EQ('W', woman->GetSex());
    }
}

TEST(UniqPtrVector, PartionCopy)
{
    EXPECT_CALL(mock, dTor())
        .Times(20);

    std::vector<std::unique_ptr<Human>> humans;

    for (int i = 0; i < 20; ++i)
    {
        if (rand() % 2 == 1)
        {
            humans.emplace_back(new Human('M', i + 1));
        }
        else
        {
            humans.emplace_back(new Human('W', i + 1));
        } 
    }

    std::random_shuffle(humans.begin(), humans.end());

    std::vector<std::unique_ptr<Human>> men, women;

    // Devide vector to M and W parts
    auto it = std::stable_partition(humans.begin(), humans.end(), [](std::unique_ptr<Human> const& h) { return h->GetSex() == 'M'; });

    std::move(humans.begin(), it, std::back_inserter(men));
    std::move(it, humans.end(), std::back_inserter(women));

    for (const auto& man : men)
    {
        ASSERT_EQ('M', man->GetSex());
    }

    for (const auto& woman : women)
    {
        ASSERT_EQ('W', woman->GetSex());
    }

    auto oldSize = humans.size();

    humans.erase(std::remove_if(humans.begin(), humans.end(), 
        [](std::unique_ptr<Human> const& h)
    {
        return h == nullptr;
    }), humans.end());

    ASSERT_EQ(0, humans.size());
    ASSERT_EQ(oldSize, men.size() + women.size());

    std::vector<std::unique_ptr<Human>> women2;
}

void FillRandomVector(std::vector<Human>& v, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        if (std::rand() % 2 == 1)
        {
            v.emplace_back('M', i + 1);
        }
        else
        {
            v.emplace_back('W', i + 1);
        }
    }

    std::random_shuffle(v.begin(), v.end());
}

void PrepareMap(std::map<int, Human>& map)
{
    EXPECT_CALL(mock, dTor())
        .Times(AtLeast(1));

    std::vector<Human> vec;
    FillRandomVector(vec, 10);

    std::for_each(vec.begin(), vec.end(),
        [&](Human& h)
    {
        map.emplace(h.GetNum(), h);
    });
}

TEST(MapTest, MapFind)
{ 
    std::map<int, Human> map;

    PrepareMap(map);

    auto it = map.find(5);

    char s = it->second.GetSex();

    map.at(5) = Human(s == 'M' ? 'W' : 'M', 11);

    ASSERT_NE(s, it->second.GetSex());
    ASSERT_NE(5, it->second.GetNum());
}

TEST(MapTest, MapBounds)
{
    std::map<int, Human> map;

    PrepareMap(map);

    //
    // map keys: 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
    // upper_bound return first comp(it, 5) = false
    // lower_bound return !compt(it, 5)&&!comp(5, it) or first comp(it, 5) = false
    //
    auto it = map.upper_bound(5);
    ASSERT_EQ(6, it->first);

    it = map.lower_bound(5);
    ASSERT_EQ(5, it->first);
}

TEST(ListTest, List)
{
    EXPECT_CALL(mock, dTor())
        .Times(AtLeast(1));

    std::vector<Human> vec;
    FillRandomVector(vec, 10);

    std::list<Human> l(vec.begin(), vec.end());

    l.emplace_back('W', 15);

    EXPECT_EQ(l.back().GetNum(), 15);
    EXPECT_EQ(l.back().GetSex(), 'W');

    l.pop_back();

    l.sort();

    int i = 1;
    for (auto& h : l)
    {
        ASSERT_EQ(i, h.GetNum());
        ++i;
    }


    std::list<Human> l2;

    auto it = l.begin();
    auto itEnd = l.begin();

    std::advance(it, 2);
    std::advance(itEnd, 5);

    l2.splice(l2.begin(), l, it, itEnd);

    ASSERT_EQ(3, l2.size());
    i = 3;
    for (auto& h : l2)
    {
        ASSERT_EQ(i, h.GetNum());
        ++i;
    }
}