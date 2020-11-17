#!/bin/bash

if [ -z "$1" ]
    then
        echo "No argument supplied"
        exit 1
fi

cleos wallet create -n $1 -f "$1_wallet_key.txt"

cleos wallet unlock -n $1 < $1_wallet_key.txt

cleos wallet create_key -n $1 > $1_pub_key.txt

cleos wallet import -n $1 --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3

var=`cat $1_pub_key.txt`
cleos create account eosio $1 ${var:47:53}

mv $1_wallet_key.txt eosio-wallet
mv $1_pub_key.txt eosio-wallet
