<!DOCTYPE html>
<html>
<head>
<title>灯籠通過者リスト</title>
<meta charset="utf-8">
<link rel="stylesheet" type="text/css" href="Menu.css">
<?php $placeid=$_GET['placeid'];                                       //場所のコードを受け取る ?>
</head>
<body>
<div>
  <h1 style ="text-align:center;background-color:#111111; color:white;box-shadow: 0px 10px 8px rgba(2,2,0,0.5);">やまおくのほそみち管理ページ </h4>
  <!--text-align:center--場所（センター）__background-color:#111111--背景色(黒)__color:white--文字の色__box-shadow (横への影)px (縦方向への影)px (影のぼかし具合)px rgba(色)-->
</div>
<dev id="center">
  <div id="menu">
    <div id="left">
    <?php include ('Menubar.html'); ?>
  </div>
<body>
<h1 style="color:black">灯籠ID:
  <?php
    $id1= substr($placeid,0,1);
    $id2=substr($placeid,1,1);
    $placenumber=(hexdec($id1))*2+(hexdec($id2)+1);                                               //placeのIDをナンバーに変える
    echo $placenumber;
  ?>
</h1>
<?php
$mysqli=mysqli_connect('localhost','root','Keyu3102','test2');                                    //MySQLのtest2へのアクセス
if (mysqli_connect_errno()) {
 printf("Connect failed: %s\n", mysqli_connect_error());
 exit();
}
$sqlplayer="SELECT * FROM log WHERE place=$placeid \n ORDER BY time  DESC";
//  echo "sqlplayer:", $sqlplayer, "<br>";                                                                    //デバッグ用
$Resultplayer=mysqli_query($mysqli,$sqlplayer);
while (($rowplayer = mysqli_fetch_assoc($Resultplayer))){            //まとまりの解釈が問題を起こしていたため1つ目の条件を（ ）で囲っておく
  //echo "rowplayer:",$rowplayer["tagid"],"<br>";                                                   //デバッグ用
  $entryidsql="SELECT * FROM relate WHERE tagid='$rowplayer[tagid]' ";
  //echo "entryidsql:$entryidsql<br>";                                                                     //デバッグ用
  $resentryid=mysqli_query($mysqli,$entryidsql);
  $entryid=mysqli_fetch_assoc($resentryid) ;

  echo "エントリーID--\"$entryid[id]\"" ,"_____通過時刻--\"$rowplayer[time]\"<br>";
}
?>

</body>
</html>
