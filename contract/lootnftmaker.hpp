/**
 *  @dev minakokojima
 *  env: EOSIO.CDT v1.3.2
 *  @copyright MinakoKojima
 */
#pragma once
#include "NFT.hpp"

using eosio::name;
using eosio::datastream;

class [[eosio::contract("lootnftmaker")]] lootnftmaker : public contract {
public:
    lootnftmaker( name receiver, name code, datastream<const char*> ds ) :
    contract( receiver, code, ds ){}

    /*
    _global(receiver, receiver.value),
    _shovel(receiver, receiver.value),{}
    
    TABLE global : public global_info {};
    
    TABLE shovel : public NFT::tradeable_NFT {
        uint64_t level;
    };
    
    typedef eosio::multi_index<"shovel"_n, shovel> shovel_t;
    shovel_t _shovel;

    ACTION init();
    ACTION clear();
    
    void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        
        auto &thiscontract = *this;
        
        switch (action) {
                EOSIO_DISPATCH_HELPER(lootnftmaker,
                                      (init)
                                      (clear)
                                      (draw)
                                      )
        }
    }
    */
};

extern "C" {
    [[noreturn]] void apply(name receiver, name code, uint64_t action) {
        lootnftmaker p( name(receiver), name(code), datastream<const char*>(nullptr, 0) );
        p.apply(receiver, code, action);
        eosio_exit(0);
    }
}
