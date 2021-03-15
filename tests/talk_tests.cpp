#include <boost/test/unit_test.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/chain/permission_object.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <eosio/testing/tester.hpp>

using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;

BOOST_AUTO_TEST_SUITE(talk_tests)

BOOST_AUTO_TEST_CASE(post) try {
    tester t{setup_policy::none};

    // Load contract
    t.create_account(N(talk));
    t.set_code(N(talk), read_wasm("talk.wasm"));
    t.set_abi(N(talk), read_abi("talk.abi").data());
    t.produce_block();

    // Create users
    t.create_account(N(john));
    t.create_account(N(jane));
    t.create_account(N(jack));

    // Test "post" action
    t.push_action(
        N(talk), N(post), N(john),
        mutable_variant_object //
        ("id", 1)              //
        ("reply_to", 0)        //
        ("user", "john")       //
        ("content", "post 1")  //
    );
    t.push_action(
        N(talk), N(post), N(jane),
        mutable_variant_object //
        ("id", 2)              //
        ("reply_to", 0)        //
        ("user", "jane")       //
        ("content", "post 2")  //
    );
    t.push_action(
        N(talk), N(post), N(john),
        mutable_variant_object       //
        ("id", 3)                    //
        ("reply_to", 2)              //
        ("user", "john")             //
        ("content", "post 3: reply") //
    );


    // jane likes johns post
    t.push_action(
        N(talk), N(likepost), N(jane),
        mutable_variant_object       //
        ("post_id", 1)               //
        ("user", "jane")             //
        ("react", 1)                 //
    );

    // jane changes reaction type from 'like' to 'love'
    t.push_action(
        N(talk), N(likepost), N(jane),
        mutable_variant_object       //
        ("post_id", 1)               //
        ("user", "jane")             //
        ("react", 2)                 //
    );

    // jack also reacts to johns post
    t.push_action(
        N(talk), N(likepost), N(jack),
        mutable_variant_object       //
        ("post_id", 1)               //
        ("user", "jack")             //
        ("react", 4)                 //
    );

    // jane deletes like 
    t.push_action(
        N(talk), N(likepost), N(jane),
        mutable_variant_object       //
        ("post_id", 1)               //
        ("user", "jane")             //
        ("react", 0)                 //
    );

    // Can't reply to non-existing message
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
                N(talk), N(post), N(john),
                mutable_variant_object       //
                ("id", 4)                    //
                ("reply_to", 99)             //
                ("user", "john")             //
                ("content", "post 3: reply") //
            );
        }(),
        fc::exception);

    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
                N(talk), N(post), N(john),
                mutable_variant_object       //
                ("id", 4)                    //
                ("reply_to", 99)             //
                ("user", "john")             //
                ("content", "post 3: reply") //
            );
        }(),
        fc::exception);        

    // must have proper authorization
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
                N(talk), N(likepost), N(john),
                mutable_variant_object       //
                ("post_id", 1)               //
                ("user", "jane")             //
                ("react", 1)                 //
            );
        }(),
        fc::exception);                    

     // cannot like yoru own post
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
                N(talk), N(likepost), N(john),
                mutable_variant_object       //
                ("post_id", 1)               //
                ("user", "john")             //
                ("react", 1)                 //
            );
        }(),
        fc::exception);     

    // cannot like a post which does not exist    
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
                N(talk), N(likepost), N(john),
                mutable_variant_object       //
                ("post_id", 3)               //
                ("user", "john")             //
                ("react", 1)                 //
            );
        }(),
        fc::exception);           
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
