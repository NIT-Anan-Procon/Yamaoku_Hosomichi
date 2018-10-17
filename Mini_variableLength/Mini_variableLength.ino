/*  Arduino Pro Mini用プログラム
 *  void loop()
 *    ・BLE,TWELITEからの信号を確認する関数(BLE_receive,zgb_receive)を呼び出す
 *    ・1秒ごとに送信関数(merge)を呼び出す
 * 
 *  void BLE_receive()
 *    ・BLE Nanoから送られてきたタグIDを配列(BLE_strge[],Ack_strge[])に格納
 *    
 *  zgb_strge()
 *    ・TWELITE dipから送られてきた信号を配列(zgb_strge[],ack[])に格納
 *  
 *  void merge()
 *    ・上り方向への送信用配列にヘッダとデータ(BLE_strge[],zgb_strge[])を格納
 *    ・下り方向への送信用配列にヘッダとackデータ(zgb_strge[])を格納
 *    
 *  void ack_check()
 *    ・Ackが帰ってきているかをチェックし、送信確認用配列(ack_strge[])を管理
 *    
 *  byte ID_Judge()
 *    ・親は可変抵抗の値によってIDを決定する(子はハードウェアでIDを設定する)
 *  
 *  float readSens_ch(int ch, int n, int intarvalms)
 *  ・ID決定用ポートのanalogRead(ch)のn回平均値を返す
 * 
 *  void countSend()
 *    ・ArduinoProからのALM割り込み時処理
 *    
 *  void ALM_comp(byte type)
 *    ・TWELITEとArduinoProから送られてきた一番危険度が高いALMを保持する（送信用）
 *    
 *  int ALM_check(byte type_ALM, byte id)
 *    ・Proに命令する発光パターンを設定する
 */


#include<skRTClib.h>

/*Mini_variableLength.ino*/ //ver.1.05
#define VerNo "Ver1.05"

#include <avr/io.h>
#include <avr/interrupt.h>

#define LED_ALM 10     //LED点灯指示ALMのPWM出力
#define LED_Speed 9     //ALM検出Speed入力端子
#define LED_L_R 11       //ALM検出L-R入力端子
volatile int ALM_Flg = 0;    //ALM受信フラグ 1:上りLOW、2:上りHigh、E:下りLOW、F:下りHigh
int LED_ALM_count=0;    //LED点灯指示のFlgを維持するカウンタ

#define BAUDRATE 9600   //TWELITEボーレート

#include <SoftwareSerial.h>
SoftwareSerial Zigb(6,5);  //RX, TX
#define ZigbPW 3
#define ZigbPW2 7
#define ZigbGND 4

// for SD shield
#define chipSelect 8

#define IdAdGND 13  //ID判定用GND
#define IdAdPW 15   //ID判定用3.3V出力

byte DATA_B[8];    //BLEからの受信データ格納配列

byte MyID=0;      //自分のID番号
int Parent=0;   //親機フラグ

byte BLE_strge[40]; //自機が発見し、上り方向に送信するBLEタグID
byte ack_strge[40]; //ackが帰ってきているかの確認用

unsigned long Timems = 0;
unsigned long TimeLEDstopUp = 0;
unsigned long TimeLEDstopDw = 0;
#define LED_ON_Period  5000       //ALMによるLEDをONしておく時間の長さms
int LED_UP_Flg=0;                 //ALMによるLEDのONフラグ（上り用）
int LED_DW_Flg=0;                 //ALMによるLEDのONフラグ（下り用）

#define PIN_ALM 2                 //

byte zgb_strge[40]; //下手からのZigBee通信で受け取った、上り方向に送信するBLEタグID
byte ack[40]; //上手からのZigBee通信で受け取ったack

byte send_up[41];   //上り方向に送信
byte send_down[41]; //下り方向に送信
int count_BLE_strge=0;  //BLEタグID受信バイト数カウント
int count_zgb_strge=0;  //zgbタグID受信バイト数カウント
int count_up=0;         //上り方向送信バイト数カウント
int count_down=0;       //下り方向送信バイト数カウント
int sendBLE_count=0;    //ack待ち数カウント
int ack_count=0;        //ackのバイト数カウント

int Pwm=0;            //ArduinoPro出力用PWM値

byte ALM_up = 0x00;   //上り方向ALM
byte ALM_down=0x0F;   //下り方向ALM

void setup(){
  Serial.begin(38400); //BLEとの通信速度
  Zigb.begin(BAUDRATE);
  MyID=ID_Judge();        //AD0値を読み込んで親機の場合自分のIDを確定

  Serial.print(Parent);
  Serial.print(" MyID:");
  Serial.println(MyID);

  pinMode(PIN_ALM, INPUT);           //PIN_ALM(デジタルI/Oピン2)を出力
  attachInterrupt(0, countSend, FALLING);   //割り込み番号0:FALLINGピンの状態がHIGHからLOWに変わったときにcountSend関数を呼び出し

  pinMode(LED_L_R, INPUT);    //LED(L-R)入力に設定
  digitalWrite(LED_L_R, HIGH);    //プルアップ
  pinMode(LED_Speed, INPUT);    //LED(L-R)入力に設定
  pinMode(LED_ALM, OUTPUT);   //LED点灯指示出力に設定
  digitalWrite(LED_ALM, LOW);     //LED点灯指示出力をLOW(O)に設定

  pinMode(ZigbPW, OUTPUT);    //Zigbee電源用ポート出力に設定
  digitalWrite(ZigbPW, HIGH);   //Zigbee電源用ポートに3.3V出力
  pinMode(ZigbPW2, OUTPUT);   //Zigbee電源用ポート2出力に設定
  digitalWrite(ZigbPW2, HIGH);   //Zigbee電源用ポート2に3.3V出力
  pinMode(ZigbGND, OUTPUT);   //ZigbeeGND用ポート出力に設定
  digitalWrite(ZigbGND, LOW);   //ZigbeeGND用ポートに0V出力

  delay(200);
  while ( Zigb.available()) {
    char readchar = Zigb.read();
  }
}

void loop(){
  BLE_receive();
  zgb_receive();
  
  merge();
/*
  Serial.println("ALM");
  Serial.println(ALM_up, HEX);
  Serial.println(ALM_down, HEX);
*/
  //LED点灯パターンをPwmでProに出力
  if(LED_ALM_count>0){
    analogWrite(LED_ALM, Pwm*31);
    
    Serial.print("Pwm=");
    Serial.println(Pwm);
    Serial.println("LED Glow");

    LED_ALM_count=0;
	  delay(100);
  }else{
    Pwm=0;
    digitalWrite(LED_ALM, LOW);
  }
  
  delay(1000);
}

/* BLEレシーバから送られてきたIDを上り送信用配列とack確認用配列に格納 */
void BLE_receive(){
  memset(DATA_B, 0x00, 8);
  String response = "";
  int count = 0;
  int i=0;

  /* responseに足し込んでいく */
  while(Serial.available()){
    char readchar = Serial.read();
    response += readchar;
    Serial.print(readchar, HEX);
    Serial.print(" ");
    delay(20);
  }
  /*BLEタグIDを配列に格納*/
  if(response.length()>=2){
    //Serial.print("response=");
    //Serial.println(response);
    for(int n=0; n<response.length(); n++){
      DATA_B[n]=(byte)response[n];
    }
    count = DATA_B[0];  //発見タグ数
    Serial.print("count=");
    Serial.println(count, HEX);
     /*
      BLE_strge[], Ack_strge[]
    　  | MyID byte Parent tagID          |
        | xxxx xxxx x      xxx  xxxx xxxx |
      */
    for(int m=0; m<count; m++){
      BLE_strge[m*2] = MyID<<4;
      BLE_strge[m*2] += Parent<<3;
      BLE_strge[m*2] += DATA_B[m*2+1];
      BLE_strge[m*2+1] = DATA_B[m*2+2];
      
      ack_strge[m*2] = BLE_strge[m*2];
      ack_strge[m*2+1] = BLE_strge[m*2+1];
      sendBLE_count+=2;  //sendしたタグデータbyte数(ack確認用)
      count_BLE_strge+=2; //今回は発見して送信するタグデータbyte数(送信用)
    }
    Serial.print("sendBLE_count=");
    Serial.println(sendBLE_count, HEX);
    Serial.print("count_BLE_strge=");
    Serial.println(count_BLE_strge, HEX);
  }
}

/*TWELITE dipから送られてきた信号を上り方向への送信用配列と下り方向へのAck送信用配列に格納*/
void zgb_receive(){
  String response = "";
  int dt_no = 0;
  byte sendid;
  byte receiveID=0;   //送信元ID
  byte data_type=0;   //データ種別

  if(Zigb.available()){
    /*responseに足し込む*/
    Serial.print("zgb=");
    while(Zigb.available()){
      char readchar = Zigb.read();
      response += readchar;
      Serial.print(readchar,HEX);
      Serial.print(" ");
    }
    Serial.println();
    
    /*ヘッダの発信元IDとデータ種別を抽出*/
    receiveID = (byte)response[0]>>4;
    data_type = (byte)response[0]&0x0F;
  
  //有効データのみ通す　親は全ての子からの通信を受けるので隣接した灯籠からの自分に向けた信号のみ受け取る
    if(Parent==0  ||  (Parent==1 && MyID+1==receiveID && (0<=data_type && data_type<=2))  ||  (Parent==1 && MyID==receiveID && (0xD<=data_type && data_type<=0xF))){
      
      for(int m=2; m<response.length(); m+=2){
        //Serial.println("ZGB receive");
        //有効データ転送とBLE受信ならACKの返送
        if(Parent==0 || (Parent==1 && MyID+1==receiveID && (data_type>=0 || data_type<=2))){
          //Serial.println("格納中");
          //タグ発見場所とタグIDをストレージに前詰めで格納
          zgb_strge[m-2] = response[m];
          zgb_strge[m-1] = response[m+1];
  
          Serial.print(zgb_strge[m-2],HEX);
          Serial.print(" ");
          Serial.print(zgb_strge[m-1],HEX);
          
          if(Parent==1 && zgb_strge[m-2]&0x08==0){
            zgb_strge[m-2] = (zgb_strge[m-2]&0x0f) + (MyID<<4); //子が発見したBLEデータに発見ID情報追加
          }
          count_zgb_strge+=2; //zgb_strge[]格納数カウントアップ
        }
        /*上手からのAckを確認用配列に格納*/
        if(Parent==0 || Parent==1 && MyID==receiveID && data_type>=0xD || data_type<=0xF){
          ack[m-2] = response[m];
          ack[m-1] = response[m+1];
          ack_count+=2;
          Serial.print(ack[m-2],HEX);
          Serial.print(" ");
          Serial.print(ack[m-1],HEX);
          Serial.print(" ");    
        }
      }
      ack_check();
      Pwm = ALM_check(data_type, receiveID);
      //危険度を低くしてセット
      if(data_type>=0xD && data_type<0xF) data_type++;
      else if(data_type>0 && data_type<=2) data_type--;
      ALM_comp(data_type);
    }
  }
}

/*上り方向と下り方向にデータ送信*/
void merge(){
  int i;

  /*ヘッダ*/
  send_up[0] = MyID<<4;
  send_down[0] =  MyID<<4;
  
  send_up[0] += ALM_up;
  send_down[0] += ALM_down;

  if(ALM_up>0 && ALM_down<0xF){
    Serial.print("ALM_up=");
    Serial.println(ALM_up,HEX);
    Serial.print("ALM_down=");
    Serial.println(ALM_down,HEX);
  }

  /*上り方向送信用配列にデータ挿入*/
  for(i=0; i<count_BLE_strge; i++){
    send_up[2+count_up++] = BLE_strge[i];
    Serial.print(BLE_strge[i], HEX);
    Serial.print("(b),");
  }
  for(i=0; i<count_zgb_strge; i++){
    send_up[2+count_up++] = zgb_strge[i];
    Serial.print(zgb_strge[i],HEX);
    Serial.print("(z),");
  }

  /*下り方向用送信配列*/
  for(i=0; i<count_zgb_strge; i++){
    send_down[2+count_down++] = zgb_strge[i];
  }

  send_up[1] = count_up;
  send_down[1] = count_down;

/*送信*/
/*上り方向*/
  if(count_up>0){
    Zigb.write(send_up, count_up+2);
    Serial.println();
    Serial.print("send_up = ");
    for(i=0; i<count_up+2; i++){
      Serial.print(send_up[i],HEX);
      Serial.print(" ");
    }
    Serial.println();
    delay(500);
  }else if(0<ALM_up && ALM_up<=2){
    Zigb.write(send_up, count_up+2);
    
    Serial.print("ALM_upSend: ");
    Serial.println(send_up[0],HEX);
    delay(500);
  }
  
/*下り方向*/
  if(count_down>0){
    Zigb.write(send_down, count_down+2);
    Serial.print("send_down = ");
    for(i=0; i<count_down; i++){
      Serial.print(send_down[i],HEX);
      Serial.print(" ");
    }
    Serial.println();
    delay(500);
  }else if(0xD<=ALM_down && ALM_down<0xF){
    Zigb.write(send_down, count_down+2);
    
    Serial.print("ALM_downSend: ");
    Serial.println(send_down[0],HEX);
    delay(500);
  }
 
  ALM_up=0;
  ALM_down=0xF;
  
  count_BLE_strge = 0;
  count_zgb_strge = 0;
  count_up = 0;
  count_down=0;

/*デバッグ用*/
/*
  send_up[0]=0x00;
  send_up[1]=0x02;
  send_up[2]=0x0B;
  send_up[3]=0x92;
  Zigb.write(send_up, 4);
  Serial.println("デバッグ用上りデータ送信");
 */
  for(int i=0; i<41; i++){
    send_down[i]=0;
    send_up[i] = 0;
  }
}

/*Ackが帰ってきているかチェックする*/
void ack_check(){
  Serial.print("\nack_check=");
  for(int i=0; i<sendBLE_count; i++){
    Serial.print(ack_strge[i], HEX);
    Serial.print(" ");
  }

  /*自分が発見したタグIDと一致するタグIDが帰ってきたら消し込み*/
  for(int x=0; x<ack_count; x+=2){
    for(int y=0; y<sendBLE_count; y+=2){
      
      if(Parent==0) ack[y]=ack[y]&0x07; /*ack_strge[]にID,Parent値を合わせる*/
      
       /*ackと一致したack_strgeを初期化して前詰め*/
      if(ack[x]==ack_strge[y] && ack[x+1]==ack[y+1]){
        ack_strge[y]=0;
        ack_strge[y+1]=0;
        for(int z=y; z<sendBLE_count-1; z+=2){
          ack_strge[z] = ack_strge[z+2];
          ack_strge[z+1]=ack_strge[z+3];
        }
        sendBLE_count-=2;
      }
    }
  }
  
  //デバッグ用Serial.print
  Serial.print("\nack_checked=");
  for(int i=0; i<sendBLE_count; i++){
    Serial.print(ack_strge[i], HEX);
    Serial.print(" ");
  }
  Serial.print("\nsendBLE_count=");
  Serial.println(sendBLE_count);
  delay(500);
}

byte ID_Judge(){
	byte IDno;
    pinMode(IdAdGND, OUTPUT);		//ID判定用可変抵抗へのGND用ポートを出力に設定
    pinMode(IdAdPW, OUTPUT);		//ID判定用可変抵抗への電源用ポートを出力に設定
    digitalWrite(IdAdGND, LOW);   //ID判定用可変抵抗へのGND用ポートを0V出力
    digitalWrite(IdAdPW, HIGH);   //ID判定用可変抵抗への電源用ポートに3.3V出力
    float val_w=readSens_ch(0,2,10);  //10ms間隔で2回測定の平均値(AD0ch, 読取回数, intarvalms)
    if      (val_w > 992)  IDno=0x0F; 
    else if (val_w > 928)  IDno=0x0E;      
    else if (val_w > 864)  IDno=0x0D;      
    else if (val_w > 800)  IDno=0x0C;      
    else if (val_w > 736)  IDno=0x0B;      
    else if (val_w > 672)  IDno=0x0A;      
    else if (val_w > 608)  IDno=0x09;      
    else if (val_w > 544)  IDno=0x08;      
    else if (val_w > 480)  IDno=0x07;      
    else if (val_w > 416)  IDno=0x06;      
    else if (val_w > 352)  IDno=0x05;      
    else if (val_w > 288)  IDno=0x04;      
    else if (val_w > 224)  IDno=0x03;      
    else if (val_w > 160)  IDno=0x02;      
    else if (val_w > 96 )  IDno=0x01;      
    if (val_w > 32 )  Parent=1;       //AD0の値が32より大きければ親機

    pinMode(IdAdGND, INPUT);	//ID判定用可変抵抗へのGND用ポートを入力に設定
    pinMode(IdAdPW, INPUT);		//ID判定用可変抵抗への電源用ポートを入力に設定

    if(Parent==0)   //子はIDを0にセットする
      IDno=0;
      
    return IDno;
}

float readSens_ch(int ch, int n, int intarvalms){
  int sva =0;
  for (int i = 0; i <n ; i++){ //n回平均
    delay(intarvalms);
    sva = (analogRead(ch) + sva);
  }
  return sva/n;
}

//スイッチ割り込み処理（送信割込）
void countSend() {
  Serial.print("countSend ");
  if(digitalRead(LED_Speed)==LOW) ALM_Flg=2;   //LOWの時Speedを2に設定
  else ALM_Flg=1;                 //HIGHの時Speedを1に設定
  if(digitalRead(LED_L_R)==LOW) ALM_Flg = 0x0F - ALM_Flg;   //LOWの時下り，ALM＿Flgを反転して1→E,2→D
  Serial.print("ALM_Flg=");
  Serial.println(ALM_Flg,HEX);
  delay(20);
  ALM_comp(byte(ALM_Flg));
  ALM_Flg = 0;
  delay(10);
}

void ALM_comp(byte type){
  Serial.println("ALM_comp");
  if(0<type &&  type<=2){        //ALMデータ上り方向転送
    //type--;    //上り方向のデータ種別(1 2)を-1して設定
    if(ALM_up<type) ALM_up=type;      //危険度の高いものを残す
    Serial.print(type);
    Serial.print("type  ALM_up");
    Serial.println(ALM_up);
  }
  if(0xD<=type && type< 0xF){      //ALMデータ下り方向転送
    //type++;                             //下り方向のデータ種別(D E)を+1して設定
    if(ALM_down>type) ALM_down=type;  //危険度の高い方を残す
    Serial.print(type);
    Serial.print("type  ALM_down");
    Serial.println(ALM_down);
  }
}

int ALM_check(byte type_ALM, byte id){
  if(type_ALM==1 || type_ALM==2){   //上り方向のLED点灯をセット
    if( Parent==0 || id == MyID+1) { //子機または親機で受信IDと自分ID+1が同じならLED点灯
      LED_UP_Flg=type_ALM;                 //ALMによるLEDフラグ（上り用）をセット
      LED_ALM_count=5;                         //ALMフラグを保持するカウンタに5をセット
      Serial.print("LED_UP_Flg=");
      Serial.println(LED_UP_Flg,HEX);
    }
  }else if(type_ALM==0x0E || type_ALM==0x0D){   //下り方向のLED点灯をセット
    if( Parent==0 || id == MyID) { //子機または親機で受信IDと自分IDが同じならLED点灯
      LED_DW_Flg=0x0F-type_ALM;                 //ALMによるLEDフラグ（下り用）をセット
      LED_ALM_count=5;                         //ALMフラグを保持するカウンタに5をセット
      Serial.print("LED_DW_Flg=");
      Serial.println(LED_DW_Flg,HEX);
    }
  }

/*Proの点灯パターン判定ロジック
    if(x==1 || x==5 || x==7) //上りALM1
    if(x==2 || x==6 || x==8) //上りALM2
    if(x==3 || x==5 || x==6) //下りALM1
    if(x==4 || x==7 || x==8) //下りALM2
*/
  int lightPat=0;                   //点灯パターン=0(OFF)
  if( LED_ALM_count>0 ){
    //ALMによるLED点灯指令信号をPWMでArduinoPro側に出力
    if(LED_DW_Flg==0)      lightPat=LED_UP_Flg;   //1,2(UP)
    else if(LED_UP_Flg==0) lightPat=2+LED_DW_Flg; //3,4(DW)
    else lightPat=2+LED_DW_Flg*2+LED_UP_Flg;    //5,6(DW1+UP),7,8(DW2+UP)
  }else{
    LED_DW_Flg=0;      //ALMによるLEDフラグをクリア
    LED_UP_Flg=0xF;      //ALMによるLEDフラグをクリア
  }
  Serial.print("lightPat=");
  Serial.println(lightPat);
  //analogWrite( LED_ALM, Pwm*31 );  //PWM出力
  if(lightPat>0)  delay(100);

  return lightPat;
}
