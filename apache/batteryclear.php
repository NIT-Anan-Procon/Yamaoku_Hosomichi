<?php
echo "消去実行中<br>";
$place = substr($_GET['place'],1,2);
$mysqli =new mysqli('localhost','root','Keyu3102','test2');
if($mysqli->connect_errno){
    echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
}                                                                                                                                    //SQLへの接続
  mysqli_set_charset($mysqli,'utf8');
  $sql="UPDATE `ne` SET `battery`=\"0\" WHERE place=\"$place\"";      //先頭が０の場合""で囲ってやらないとSQLで消される
  $res = mysqli_query($mysqli,$sql);                                                                               //ここでSQL実行
  $mysqli->close();
?>
<!DOCTYPE html>
<html>
<body>
<meta http-equiv="refresh" content="0; URL='index.html'" />
</body>
</html>
