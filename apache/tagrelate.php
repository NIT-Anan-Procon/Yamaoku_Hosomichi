<!DOCTYPE html>
<html>
<head>
<title>データを入力する</title>
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
  <div id="container">
<?php
$mysqli =new mysqli('localhost','root','Keyu3102','test2');
if($mysqli->connect_errno){
    echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
}

mysqli_set_charset($mysqli,'utf8');
$name=$_POST["yourname"];            //人の名前
$tagid = $_POST["taginfo"];         //ここでタグの3文字をまとめて受信、$tagに入れる
if (mb_strlen($tagid)==3) {
  $sql="INSERT INTO relate (tagid,name) VALUES ('$tagid','$name')";
  $res = mysqli_query($mysqli,$sql);
  $sqlnumber="SELECT*FROM relate WHERE tagid ='$tagid'";
  $resnumber=mysqli_query($mysqli,$sqlnumber);
  $row = mysqli_fetch_assoc($resnumber);
  $number =$row['id'];
  echo "<br><h1>エントリーID:$number</h1>";
} else {
  echo "タグIDが間違っています。タグIDは3文字です";
}
echo '<br><a href="tagrelate.html">入力画面に戻る</a>';
$mysqli->close();
?>
</div>
<div class-"clear"></div>
</body>
</html>
