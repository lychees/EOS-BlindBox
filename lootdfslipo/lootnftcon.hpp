#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <string>
#include <math.h>

namespace eosiosystem {
   class system_contract;
}

using namespace eosio;


#define YFCDSS_MINEPOOL_INDEX 3
#define DFSDSS_MINEPOOL_INDEX 4


#define SYSPARAM_ADMIN_ACCOUNT		1
#define SYSPARAM_CONTRACT_ENABLE	2

#define DFSDSS_POND_INDEX 3
#define YFCDSS_POND_INDEX 4

#define ASSET_EOS_SYMBOL symbol(4, EOS)
#define EOS_ACCOUNT name(eosio.token)

#define LOOT_SYMBOL symbol("LOOT", 4)
#define TIME_SYMBOL symbol("TIME", 8)

#define ASSET_USDT_SYMBOL S(4, USDT)
#define USDT_ACCOUNT name("tethertether")

#define ASSET_TIME_SYMBOL S(8, TIME)
#define TIME_ACCOUNT name("xpettimecore")

#define ASSET_LOOT_SYMBOL S(4, LOOT)
#define LOOT_ACCOUNT name("lootglobcore")

#define CONTRACT_HEAD_ID    1

#define max(a,b) (a>b?a:b)

class [[eosio::contract("lootnftcon")]] lootnftcon : public contract {
public:
    using contract::contract;

    [[eosio::action]] void rmparam(uint64_t id) {
        require_auth(_self);
        sysparam_index paramtable(_self, _self.value);
        paramtable.erase(paramtable.find(id));
    }

    [[eosio::action]] void setparam(uint64_t id, std::string val) {
        require_auth(_self);
        setparam_(id, val);
    }

    [[eosio::action]] void buyref(const name acc) {
        require_auth(acc);
	    buyref_(acc);
    }

private:
    double stringtodouble(std::string str) {
        double dTmp = 0.0;
        int iLen = str.length();
        int iPos = str.find(".");
        std::string strIntege = str.substr(0,iPos);
        std::string strDecimal = str.substr(iPos + 1,iLen - iPos - 1 );
        for (int i = 0; i < iPos; i++) {
            if (strIntege[i] >= '0' && strIntege[i] <= '9') {
                dTmp = dTmp * 10 + strIntege[i] - '0';
            }
        }
        for (int j = 0; j < strDecimal.length(); j++) {
            if (strDecimal[j] >= '0' && strDecimal[j] <= '9') {
                dTmp += (strDecimal[j] - '0') * pow(10.0,(0 - j - 1));
            }
        }
        return dTmp;
	}

    void setparam_(uint64_t id, const std::string& val){
        sysparam_index paramtable(_self, _self.value);

        auto iter = paramtable.find(id);
        if(iter != paramtable.end()){
            paramtable.modify(iter, same_payer, [&]( auto& s ) {
                s.val = val;
            });
        }else{
            paramtable.emplace( _self, [&]( auto& s ) {
                s.id = id;
                s.val = val;
            });
        }
    }

    inline std::string getsysparam(const uint64_t& key) const {
       sysparam_index paramtable(_self, _self.value);
       auto iter = paramtable.find(key);
       if(iter == paramtable.end()){
	        return std::string("");
       }else{
	        return iter->val;
       }
    }

    inline uint64_t toInt(const std::string& str) {
        if (str.empty() || str == "") {
            return 0;
        }
        else {
            std::string::size_type sz = 0;
            return std::stoull(str, &sz, 0);
        }
    }

    inline float toFloat(const std::string& str) {
        if (str.empty() || str == "") {
            return 0;
        }
        else {
            return stringtodouble(str);
        }
    }

    name get_admin() const {
         const std::string adminAccount = getsysparam(SYSPARAM_ADMIN_ACCOUNT);
         if (adminAccount.empty()) {
             return _self;
         }
         else {
             return name(adminAccount.c_str());
         }
   }

   bool contractenable() {
	    return toInt(getsysparam(SYSPARAM_CONTRACT_ENABLE)) == 1;
   }

   
   void require_auth_contract() const { eosio::require_auth(_self); }

   void require_auth_admin() const { eosio::require_auth(get_admin()); }


    //发送资金
    inline void transfer_asset(
        const name to,
        const eosio::asset& amount,
        const std::string& memo) {
        require_auth_admin();
        action(permission_level{_self, "active"_n}, "eosio.token"_n, "transfer"_n, std::make_tuple(_self, to, amount, memo)).send();
    }

    //发送NFT
    inline void transfer_nft(
        const uint64_t& tokenid,
        const name nftcontract,
        const name to,
        const std::string& memo) {
	    action(permission_level{_self, "active"_n}, nftcontract, "transfer"_n, std::make_tuple(tokenid, to, memo)).send();
    }


private:
    TABLE sysparam {
	    uint64_t id;
	    std::string	val;
	    uint64_t primary_key() const { return id; }
    };
   typedef multi_index<"sysparams"_n, sysparam> sysparam_index;

    TABLE log {
        uint64_t  id;
        name	from;
        uint64_t	acttime;
        std::string memo;
        uint64_t primary_key() const { return id; }
    };
    typedef multi_index<"log"_n, log> log_index;

   
    TABLE contracthead {
        uint64_t id;
        uint64_t usercount;
        uint64_t primary_key() const { return id; }

        void adduser(){
            usercount ++;
        }

        void subuser(){
            if(usercount > 0){
                usercount --;
            }
        }
   };
   typedef multi_index<"contracthead"_n, contracthead> contracthead_index;

    //用户表
    TABLE accounts {
        name acc;
	    uint64_t refid;
	    uint64_t myid;
	    uint64_t primary_key() const { return acc.value; }
    };
    typedef multi_index<"accounts"_n, accounts> accounts_index;

    //推荐人表
    TABLE referer{
	    uint64_t id;
	    name	 acc;
	    uint64_t primary_key() const { return id; }
    };
    typedef multi_index<"referers"_n, referer> referers_index;

   //盲盒表，支持OWNER索引
    TABLE box{
	    uint64_t id;
	    name owner;
	    uint64_t primary_key() const { return id; }
	    uint64_t get_secondary_1() const { return owner.value;}
    };
   typedef multi_index<"boxs"_n, box, indexed_by<"byowner"_n, const_mem_fun<box, uint64_t, &box::get_secondary_1>> > box_index;

private:
    void log(const name& from, const std::string& memo);

    void clearlog();


private://业务内容	

    //买推荐人ID
    void buyref_(const name& acc) {
	    auto iter = accounttable.find(acc.value);
	    check(iter == accounttable.end(), "Account doesn't exists");

        paramtable.modify(iter, same_payer, [&]( auto& s ) {
            s.myid = s.id;
        });
    }

    //开盒
    void openbox_( const name acc, const uint64_t boxid) {
    }

    //买盒
    void buybox_( const name acc){
    }

private:
    void adduser(const name& acc, uint64_t refid) {
        contracthead_index contracthead(_self, _self.value);
        auto citer = contracthead.find(CONTRACT_HEAD_ID);

        check(citer != contracthead.end(), "contract not init");

        contracthead.modify(citer, _self, [&](auto& p) {
            p.adduser();
        }); 

	    accounts_index accounttable(_self, _self.value);
	    auto iter = accounttable.find(acc.value);
	    check(iter != accounttable.end(), "Account already exists");

	    accounttable.emplace( _self, [&]( auto& s ) {
            s.acc = acc;
            s.refid = refid;
		    s.myid = 0;
        });

	    std::string memo = "";
	    memo += "join-" + std::to_string(refid);
	    log(acc, memo);
    }
};