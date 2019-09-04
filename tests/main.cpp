#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test.hpp>
#include <eosio/chain/exceptions.hpp>
#include <fc/log/logger.hpp>
#include <iostream>

#define BOOST_TEST_STATIC_LINK

void translate_fc_exception(const fc::exception& e) {
    std::cerr << "\033[33m" << e.to_detail_string() << "\033[0m" << std::endl;
    BOOST_TEST_FAIL("Caught Unexpected Exception");
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[]) {
    // Turn off blockchain logging if no --verbose parameter is not added
    // To have verbose enabled, use "./unit_tests -- --verbose"
    bool        is_verbose  = false;
    std::string verbose_arg = "--verbose";
    for (int i = 0; i < argc; i++) {
        if (argv[i] == verbose_arg)
            is_verbose = true;
    }

    if (is_verbose)
        fc::logger::get(DEFAULT_LOGGER).set_log_level(fc::log_level::debug);
    else
        fc::logger::get(DEFAULT_LOGGER).set_log_level(fc::log_level::off);

    // Register fc::exception translator
    boost::unit_test::unit_test_monitor.template register_exception_translator<fc::exception>(&translate_fc_exception);

    std::srand(time(NULL));
    std::cout << "Random number generator seeded to " << time(NULL) << std::endl;
    return nullptr;
}
