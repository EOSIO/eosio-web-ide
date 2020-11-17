#All default accounts will have the same password
if [ -z "$1" ]
    then
        echo "No argument supplied"
        exit 1
fi

var=`cat pub_key.txt`

# User Account
cleos create account eosio $1 ${var:47:53}

#Allows park.vt to do transactions for you
cleos set account permission $1 active '{"threshold": 1, "keys":[{"key":"EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV", "weight":1}], "accounts":[{"permission":{"actor":"patient","permission":"eosio.code"},"weight":1}], "waits":[] }' owner -p $1
