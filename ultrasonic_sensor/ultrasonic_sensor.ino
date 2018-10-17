/*ultrasonic_sensor.ino   旧基板V2.0用  超音波センサとLED点灯制御*/
//demo

#define VernNo "Ver1.15"

//Arduinoライブラリ
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

//---------------------------------
#define max_range 100 //100cm
//---------------------------------

#define BAUDRATE 38400   //ボーレート

#define ALM_LR 3          //miniからALM入力LRの向きHighで上り，LOWで下り

// for SD shield
#define CMD 11        //SD_CMD 端子番号 pin11-13 SD用
#define chipSelect 8  //chipselect 端子番号
#define TRON 10      //PVの8.2Ω短絡による電流測定ON
int SD_Flg=0;       //SD有無チェック
File dataFile;      //SD出力ファイル名
int Initialization=1;     //SDカード1行目出力判断用

#define US_PowerPin 2 // US_Power
#define US_L_trigPin 4 // US_L Trigger Pin
#define US_L_echoPin 5 // US_L Echo Pin
#define US_R_trigPin 6 // US_R Trigger Pin
#define US_R_echoPin 7 // US_R Echo Pin

//#define max_range 450

#define LED_L_Trg 9   // Arduino LED_Trg 割り込み用出力，LOWで割り込み＆出力中信号，Highで信号読込モード
#define LED_ALM IN 16 // (analog 2)Arduino MiniからALM入力
#define ALM_IN_ADch 2 // (analog 2)Arduino MiniからALM入力
#define LED_LR_Sig 15 // (analog 1)Arduino MiniへL-R出力 HighでL，LOWでR  (電池電圧測定と兼用)
#define LED_S_Sig 19  // (analog 5)Arduino MiniへSpeed出力 Highで低速，LOWで高速

#define LED_R_Pin 14 // (analog 0) LED_R ON出力ポート HighでON
#define LED_L_Pin 18 // (analog 4) LED_L ON出力ポート HighでON

#define US_timeout_max 27000 //4.5mm
#define LED_on_time 300 // LEDのOntime (ms)
#define LED_Flashing_time 5000 // LEDの点灯時間 (ms)
#define LED_longOff_time 19600 // LEDの稼働中表示点滅Off時間 (ms)

/*変数定義*/

unsigned long Timems=0;   //現在時刻
unsigned long Num=0;      //ループカウンタ
unsigned long Time[2];      //速度検出時刻（処理速度計測用）
unsigned long Time_old[2]; //速度算出用1回前の時刻を記録
int timelog[2];//デバッグ用
unsigned long LED_Timer_On[2];  //[0]:L [1]:R　OFF→ONに切り替えるms時間を確定
unsigned long LED_Timer_Off[2]; //[0]:L [1]:R　ON→Offに切り替えるms時間を確定
unsigned long LED_ON_Timer[2];  //[0]:L [1]:R　点灯を続ける時間を確定ms

int LED_OffTime[2];  //LED_OffTime[0]:L [1]:R　LEDのOFF時間の設定
int LED_f[2];   //LED点灯フラグ　[0]:L [1]:R　　1:ON　0:OFF
int Distance[2];   //距離 US_L [0] US_R [1]
int Distance_old[2]; //一つ前の距離 US_L [0] US_R [1]
int Distance_old_log[2]; //一つ前の距離 US_L [0] US_R [1]
int Dist_cout[2];   //距離を検出したカウント　[0]:L [1]:R　
int Dist_f=0;      //距離検出フラグ 1:検出 0:検出無し
int Dist_average[2]; //距離の平均値 [0]:L [1]:R
int Dist_average_old[2]; //距離の平均値の一つ前の値 [0]:L [1]:R
int Velocity[2];  //速度の値 [0]:L [1]:R
int Velocity_old[2];  //一つ前の速度の値 [0]:L [1]:R
int Velocity_ave[2];  //一つ前との平均速度の値 [0]:L [1]:R
int V_count[2];  //速度連続検出カウント [0]:L [1]:R
int Velo_F[2];  //検知速度 [0]:L [1]:R
int send_f=0;   //速度検出によるデータ送信フラグ
int LED_S_f[2]; //受信によるLED点灯フラグ 1:低速 2:高速
// int US_timeout=27000;   //超音波センサのタイムアウト変数
int US_timeout[2];     //超音波センサのタイムアウト変数

/*日射量と電池電圧測定用*/
float Eneloop=5.88;                   //Eneloop電池電圧 
#define Eneloop_const 0.0064453125      //Eneloop電池電圧AD値からの換算係数 
float Radiation = 0;                 //2Wパネル測定日射量
#define Radiation_constant 1.41     //2Wパネル日射量センサ定数 0602見直し1.45→1.41
int val[4];//，ad値の平均値


void setup()
{
  Serial.begin(BAUDRATE);
  pinMode(TRON,OUTPUT);  
  digitalWrite(TRON,LOW);
  // The chipSelect pin you use should also be set to output
  pinMode(chipSelect, OUTPUT);
  pinMode(12,OUTPUT); //信号用ピン                  
  
  pinMode( US_L_echoPin, INPUT );
  pinMode( US_L_trigPin, OUTPUT );
  pinMode( US_R_echoPin, INPUT );
  pinMode( US_R_trigPin, OUTPUT );
  pinMode( US_PowerPin, OUTPUT );
  digitalWrite(US_PowerPin, HIGH);//US_Power ON

  pinMode(LED_S_Sig, OUTPUT);

  pinMode(ALM_LR, INPUT);		//miniからALM入力(R-L)

  pinMode(LED_L_Pin, OUTPUT);
  digitalWrite(LED_L_Pin, LOW);//消灯

  pinMode(LED_R_Pin, OUTPUT);
  digitalWrite(LED_R_Pin, HIGH);//点灯
  delay(300);
  digitalWrite(LED_R_Pin, LOW);//消灯

  Serial.println("--- during startup WDT_setup---");

  for (int n=0 ; n<2 ; n++){
    LED_Timer_On[n] = 0;
    LED_Timer_Off[n] = 0;
    LED_f[n] = 0;
    Time_old[n] =0;
    Dist_cout[n] =0;
    Dist_average[n]=max_range;
    US_timeout[n]=US_timeout_max;
  }

  if(SD.begin(chipSelect)){
    SD_Flg=1; //SDチェック
    dataFile = SD.open("Distlog.csv",FILE_WRITE);
    Serial.println(" File Open ");
  }

  val[1]=ReadSens_ch(1,4,50);      //Eneloop電圧AD1の5回平均値(個別ch, 読取回数, intarvalms) 
  Eneloop=val[1]*Eneloop_const;
}
//==============================================================================================
void loop()
{
  Timems=millis();    //現在時刻の読み込み
  for(int n=0 ; n<2 ; n++){       //LとRで，距離と速度を検出
    send_f=0;         //送信フラグをクリア
    Velocity[n] = 0;        //速度をクリア
    Velocity_ave[n] = 0;    //平均速度をクリア
    Velo_F[n]=0;            //速度フラグをクリア
    Distance[n]=DistanceM(n,US_timeout[n]);    //0はL，1はR
    Time[n]=millis(); //測定時刻
    if( Distance[n] < max_range &&  Distance[n]>5){
      Dist_cout[n]++;
      if( Dist_cout[n] >= 2 && Distance_old[n]-Distance[n] > 0){     //有効距離連続2回検出し，近づいていたら速度計算 
        if( Time[n]-Time_old[n] > 0 ){    //時刻が有効か確認
          Velocity[n] = (Distance_old[n]-Distance[n])*36/(Time[n]-Time_old[n]); //距離検出した前回の時刻との差で割算
          V_count[n]++;
          if( V_count[n] >= 2 ){     //有効距離連続2回検出し，近づいていたらVelo_countを+
            Velocity_ave[n]=(Velocity_old[n]+Velocity[n])/2;       //一つ前の速度と平均         
            if(Velocity_ave[n] >  5)   Velo_F[n]=1;     //ジョギングレベル
            if(Velocity_ave[n] > 20)   Velo_F[n]=2;     //車両レベル
            if( Velocity_ave[n] > 100) Velo_F[n]=0;     //異常値
          }else Velocity_ave[n]=0;
          Velocity_old[n]=Velocity[n]; //速度をoldに保存
        }
      }else{
        V_count[n]=0;     //速度が有効でない時，V_countの初期化
      } 
      Time_old[n]=Time[n];     //距離検出した時の時刻をoldに保存
      Distance_old[n]=Distance[n]; //距離をoldに保存
    }else{
      Dist_cout[n]=0;    //カウンタをクリア
    }
  }
  
  Dist_f=0;   //距離検出フラグを初期化
  for(int n=0;n<2;n++){   //ALM検出送信とLED_OffTime設定、LEDフラグ設定
    if( Dist_cout[n] > 0 ) Dist_f=1;    //距離検出中なら距離検出フラグを1に設定
    if(Velo_F[n] > 0){
        ALM_send( n, Velo_F[n]);  //ALM検出送信
        send_f=1;           //速度検出値があれば送信フラグをON
    }
    if(Velo_F[n]>=1 || LED_S_f[n]>=1 ){  //歩行者以上の検知もしくは、前方からの検知情報有り
      if(LED_OffTime[n^0x01] > 1200) LED_OffTime[n^0x01] = 1200; //前方から歩行者通知 LEDのOff時間が700より大きい時700msを設定
      if((LED_S_f[n]==2 || Velo_F[n]==1) && LED_OffTime[n^0x01] > 500){ //歩行者レベル または 前方から車両
        LED_OffTime[n^0x01] = 500;   //LEDのOff時間が300より大きい時300msを設定
      }  
      if(Velo_F[n]==2) LED_OffTime[n^0x01] = 100;  //車両レベル LEDのOff時間を100msで設定
      LED_ON_Timer[n^0x01] = Timer_set(LED_Flashing_time);             //5秒間点滅を設定
      LED_f[n^0x01]=1;
      LED_Timer_On[n^0x01] = Timer_set(LED_OffTime[n^0x01]);   //オフ時間を加えてON時刻をセット
    }
    if(Velo_F[n]>0){
        Serial.print("[");
        Serial.print(n);
        Serial.print("]=");
        Serial.print(Velo_F[n]);
        Serial.print(" D=");
        Serial.print(Distance[n]);
        Serial.print(" V=");
        Serial.print(Velocity[n]);
        Serial.print(" V_ave=");
        Serial.println(Velocity_ave[n]);
    }
  }

  if(send_f==0 && Dist_f==0) ALM_receive();     //送信フラグが0かつ距離検出中で無いなら信号読込

  for(int n=0;n<2;n++){   //LED点滅制御
    //点滅範囲時間を超えていたら5700msのoffタイムを設定
    if(millis() > LED_ON_Timer[n] ) LED_OffTime[n]=LED_longOff_time; 
    if(LED_f[n]==1 && millis() > LED_Timer_Off[n] ){     //Offタイマーの時間を越えていたら
        LED_f[n]=0;                       //LEDフラグを0（OFF）にセット
        LED_Timer_On[n] = Timer_set(LED_OffTime[n]);   //オフ時間を加えてON時刻をセット
        LED_Timer_Off[n] = LED_Timer_On[n]+100;          //off時刻ががon時刻の後になるようにセット
//        Serial.print("OFF_OFFTime[");
//        Serial.print(n);
//        Serial.print("]=");
//        Serial.print(LED_OffTime[n]);
    }
    if(millis() > LED_Timer_On[n] ){     //Onタイマーの時間を越えていたら
        LED_f[n]=1;                      //LEDフラグを1（ON）にセット
        LED_Timer_Off[n] = Timer_set(LED_on_time);       //オン時間を加えてoff時刻をセット
        LED_Timer_On[n] = LED_Timer_Off[n]+100;          //on時刻ががOff時刻の後になるようにセット
    }
  }

  if(LED_f[0]==1 ) digitalWrite(LED_L_Pin, HIGH);//フラグが1なら点灯
  else digitalWrite(LED_L_Pin, LOW);//消灯
  if(LED_f[1]==1 ) digitalWrite(LED_R_Pin, HIGH);//フラグが1なら点灯
  else digitalWrite(LED_R_Pin, LOW);//消灯

  if(SD_Flg==1 && (Dist_cout[0]+Dist_cout[1]) >0 ) WriteSD();  //SD書きだし   

  if(Num>3000 && (send_f==0 && Dist_f==0) ){    //3000ループに１回程度呼び出す。
    if(SD_Flg==1){     //SD_File CloseとOpen
      dataFile.close();
      Serial.println(" File close ");
      digitalWrite(LED_L_Pin, HIGH);
      digitalWrite(LED_R_Pin, HIGH);
      EneloopV(); //電池電圧チェック
      delay(300); 
      digitalWrite(LED_L_Pin, LOW);
      digitalWrite(LED_R_Pin, LOW);
      dataFile = SD.open("Distlog.csv",FILE_WRITE);
      Serial.println(" File Open ");
      Num=0;
    }
  }
  Num++;
} //=====================================================================================

/* 距離測定  */
int DistanceM(int n,int UsTimeOut) {         //nは0がL，1がR，ポート番号のシフト用L=0,R=+2
  int Duration = 0; //受信した間隔
  int Distance_m = 0; //距離測定値ローカル変数
  int trigPin=US_L_trigPin + n*2;   //Rは+2
  digitalWrite(trigPin, LOW);    
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH ); //超音波を出力 
  delayMicroseconds(11); //
  digitalWrite( trigPin, LOW );  
  Duration = pulseIn( US_L_echoPin + n*2, HIGH,UsTimeOut ); //センサからの入力
  if(Duration == 0 ) Distance_m=max_range;
  else  Distance_m = Duration * 0.0170; // (cm) 音速を340m/sに設定 0.017=0.0340/2
//  if(Distance_m>max_range ) Distance_m=max_range;
  return Distance_m;
}

/* ALM信号送信 */
void ALM_send(int LR_flg,int S_flg){
  digitalWrite(LED_L_Trg, HIGH);//プルアップ
  
  // (analog 0) Highで上り（Lセンサ）L，LOWで下り（Rセンサ）
  pinMode(LED_LR_Sig, OUTPUT);
  if(LR_flg == 0)  digitalWrite(LED_LR_Sig, HIGH);//上り
  else  digitalWrite(LED_LR_Sig, LOW);//下り
  // (analog 4)Arduino MiniへSpeed出力 Highで低速，LOWで高速
  if(S_flg == 2)  digitalWrite(LED_S_Sig, LOW);//高速
  else digitalWrite(LED_S_Sig, HIGH);//低速
  delay(10);

  pinMode(LED_L_Trg, OUTPUT);
  digitalWrite(LED_L_Trg, LOW);//トリガ出力
  delay(10);
  
  digitalWrite(LED_L_Trg, HIGH);//トリガOFF
  pinMode(LED_L_Trg, INPUT);
  digitalWrite(LED_L_Trg, HIGH);//プルアップ
  pinMode(LED_LR_Sig, INPUT);  //アナログリード用に入力に戻す
  send_f=0;
}

/* ALM信号受信 */
void ALM_receive(){
    delay( 10 );
    int AD=ReadSens_ch(ALM_IN_ADch,10,1);   //ADch0 1ms間隔で10回測定平均値を取得 
    LED_S_f[0]=0;
    LED_S_f[1]=0;

    if(AD >250 ) {
      if(digitalRead(ALM_LR)==LOW)   LED_S_f[1]=2; //Highで上り，LOWで下り
      else LED_S_f[0]=2;
    }els if(AD >10 ) {
      if(digitalRead(ALM_LR)==LOW)   LED_S_f[1]=1; //Highで上り，LOWで下り
      else LED_S_f[0]=0;
    }
    if(AD>10){
      Serial.print(" UP=");
      Serial.print(LED_S_f[0]);
      Serial.print(" DW=");
      Serial.println(LED_S_f[1]);
    }
}


void WriteSD(){
//  SD.begin(chipSelect);

//  File dataFile = SD.open("Distlog.csv",FILE_WRITE);

  // if the file is available, writ
  if (dataFile) {

    dataFile.print(Num) ;                   //ループカウンタ
    dataFile.print(',');
    dataFile.print(Timems) ;                   //計測開始時の時刻を出力
    dataFile.print(',');
    dataFile.print(Time[0]-Timems) ;                   //L測定の時間
    dataFile.print(',');
    dataFile.print(Time[1]-Time[0]);                   //R測定の時間
    dataFile.print(',');

    for(int n=0;n<2;n++){
      dataFile.print(Distance[n]) ;                   //距離を出力
      dataFile.print(',');
      dataFile.print(Dist_cout[n]) ;                   //有効距離計測カウントを出力
      dataFile.print(',');
      dataFile.print(LED_f[n]);                       //LEDfrg
      dataFile.print(',');
      dataFile.print(LED_OffTime[n]);                 //offtime
      dataFile.print(',');
      dataFile.print(Velocity[n]);                   //速度
      dataFile.print(',');
      dataFile.print(Velocity_ave[n]);                   //速度の平均値
      dataFile.print(',');
    }
    dataFile.print(Radiation);       //2Wパネル太陽電池で計測した日射量を出力
    dataFile.print(',');
    dataFile.print(Eneloop);                        //Eneloop電池電圧
    dataFile.print(',');
    
    if ( Initialization == 1){
      dataFile.print("No");
      dataFile.print(',');
      dataFile.print("Time");
      dataFile.print(',');
      dataFile.print("Time_L");
      dataFile.print(',');
      dataFile.print("Time_R");
      dataFile.print(',');
      dataFile.print("Distance_L") ;                   //L側の距離
      dataFile.print(',');
      dataFile.print("Dist_cout_L") ;                   //L側の有効計測カウント
      dataFile.print(',');
      dataFile.print("LED_f_L");                       //LEDfrg
      dataFile.print(',');
      dataFile.print("LED_OffTime_L");                 //offtime
      dataFile.print(',');
      dataFile.print("Velocity_L");                   //速度
      dataFile.print(',');
      dataFile.print("Velocity_ave_L");                   //速度の平均値
      dataFile.print(',');

      dataFile.print("Distance_R") ;                   //L側の距離
      dataFile.print(',');
      dataFile.print("Dist_cout_Raspberrypaiへの電源") ;                   //L側の有効計測カウント
      dataFile.print(',');
      dataFile.print("LED_f_R");                       //LEDfrg
      dataFile.print(',');
      dataFile.print("LED_OffTime_R");                 //offtime
      dataFile.print(',');
      dataFile.print("Velocity_R");                   //速度
      dataFile.print(',');
      dataFile.print("Velo_F_R");                   //V_Frag
      dataFile.print(',');
      dataFile.print("Velocity_ave_R");                   //速度の平均値
      dataFile.print(',');
      
      dataFile.print("Radiation");
      dataFile.print(',');
      dataFile.print("V_Eneloop");              
      dataFile.print(',');
      dataFile.print(VernNo);
      Initialization=0;
    }
    dataFile.println("");      //改行
//    dataFile.close();
  }
}
unsigned long Timer_set(int Ti){   //現在のms時刻に引数のms時間を加えた値を返す
  unsigned long x=millis();
          x = x + Ti;                 //タイマーをセット
          if( x < Ti ) x = Ti;     //タイマーのオーバーフロー時の処理
  return x;
}

void EneloopV(){
  digitalWrite(TRON,HIGH);         //TR ON  2Wパネル日射量測定の為太陽電池出力短絡ON
  val[3]=ReadSens_ch(3,4,50);      //2Wパネル日射量AD3の5回平均値(個別ch, 読取回数, intarvalms)
  digitalWrite(TRON,LOW);          //TR OFF  太陽電池出力短絡OFF
  Radiation=val[3]*Radiation_constant;

//Eneloop電池電圧が5.9V以上で日射量があるとき、電流測定回路をONにして過充電を防止する。
  val[1]=ReadSens_ch(1,4,50);      //Eneloop電圧AD1の4回平均値(個別ch, 読取回数, intarvalms)(delay200も兼ねている)
  Eneloop=val[1]*Eneloop_const;
  if (Eneloop>6.0 ){
    digitalWrite(TRON,HIGH);//TR ON  電流測定回路ON 
  }
}

float ReadSens_ch(int ch, int n, int intarvalms){
        int sva =0;
        for (int i = 0; i <n ; i++){ //n回平均
            delay(intarvalms);
            sva = (analogRead(ch) + sva);
        }
        return sva/n;
}

