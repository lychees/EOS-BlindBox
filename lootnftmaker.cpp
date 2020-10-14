/**
 *  @dev minakokojima
 *  env: EOSIO.CDT v1.3.2
 *  @copyright MinakoKojima
 */

#include "lootnftmaker.hpp"

void lootnftmaker::init() {
    require_auth(_self);
}

void lootnftmaker::clear() {
    require_auth(_self);
    
    singleton_global_t global(_self, _self.value);
    global.remove();
}

void lootnftmaker::draw() {
    require_auth(_self);
}
