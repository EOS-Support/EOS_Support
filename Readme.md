# We Support EOS

## deploy the contract

### build

```shell
eosio-cpp wesupporteos.cpp -o wesupporteos.wasm
```

### deploy

```shell
cleos set contract {{CONTRACT_ACCOUNT}} ./ wesupporteos.wasm wesupporteos.abi
```

### add eosio.code permission

```shell
cleos set account permission {{CONTRACT_ACCOUNT}} active --add-code 
```

## use

### how to donate

transfer EOS to the contract with memo of eden member account

### how to claim

push the action `claim`