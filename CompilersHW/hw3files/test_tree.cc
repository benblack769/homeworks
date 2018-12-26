/*
 * Tree class unit tests
 */

#include "catch.hpp"
#include "tree.hh"
#include "vectree.hh"
#include "ptrtree.hh"
#include <array>
#include <memory>

class TestTrees{
public:
    virtual ~TestTrees() = default;
    virtual tree::Tree & operator [](size_t i)const = 0;
};
template<class TreeType>
class TestTreesType:
    public TestTrees{
public:
    static constexpr size_t array_size = 7;
    std::array<TreeType *,array_size> trees;
    TestTreesType(){
        trees[0] = new TreeType(15);
        trees[1] = new TreeType(10);
        trees[2] = new TreeType(7,*trees[0],*trees[1]);
        trees[3] = new TreeType(3);
        trees[4] = new TreeType(8);
        trees[5] = new TreeType(5,*trees[3],*trees[4]);
        trees[6] = new TreeType(4,*trees[2],*trees[5]);
    }
    ~TestTreesType(){
        for(size_t i = 0; i <  array_size; i++){
            delete trees[i];
        }
    }
    virtual tree::Tree & operator [](size_t i)const override{
        return *trees[i];
    }
};
TEST_CASE("size is computed correctly for trees", "[vectree]") {
    std::unique_ptr<TestTrees> trees_types[2] = {std::unique_ptr<TestTrees>(new TestTreesType<tree::VecTree>()), std::unique_ptr<TestTrees>(new TestTreesType<tree::PtrTree>())};
    std::string case_types[2] = {"VecTree", "PtrTree"};
    for(int t_case = 0; t_case < 2; t_case++){
        TestTrees & trees = *trees_types[t_case];
        SECTION("leaf case: "+case_types[t_case]) {
            REQUIRE(trees[0].size() == 1);
        }

        SECTION("compound tree: "+case_types[t_case]) {
            REQUIRE(trees[4].size() == 1);
            REQUIRE(trees[2].size() == 3);
            REQUIRE(trees[6].size() == 7);
        }
    }
}

TEST_CASE("Path is computed correctly", "[vectree]") {
    std::unique_ptr<TestTrees> trees_types[2] = {std::unique_ptr<TestTrees>(new TestTreesType<tree::VecTree>()), std::unique_ptr<TestTrees>(new TestTreesType<tree::PtrTree>())};
    std::string case_types[2] = {"VecTree", "PtrTree"};
    for(int t_case = 0; t_case < 2; t_case++){
        TestTrees & trees = *trees_types[t_case];
        SECTION("check gotten paths: "+case_types[t_case]) {
            REQUIRE(trees[2].pathTo(7) == "");
            REQUIRE(trees[2].pathTo(15) == "L");
            REQUIRE(trees[2].pathTo(10) == "R");
            REQUIRE(trees[6].pathTo(4) == "");
            REQUIRE(trees[6].pathTo(15) == "LL");
            REQUIRE(trees[6].pathTo(8) == "RR");
            REQUIRE(trees[6].pathTo(3) == "RL");
        }
    }
}


TEST_CASE("getByPath works correctly", "[vectree]") {
    std::unique_ptr<TestTrees> trees_types[2] = {std::unique_ptr<TestTrees>(new TestTreesType<tree::VecTree>()), std::unique_ptr<TestTrees>(new TestTreesType<tree::PtrTree>())};
    std::string case_types[2] = {"VecTree", "PtrTree"};
    for(int t_case = 0; t_case < 2; t_case++){
        TestTrees & trees = *trees_types[t_case];
        SECTION("check paths: "+case_types[t_case]) {
            REQUIRE(trees[6].getByPath("LL") == 15);
            REQUIRE(trees[6].getByPath("RR") == 8);
            REQUIRE(trees[6].getByPath("RL") == 3);
            REQUIRE(trees[6].getByPath("L") == 7);
            REQUIRE(trees[6].getByPath("") == 4);
            REQUIRE(trees[2].getByPath("L") == 15);
        }
    }
}

TEST_CASE("ballanced trees verified for VecTree", "[vectree]") {
    using namespace tree;
    const VecTree tree1(15);
    const VecTree tree2(10);
    const VecTree tree3(7,tree1,tree2);
    REQUIRE_THROWS(VecTree(3,tree2,tree3));
}

TEST_CASE("ballanced trees not required for PtrTree", "[ptrtree]") {
    using namespace tree;
    const PtrTree tree1(15);
    const PtrTree tree2(10);
    const PtrTree tree3(7,tree1,tree2);
    REQUIRE_NOTHROW(PtrTree(3,tree2,tree3));
}
