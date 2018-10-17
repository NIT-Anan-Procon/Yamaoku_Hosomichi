<!DOCTYPE html>
<html>
<head>
  <title>入力データ</title>
  <meta charset="utf-8">
  <link rel="stylesheet" type="text/css" href="Menu.css">
</head>
<body>
<div>
  <h1 style ="text-align:center;background-color:#111111; color:white;box-shadow: 0px 10px 8px rgba(2,2,0,0.5);">やまおくのほそみち管理ページ </h4>
  <!--text-align:center場所（センター）__background-color:#111111背景色(黒)__color:white文字の色__box-shadow (横への影)px (縦方向への影)px (影のぼかし具合)px rgba(色)-->
</div>
<dev id="container">
  <div id="menu">
    <div id="left">
    <?php include ('Menubar.html'); ?>
    </div>
  </div>
  <div id="container" style=color:black>
  <h1>入力データ</h1>

<?php
$mysqli =new mysqli('localhost','root','Keyu3102','test2');
if($mysqli->connect_errno){
  echo $mysqli->connect_errno.';'.$mysqli->connect_errnor;
}
mysqli_set_charset( $mysqli, 'utf8');                               //SQlへの接続

$pointx=$_POST['x'];                                                       //フォームから送られてきた座標データを受け取る
$pointy=$_POST['y'];
$place=$_POST['place'];
$time=$_POST['time'];
$yellowtime=$_POST['yellowtime'];

echo "緯度:$pointx&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
echo "経度:$pointy<br>";
$id1= substr($place,0,1);
$id2=substr($place,1,1);
$placenumber=($id1)*2+$id2+1;                                               //placeのIDをナンバーに変える
echo "見張り灯籠設置場所番号:$placenumber<br>";
if (is_numeric($pointx) && is_numeric($pointy)) {         //すべて数字かどうかを判定
  if($yellowtime>$time){
    $sql ="INSERT INTO ne(place,NPoint,EPoint,time,yellowtime) VALUES ('$place','$pointx','$pointy','$time','$yellowtime')";
    if (mb_strlen($place)==2) {                             //Placeが3文字で表現されているかを判定
      $res = mysqli_query($mysqli,$sql);


      //成功かどうかを判別する
      ob_start();               // 出力バッファリング開始
      var_dump($res);
      $dump=ob_get_contents();  //からのダンプ代入
      ob_end_clean();           // 出力バッファを消去してバッファリング終了
      $dump=substr($dump,5,1);  //5文字目の「ｔ」か「ｆ」を使って判別する

      if ($dump=="t"){
        echo "<h3>入力完了</h3><br>";
        echo '<iframe src="http://maps.google.co.jp/maps?q=',"$pointx,$pointy",'&output=embed&t=m&z=16&hl=ja" frameborder="0" scrolling="no" marginheight="0" marginwidth="0" width="630" height="500"></iframe><br>';    //GoogleMapの埋め込み
      } else {
        echo "<p><h3>失敗しました</h3></p>";
      }
    } else {
      echo "登録情報不足:見張り灯籠の場所番号が入力されていません。";
    }
  }else{
    echo "異常値入力:通過予想時間が異常検出時間よりも短い、または同じです。";
  }
} else {
  echo "登録情報不足:見張り灯籠の設置場所が登録されていません。";
}


$mysqli->close();
echo '<br><a href="setpoint.html">入力画面に戻る</a>';
?>
</div>
<div class-"clear"></div>
</body>
</html>
