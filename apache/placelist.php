<!DOCTYPE html>
<html>
<head>
  <title>見張り灯籠設置場所一覧</title>
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
  <h1 style=color:black;>見張り灯籠設置場所一覧</h1>
  <iframe src="allplace.php" frameborder="0" scrolling="no" marginheight="0" marginwidth="0" width="630" height="480" style=color:black;></iframe><br>
<HR/>

  <?php
  $mysqli=mysqli_connect('localhost','root','Keyu3102','test2');    //MySQLのtest2へのアクセス
if (mysqli_connect_errno()) {
  printf("Connect failed: %s\n", mysqli_connect_error());
  exit();
}
  $sql = "SELECT * FROM ne ORDER BY place ASC";                                        //テーブルneを選択
  $Result=mysqli_query($mysqli,$sql);                               //結果の代入
  while ($row = mysqli_fetch_assoc($Result)){                         //テーブルないすべてのデータを出力するまでループ
    $place=$row['place'];
    $id1= substr($place,0,1);
    $id2=substr($place,1,1);
    $placenumber=($id1)*2+$id2+1;                                               //placeのIDをナンバーに変える
    //echo "ID:",$row['id'],"<br>";
    echo "灯籠番号:",$placenumber,"<br>";
    $pointx=$row['NPoint'];                 //GoogleMapの埋め込み緯度
    $pointy=$row['EPoint'];                 //GoogleMapの埋め込み経度
    echo '<iframe src="http://maps.google.co.jp/maps?q=',"$pointx,$pointy",'&output=embed&t=m&z=16&hl=ja" frameborder="0" scrolling="no" marginheight="0" marginwidth="0" width="300" height="270"></iframe>';    //GoogleMapの埋め込み
    echo "<HR />";
  }
  $mysqli->close();
  ?>
</div>
<div class-"clear"></div>
</body>
</html>
