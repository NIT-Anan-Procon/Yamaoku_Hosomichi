#coding=utf-8
import serial
import sys
import subprocess
import binascii
from struct import *

print ("\n-------------- SerialRead ----------------\n")
print ('動作モードを選択してください。トングル接続状態での動作モードでは"0"を、システムテストの場合は"1"を入力してください')
mode=input('動作モードの入力>>')

if mode=='0' :
    while True:                                                                     #トングルありの通常モード
        print ("\n================ Python ==================\n")
        print('動作モード:通常')
        ser = serial.Serial('COM3',115200,timeout=None)
        line=str(ser.read(1))                                               #まずシリアル通信を直に受ける
        check=line[5]
        if check.isdecimal():
            byte=ser.read(1)                                                    #読込するためのバイト数を読み取る
            bytes =str(binascii.b2a_hex(byte), 'utf-8')                      #じゃけんバイト数をわかりやすく変換しましょうね～
            size=int(bytes,16)
            i=0
            print ("\n##############読み込み開始 ##############\n")
            print ("通信容量:",size*2,"バイト")
            while size>i:
                print("ループ回数:",i)
                i+=1
                tagdata =ser.read(2)
                tagdatastr=str(binascii.b2a_hex(tagdata), 'utf-8')                     #すばらしい変換（hex->str）

                print ("受信データ:",tagdatastr)
                tagdata=(bin(int(tagdatastr,16))).strip("0b").zfill(8)                                   #2文字目ここで一気に16進数から2進数を出す（"0b"は粛清されました）１桁文字になるときは0埋めします
                place01=tagdata[0]                                                                  #タグの場所データの01部分
                tagid8=int(tagdata[1:4],2)                                                                 #タグIDの最初の3bitを2進数に変換
                ctagplace=tagdatastr[0]+str(tagdata[0])                                                                                                         #コマンドライン引数用tagid
                ctagid=str(tagid8)+str(tagdatastr[2:4])

                PS = "D:/Program/xampp/php/php.exe D:/Procon/web/apache/serial/serialplace.php %s %s" %(ctagid,ctagplace)            #Stiing型の引数つけてコマンド実行
                subprocess.call(PS)
            ser.close()
            print ("\n############## 読み込み終了 ##############\n")
        else:
            print("\nNot found Serial Data")
            ser.close()

##########################################トングルがないときのテストモード##########################################
elif mode=='1' :
    while True:                                                                     #トングルありの通常モード
        print ("\n================ Python ==================\n")
        print('動作モード:テスト')
        print("Warning:現在テストモードで動作しています。\nこれはコンピュータ内部の仮想ポートを使ったデバックシステムであるため、USB型Zigbeeトングルでは受信できません。\n仮想ポートは[COM12]を使用してください\n")
        ser = serial.Serial('COM12',115200,timeout=None)
        line=str(ser.read(1))                                               #まずシリアル通信を直に受ける
        print("先頭判別データ:",line)
        check=line[5]
        print ("チェック:",check)
        if check.isdecimal():
            byte=ser.read(1)                                                    #読込するためのバイト数を読み取る
            bytes =str(binascii.b2a_hex(byte), 'utf-8')                      #じゃけんバイト数をわかりやすく変換しましょうね～
            size=int(bytes,16)
            i=0
            print ("\n##############読み込み開始 ##############\n")
            print ("通信容量:",size,"バイト")
            while size>i*2:
                print("ループ回数:",i)
                i+=1
                tagdata =ser.read(2)
                tagdatastr=str(binascii.b2a_hex(tagdata), 'utf-8')                     #すばらしい変換（hex->str）

                print ("受信データ:",tagdatastr)
                tagdata=(bin(int(tagdatastr[1],16))).strip("0b").zfill(4)                                   #2文字目ここで一気に16進数から2進数を出す（"0b"は粛清されました）１桁文字になるときは0埋めします    バグの原因は０梅である    0110のときに0011になる
                print("2進数化結果:",tagdata)
                place01=tagdata[0]                                                                  #タグの場所データの01部分
                print ("place01",place01)
                tagid8=int(tagdata[1:4],2)                                                                 #タグIDの最初の3bitを2進数に変換
                print ("tagid8:",tagid8)
                ctagplace=tagdatastr[0]+str(tagdata[0])                                                                                                         #コマンドライン引数用tagid
                ctagid=str(tagid8)+str(tagdatastr[2:4])
                print(ctagplace)
                print(ctagid)

                PS = "D:/Program/xampp/php/php.exe D:/Procon/web/apache/serial/serialplace.php %s %s" %(ctagid,ctagplace)            #Stiing型の引数つけてコマンド実行
                subprocess.call(PS)
            ser.close()
            print ("\n############## 読み込み終了 ##############\n")
        else:
            print("\nNot found Serial Data")
            ser.close()
else :
    subprocess.call("powershell cls")
    subprocess.call("python serialread.py")
