#include <LoggerAPI.h>
#include <EventAPI.h>
#include <mc/BlockInstance.hpp>
#include <mc/Block.hpp>
#include <mc/Player.hpp>
#include <FormUI.h>
#include <mc/BlockActorDataPacket.hpp>
#include <mc/SignBlockActor.hpp>
#include <mc/Level.hpp>
#include <mc/Packet.hpp>
#include "version.h"

extern Logger logger;

using namespace std;

Player* plr = nullptr;

vector<string> split(const string& str, const string& pattern){
	vector<string> res;
	if (str == "")
		return res;
	string strs = str + pattern;
	size_t pos = strs.find(pattern);
	while (pos != strs.npos){
		string temp = strs.substr(0, pos);
		res.push_back(temp);
		strs = strs.substr(pos + 1, strs.size());
		pos = strs.find(pattern);
	}
	return res;
}

void EditSignForm(string signtext , Player* pl , SignBlockActor* blen){
	Form::CustomForm EditForm("告示牌编辑");
	auto inputdata = split(signtext, "\n");
    auto num = inputdata.size();
    for (int i = 1; i <= 4; i++) {
        if (i <= num){
			EditForm.addInput("signText" + to_string(i), "告示牌第" + to_string(i) + "行", "请输入告示牌第" + to_string(i) + "行文本", inputdata[i - 1]);
		}
        else{
			EditForm.addInput("signText" + to_string(i), "告示牌第" + to_string(i) + "行", "请输入告示牌第" + to_string(i) + "行文本");
		}
    }
    EditForm.sendTo(pl, [blen](Player* pl, auto result){
        if (result.empty()){
			pl->sendText("告示牌编辑已关闭！");
			plr = nullptr;
			return;
		}
		else{
			string newtext = "";
			for (auto i = 1; i <= 4; i++) {
                string str = result["signText" + to_string(i)]->getString();
                if (i != 4){
					newtext += str + "\n";
				}
                else{
					newtext += str;
				}        
            }
            blen->setMessage(newtext, "");
            blen->setChanged();
			pl->sendText("告示牌编辑成功！");
			plr = nullptr;
			return;
		}
    });
}

void PluginInit(){
    logger.info("当前版本 {}.{}.{}",PLUGIN_VERSION_MAJOR,PLUGIN_VERSION_MINOR,PLUGIN_VERSION_REVISION);
    logger.info("插件作者 Tsubasa6848");

	Event::PlayerUseItemOnEvent::subscribe([](const Event::PlayerUseItemOnEvent& event) {
		auto pl = event.mPlayer;
        auto block = event.mBlockInstance;
		auto blocktype = block.getBlock()->getTypeName();
		if(blocktype.ends_with("_sign") && pl->isSneaking() && pl->isOP()){
			auto blen = (SignBlockActor*)block.getBlockEntity();
			if(blen){
				auto signtext = blen->getNbt()->getString("Text");
				if(plr == nullptr || plr != pl){
					EditSignForm(signtext , pl , blen);
					plr = pl;
				}
				return false;
			}
			else return true;
		}
        else return true;
    });
}