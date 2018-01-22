/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/


#ifndef RaiMinerApi_h
#define RaiMinerApi_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

#define RAIMINER_HOST "raiminer.com"
#define minerdebug false

struct MinerStats {
	unsigned long xrb_mined;
	unsigned long xrb_withdrawn;
	unsigned long xrb_balance;
	double balance;
	String error;
	unsigned long auto_withdraw_timer;
};

struct MinerWithdraw{
	String payment;
	String error;	
};


class RaiMinerApi
{
  public:
    RaiMinerApi (Client &client, String xrb_adr);
    MinerStats GetStats();
    MinerStats GetStats(bool autoWithdraw);
	MinerWithdraw Withdraw();
    int Port = 80;

  private:
    String SendPostToRaiMiner(String payload);
    MinerStats GetStats(String xrb_adr);
	MinerWithdraw Withdraw(String xrb_adr);
    Client *client;
	String adress;
};

#endif
