<!DOCTYPE html>
<html>
<head>
<title>選手の検出状況</title>
<meta charset="utf-8" http-equiv="refresh" content="10">
<link rel="stylesheet" type="text/css" href="Menu.css">
</head>
<body>
<div>
  <h1 style ="text-align:center;background-color:#111111; color:white;box-shadow: 0px 10px 8px rgba(2,2,0,0.5);">やまおくのほそみち管理ページ </h4>
  <!--text-align:center--場所（センター）__background-color:#111111--背景色(黒)__color:white--文字の色__box-shadow (横への影)px (縦方向への影)px (影のぼかし具合)px rgba(色)-->
</div>
<dev id="container">
  <div id="menu">
    <div id="left">
    <?php include ('Menubar.html'); ?>
  </div>
</div>
<body>
  <dev id="center">
<?php
$mysqli =new mysqli('localhost','root','Keyu3102','test2');
if($mysqli->connect_errno){
    echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
}                                                                                                           //SQLへの接続

mysqli_set_charset( $mysqli, 'utf8');
$sql ="SELECT *FROM relate ORDER BY time ASC";
$res=mysqli_query($mysqli,$sql);
while($row = mysqli_fetch_assoc($res)){
  $place=$row['place'];                                        //最終検出場所をロード
  $datatime =$row['time'];                                     //データーベースの時間をロード
  $now = date('Y-m-d H:i:s');                                  //現在時刻を保存
  $now = strtotime($now);                                      //タイムスタンプ形式の現在時間
  $datatimestamp = strtotime($datatime);                       //タイムスタンプ形式のデータベースの時間
  $dif_timenow =time_diff($datatimestamp,$now);                //データーベースとの時間差
  $sqlplace ="SELECT *FROM ne WHERE place=$row[place]";   //場所IDを利用してデーターベース照合
  $resplace=mysqli_query($mysqli,$sqlplace);
  $rowplace = mysqli_fetch_assoc($resplace);
    echo "最終検出時間:",$datatime,"<br>";
    echo "最終検出時間差:$dif_timenow<br>";
    if(strtotime(time_diff($datatimestamp,$now))>=strtotime("00:".$rowplace['yellowtime'].":00") ||time_diff($datatimestamp,$now)=="24時間以上未検出") {
      echo "<font color=red>",$row['name'],"は危険です</font>";
    } else if (strtotime(time_diff($datatimestamp,$now))>=strtotime("00:".$rowplace['time'].":00")){
      echo "<font color=deeppink>",$row['name'],"は遅れています</font>";
    } else {
      echo "<font size=3>名前:",$row['name']," </font>";
    }
    echo "<a href=\"listupplace.php?place='$place'\" target=\"_blank\">マップで確認</a>";
    echo "<hr />";

}
echo "<title>長期間未検出選手一覧</title>";
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
</body>
</dev>
</html>
