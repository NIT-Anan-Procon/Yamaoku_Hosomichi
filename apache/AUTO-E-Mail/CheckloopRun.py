import subprocess
from time import sleep
import datetime
mailadress= input('送信先のメールアドレスを入力>>')
print ('-------------------------------------------------------')
while True:
    cmd ="D:/Program/xampp/php/php.exe AUTO-E-Mail\check.php %s" %(mailadress)                    #PowerShellで実行するコマンドの生成ちなみに絶対パスがあるので守らないと死ぬ
    d = datetime.datetime.today()                                                                                                                                       #実行した時間がわかるのはいいことだね
    print('実行時間:', d)
    subprocess.call(cmd)
    print('-------------------------------------------------------')
    sleep (30)                                                                                                                                                                      #流石に１秒間に数十回カウントしたくないからね仕方ないね(HDDキラーができます)
