<?php
$mailadress=$argv[1];                                           //引数の取得（これがメアド）
echo "メールアドレス:$mailadress\n";
$mysqli =new mysqli('localhost','root','Keyu3102','test2');
if($mysqli->connect_errno){
    echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
}                                                                                                           //SQLへの接続
mysqli_set_charset( $mysqli, 'utf8');
$sql ="SELECT *FROM relate ORDER BY time ASC";
if ($res=mysqli_query($mysqli,$sql)) {
  $poorplayer=0;                                                  //行方不明者数の初期化
  while($row = mysqli_fetch_assoc($res)){
    $place=$row['place'];                                        //最終検出場所をロード
    $datatime =$row['time'];                                     //データーベースの時間をロード
    $now = date('Y-m-d H:i:s');                                  //現在時刻を保存
    $now = strtotime($now);                                      //タイムスタンプ形式の現在時間
    $datatimestamp = strtotime($datatime);                       //タイムスタンプ形式のデータベースの時間
    $dif_timenow =time_diff($datatimestamp,$now);                //データーベースとの時間差
    $sqlplace ="SELECT *FROM ne WHERE place=$row[place]";   //場所IDを利用してデーターベース照合
    if($resplace=mysqli_query($mysqli,$sqlplace)){
      $rowplace = mysqli_fetch_assoc($resplace);

      if(strtotime(time_diff($datatimestamp,$now))>=strtotime("00:".$rowplace['yellowtime'].":00") ||time_diff($datatimestamp,$now)=="24時間以上未検出") {
        echo "最終検出時間:",$datatime,"\n";
        echo "最終検出時間差:",$dif_timenow,"\n";
        $id1= substr($place,0,1);
        $id2=substr($place,1,1);
        $placenumber=(hexdec($id1))*2+(hexdec($id2)+1);
        echo "検出した灯籠ID:",$placenumber;
        echo $row['name'],"は危険です\n";
        $poorplayer++;                                        //行方不明者のカウント
        if($poorplayer==1){
          $playerdata="最終検出時間:$datatime\n経過時間--$dif_timenow\n{$row['name']}は危険です\nマップで確認するためのURL：listupplace.php?place='$place'\n\n";    //メールに乗せる選手データの生成
        } else {
          $playerdata="最終検出時間:$datatime\n経過時間--$dif_timenow\n{$row['name']}は危険です\nマップで確認するためのURL：listupplace.php?place='$place'\n\n$playerdata";    //メールに乗せる選手データの生成
        }
      }
    }
}



}
echo $playerdata;
if ($poorplayer>0){                                                             //行方不明者がいたら送信する
  $message= $poorplayer."人が行方不明です\n";
  echo $message;                                                               //行方不明者の人数を出力

  $to =$mailadress;                                                                   //メール送信先などの設定
  $subject = 'やまおくのほそみち行方不明者通知';
  $body ="$message$playerdata";                                              //まず報告用に文章を作る

  if (mb_send_mail($to, $subject, $body)) {                             //メール送信成功かどうかの判定兼送信
      echo "送信完了\n";
      sleep (240);                                                                      //メールクラッシャーになりたくないでござる
  } else {
      echo "送信失敗\n";
  }
}


//***************************************
// 日時の差を計算
//***************************************
function time_diff($time_from, $time_to)
{
    // 日時差を秒数で取得
    $dif = $time_to - $time_from -60*60*9;      //時差に対応するため60秒*60分*9時間の処理をかけている
    // 時間単位の差
    $dif_time = date("H:i:s", $dif);
    // 日付単位の差
    $dif_days = (strtotime(date("Y-m-d", $dif)) - strtotime("1970-01-01")) / 86400;
    if ($dif_days==0) {
      return "{$dif_time}";
    } else {
    return "24時間以上未検出";
  }
}
?>
