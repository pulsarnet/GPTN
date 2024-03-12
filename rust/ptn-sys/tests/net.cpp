#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "userenv.lib")


#include <ptn/net.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

namespace net = ptn::net;
namespace vertex = net::vertex;

TEST_CASE("Create Petri Net") {
    const auto net = net::PetriNet::create();
    REQUIRE(net);
}

TEST_CASE("Work with vertecies of Petri Net") {
    const auto net = net::PetriNet::create();
    REQUIRE(net != nullptr);
    SECTION("get non-existing vertex from net") {
        constexpr auto p_idx = vertex::VertexIndex { vertex::VertexType::Position, 1 };
        constexpr auto t_idx = vertex::VertexIndex { vertex::VertexType::Transition, 1 };
        REQUIRE(!net->vertex(p_idx));
        REQUIRE(!net->vertex(t_idx));
    }
    SECTION("add position to net") {
        auto position = net->add_position();
        REQUIRE(position);
        constexpr auto p_idx = vertex::VertexIndex { vertex::VertexType::Position, 1 };
        REQUIRE(net->vertex(p_idx) == position);
        REQUIRE(net->positions().size() == 1);
    }
    SECTION("add transition to net") {
        auto transition = net->add_transition();
        REQUIRE(transition);
        constexpr auto t_idx = vertex::VertexIndex { vertex::VertexType::Transition, 1 };
        REQUIRE(net->vertex(t_idx) == transition);
        REQUIRE(net->transitions().size() == 1);
    }
    SECTION("benchmark") {
        // BENCHMARK
        const auto p_bench_net = net::PetriNet::create();
        BENCHMARK("add position to net") {
            return p_bench_net->add_position();
        };

        constexpr auto p_idx = vertex::VertexIndex { vertex::VertexType::Position, 1 };
        BENCHMARK("get position from net") {
            return p_bench_net->vertex(p_idx);
        };

        const auto t_bench_net = net::PetriNet::create();
        BENCHMARK("add transition to net") {
            return t_bench_net->add_transition();
        };

        constexpr auto t_idx = vertex::VertexIndex { vertex::VertexType::Transition, 1 };
        BENCHMARK("get transition from net") {
            return t_bench_net->vertex(t_idx);
        };
    }
}