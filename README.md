# EOSIO web-based integrated development environment (IDE) for decentralized applications ![EOSIO Alpha](https://img.shields.io/badge/EOSIO-Alpha-blue.svg)

[![Software License](https://img.shields.io/badge/license-MIT-lightgrey.svg)](./LICENSE)

This repository enables developers to instantiate a browser based IDE and runs the required EOSIO blockchain components (nodeos, keosd, cleos) in a personal cloud instance. All custom code created by the developer is securely managed on the developer's personal GitHub account. Zero installation is required on the user's machine. User's can simply point their browser to the provided URL below and begin compiling the provided example smart contract (C++) and the associated front-end decentralized application (EOSJS), deploy the application on the blockchain, and experience the application in under a few minutes.  

# Setup

1. Fork this repo to your personal GitHub account so that you can save your work into your personal Github account.

2. Point your browser to the following URL https://gitpod.io/#https://github.com/your-github-account/eosio-web-ide to start the IDE. You will be automatically prompted to create a Gitpod account (all types of Gitpod accounts (including free) will work). You can also choose to provide multiple developers push access to your personal github fork of this repo to collaborate with them (one developer working on the smart contract (C++) while the other working on the front-end decentralized application (EOSJS), etc.). Each such developer sharing access to the forked repo will get their own copy of the EOSIO blockchain components to enable independent development.

You can test drive the system by accessing the IDE at https://gitpod.io/#https://github.com/EOSIO/eosio-web-ide (however you will not be able to save your work into the EOSIO/eosio-web-ide Github repository)

# Follow the instructions below to work inside the IDE environment

## Opening a terminal

To open a terminal, use the Terminal drop-down menu in the IDE user interface.

## Building sample contract

Run this in a terminal:

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

Run this in a terminal:
```
gp preview $(gp url 8000)

```

## Building and running unit tests

```
./build-tests
./tester

```

## Resetting the chain

To remove the existing chain and create another:

* Switch to the terminal running `nodeos`
* Press `ctrl+c` to stop it
* Run the following

```
rm -rf ~/eosio/chain
nodeos --config-dir ~/eosio/chain/config --data-dir ~/eosio/chain/data -e -p eosio --plugin eosio::chain_api_plugin

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
