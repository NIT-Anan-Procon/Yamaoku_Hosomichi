<!DOCTYPE html>
<html>
<head>
<title>選手検索</title>
<meta charset="utf-8">
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
</dev id="right">
<?php
echo "<title>検索結果</title>";
$mysqli =new mysqli('localhost','root','Keyu3102','test2');
if($mysqli->connect_errno){
    echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
}

mysqli_set_charset($mysqli,'utf8');

if (mb_strlen($_POST["playername"])==0) {
  echo "<p><h3>エントリーIDが入力されていません。検索画面に戻って入力してください</h3></p>";
} else {
  $playername=$_POST["playername"];
  $sql="SELECT * FROM relate WHERE id = '$playername'";                //抽出(一部でも単語がかぶるとみれますのでニコ動みたいな不便さはありません)
  $res = mysqli_query($mysqli,$sql);


  ob_start();               // 出力バッファリング開始
  var_dump($res);
  $dump=ob_get_contents();  //からのダンプ代入
  ob_end_clean();           // 出力バッファを消去してバッファリング終了
  $dump=substr($dump,135,3);  //135文字目のint(0)の(0)を使って判別する

  if ($dump=="(0)") {
    echo "<p>選手が見つかりませんでした。もう一度エントリーIDを確認してください</p>";
  }else {
    while ($row = mysqli_fetch_assoc($res)){                         //検索結果すべてのデータを出力するまでループ
      echo "選手の名前:",$row['name'],"<br>";
      echo "最後の検出時間:",$row['time'],"<br>";
      $placedata = $row['place'];
      $sqlplace = "SELECT * FROM ne WHERE place='$placedata'";                        //場所のコードを抽出
      $Result=mysqli_query($mysqli,$sqlplace);
      $rowplace = mysqli_fetch_assoc($Result);
      $pointx=$rowplace['NPoint'];                                      //北緯
      $pointy=$rowplace['EPoint'];                                      //東経
      echo "検出場所<br>",'<iframe src="http://maps.google.co.jp/maps?q=',"$pointx,$pointy",'&output=embed&t=m&z=16&hl=ja" frameborder="0" scrolling="no" marginheight="0" marginwidth="0" width="630" height="450"></iframe>';    //GoogleMapの埋め込み;
      echo "<HR />";
    }
  }
}
echo '<a href="searchplayer.html">検索画面に戻る</a>';
  ?>
</dev>
