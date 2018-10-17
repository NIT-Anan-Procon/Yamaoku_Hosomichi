<?php
echo "<title>最終検知場所</title>";
$mysqli =new mysqli('localhost','root','Keyu3102','test2');

if($mysqli->connect_errno){
  echo $mysqli->connect_errno.';'.$mysqli->connect_errnor;
}
mysqli_set_charset( $mysqli, 'utf8');
$placeid=$_GET['place'];                                       //場所のコードを受け取る
$sql = "SELECT * FROM ne WHERE place =$placeid";    //Placeと一致するものだけを表示していくための抽出
$Result=mysqli_query($mysqli,$sql);
$row = mysqli_fetch_assoc($Result);
$pointx=$row['NPoint'];                                       //北緯
$pointy=$row['EPoint'];                                       //東経
echo '<iframe src="http://maps.google.co.jp/maps?q=',"$pointx,$pointy&sensor=true",'&output=embed&t=m&z=16&hl=ja" frameborder="0" scrolling="no" marginheight="0" marginwidth="0" width="600" height="450"></iframe>';    //GoogleMapの埋め込み
?>
