#include "RaiMinerApi.h"


unsigned long withdraw_time = 1000 * 60 * 61; // mean time between withdraws (61min)
unsigned long withdraw_duetime = 0;


RaiMinerApi::RaiMinerApi(Client &client, String xrb_adr)	{
  this->client = &client;
  this->adress = xrb_adr;
}



String RaiMinerApi::SendPostToRaiMiner(String payload) {
  
  
  	String body="";
	body.reserve(500);
	bool finishedHeaders = false;
	bool currentLineIsBlank = true;
	bool startFound = false;
	long now;
	bool avail;

	if ( !client->connect(RAIMINER_HOST, Port) ) {
		return "{\"error\":\"could not connect to api\"}";
	}

	if (minerdebug) Serial.println(".... connected to server");
	if (minerdebug) Serial.print("payload: ");
	if (minerdebug) Serial.println(payload);
	
	String a="";
	char c;
	int ch_count=0;
	
	client->println("POST /api.php HTTP/1.1");
	client->println("Host: " RAIMINER_HOST);
	client->println("Accept: */*");
	client->println("Content-Type: application/x-www-form-urlencoded");
	client->print("Content-Length: ");
	client->println(payload.length());
	client->println();
	client->print(payload);
	client->println();

	now=millis();
	avail=false;
	
	while (millis()-now<1500) {
		while (client->available()) {
			char c = client->read();
			//Serial.write(c);

			if(!finishedHeaders){
				if (currentLineIsBlank && c == '\n') {
					finishedHeaders = true;
				}
			} else {
				if ( !startFound && c == '{') startFound = true;
				if (startFound){
					body=body+c;
					ch_count++;
				}
			}

			if (c == '\n') {
				currentLineIsBlank = true;
			} else if (c != '\r') {
				currentLineIsBlank = false;
			}
			avail=true;
		}
		if (avail) {
			while (!body.endsWith("}") && body.length() > 0) body = body.substring(0,body.length()-1);
			if (minerdebug) Serial.println("(response:)");
			if (minerdebug) Serial.println(body);
			if (minerdebug) Serial.println("(END)");
			break;
		}
	}

	if (minerdebug) Serial.println("------------------------");
	
	return body;

}



MinerStats RaiMinerApi::GetStats() {
	
	return GetStats(this->adress);
	
}

MinerStats RaiMinerApi::GetStats(bool autoWithdraw) {
	
	unsigned long timeNow;
	if (autoWithdraw) timeNow = millis();
	
	if (withdraw_duetime == 0 && autoWithdraw){
		withdraw_duetime = timeNow + withdraw_time;
	}
	
	if (timeNow > withdraw_duetime && autoWithdraw)  {    
		Withdraw(this->adress);
	}
	
	return GetStats(this->adress);
	
}



MinerStats RaiMinerApi::GetStats(String xrb_adr) {
		
	String payload = "address="+xrb_adr+"&action=load";
	String response = SendPostToRaiMiner(payload);
	
	MinerStats responseObject = MinerStats();
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(response);
	
	if (root.success()) {
		
		unsigned long temp = root["total"].as<float>()*root["xrbPerHash"].as<float>();    		
		responseObject.xrb_mined = temp;
		
		temp = root["withdrawn"].as<float>()*root["xrbPerHash"].as<float>();    		
		responseObject.xrb_withdrawn = temp;
		
		temp = root["balance"].as<float>()*root["xrbPerHash"].as<float>();    		
		responseObject.xrb_balance = temp;
		
		responseObject.balance =  root["balance"].as<long>();
		
		if (root.containsKey("error")) {
			responseObject.error = root["error"].as<String>();
		}
		
	} else {
		responseObject.error = "Failed to parse JSON";
	}

    return responseObject;
	
}

MinerWithdraw RaiMinerApi::Withdraw() {
	
	return Withdraw(this->adress);
	
}

MinerWithdraw RaiMinerApi::Withdraw(String xrb_adr) {
	
	MinerStats Current = GetStats(xrb_adr);
	
	MinerWithdraw responseObject = MinerWithdraw();
		
	if (Current.balance < 1){
		responseObject.error = "insufficient balance";
		return responseObject;
	}
		
		
	String payload = "address="+xrb_adr+"&amount="+Current.balance+"&action=withdraw";
	String response = SendPostToRaiMiner(payload);	
	
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(response);
	
	if (root.success()) {
		
		String responseString = root["payment"].as<String>();    		
		responseObject.payment = responseString;
				
		if (root.containsKey("error")) {
			responseObject.error = root["error"].as<String>();
		}
		
	} else {
		responseObject.error = "Failed to parse JSON";
	}

    return responseObject;
	
	
}