# EOSIO Quickstart Web IDE for decentralized applications !

# Setup
Point your browser to the following URL https://gitpod.io/#https://github.com/your-github-account/eosio-web-ide to start the IDE. You will be automatically prompted to create a Gitpod account (all types of Gitpod accounts (including free) will work). You can also choose to provide multiple developers push access to your personal github fork of this repo to collaborate with them (one developer working on the smart contract (C++) while the other working on the front-end decentralized application (EOSJS), etc.). Each such developer sharing access to the forked repo will get their own copy of the EOSIO blockchain components to enable independent development.

You can test drive the system by accessing the IDE at https://gitpod.io/#https://github.com/EOSIO/eosio-web-ide (however you will not be able to save your work into the EOSIO/eosio-web-ide Github repository)

# Instructions

The following instructions assume that the Web IDE was started successfully (see [Setup](#setup)).

## Opening a terminal

To open a terminal, use the Terminal drop-down menu in the IDE user interface.

## Building sample contract

The source code for the sample smartcontract is at `contract/talk.cpp` within the IDE. To compile the contract, run this in a terminal:

```
eosio-cpp contract/integral.cpp

```

This will produce `integral.abi` and `integral.wasm`.

## Installing the contract

Run this in a terminal:

```
cleos create account eosio integral EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set code integral integral.wasm
cleos set abi integral integral.abi

```

## Creating users and using the contract

Run this in a terminal:
```
cleos create account eosio alice EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos push action integral create '[ "integral", "1000000000.0000 INTA"]' -p integral@active

```

## Listing the messages

Run this in a terminal:
```
cleos get currency stats integral INTA

```

## Viewing the front-end decentralized web app (EOSJS):

The source code for the React WebApp is at `webapp/src/index.tsx` within the IDE. To preview the WebApp run this in a terminal:

```
gp preview $(gp url 8000)

```


