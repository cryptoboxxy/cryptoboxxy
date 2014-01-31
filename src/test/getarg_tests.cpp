#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-CBOX");
    BOOST_CHECK(GetBoolArg("-CBOX"));
    BOOST_CHECK(GetBoolArg("-CBOX", false));
    BOOST_CHECK(GetBoolArg("-CBOX", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-CBOXo"));
    BOOST_CHECK(!GetBoolArg("-CBOXo", false));
    BOOST_CHECK(GetBoolArg("-CBOXo", true));

    ResetArgs("-CBOX=0");
    BOOST_CHECK(!GetBoolArg("-CBOX"));
    BOOST_CHECK(!GetBoolArg("-CBOX", false));
    BOOST_CHECK(!GetBoolArg("-CBOX", true));

    ResetArgs("-CBOX=1");
    BOOST_CHECK(GetBoolArg("-CBOX"));
    BOOST_CHECK(GetBoolArg("-CBOX", false));
    BOOST_CHECK(GetBoolArg("-CBOX", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noCBOX");
    BOOST_CHECK(!GetBoolArg("-CBOX"));
    BOOST_CHECK(!GetBoolArg("-CBOX", false));
    BOOST_CHECK(!GetBoolArg("-CBOX", true));

    ResetArgs("-noCBOX=1");
    BOOST_CHECK(!GetBoolArg("-CBOX"));
    BOOST_CHECK(!GetBoolArg("-CBOX", false));
    BOOST_CHECK(!GetBoolArg("-CBOX", true));

    ResetArgs("-CBOX -noCBOX");  // -CBOX should win
    BOOST_CHECK(GetBoolArg("-CBOX"));
    BOOST_CHECK(GetBoolArg("-CBOX", false));
    BOOST_CHECK(GetBoolArg("-CBOX", true));

    ResetArgs("-CBOX=1 -noCBOX=1");  // -CBOX should win
    BOOST_CHECK(GetBoolArg("-CBOX"));
    BOOST_CHECK(GetBoolArg("-CBOX", false));
    BOOST_CHECK(GetBoolArg("-CBOX", true));

    ResetArgs("-CBOX=0 -noCBOX=0");  // -CBOX should win
    BOOST_CHECK(!GetBoolArg("-CBOX"));
    BOOST_CHECK(!GetBoolArg("-CBOX", false));
    BOOST_CHECK(!GetBoolArg("-CBOX", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--CBOX=1");
    BOOST_CHECK(GetBoolArg("-CBOX"));
    BOOST_CHECK(GetBoolArg("-CBOX", false));
    BOOST_CHECK(GetBoolArg("-CBOX", true));

    ResetArgs("--noCBOX=1");
    BOOST_CHECK(!GetBoolArg("-CBOX"));
    BOOST_CHECK(!GetBoolArg("-CBOX", false));
    BOOST_CHECK(!GetBoolArg("-CBOX", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", "eleven"), "eleven");

    ResetArgs("-CBOX -bar");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", "eleven"), "");

    ResetArgs("-CBOX=");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", "eleven"), "");

    ResetArgs("-CBOX=11");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", "eleven"), "11");

    ResetArgs("-CBOX=eleven");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-CBOX", 0), 0);

    ResetArgs("-CBOX -bar");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-CBOX=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-CBOX=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--CBOX");
    BOOST_CHECK_EQUAL(GetBoolArg("-CBOX"), true);

    ResetArgs("--CBOX=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-CBOX", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noCBOX");
    BOOST_CHECK(!GetBoolArg("-CBOX"));
    BOOST_CHECK(!GetBoolArg("-CBOX", true));
    BOOST_CHECK(!GetBoolArg("-CBOX", false));

    ResetArgs("-noCBOX=1");
    BOOST_CHECK(!GetBoolArg("-CBOX"));
    BOOST_CHECK(!GetBoolArg("-CBOX", true));
    BOOST_CHECK(!GetBoolArg("-CBOX", false));

    ResetArgs("-noCBOX=0");
    BOOST_CHECK(GetBoolArg("-CBOX"));
    BOOST_CHECK(GetBoolArg("-CBOX", true));
    BOOST_CHECK(GetBoolArg("-CBOX", false));

    ResetArgs("-CBOX --noCBOX");
    BOOST_CHECK(GetBoolArg("-CBOX"));

    ResetArgs("-noCBOX -CBOX"); // CBOX always wins:
    BOOST_CHECK(GetBoolArg("-CBOX"));
}

BOOST_AUTO_TEST_SUITE_END()
