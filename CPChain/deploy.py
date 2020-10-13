#!/usr/bin/python3

from cpc_fusion import Web3
from solc import compile_source
from cpc_fusion.contract import ConciseContract

keypath = "/Users/macbook/Desktop/hack/datadir/keystore/UTC--2019-04-20T12-28-42.789354000Z--cfb3861ea8a1b69cdaa4beaf1a7237d8638e46e9"

# your password
password = "97435674"

def create_contract():
    # Solidity source code
    contract_source_code = '''
        pragma solidity ^0.4.24;
        library Library {
        struct data {
        int val;
        uint8 flag;
        }
        }
        
        contract Metacoin {
        using Library for Library.data;
        mapping(address => Library.data) public balances;

        
        
   
        
        // TODO: refine this
        function join() returns(bool sufficient){
        if (balances[msg.sender].flag == 0) {
        balances[msg.sender].val = 10000;
        balances[msg.sender].flag = 1;
        return true;
        }
        return false;
        }
        
        function sendcoin(address player1, address player2, address player3, address player4, int amount1, int amount2, int amount3, int amount4) {
        balances[player1].val += amount1;
        balances[player2].val += amount2;
        balances[player3].val += amount3;
        balances[player4].val += amount4;
        }
        
        
        function getcoin(address addr) returns(int) {
        return balances[addr].val;
        }
        }
        
        '''
    print(contract_source_code)
    
    compiled_sol = compile_source(contract_source_code)  # Compiled source code
    contract_interface = compiled_sol['<stdin>:Metacoin']

    return contract_interface

def deploy(contract_interface, keypath: str, password: str, addr: str):
    # web3.py instance
    w3 = Web3(Web3.HTTPProvider('http://127.0.0.1:8501'))

    # set pre-funded account as sender
    # w3.cpc.defaultAccount = w3.cpc.accounts[0]

    # Instantiate and deploy contract
    Metacoin = w3.cpc.contract(abi=contract_interface['abi'], bytecode=contract_interface['bin'])

    # Submit the transaction that deploys the contract
    # your keypath
    # change the keypath to your keystore file
    # keypath = "/Users/macbook/Desktop/hack/datadir/keystore/UTC--2019-04-20T12-28-42.789354000Z--cfb3861ea8a1b69cdaa4beaf1a7237d8638e46e9"
    # your password
    # password = "97435674"
    # your account address
    from_addr = w3.toChecksumAddress(addr)
    tx_hash = Metacoin.constructor().raw_transact({
        # Increase or decrease gas according to contract conditions
        'gas': 1000000,
        'from': from_addr,
        'value': 0
    }, keypath, password, 42)
    # tx_hash = Greeter.constructor().transact()

    # print('*********',w3.cpc.getTransactionReceipt(tx_hash_raw))
    print('*********', w3.cpc.getTransactionReceipt(tx_hash))

    # Wait for the transaction to be mined, and get the transaction receipt
    tx_receipt = w3.cpc.waitForTransactionReceipt(tx_hash)
    # tx_receipt1 = w3.cpc.waitForTransactionReceipt(tx_hash_raw)
    print(tx_receipt)
    # print(tx_receipt1)


'''

if __name__ == '__main__':
    contract_interface = create_contract()
    deploy(contract_interface)

'''


