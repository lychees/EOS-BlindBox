#include "lootnftcon.hpp"


void lootnftcon::log(const name& from, const std::string& memo){
	auto maxId = 0;

	log_index logs(_self, _self.value);

	logs.emplace(_self, [&](auto& p) {
		p.id = max(1, logs.available_primary_key());
		p.from = from;
		p.acttime = current_time_point().sec_since_epoch() ;
		p.memo = memo;
		maxId = p.id;
		});
	clearlog();
}

void lootnftcon::clearlog() {
	log_index logs(_self, _self.value);

	auto iter = logs.begin();
	if (iter == logs.end()) {
		return;
	}
	
	if(logs.rend() == logs.rbegin()){
		return ;
	}

	if ((logs.rbegin()->id - iter->id) >= 100) {
		logs.erase(iter);
	}
	iter = logs.begin();
}