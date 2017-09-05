#include "stdafx.h"
#include <vector>
#include <memory>
#include <algorithm>

using namespace ::testing;

namespace
{
    struct Human
    {
    public:
        Human(char sex, int number) : 
            m_sex(sex), m_number(number)
        {
        }

        inline char GetSex() { return m_sex; };
        inline int GetNum() { return m_number; };

    private:
        Human();

    private:
        int m_number = 0;
        char m_sex = ' ';
    };

    typedef std::shared_ptr<Human> HumanPtr;

    void FillRandomVector(std::vector<HumanPtr>& v, size_t count)
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

    //class PtrVector : public Test
    //{
    //protected:
    //    void SetUp() override
    //    {
    //        FillRandomVector(m_humans, 0);
    //    }

    //private:
    //    std::vector<std::unique_ptr<Human>> m_humans;
    //};
}

TEST(PtrVector, SortVector)
{
    std::vector<HumanPtr> vec;
    size_t count = 20;

    FillRandomVector(vec, count);

    std::sort(vec.begin(), vec.end(), [](HumanPtr l, HumanPtr r) { return l->GetNum() < r->GetNum(); });

    int i = 1;
    for (const auto& human : vec)
    {
        
        ASSERT_EQ(i, human->GetNum());
        ++i;
    }
}