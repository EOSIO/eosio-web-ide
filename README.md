# EOSIO Quickstart Web IDE for decentralized applications ![EOSIO Alpha](https://img.shields.io/badge/EOSIO-Alpha-blue.svg)

[![Software License](https://img.shields.io/badge/license-MIT-lightgrey.svg)](./LICENSE)

EOSIO Quickstart Web IDE lets developers start building full-stack EOSIO applications in a matter of minutes. 

Powered by Gitpod.io and Docker, it provides developers with a personal single-node EOSIO blockchain for development and testing purposes without a need of going through advanced local environment setup. It also includes an example application with a smart contract and web frontend, connected to the blockchain. Developers can also use EOSIO tools like cleos and  eosio.cdt straight out of the box. This project requires zero installation on the user's machine. All code is stored and managed on the developer's personal GitHub account, with the changes saved automatically.

We built this project with ease of use and simplicity in mind. It can be used by new developers trying out or learning EOSIO, as well as advanced developers and teams. It is especially useful in the environments where users don't have full control over the systems they work on (universities, banks, government organizations, etc.) or when they have lower-than-required computer specs to run EOSIO locally.

We hope you will find this project useful and welcome feedback on future improvements.

# Setup

1. Fork this repo to your personal GitHub account so that you can save your work into your personal Github account.

2. Point your browser to the following URL https://gitpod.io/#https://github.com/your-github-account/eosio-web-ide to start the IDE. You will be automatically prompted to create a Gitpod account (all types of Gitpod accounts (including free) will work). You can also choose to provide multiple developers push access to your personal github fork of this repo to collaborate with them (one developer working on the smart contract (C++) while the other working on the front-end decentralized application (EOSJS), etc.). Each such developer sharing access to the forked repo will get their own copy of the EOSIO blockchain components to enable independent development.

You can test drive the system by accessing the IDE at https://gitpod.io/#https://github.com/EOSIO/eosio-web-ide (however you will not be able to save your work into the EOSIO/eosio-web-ide Github repository)

# Instructions

The following instructions assume that the Web IDE was started successfully (see [Setup](#setup)).

## Opening a terminal

To open a terminal, use the Terminal drop-down menu in the IDE user interface.

## Building sample contract

The source code for the sample smartcontract is at `contract/talk.cpp` within the IDE. To compile the contract, run this in a terminal:

```
eosio-cpp contract/talk.cpp

```

This will produce `talk.abi` and `talk.wasm`.

## Installing the contract

Run this in a terminal:

```
cleos create account eosio talk EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set code talk talk.wasm
cleos set abi talk talk.abi

```

## Creating users and using the contract

Run this in a terminal:
```
cleos create account eosio bob EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio jane EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos push action talk post '[1000, 0, bob, "This is a new post"]' -p bob
cleos push action talk post '[2000, 0, jane, "This is my first post"]' -p jane
cleos push action talk post '[1001, 2000, bob, "Replying to your post"]' -p bob

```

## Listing the messages

Run this in a terminal:
```
cleos get table talk '' message

```

## Viewing the front-end decentralized web app (EOSJS):

The source code for the React WebApp is at `webapp/src/index.tsx` within the IDE. To preview the WebApp run this in a terminal:

```
gp preview $(gp url 8000)

```

## Building and running the unit test

The source code for the unit test is at the `tests` directory within the IDE. To build the tests, run this in the terminal:

```
./build-tests

```

This will produce the `tester` binary, which can be run from the terminal to start the actual unit test:

```
./tester

```

The unit test creates the `talk_tests` test suite and verifies that the following statements are executed without error:

1. Create user account `talk`.
2. Load the `talk` smart contract in the `talk` account sandbox.
2. Create user accounts `john` and `jane`.
3. Test the `post` action by performing the following:
   1. Push the `post` action from `talk` to `john` with message "`post 1`" identified as `1` and addressed to message `0` (sent by noone).  
      This posts the message `1` from `john` to noone in the chat.
   2. Push the `post` action from `talk` to `jane` with message "`post 2`" identified as `2` and addressed to message `0` (sent by noone).  
      This posts the message `2` from `jane` to noone in the chat.
   3. Push the `post` action from `talk` to `john` with message "`post 3: reply`" identified as `3` and addressed to message `2` (sent by `jane`).  
      This posts the reply message `3` from `john` to `jane` in the chat.
4. Test failure of the `post` action if message is addressed to a non-existant message id.

## Resetting the chain

To remove the existing chain and create another:

* Switch to the terminal running `nodeos`
* Press `ctrl+c` to stop it
* Run the following

```
rm -rf ~/eosio/chain
nodeos --config-dir ~/eosio/chain/config --data-dir ~/eosio/chain/data -e -p eosio --plugin eosio::chain_api_plugin --contracts-console

```

Note: if the web app is currently open, then it will cause errors like the following. You may ignore them:

```
FC Exception encountered while processing chain.get_table_rows
```

## Contributing

[Contributing Guide](./CONTRIBUTING.md)

[Code of Conduct](./CONTRIBUTING.md#conduct)

## License

[MIT](./LICENSE)

## Important

See [LICENSE](LICENSE) for copyright and license terms.

All repositories and other materials are provided subject to the terms of this [IMPORTANT](important.md) notice and you must familiarize yourself with its terms.  The notice contains important information, limitations and restrictions relating to our software, publications, trademarks, third-party resources, and forward-looking statements.  By accessing any of our repositories and other materials, you accept and agree to the terms of the notice.
