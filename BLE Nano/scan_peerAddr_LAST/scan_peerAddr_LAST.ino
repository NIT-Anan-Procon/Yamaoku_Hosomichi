/*
 * BLEレシーバプログラム
 * edit:松原稜
 * 
 * nRF_ExamplesのTicker_Taskでタイマ割り込み
 * 
 * postTime():タイマ割り込みで実行　シリアル出力してset_empty()呼び出し
 * set_empty():配列postAddr[]をEMPTY(-1)にリセット
 * scanCallback():発見したタグのMACアドレスを配列postAddr[]に保持
 *  checkSum...MACアドレス1byte6個の数字を足しこむ　最大10進1530(0xFF 6ケタ)
 *  
 */
#include<nRF5x_BLE_API.h>
#include<stdio.h>

#define EMPTY 0

Ticker ticker1;   //タイマ定義
BLE ble;
int sumAddr[20]={};   //取得アドレスの合計値を被りなく格納
int lastAddr[20]={};  //前回のsumAddrを記憶する配列
byte postAddr[42]={}; //lastAddrと一致しない3桁のintであるsumAddrを2byteとして格納
int checkSum=0;       //取得アドレスの合計値を一時的に格納する変数

void setup() {
  Serial.begin(38400);  //シリアル通信レートを38400に設定
  //配列に0を代入して初期化
  set_empty_sum();
  set_empty_last();
  set_empty_post();
  //bleの設定
  ble.init();
  ble.setScanParams(1000, 200, 0, false); //
  ble.setActiveScan(true);
  ble.startScan(scanCallBack);
  ticker1.attach(postTime, 10);
}

void loop() {
  // put your main code here, to run repeatedly:
  ble.waitForEvent();
}

void scanCallBack(const Gap::AdvertisementCallbackParams_t *params) {
  //BLEタグ発見時アドレスの合計値をpostAddr[]に格納
  int i;
  for(i=0; i<6; i++) {
    //デバッグ用
    /*
    Serial.print(params->peerAddr[i],HEX);
    Serial.print(" ");
    */
    checkSum += params->peerAddr[i];
  }
  //Serial.println();
  //sumAddrにひとつのアドレスを複数回格納しないようにする。
  for(i=0; i<20; i++){
    if(sumAddr[i]==checkSum) break;
    if(sumAddr[i]==0){
      sumAddr[i]=checkSum;
      break;
    }
  }
  checkSum=0;
}

void postTime(void){
  //10秒おきに呼び出され、postAddr[]をシリアル送信
  //int i=0;
  int count=0;

  count=existence();  //送信するアドレスの数
  postAddr[0]=(byte)count;  //アドレスをbyte型に変換
//デバッグ用
/*
  Serial.print("sum: ");
  while(sumAddr[i]!=0){
    Serial.print(sumAddr[i],HEX);
    Serial.print(",");
    i++;
  }
  Serial.println();

  i=0;
  Serial.print("post: ");
  Serial.print(postAddr[i]);
  Serial.write(",");
  while(i<count*2){
    Serial.print(postAddr[i+1],HEX);
    Serial.write(",");
    Serial.print(postAddr[i+2],HEX);
    Serial.write(",");
    i+=2;
  }
  Serial.println();
  Serial.println();
*/
//送信するアドレスの種類*2+1バイト送信
  if(postAddr[0]>0)
    Serial.write(postAddr, count*2+1);
  set_empty_sum();
  set_empty_post();
}

int existence(){
  //sumAddrとlastAddrを比較、一致しなければtemp[]に格納し、shiftに格納 sumをlastに格納 送信するAddrの数countを返す
  int i=0, j=0, count=0;
  boolean unique=true;
  int temp[20]={};

  while(sumAddr[i]!=0){
    //一致しているアドレスを探す
    if(i>=20) break;
    j=0;
    while(lastAddr[j]!=0){
      if(sumAddr[i]==lastAddr[j]){
        unique=false;
        break;
      }
      j++;
    }
    //一致しているアドレスが無ければ一時的に配列に格納
    if(unique){
      temp[count++]=sumAddr[i];
    }
    unique=true;  //フラグ初期化
    i++;
  }

  shift(count, temp);  //アドレスを2byteに分けて送信用のpostAddrに格納
//デバッグ用Serial.print()
/*
  i=0;
  Serial.print("last: ");
  while(lastAddr[i]!=0){
    Serial.print(lastAddr[i],HEX);
    Serial.print(",");
    i++;
  }
  Serial.println();
*/
  i=0;
  set_empty_last();     //lastAddr初期化
  while(sumAddr[i]!=0){
    //今回検出したアドレスをlastAddrに格納
    lastAddr[i]=sumAddr[i];
    i++;
  }

  return count;
}

void shift(int count, int temp[]){
  int i=0;
  int j=0;
  while(j<count){
    //3桁のAddrを2byteにして格納、検出したAddrの回数繰り返す
    postAddr[i+1] = byte(temp[j]>>8);
    postAddr[i+2] = byte(temp[j]);
    i+=2;
    j++;
  }
}

//初期化用メソッド
void set_empty_sum(){
  int i;
  for(i=0; i<20; i++)
    sumAddr[i]=EMPTY;
}
void set_empty_post(){
  int i;
  for(i=0; i<42; i++)
    postAddr[i]=EMPTY;
}
void set_empty_last(){
  int i;
  for(i=0; i<20; i++)
    lastAddr[i]=EMPTY;
}

