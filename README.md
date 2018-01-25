# arduino-raiminer-api
A wrapper around the raiminer.com api for ESP8266 (Arduino IDE).

https://raiminer.com is a Cryptocurrency Miner for RaiBlocks. RaiBlocks are not mineable, so you're mining Monero for them in your browser. They just convert the mined Monero and do all payouts in Rai.

This Library allows to display your mining progress through the raiminer.com api. It is NOT possible to actually mine RaiBlocks or Monero with your ESP8266. 



## Usage

The simplest way to see how it works is the check out [the example that comes with the library](https://github.com/benjholl/arduino-raiminer-api/blob/master/examples/ESP8266/GetRaiMinerStats/GetRaiMinerStats.ino).


### Get Stats

```
RaiMinerApi rai(client, xrb_adress);
MinerStats stats = rai.GetStats();
```
This simply returns a MinerStats Object:
```
struct MinerStats {
	unsigned long xrb_mined;
	unsigned long xrb_withdrawn;
	unsigned long xrb_balance;
	double balance;
	String error;	
	unsigned long auto_withdraw_timer;
};
```


### Withdraw

```
RaiMinerApi rai(client, xrb_adress);
MinerStats stats = rai.Withdraw();
```
tries to withdraw your mined uXRB to your Wallet. Note that withdrawals are limited to one per two hours, 10 uXRB minimum.
You will get a MinerWithdraw Object as response: 
```
struct MinerWithdraw{
	String payment;
	String error;	
};
```
`payment` will return a payment ID which could be used to check the progress of your payment and get the Block ID on success.
Checking the progress and retrieving the Block ID are not implemnented in this library, but you should see the payment get through to your Wallet within a few seconds.


### automatic Withdraw

if you do Status requests on a regular basis, you can use the following Code to automatically withdraw your XRB every 121 Minutes:
```
RaiMinerApi rai(client, xrb_adress);
MinerStats stats = rai.GetStats(true);
```
Note that the first withdraw request will always be sent 121 Minutes after loading the Program, the second one 242 Minutes after loading the Program and so on. 
