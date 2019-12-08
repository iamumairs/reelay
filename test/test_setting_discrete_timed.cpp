#include "iostream"
#include "vector"

#include "catch.hpp"

#include "reelay/monitors.hpp"

using input_t = std::map<std::string, std::string>;

TEST_CASE( "Atoms" ) {

    SECTION( "GreaterThan" ) {

        std::vector<input_t> sequence = std::vector<input_t>();
       
        sequence.push_back( input_t{{"x1", "3.3"}} );
        sequence.push_back( input_t{{"x1", "3.4"}} );
        sequence.push_back( input_t{{"x1", "3.5"}} );

        auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("x1 > 3.4");

        auto result = std::vector<bool>();

        for(const auto& row : sequence){
            net1->update(row);
            result.push_back(net1->output());
        }

        auto expected = std::vector<bool>({0,0,1});

        CHECK(result == expected);
    }

    SECTION( "GreaterEqual" ) {

        std::vector<input_t> sequence = std::vector<input_t>();
       
        sequence.push_back( input_t{{"x1", "3.3"}} );
        sequence.push_back( input_t{{"x1", "3.4"}} );
        sequence.push_back( input_t{{"x1", "3.5"}} );

        auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("x1 >= 3.4");

        auto result = std::vector<bool>();

        for(const auto& row : sequence){
            net1->update(row);
            result.push_back(net1->output());
        }

        auto expected = std::vector<bool>({0,1,1});

        CHECK(result == expected);
    }

    SECTION( "LessThan" ) {

        std::vector<input_t> sequence = std::vector<input_t>();
       
        sequence.push_back( input_t{{"x1", "3.3"}} );
        sequence.push_back( input_t{{"x1", "3.4"}} );
        sequence.push_back( input_t{{"x1", "3.5"}} );

        auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("x1 < 3.4");

        auto result = std::vector<bool>();

        for(const auto& row : sequence){
            net1->update(row);
            result.push_back(net1->output());
        }

        auto expected = std::vector<bool>({1,0,0});

        CHECK(result == expected);
    }

    SECTION( "LessEqual" ) {

        std::vector<input_t> sequence = std::vector<input_t>();
       
        sequence.push_back( input_t{{"x1", "3.3"}} );
        sequence.push_back( input_t{{"x1", "3.4"}} );
        sequence.push_back( input_t{{"x1", "3.5"}} );

        auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("x1 <= 3.4");

        auto result = std::vector<bool>();

        for(const auto& row : sequence){
            net1->update(row);
            result.push_back(net1->output());
        }

        auto expected = std::vector<bool>({1,1,0});

        CHECK(result == expected);
    }

    SECTION( "CustomPredicates" ) {

        std::vector<input_t> sequence = std::vector<input_t>();
       
        sequence.push_back( input_t{{"x1", "3.3"}, {"x2", "1.7"}} );
        sequence.push_back( input_t{{"x1", "3.4"}, {"x2", "1.6"}} );
        sequence.push_back( input_t{{"x1", "3.5"}, {"x2", "1.9"}} );

        std::map<std::string, std::function<bool(const input_t &)>> predicates = {
            {"sum_x1_and_x2_gt_5", [](const input_t& row){return (std::stof(row.at("x1")) + std::stof(row.at("x2"))) > 5.0;} }
        };

        auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("$sum_x1_and_x2_gt_5", predicates);

        auto result = std::vector<bool>();

        for(const auto& row : sequence){
            net1->update(row);
            result.push_back(net1->output());
        }

        auto expected = std::vector<bool>({0,0,1});

        CHECK(result == expected);
    }
}

TEST_CASE( "Boolean Operations" ) {

    SECTION( "Disjunction" ) {

        std::vector<input_t> sequence = std::vector<input_t>();
       
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );


		auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("p1 or p2");

		auto result = std::vector<bool>();

    	for(const auto& row : sequence){
        	net1->update(row);
        	result.push_back(net1->output());
    	}

    	auto expected = std::vector<bool>({0,1,1,1});

        CHECK(result == expected);
    }

    SECTION( "Conjuction" ) {

        std::vector<input_t> sequence = std::vector<input_t>();
       
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );


        auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("p1 and p2");

        auto result = std::vector<bool>();

        for(const auto& row : sequence){
            net1->update(row);
            result.push_back(net1->output());
        }

        auto expected = std::vector<bool>({0,0,0,1});

        CHECK(result == expected);
    }

    SECTION( "Implication" ) {

        std::vector<input_t> sequence = std::vector<input_t>();
       
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );


        auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("p1 -> p2");

        auto result = std::vector<bool>();

        for(const auto& row : sequence){
            net1->update(row);
            result.push_back(net1->output());
        }

        auto expected = std::vector<bool>({1,0,1,1});

        CHECK(result == expected);
    }

    SECTION( "Negation" ) {

        std::vector<input_t> sequence = std::vector<input_t>();
       
        sequence.push_back( input_t{{"p1", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}} );


        auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("not p1");

        auto result = std::vector<bool>();

        for(const auto& row : sequence){
            net1->update(row);
            result.push_back(net1->output());
        }

        auto expected = std::vector<bool>({1,0});

        CHECK(result == expected);
    }
}

TEST_CASE( "Untimed Temporal Operations" ) {

  SECTION("Previous") {

    std::vector<input_t> sequence = std::vector<input_t>();

    sequence.push_back(input_t{{"p1", "1"}, {"p2", "0"}});
    sequence.push_back(input_t{{"p1", "0"}, {"p2", "1"}});
    sequence.push_back(input_t{{"p1", "1"}, {"p2", "0"}});
    sequence.push_back(input_t{{"p1", "0"}, {"p2", "1"}});

    auto net1 =
        reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic(
            "pre p1");
    auto net2 =
        reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic(
            "pre p2");

    auto result1 = std::vector<bool>();
    auto result2 = std::vector<bool>();

    for (const auto &s : sequence) {
      net1->update(s);
      net2->update(s);
      result1.push_back(net1->output());
      result2.push_back(net2->output());
    }

    auto expected1 = std::vector<bool>({0, 1, 0, 1});
    auto expected2 = std::vector<bool>({0, 0, 1, 0});

    CHECK(result1 == expected1);
    CHECK(result2 == expected2);
  }

  SECTION("Past Always") {

    std::vector<input_t> sequence = std::vector<input_t>();

    sequence.push_back(input_t{{"p1", "1"}, {"p2", "1"}});
    sequence.push_back(input_t{{"p1", "1"}, {"p2", "1"}});
    sequence.push_back(input_t{{"p1", "1"}, {"p2", "0"}});
    sequence.push_back(input_t{{"p1", "1"}, {"p2", "0"}});

    auto net1 =
        reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic(
            "historically p1");
    auto net2 =
        reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic(
            "historically p2");

    auto result1 = std::vector<bool>();
    auto result2 = std::vector<bool>();

    for (const auto &s : sequence) {
      net1->update(s);
      net2->update(s);
      result1.push_back(net1->output());
      result2.push_back(net2->output());
    }

    auto expected1 = std::vector<bool>({1, 1, 1, 1});
    auto expected2 = std::vector<bool>({1, 1, 0, 0});

    CHECK(result1 == expected1);
    CHECK(result2 == expected2);
    }

    SECTION( "Past Sometime" ) {

        std::vector<input_t> sequence = std::vector<input_t>();
       
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );

        auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("once p1");
        auto net2 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("once p2");

        auto result1 = std::vector<bool>();
        auto result2 = std::vector<bool>();

        for(const auto& s : sequence){
            net1->update(s);
            net2->update(s);
            result1.push_back(net1->output());
            result2.push_back(net2->output());
        }

        auto expected1 = std::vector<bool>({0,0,1,1});
        auto expected2 = std::vector<bool>({0,0,0,0});

        CHECK(result1 == expected1);
        CHECK(result2 == expected2);
    }

    SECTION( "Since" ) {

        std::vector<input_t> sequence = std::vector<input_t>();
       
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );

        auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("p1 since p2");

        auto result1 = std::vector<bool>();

        for(const auto& s : sequence){
            net1->update(s);
            result1.push_back(net1->output());
        }

        auto expected1 = std::vector<bool>({0,1,1,1,1,0,0,1,1,1,0,0,1,1});

        CHECK(result1 == expected1);
    }
}


TEST_CASE("Timed Temporal Operations")
{

	SECTION("Timed Once")
	{

        std::vector<input_t> sequence = std::vector<input_t>();

        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );

		auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("once[2:4] p2");

		auto result1 = std::vector<bool>();

		for (const auto &s : sequence)
		{
			net1->update(s);
			result1.push_back(net1->output());
		}

		auto expected1 = std::vector<bool>({0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1});

		CHECK(result1 == expected1);
	}

	SECTION("Timed Once Zero")
	{

        std::vector<input_t> sequence = std::vector<input_t>();

        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );

		auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("once[:4] p2");

		auto result1 = std::vector<bool>();

		for (const auto &s : sequence)
		{
			net1->update(s);
			result1.push_back(net1->output());
		}

		auto expected1 = std::vector<bool>({0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1});

		CHECK(result1 == expected1);
	}

	SECTION("Timed Once Inf")
	{

        std::vector<input_t> sequence = std::vector<input_t>();

        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );

		auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("once[2:] p2");

		auto result1 = std::vector<bool>();

		for (const auto &s : sequence)
		{
			net1->update(s);
			result1.push_back(net1->output());
		}

		auto expected1 = std::vector<bool>({0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1});

		CHECK(result1 == expected1);
	}

	SECTION("Timed Always")
	{
        std::vector<input_t> sequence = std::vector<input_t>();

        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );

		auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("historically[2:4] p1");

		auto result1 = std::vector<bool>();

		for (const auto &s : sequence)
		{
			net1->update(s);
			result1.push_back(net1->output());
		}

		auto expected1 = std::vector<bool>({1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1});

		CHECK(result1 == expected1);
	}

	SECTION("Timed Always Zero")
	{
        std::vector<input_t> sequence = std::vector<input_t>();

        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );

		auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("historically[:4] p1");

		auto result1 = std::vector<bool>();

		for (const auto &s : sequence)
		{
			net1->update(s);
			result1.push_back(net1->output());
		}

		auto expected1 = std::vector<bool>({0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0});

		CHECK(result1 == expected1);
	}

	SECTION("Timed Always Inf")
	{
        std::vector<input_t> sequence = std::vector<input_t>();

        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );

		auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("historically[2:] p1");

		auto result1 = std::vector<bool>();

		for (const auto &s : sequence)
		{
			net1->update(s);
			result1.push_back(net1->output());
		}

		auto expected1 = std::vector<bool>({1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0});

		CHECK(result1 == expected1);
	}

	SECTION("Timed Since")
	{

        std::vector<input_t> sequence = std::vector<input_t>();

        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );

		auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("p1 since[2:4] p2");

		auto result1 = std::vector<bool>();

		for (const auto &s : sequence)
		{
			net1->update(s);
			result1.push_back(net1->output());
		}

		auto expected1 = std::vector<bool>({0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0});

		CHECK(result1 == expected1);
	}

	SECTION("Timed Since Zero")
	{

        std::vector<input_t> sequence = std::vector<input_t>();

        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );

		auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("p1 since[:4] p2");

		auto result1 = std::vector<bool>();

		for (const auto &s : sequence)
		{
			net1->update(s);
			result1.push_back(net1->output());
		}

		auto expected1 = std::vector<bool>({0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0});

		CHECK(result1 == expected1);
	}

	SECTION("Timed Since Inf")
	{

        std::vector<input_t> sequence = std::vector<input_t>();

        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "1"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "1"}, {"p2", "0"}} );
        sequence.push_back( input_t{{"p1", "0"}, {"p2", "0"}} );

		auto net1 = reelay::discrete_timed<int>::monitor<input_t>::from_temporal_logic("p1 since[2:] p2");

		auto result1 = std::vector<bool>();

		for (const auto &s : sequence)
		{
			net1->update(s);
			result1.push_back(net1->output());
		}

		auto expected1 = std::vector<bool>({0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0});

		CHECK(result1 == expected1);
	}
}